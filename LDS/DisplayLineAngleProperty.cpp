//<LOCALE_TRANSLATE BY hxr />
#include "stdafx.h"
#include "LDS.h"
#include "LDSView.h"
#include "MainFrm.h"
#include "CfgPartNoDlg.h"
#include "env_def.h"
#include "GlobalFunc.h"
#include "LmaDlg.h"
#include "InputJgGDlg.h"
#include "CutWingAngleDlg.h"
#include "DesignJgWingNorm.h"
#include "Query.h"
#include "OddmentDesignDlg.h"
#include "AddNewBoltDlg.h"
#include "PlateLsRefDlg.h"
#include "DatumPointDlg.h"
#include "RelativeObjectDlg.h"
#include "dlg.h"
#include "ReportPointDlg.h"
#include "PartUcsDlg.h"
#include "ModifyRelativeObjPartNoDlg.h"
#include "FaceOffsetDlg.h"
#include "GroupJgStyleDlg.h"
#include "XhCharString.h"
#include "ColorSchemaDlg.h"
#include "PartWorkPlaneDlg.h"
#include "ExtraAngleOddSetDlg.h"
#include "DesignVectorPropDlg.h"
#include "DatumPointPropertyDlg.h"
#include "DesignShadowDlg.h"
#include "DatumLinePropertyDlg.h"
#include "Expression.h"
#include "PartLib.h"
#include "OutputInfo.h"
#include "LabelProp.h"
#include "OpenSlotDlg.h"
#include "NewAngleLocaModeMacroDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//from DisplayPropertyPage.cpp
BOOL SetPartMutiObjsProp(CLDSObject *pObj, char *propName,long idClassType=0);
void SetPropItemReadOnly(CLDSDbObject *pObj,char *propName,BOOL bReadOnly);

#ifndef __TSA_
BOOL ModifyLineAngleItemStatus(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSLineAngle::SetPropStatus(pItem->m_idProp,pItem->m_bHideChildren);
	return TRUE;
}
BOOL LineAnglePropItemHelpLiveString(CPropertyList* pPropList,CPropTreeItem *pItem)
{
	CLDSLineAngle *pLineAngle=NULL;
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(pView->selectObjs.GetNodeNum()>0)
		pLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	else
		return FALSE;
	if( (pItem->m_idProp==CLDSLineAngle::GetPropID("norm_x_wing")||pItem->m_idProp==CLDSLineAngle::GetPropID("norm_x_wing.hViceJg"))&&
		!pLineAngle->des_norm_x.bByOtherWing&&!pLineAngle->des_norm_x.bSpecific&&pLineAngle->des_norm_x.hViceJg>0x20)
	{	//֫���߷���Ϊ���˼������淨��
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pLineAngle->des_norm_x.hViceJg);
		return TRUE;
	}
	else if((pItem->m_idProp==CLDSLineAngle::GetPropID("norm_y_wing")||pItem->m_idProp==CLDSLineAngle::GetPropID("norm_y_wing.hViceJg"))&&
		!pLineAngle->des_norm_y.bByOtherWing&&!pLineAngle->des_norm_y.bSpecific&&pLineAngle->des_norm_y.hViceJg>0x20)
	{	//֫���߷���Ϊ���˼������淨��
		g_pSolidDraw->ReleaseSnapStatus();
		g_pSolidDraw->SetEntSnapStatus(pLineAngle->des_norm_y.hViceJg);
		return TRUE;
	}
	else if (pItem->m_idProp == CLDSLineAngle::GetPropID("oddStart.m_hRefPart1"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		//if(pLineAngle->desStartOdd.m_iOddCalStyle==0)
		g_pSolidDraw->SetEntSnapStatus(pLineAngle->desStartOdd.m_hRefPart1);
		return TRUE;
	}
	else if (pItem->m_idProp == CLDSLineAngle::GetPropID("oddStart.m_hRefPart2"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		//if(pLineAngle->desStartOdd.m_iOddCalStyle==0)
		g_pSolidDraw->SetEntSnapStatus(pLineAngle->desStartOdd.m_hRefPart2);
		return TRUE;
	}
	else if (pItem->m_idProp == CLDSLineAngle::GetPropID("oddEnd.m_hRefPart1"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		//if(pLineAngle->desEndOdd.m_iOddCalStyle==0)
		g_pSolidDraw->SetEntSnapStatus(pLineAngle->desEndOdd.m_hRefPart1);
		return TRUE;
	}
	else if (pItem->m_idProp == CLDSLineAngle::GetPropID("oddEnd.m_hRefPart2"))
	{
		g_pSolidDraw->ReleaseSnapStatus();
		//if(pLineAngle->desEndOdd.m_iOddCalStyle==0)
		g_pSolidDraw->SetEntSnapStatus(pLineAngle->desEndOdd.m_hRefPart2);
		return TRUE;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("work_wing.hViceJg"))
	{	//֫���߷���Ϊ���˼������淨��
		g_pSolidDraw->ReleaseSnapStatus();
		long hWorkPlaneRefJg=pLineAngle->GetWorkWingX0Y1()==0?pLineAngle->des_norm_x.hViceJg:pLineAngle->des_norm_y.hViceJg;
		g_pSolidDraw->SetEntSnapStatus(hWorkPlaneRefJg);
		return TRUE;
	}
	else
	{
		g_pSolidDraw->ReleaseSnapStatus();
		ARRAY_LIST<long> harr(0,pView->selectObjs.GetNodeNum());
		for(CLDSObject* pObj=pView->selectObjs.GetFirst();pObj;pObj=pView->selectObjs.GetNext())
			harr.append(pObj->handle);
		g_pSolidDraw->SetEntSnapStatus(harr.m_pData,harr.GetSize());
	}
	JGZJ jgzj;
	if(pItem->m_idProp==CLDSLineAngle::GetPropID("wingXG"))
	{
		getjgzj(jgzj,pLineAngle->GetWidth());
		bool specialG=true;
		if(jgzj.g==pLineAngle->xWingXZhunJu.g || !pLineAngle->m_bEnableTeG)
			specialG=false;
#ifdef AFX_TARG_ENU_ENGLISH
		pItem->m_lpNodeInfo->m_strPropHelp="The executing bolt g of X angle leg";
#else 
		if(specialG)
			pItem->m_lpNodeInfo->m_strPropHelp=(char*)CXhChar50("�Ǹ�X֫��ǰִ������׼��, ��׼׼��Ϊ%d",jgzj.g);
		else
			pItem->m_lpNodeInfo->m_strPropHelp="�Ǹ�X֫��ǰִ�б�׼׼��(׼�����Ĭ��ֵ)";
#endif
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("wingYG"))
	{
		getjgzj(jgzj,pLineAngle->GetWidth());
		bool specialG=true;
		if(jgzj.g==pLineAngle->xWingYZhunJu.g || !pLineAngle->m_bEnableTeG)
			specialG=false;
#ifdef AFX_TARG_ENU_ENGLISH
		pItem->m_lpNodeInfo->m_strPropHelp="The executing bolt g of Y angle leg";
#else 
		if(specialG)
			pItem->m_lpNodeInfo->m_strPropHelp=(char*)CXhChar50("�Ǹ�Y֫��ǰִ������׼��, ��׼׼��Ϊ%d",jgzj.g);
		else
			pItem->m_lpNodeInfo->m_strPropHelp="�Ǹ�Y֫��ǰִ�б�׼׼��(׼�����Ĭ��ֵ)";
#endif
	}
	else
		return FALSE;
	return TRUE;
}
void UpdateLineAngle_DesPos_SimpleHuoquJg_TreeItem(CPropertyList *pPropList,CPropTreeItem *pLeaderItem,BOOL bStart);
void UpdateLineAngle_DesPos_SimpleDatumPoint_TreeItem(CPropertyList *pPropList,CPropTreeItem *pPropItem,BOOL bStart);
void UpdateLineAngle_OpenSlot_TreeItem(CPropertyList *pPropList,CPropTreeItem *pPropItem);
BOOL FireAnglePopMenuClick(CPropertyList* pPropList,CPropTreeItem* pItem,CString sMenuName,int iMenu)
{
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(pView->selectObjs.GetNodeNum()==0)
		return FALSE;
	CXhChar100 sText;
	CMultiOffsetPos* pCurrPos=NULL;
	BOOL bFinished = FALSE, bStart = TRUE, bReCalAnglePos = FALSE;
	bool bUserSpecified=(sMenuName.Compare("�����ж�")==0) ? false : true;
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	if( pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleDatumPoint")||
		pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleDatumPoint"))
	{	//���㶨λ����
		if(iMenu==4)
		{	//����
			AfxMessageBox("�ݲ�֧�ֹ��߶�λ,���л�������������λ��ģʽ������Ϊ��ָ����ƽ�ơ���ָ����ͶӰ����");
			return FALSE;
		}
		bStart=(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleDatumPoint"));
		pItem->m_lpNodeInfo->SetCheck(bUserSpecified?-1:1);
		for(CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pCurrPos=bStart?&pLineAngle->desStartPos:&pLineAngle->desEndPos;
			pCurrPos->bUdfDatumPointMode=bUserSpecified;
			if(pCurrPos->m_iSimpleDatumPoint!=iMenu-1&&iMenu==1&&pCurrPos->datumPoint.xRsltPosition.IsZero())
			{	//��㶨λ����ת��Ϊ���㶨λ
				GEPOINT tippoint;
				if(bStart)
					tippoint=(pLineAngle->pStart!=NULL)?pLineAngle->pStart->xOriginalPos:pLineAngle->xPosStart;
				else
					tippoint=(pLineAngle->pEnd!=NULL)?pLineAngle->pEnd->xOriginalPos:pLineAngle->xPosEnd;
				pLineAngle->desStartPos.datumPoint.SetPosition(tippoint);
			}
			pCurrPos->m_iSimpleDatumPoint=iMenu-1;
			pLineAngle->IntelliUpdateSimplePosParam(bStart);
		}
		if(pFirstLineAngle->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		pItem->m_lpNodeInfo->m_bMutiObjsProp = (!pCurrPos->bUdfDatumPointMode)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pItem->m_idProp);
		UpdateLineAngle_DesPos_SimpleDatumPoint_TreeItem(pPropList,pItem,bStart);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.cWorkWingStyle")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.cWorkWingStyle"))
	{	//�Ǹֶ�λ����֫
		bStart=(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.cWorkWingStyle"));
		pItem->m_lpNodeInfo->SetCheck(bUserSpecified?-1:1);
		for(CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pCurrPos=bStart?&pLineAngle->desStartPos:&pLineAngle->desEndPos;
			pCurrPos->cWorkWingStyle=iMenu;
			bReCalAnglePos = TRUE;
		}
		if(pFirstLineAngle->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		pItem->m_lpNodeInfo->m_bMutiObjsProp = (pCurrPos->cWorkWingStyle==0)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pItem->m_idProp);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleDestLineMode")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleDestLineMode"))
	{	//Ŀ���׼��
		bStart=(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleDestLineMode"));
		pItem->m_lpNodeInfo->SetCheck(bUserSpecified?-1:1);
		for(CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pCurrPos=bStart?&pLineAngle->desStartPos:&pLineAngle->desEndPos;
			//����һ�δ�����һ�鲻�ɷָ�Ĵ��룬�Ժ��кð취�ټ��ɵ�ciDestLineMode�ĸ�ֵ�����С�wjh-2016.11.30
			char cDatumAngleWing=pCurrPos->cDatumAngleWing;
			if(iMenu==CMultiOffsetPos::TO_AXISLINE)
			{
				BOOL bUdfLappedDatumWing=pCurrPos->bUdfLappedDatumWing;
				pCurrPos->bUdfLappedDatumWing=FALSE;
				char cLappedWing=pLineAngle->GetLappedWorkWing(bStart,NULL);
				pCurrPos->bUdfLappedDatumWing=bUdfLappedDatumWing;
				pCurrPos->cDatumAngleWing=cLappedWing;
			}
			pCurrPos->ciDestLineMode=iMenu;

			pCurrPos->cDatumAngleWing=cDatumAngleWing;
			pLineAngle->IntelliUpdateSimplePosParam(bStart);
			bReCalAnglePos = TRUE;
		}
		if(pFirstLineAngle->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		pItem->m_lpNodeInfo->m_bMutiObjsProp = (pCurrPos->ciDestLineMode==0)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pItem->m_idProp);
		UpdateLineAngle_DesPos_SimpleHuoquJg_TreeItem(pPropList,pItem->m_pParent,bStart);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleDenpendWing")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleDenpendWing"))
	{	//�������֫
		bStart=(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleDenpendWing"));
		pItem->m_lpNodeInfo->SetCheck(bUserSpecified?-1:1);
		for(CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pCurrPos=bStart?&pLineAngle->desStartPos:&pLineAngle->desEndPos;
			pCurrPos->bUdfLappedDatumWing=bUserSpecified;
			if(iMenu==1)
				pCurrPos->cSimpleDatumAngleWing='X';
			else if(iMenu==2)
				pCurrPos->cSimpleDatumAngleWing='Y';
			pLineAngle->IntelliUpdateSimplePosParam(bStart);
		}
		if(pFirstLineAngle->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		pItem->m_lpNodeInfo->m_bMutiObjsProp = (!pCurrPos->bUdfLappedDatumWing)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pItem->m_idProp);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleSelfWorkWing")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleSelfWorkWing"))
	{	//������֫
		bStart=(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleSelfWorkWing"));
		pItem->m_lpNodeInfo->SetCheck(bUserSpecified?-1:1);
		for(CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pCurrPos=bStart?&pLineAngle->desStartPos:&pLineAngle->desEndPos;
			pCurrPos->cSimpleSelfWorkWing=iMenu;
			pLineAngle->IntelliUpdateSimplePosParam(bStart);
		}
		if(pFirstLineAngle->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		pItem->m_lpNodeInfo->m_bMutiObjsProp = (pCurrPos->cSimpleSelfWorkWing==0)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pItem->m_idProp);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleWingXTipCtrlPoint")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleWingXTipCtrlPoint")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleWingYTipCtrlPoint")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleWingYTipCtrlPoint"))
	{
		bool bHasCoordX=sMenuName.Find('X')>=0;
		bool bHasCoordY=sMenuName.Find('Y')>=0;
		bool bHasCoordZ=sMenuName.Find('Z')>=0;
		char work_wing='X';
		long idOtherWing=0;
		if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleWingXTipCtrlPoint")
			||pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleWingXTipCtrlPoint"))
		{
			bStart=(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleWingXTipCtrlPoint"));
			idOtherWing=bStart?CLDSLineAngle::GetPropID("desStartPos.SimpleWingYTipCtrlPoint"):CLDSLineAngle::GetPropID("desEndPos.SimpleWingYTipCtrlPoint");
			work_wing='X';
		}
		else //if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleWingYTipCtrlPoint")
			//||pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleWingYTipCtrlPoint"))
		{
			bStart=(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleWingYTipCtrlPoint"));
			idOtherWing=bStart?CLDSLineAngle::GetPropID("desStartPos.SimpleWingXTipCtrlPoint"):CLDSLineAngle::GetPropID("desEndPos.SimpleWingXTipCtrlPoint");
			work_wing='Y';
		}
		pItem->m_lpNodeInfo->SetCheck(bUserSpecified?-1:1);
		for(CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pCurrPos=bStart?&pLineAngle->desStartPos:&pLineAngle->desEndPos;
			pCurrPos->bUdfCtrlCoordsMode=bUserSpecified;
			GEPOINT lenStdVec=pLineAngle->End()-pLineAngle->Start();
			normalize(lenStdVec);	//Standarized(lenStdVec);
			GEPOINT vWingNormX=pLineAngle->get_norm_x_wing();
			GEPOINT vWingNormY=pLineAngle->get_norm_y_wing();
			if(!bHasCoordX&&!bHasCoordZ&&!bHasCoordY)	//�������
			{	//��֫�������ת������ʱӦ������֫ƫ�Ʒ�ʽ
				if(pCurrPos->spatialOperationStyle!=18||pCurrPos->spatialOperationStyle!=19)
					pCurrPos->wing_x_offset.gStyle=pCurrPos->wing_y_offset.gStyle=0;
				pCurrPos->spatialOperationStyle=18;	//ȫ������������ʾתΪ������Ƶ�ͶӰ
			}
			else //if(pCurrPos->spatialOperationStyle==15)
			{
				if( pCurrPos->coordCtrlType!=2&&(fabs(lenStdVec.z)>EPS_COS2||	//�п���ͼ��������,�ӽ���ֱ�˼����Բ���ƽ��
					pLineAngle->Layer(0)=='S'||pLineAngle->Layer(0)=='L'))	//����˼�
				{
					pCurrPos->spatialOperationStyle=19;	//������Ƶ�ƽ��
					pCurrPos->face_offset_norm.Set(0,0,1);	//ƽ����Ϊˮƽ��
					if(fabs(lenStdVec.x)<fabs(lenStdVec.y))	//�����¶�С�������¶�
					{	//�Բ���Ϊƽ�����׼
						if(fabs(vWingNormX.x)>fabs(vWingNormY.x))
							pCurrPos->cFaceOffsetDatumLine='X';
						else
							pCurrPos->cFaceOffsetDatumLine='Y';
					}
					else	//�����¶�С�ڲ����¶�
					{	//������Ϊƽ�����׼
						if(fabs(vWingNormX.y)>fabs(vWingNormY.y))
							pCurrPos->cFaceOffsetDatumLine='X';
						else
							pCurrPos->cFaceOffsetDatumLine='Y';
					}
				}
				else
				{
					pCurrPos->spatialOperationStyle=15;	//��֫�������
					if((bHasCoordX&&bHasCoordZ)||(bHasCoordY&&bHasCoordZ)||(bHasCoordX&&bHasCoordY))
					{
						pCurrPos->cMainCoordCtrlWing=work_wing;
						if(bHasCoordX&&bHasCoordZ)	//ָ��X,Z����
							pCurrPos->coordCtrlType=0;
						else if(bHasCoordY&&bHasCoordZ)	//ָ��Y,Z����
							pCurrPos->coordCtrlType=1;
						else //if(bHasCoordX&&bHasCoordY)	//ָ��X,Y����
							pCurrPos->coordCtrlType=2;
					}
					else if(bHasCoordX||bHasCoordZ||bHasCoordY)
					{
						pCurrPos->cMainCoordCtrlWing='X'+'Y'-work_wing;
						if(bHasCoordY)	//ָ��X,Z����
							pCurrPos->coordCtrlType=0;
						else if(bHasCoordX)	//ָ��Y,Z����
							pCurrPos->coordCtrlType=1;
						else //if(bHasCoordZ)	//ָ��X,Y����
							pCurrPos->coordCtrlType=2;
					}

					if(bHasCoordX&&bHasCoordZ)	//ָ��X,Z����
					{
						pCurrPos->face_offset_norm.Set(1,0,0);	//ƽ����ΪY-Zƽ��
						pCurrPos->ctrlWing.operStyle=pCurrPos->otherWing.operStyle=1;
					}
					else if(bHasCoordY&&bHasCoordZ)	//ָ��Y,Z����
					{
						pCurrPos->face_offset_norm.Set(0,1,0);	//ƽ����ΪX-Zƽ��
						pCurrPos->ctrlWing.operStyle=pCurrPos->otherWing.operStyle=2;
					}
					else //if(bHasCoordX&&bHasCoordY)	//ָ��X,Y����
					{
						pCurrPos->face_offset_norm.Set();
						pCurrPos->ctrlWing.operStyle=pCurrPos->otherWing.operStyle=0;
					}
				}
			}
			pLineAngle->IntelliUpdateSimplePosParam(bStart);
			bFinished=TRUE;
		}
		if(pFirstLineAngle->GetPropValueStr(pItem->m_idProp,sText)>0)
			pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
		pItem->m_lpNodeInfo->m_bMutiObjsProp = (!pCurrPos->bUdfCtrlCoordsMode)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(idOtherWing,sText)>0)
			pPropList->SetItemPropValue(idOtherWing,CString(sText));
		CPropTreeItem *pOtherWingItem=pPropList->FindItemByPropId(idOtherWing,NULL);
		if(pOtherWingItem)
			pOtherWingItem->m_lpNodeInfo->m_bMutiObjsProp = (!pCurrPos->bUdfCtrlCoordsMode)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pOtherWingItem->m_idProp);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("kaihe_base_wing_x0_y1"))
	{
		for(CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			if(iMenu==1)
				pLineAngle->kaihe_base_wing_x0_y1 = 0;
			else if(iMenu==2)
				pLineAngle->kaihe_base_wing_x0_y1 = 1;
			else if(iMenu==3)
				pLineAngle->kaihe_base_wing_x0_y1 = 3;
			else
				pLineAngle->kaihe_base_wing_x0_y1 = 2;
			if(pFirstLineAngle->GetPropValueStr(pItem->m_idProp,sText)>0)
				pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
			pItem->m_lpNodeInfo->m_bMutiObjsProp=(pLineAngle->kaihe_base_wing_x0_y1 == 2);
			pItem->m_lpNodeInfo->SetCheck(iMenu==0?1:(-1));
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddStart.m_fCollideDist")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("oddEnd.m_fCollideDist"))
	{
		if(iMenu==0)
		{	//��������ͷ
			bool bStartSide=(pItem->m_idProp==CLDSLineAngle::GetPropID("oddStart.m_fCollideDist"));
			bool firstvalue=true;
			double oddment=bStartSide?pFirstLineAngle->startOdd():pFirstLineAngle->endOdd();
			for(CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				bool calculated=false;
				if(bStartSide&&pLineAngle->desStartOdd.m_iOddCalStyle!=2)
				{
					BOOL bOldValue=pLineAngle->m_bUserSpecStartCutAngle;
					pLineAngle->m_bUserSpecStartCutAngle=FALSE;
					pLineAngle->CalStartOddment();
					pLineAngle->m_bUserSpecStartCutAngle=bOldValue;
					if(firstvalue)
						oddment=pLineAngle->startOdd();
					calculated=true;
				}
				else if(!bStartSide&&pLineAngle->desEndOdd.m_iOddCalStyle!=2)
				{
					BOOL bOldValue=pLineAngle->m_bUserSpecEndCutAngle;
					pLineAngle->m_bUserSpecEndCutAngle=FALSE;
					pLineAngle->CalEndOddment();
					pLineAngle->m_bUserSpecEndCutAngle=bOldValue;
					if(firstvalue)
						oddment=pLineAngle->endOdd();
					calculated=true;
				}
				if(calculated)
				{
					pLineAngle->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
				}
				firstvalue=false;
			}
			long idOddmentProp=bStartSide?CLDSLineAngle::GetPropID("oddStart"):CLDSLineAngle::GetPropID("oddEnd");
			pPropList->SetItemPropValue(idOddmentProp,CXhChar16("%g",oddment));
		}
		else if(iMenu>=1)
		{	//�����н�
			bool bStartSide=(pItem->m_idProp==CLDSLineAngle::GetPropID("oddStart.m_fCollideDist"));
			for(CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				bool calculated=false;
				if(bStartSide&&pLineAngle->desStartOdd.m_iOddCalStyle!=0)
				{
					CLDSLineAngle *pCutAngle1=(CLDSLineAngle*)Ta.FromPartHandle(pLineAngle->desStartOdd.m_hRefPart1,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
					CLDSLineAngle *pCutAngle2=(CLDSLineAngle*)Ta.FromPartHandle(pLineAngle->desStartOdd.m_hRefPart2,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
					if(pCutAngle1||pCutAngle2)
					{
						BOOL bOldValue=pLineAngle->m_bUserSpecStartCutAngle;
						pLineAngle->m_bUserSpecStartCutAngle=FALSE;
						if(iMenu==1)	//�����Ի�׼����Բ�����ȵ��нǼ���
							pLineAngle->CalCutAngleInfo(pCutAngle1,pCutAngle2,true,NULL, abs(ftoi(pLineAngle->desStartOdd.m_fCollideDist)));
						else if(iMenu==2)//����ײ��϶Ϊ��׼�����н�
							pLineAngle->CalCutAngleInfo(pCutAngle1,pCutAngle2,true,NULL,-abs(ftoi(pLineAngle->desStartOdd.m_fCollideDist)));
						pLineAngle->m_bUserSpecStartCutAngle=pLineAngle->m_bUserSpecStartCutAngle|bOldValue;
						calculated=true;
					}
				}
				else if(!bStartSide&&pLineAngle->desEndOdd.m_iOddCalStyle!=0)
				{
					CLDSLineAngle *pCutAngle1=(CLDSLineAngle*)Ta.FromPartHandle(pLineAngle->desEndOdd.m_hRefPart1,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
					CLDSLineAngle *pCutAngle2=(CLDSLineAngle*)Ta.FromPartHandle(pLineAngle->desEndOdd.m_hRefPart2,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
					if(pCutAngle1||pCutAngle2)
					{
						BOOL bOldValue=pLineAngle->m_bUserSpecEndCutAngle;
						pLineAngle->m_bUserSpecEndCutAngle=FALSE;
						if(iMenu==1)	//�����Ի�׼����Բ�����ȵ��нǼ���
							pLineAngle->CalCutAngleInfo(pCutAngle1,pCutAngle2,true,NULL, abs(ftoi(pLineAngle->desEndOdd.m_fCollideDist)));
						else if(iMenu==2)//����ײ��϶Ϊ��׼�����н�
							pLineAngle->CalCutAngleInfo(pCutAngle1,pCutAngle2,true,NULL,-abs(ftoi(pLineAngle->desEndOdd.m_fCollideDist)));
						pLineAngle->m_bUserSpecEndCutAngle=pLineAngle->m_bUserSpecEndCutAngle|bOldValue;
						calculated=true;
					}
				}
				if(calculated)
				{
					pLineAngle->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
				}
			}
		}
		g_pSolidDraw->Draw();
	}
	else if (pItem->m_idProp == CLDSLineAngle::GetPropID("desStartPos.LappedDatumWingG") ||
			pItem->m_idProp == CLDSLineAngle::GetPropID("desEndPos.LappedDatumWingG"))
	{
		bool bStartSide = (pItem->m_idProp == CLDSLineAngle::GetPropID("desStartPos.LappedDatumWingG")) ? true : false;
		if (iMenu == 0)
		{
			double dfSpecG = 0;
			bool bFirstValue = true;
			for (CLDSLineAngle* pLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst(); pLineAngle; pLineAngle = (CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				CMultiOffsetPos* pDesPosPara = bStartSide ? &pLineAngle->desStartPos : &pLineAngle->desEndPos;
				pDesPosPara->datumWingOffset.gStyle = 0;
				bReCalAnglePos = TRUE;
				if (bFirstValue)
				{	//��ȡ��һ���˼����׼��ֵ�����ڸ�����������ʾ
					CLDSNode* pNode = bStartSide ? pLineAngle->pStart : pLineAngle->pEnd;
					SmartRodPtr pParentAngle=bStartSide?pLineAngle->GetStartDatumPart():pLineAngle->GetEndDatumPart();
					if (pParentAngle.blValid&&pParentAngle->IsAngle())
					{
						if (pDesPosPara->IsInDatumJgWingX())
							dfSpecG = pParentAngle.pAngle->GetNodeWingXG(pNode);	//Ĭ��׼��
						else if (pDesPosPara->IsInDatumJgWingY())
							dfSpecG = pParentAngle.pAngle->GetNodeWingYG(pNode);	//Ĭ��׼��
						bFirstValue = false;
					}
				}
			}
			//����������ֵ
			pPropList->SetItemPropValue(pItem->m_idProp, CXhChar16(dfSpecG));
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("OpenSlotGroup"))
	{
		if(iMenu==0)
		{	//��ӿ���
			UINT nNum=pFirstLineAngle->xarrOpenCuts.Count;
			if(nNum>=4)
			{
				AfxMessageBox("Ŀǰ���֧��4������!");
				return FALSE;
			}
			COpenSlotDlg dlg;
			dlg.m_pCurJg=pFirstLineAngle;
			if(dlg.DoModal()==IDOK)
			{
				CLDSLineAngle::OPENING_CUT xOpenCut;
				xOpenCut.dfLateralBtmXorY=pFirstLineAngle->size_wide-dlg.m_fSlotH;
				if(dlg.m_iCutWing==0)
					xOpenCut.dfLateralBtmXorY*=-1;
				xOpenCut.dfLenPosFrom=dlg.m_fOffLen;
				xOpenCut.dfLenPosTo=dlg.m_fOffLen+dlg.m_fSlotW;
				xOpenCut.dfTipDeltaS=dlg.m_fDeltaS;
				xOpenCut.dfTipDeltaE=dlg.m_fDeltaE;
				pFirstLineAngle->xarrOpenCuts.Append(xOpenCut);
			}
		}
		else //if(iMenu==1)
		{	//��տ���
			for(CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->xarrOpenCuts.Clear();
		}
		//
		UpdateLineAngle_OpenSlot_TreeItem(pPropList,pItem);
	}
	//���½Ǹ�λ�ã�ˢ��ʵ��
	if (bReCalAnglePos)
	{
		CAngleMoveOperObject anglemodified;
		for (CLDSLineAngle *pLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst(); pLineAngle; pLineAngle = (CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->CalPosition(true);
			pLineAngle->cal_x_wing_norm();
			pLineAngle->cal_y_wing_norm();
			pLineAngle->SetModified();
			pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole, g_pSolidOper->GetScaleUserToScreen(), g_pSolidSet->GetArcSamplingLength());
			CLDSView::RedrawAngleFrameDrawing(pLineAngle);
			g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
		}
		g_pSolidDraw->Draw();
	}
	return bFinished;
}
BOOL ModifyLineAngleProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr);
void UpdateLineAngleOddProperty(CPropertyList* pPropList,CPropTreeItem* pParentItem,BOOL bStart=TRUE);
BOOL LineAngleButtonClick(CPropertyList* pPropList,CPropTreeItem* pItem)
{
	CXhChar500 sText;
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	CLDSView* pView=((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CPropTreeItem *pFindItem, *pParentItem;
	CLDSLineAngle *pFirstLineAngle=NULL, *pLineAngle=NULL;
	CReportVectorDlg vecDlg;
	CReportPointDlg pointDlg;
	CCfgPartNoDlg cfgdlg;
	CCutWingAngleDlg cutdlg;
	CDesignJgWingNorm design_norm;
	CInputJgGDlg dlg;
	COddmentDesignDlg odddlg;
	CString cmdStr, valueStr = _T("");
	//����HashTable
	CHashTable<CLDSPart*>partsetTable;
	partsetTable.Empty();
	partsetTable.CreateHashTable(100);

	if(pView->selectObjs.GetNodeNum()>0)
		pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	else
		return FALSE;
	CXhChar16 sSoftName("TMA");
#if defined(__LDS_FILE_)||defined(__LDS_)
	sSoftName.Copy("LDS");
#elif defined(__LMA_FILE_)||defined(__LMA_)
	sSoftName.Copy("LMA");
#elif defined(__TMA_FILE_)||defined(__TMA_)
	sSoftName.Copy("TMA");
#elif defined(__TSA_FILE_)||defined(__TSA_)
	sSoftName.Copy("TSA");
#endif
	try{
		if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_hPartWeldParent"))
		{	//ͨ��ѡ�񹹼��޸ĺ��Ӹ����������ڽ������޸ĵ��������ĺ��Ӹ�����
			static CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Part, Angle Weld To";
			modifyhdlg.m_sTitle="Part Weld To:";
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->m_hPartWeldParent);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ�񹹼��İ�ť
			modifyhdlg.SetCmdPickPromptStr("Please select the part which angle weld to:");
#else
			modifyhdlg.m_sCaption="�޸ĽǸֺ��Ӹ�����";
			modifyhdlg.m_sTitle="���Ӹ�����:";
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->m_hPartWeldParent);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ�񹹼��İ�ť
			modifyhdlg.SetCmdPickPromptStr("��ѡ�񺸽Ӹ�����:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_PART);
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
				if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part��",sSoftName,MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("m_hPartWeldParent"),valueStr);
					g_pSolidDraw->ReleaseSnapStatus();
					MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					{
						int flagArr[3]={1,2,4};
						CLDSLinePart* pMirLinePart=NULL;
						for(int i=0;i<3;i++)
						{
							if((mirmsg.axis_flag&flagArr[i])==0)
								continue;
							MIRMSG msg=mirmsg.GetSubMirItem(i+1);
							pMirLinePart=(CLDSLinePart*)pLineAngle->GetMirPart(msg);
							if(pMirLinePart)
							{
								CLDSPart *pMirPart=pPart->GetMirPart(msg);
								if(pMirPart)	//������ѡ�������ĶԳƹ��� wht 11-05-07
									pMirLinePart->m_hPartWeldParent=pMirPart->handle;
							}
						}
						pLineAngle->m_hPartWeldParent=pPart->handle;
						g_pSolidDraw->SetEntSnapStatus(pLineAngle->handle);
					}
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("mid_push_flat_node_handle"))
		{
			static CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Middle Push Flat Node Place";
			modifyhdlg.m_sTitle="Middle Push Flat Node:";
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->mid_push_flat_node_handle);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
			modifyhdlg.SetCmdPickPromptStr("Please select middle push flat datum node:");
#else
			modifyhdlg.m_sCaption="�޸��м�ѹ��λ�ýڵ�";
			modifyhdlg.m_sTitle="�м�ѹ��ڵ�:";
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->mid_push_flat_node_handle);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
			modifyhdlg.SetCmdPickPromptStr("��ѡ���м�ѹ���׼�ڵ�:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part��",sSoftName,MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					{
						int flagArr[3]={1,2,4};
						MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
						for(int i=0;i<3;i++)
						{
							if((mirmsg.axis_flag&flagArr[i])==0)
								continue;
							MIRMSG msg=mirmsg.GetSubMirItem(i+1);
							CLDSLineAngle* pMirLineAngle=(CLDSLineAngle*)pLineAngle->GetMirPart(msg);
							if(pMirLineAngle==NULL)
								continue;
							CLDSNode* pMirNode=pNode->GetMirNode(msg);
							if(pMirNode)
								pMirLineAngle->mid_push_flat_node_handle=pMirNode->handle;
						}
						pLineAngle->mid_push_flat_node_handle=pNode->handle;
						pLineAngle->SetModified();
						pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
						g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
					}
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("hStart"))
		{
			static CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Rod Start Node";
			modifyhdlg.m_sTitle="Start Node:";
#else
			modifyhdlg.m_sCaption="�޸ĸ˼�ʼ�˽ڵ�";
			modifyhdlg.m_sTitle="ʼ�˽ڵ�:";
#endif
			if(pFirstLineAngle->pStart)
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->pStart->handle);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select datum node:");
#else
			modifyhdlg.SetCmdPickPromptStr("��ѡ���׼�ڵ�:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			modifyhdlg.m_pInvalidNode=pFirstLineAngle->pEnd;
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part��",sSoftName,MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
				else
				{
					CUndoOperObject undo(&Ta);
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("hStart"),valueStr);
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					{
						CLDSNode* pOldNode=pLineAngle->pStart;
						pLineAngle->UpdateMirLinePartEndNode(MIRMSG(modifyhdlg.m_iMirMsg),pNode,NULL);
						pLineAngle->pStart=pNode;
						pLineAngle->SetModified();
						pLineAngle->ClearFlag();
						CAngleMoveOperObject moveangle(pLineAngle);
						pLineAngle->CalPosition();
						//���¸˼��ϵĶ˽ڵ������Ϣ
						pLineAngle->UpdateRodEndNodeRelation(pOldNode,pNode);
					}
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("sPartNo"))
		{
			CModifyRelativeObjPartNoDlg modify_partno_dlg;
			modify_partno_dlg.m_pWorkPart=pFirstLineAngle;
			if(modify_partno_dlg.DoModal()==IDOK)
			{
				pFirstLineAngle->SetPartNo(modify_partno_dlg.m_sCurObjPartNo.GetString());
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("sPartNo"),CString(modify_partno_dlg.m_sCurObjPartNo));
				SEGI iSeg;
				if(CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(modify_partno_dlg.m_sCurObjPartNo,&iSeg,NULL))
				{
					//pView->selectObjs�����๹����ŵĶκ�ͬ����������CModifyRelativeObjPartNoDlg����� wjh-2016.6.14
					pFirstLineAngle->iSeg=iSeg;
					pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("iSeg"),iSeg.ToString());
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("hEnd"))
		{
			static CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Rod End Node";
			modifyhdlg.m_sTitle="End Node:";
#else
			modifyhdlg.m_sCaption="�޸ĸ˼��ն˽ڵ�";
			modifyhdlg.m_sTitle="�ն˽ڵ�:";
#endif
			if(pFirstLineAngle->pEnd)
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->pEnd->handle);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select datum node:");
#else
			modifyhdlg.SetCmdPickPromptStr("��ѡ���׼�ڵ�:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			modifyhdlg.m_pInvalidNode=pFirstLineAngle->pStart;
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part��",sSoftName,MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
				else
				{
					CUndoOperObject undo(&Ta);
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("hEnd"),valueStr);
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					{
						CLDSNode* pOldNode=pLineAngle->pEnd;
						pLineAngle->UpdateMirLinePartEndNode(MIRMSG(modifyhdlg.m_iMirMsg),NULL,pNode);
						pLineAngle->pEnd=pNode;
						pLineAngle->SetModified();
						pLineAngle->ClearFlag();
						CAngleMoveOperObject moveangle(pLineAngle);
						pLineAngle->CalPosition();
						//���¸˼��ϵĶ˽ڵ������Ϣ
						pLineAngle->UpdateRodEndNodeRelation(pOldNode,pNode);
					}
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_xStartBeforeLift"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Start before Pre-Lift";
#else
			vecDlg.m_sCaption = "Ԥ��ǰ�Ǹ�ʼ������";
#endif
			vecDlg.m_fVectorX = pFirstLineAngle->StartBeforeLift().x;
			vecDlg.m_fVectorY = pFirstLineAngle->StartBeforeLift().y;
			vecDlg.m_fVectorZ = pFirstLineAngle->StartBeforeLift().z;
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_xStartBeforeLift.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_xStartBeforeLift.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_xStartBeforeLift.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			vecDlg.DoModal();
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("startPos"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pointDlg.m_sCaption = "Ridge Start Position";
#else
			pointDlg.m_sCaption = "�Ǹ���������������";
#endif
			pointDlg.m_bEnableLock=TRUE;
			pointDlg.m_fPointX = pFirstLineAngle->Start().x;
			pointDlg.m_fPointY = pFirstLineAngle->Start().y;
			pointDlg.m_fPointZ = pFirstLineAngle->Start().z;
			pointDlg.m_bLockPoint=pFirstLineAngle->IsStartPosLocked();
			if(pFirstLineAngle->desStartPos.jgber_cal_style!=1)
				pointDlg.m_bReadOnly=TRUE;
			if(pointDlg.DoModal()==IDOK)
			{
				sText.Printf("%f",pointDlg.m_fPointX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("startPos.x"),CString(sText));
				sText.Printf("%f",pointDlg.m_fPointY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("startPos.y"),CString(sText));
				sText.Printf("%f",pointDlg.m_fPointZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("startPos.z"),CString(sText));
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->SetStart(f3dPoint(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ));
					if(pointDlg.m_bLockPoint)
						pLineAngle->LockStartPos();
					else 
						pLineAngle->UnlockStartPos();
				}
			}
			//����ʼ�����λ���Ƿ�Ϊ����״̬����ʼ��λ�ò����Ƿ�Ϊֻ��״̬
			if(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart)
			{	//����������Ϊֻ������
				SetPropItemReadOnly(pFirstLineAngle,"startPos.x",TRUE);
				SetPropItemReadOnly(pFirstLineAngle,"startPos.y",TRUE);
				SetPropItemReadOnly(pFirstLineAngle,"startPos.z",TRUE);
				SetPropItemReadOnly(pFirstLineAngle,"desStartPos.wing_x_offset.offsetDist",TRUE);
				SetPropItemReadOnly(pFirstLineAngle,"desStartPos.wing_y_offset.offsetDist",TRUE);
			}
			else
			{	//δ����ʱ���ݶ�λ��ʽ����ֻ������
				SetPropItemReadOnly(pFirstLineAngle,"startPos.x",pFirstLineAngle->desStartPos.jgber_cal_style<1);
				SetPropItemReadOnly(pFirstLineAngle,"startPos.y",pFirstLineAngle->desStartPos.jgber_cal_style<1);
				SetPropItemReadOnly(pFirstLineAngle,"startPos.z",pFirstLineAngle->desStartPos.jgber_cal_style<1);
				//δ����ʱ����׼����������ֻ������
				SetPropItemReadOnly(pFirstLineAngle,"desStartPos.wing_x_offset.offsetDist",pFirstLineAngle->desStartPos.wing_x_offset.gStyle!=4);
				SetPropItemReadOnly(pFirstLineAngle,"desStartPos.wing_y_offset.offsetDist",pFirstLineAngle->desStartPos.wing_y_offset.gStyle!=4);
			}
			SetPropItemReadOnly(pFirstLineAngle,"hStart",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"oddStart",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"oddStart.m_iOddCalStyle",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"oddStart.m_fCollideDist",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"oddStart.m_hRefPart1",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"oddStart.m_hRefPart2",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.jgber_cal_style",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datum_jg_h",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.origin_pos_style",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.cDatumAngleWing",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datumWingOffset",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datumWingOffset.operStyle",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datumWingOffset.gStyle",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datumWingOffset.offsetDist",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.spatialOperationStyle",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datum_offset_dist",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.len_offset_type",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.len_offset_dist",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.offset_zhun",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.SimpleOffsetZhunju",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.huoqu_jg_h",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.huoqu_dist",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datumPoint",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datumPoint.x",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datumPoint.y",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datumPoint.z",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.coordCtrlType",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.cMainCoordCtrlWing",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.ctrlWing",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.ctrlWing.operStyle",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.ctrlWing.gStyle",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.ctrlWing.offsetDist",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.otherWing",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.otherWing.operStyle",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.otherWing.gStyle",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.otherWing.offsetDist",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.fLenOffsetOfCoordWing",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.hDatumLine",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datumLineStart",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			/*SetPropItemReadOnly(pFirstLineAngle,"desStartPos.start_jg_h",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.iStartJgBerStyle",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.fStartJgLengOffsetX",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.fStartJgLengOffsetY",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);*/
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datumLineEnd",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			/*SetPropItemReadOnly(pFirstLineAngle,"desStartPos.end_jg_h",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.iEndJgBerStyle",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.fEndJgLengOffsetX",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.fEndJgLengOffsetY",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);*/
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.cFaceOffsetDatumLine",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.face_offset_norm",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.face_offset_norm.x",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.face_offset_norm.y",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.face_offset_norm.z",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datum_to_ber_style",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.cWorkWingStyle",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.wing_x_offset",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.wing_x_offset.gStyle",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.wing_x_offset.offsetDist",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.wing_y_offset",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.wing_y_offset.gStyle",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.wing_y_offset.offsetDist",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("actualStartPos"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pointDlg.m_sCaption = "Actual Ridge Start Position";
#else
			pointDlg.m_sCaption = "�Ǹ��������ʵ������";
#endif
			pointDlg.m_bEnableLock=FALSE;
			pointDlg.m_bReadOnly=TRUE;
			f3dPoint actualStart,vec=pFirstLineAngle->Start()-pFirstLineAngle->End();
			normalize(vec);
			actualStart = pFirstLineAngle->Start()+vec*pFirstLineAngle->startOdd();
			pointDlg.m_fPointX = actualStart.x;
			pointDlg.m_fPointY = actualStart.y;
			pointDlg.m_fPointZ = actualStart.z;
			pointDlg.DoModal();
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.datum_jg_h")
				||pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.datum_jg_h")
				||pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleDependRod")
				||pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleDependRod"))
		{
			//�޸Ļ�׼λ�û�׼�˼�
			static CModifyPartHandleDlg modifyhdlg;
			modifyhdlg.m_iMirMsg=0;	//�Գ��޸Ļ�׼λ�õĻ�׼�˼�ʱһ�㲻��Ҫ�Գ�
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Datum Rod On Datum Point Place";
			modifyhdlg.m_sTitle="Datum Rod:";
#else
			modifyhdlg.m_sCaption="�޸Ļ���λ�û�׼�˼�";
			modifyhdlg.m_sTitle="��׼����:";
#endif
			BOOL bCurrIsStart=pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.datum_jg_h")||pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleDependRod");
			if(bCurrIsStart)
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->desStartPos.datum_jg_h);
			else
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->desEndPos.datum_jg_h);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select datum rod:");
#else
			modifyhdlg.SetCmdPickPromptStr("��ѡ���׼�˼�:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
				if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part��",sSoftName,MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
					g_pSolidDraw->ReleaseSnapStatus();
					MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					{
						int flagArr[3]={1,2,4};
						CLDSLineAngle* pMirLineAngle=NULL;
						for(int i=0;i<3;i++)
						{
							if((mirmsg.axis_flag&flagArr[i])==0)
								continue;
							MIRMSG msg=mirmsg.GetSubMirItem(i+1);
							pMirLineAngle=(CLDSLineAngle*)pLineAngle->GetMirPart(msg);
							if(pMirLineAngle)
							{
								CLDSPart *pMirDatumAngle=pPart->GetMirPart(msg);
								if(pMirDatumAngle&&pMirDatumAngle->GetClassTypeId()==CLS_LINEANGLE)
								{
									if(bCurrIsStart)
										pMirLineAngle->desStartPos.datum_jg_h=pMirDatumAngle->handle;
									else
										pMirLineAngle->desEndPos.datum_jg_h=pMirDatumAngle->handle;
								}
								else
								{
									if(bCurrIsStart)
										pMirLineAngle->desStartPos.datum_jg_h=0;
									else
										pMirLineAngle->desEndPos.datum_jg_h=0;
								}
							}
						}
						if(bCurrIsStart)
							pLineAngle->desStartPos.datum_jg_h=pPart->handle;
						else
							pLineAngle->desEndPos.datum_jg_h=pPart->handle;
						g_pSolidDraw->SetEntSnapStatus(pLineAngle->handle);
					}
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.huoqu_jg_h")
				||pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.huoqu_jg_h"))
		{
			//�޸Ļ�����׼�Ǹ�
			static CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Bending Datum Angle";
			modifyhdlg.m_sTitle="Datum Angle:";
#else
			modifyhdlg.m_sCaption="�޸Ļ�����׼�Ǹ�";
			modifyhdlg.m_sTitle="��׼�Ǹ�:";
#endif
			if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.huoqu_jg_h"))
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->desStartPos.huoqu_jg_h);
			else
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->desEndPos.huoqu_jg_h);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select datum angle:");
#else
			modifyhdlg.SetCmdPickPromptStr("��ѡ���׼�Ǹ�:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_PART);
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
				if(pPart==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part��",sSoftName,MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
#endif
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.huoqu_jg_h"))
						pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.huoqu_jg_h"),valueStr);
					else
						pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.huoqu_jg_h"),valueStr);
					g_pSolidDraw->ReleaseSnapStatus();
					MIRMSG mirmsg(modifyhdlg.m_iMirMsg);
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					{
						int flagArr[3]={1,2,4};
						CLDSLineAngle* pMirLineAngle=NULL;
						for(int i=0;i<3;i++)
						{
							if((mirmsg.axis_flag&flagArr[i])==0)
								continue;
							MIRMSG msg=mirmsg.GetSubMirItem(i+1);
							pMirLineAngle=(CLDSLineAngle*)pLineAngle->GetMirPart(msg);
							if(pMirLineAngle)
							{
								if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.huoqu_jg_h"))
									pMirLineAngle->desStartPos.huoqu_jg_h=pPart->handle;
								else
									pMirLineAngle->desEndPos.huoqu_jg_h=pPart->handle;
							}
						}
						if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.huoqu_jg_h"))
							pLineAngle->desStartPos.huoqu_jg_h=pPart->handle;
						else
							pLineAngle->desEndPos.huoqu_jg_h=pPart->handle;
						g_pSolidDraw->SetEntSnapStatus(pLineAngle->handle);
					}
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.datumPoint"))
		{
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstLineAngle;
			datumPointDlg.m_xPoint = pFirstLineAngle->desStartPos.datumPoint;
			if(IDOK==datumPointDlg.DoModal())
			{
				datumPointDlg.m_xPoint.UpdatePos(pFirstLineAngle->BelongModel());
				sText.Printf("%f",datumPointDlg.m_xPoint.Position().x);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.datumPoint.x"),CString(sText));
				CPropTreeItem *pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.x"),NULL);
				if(pFindItem)
					pFindItem->SetReadOnly(datumPointDlg.m_xPoint.datum_pos_style!=0);
				sText.Printf("%f",datumPointDlg.m_xPoint.Position().y);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.datumPoint.y"),CString(sText));
				pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.y"),NULL);
				if(pFindItem)
					pFindItem->SetReadOnly(datumPointDlg.m_xPoint.datum_pos_style!=0);
				sText.Printf("%f",datumPointDlg.m_xPoint.Position().z);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.datumPoint.z"),CString(sText));
				pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.z"),&pItem);
				if(pFindItem)
					pFindItem->SetReadOnly(datumPointDlg.m_xPoint.datum_pos_style!=0);
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->desStartPos.datumPoint=datumPointDlg.m_xPoint;
					pLineAngle->ClearFlag();
					CAngleMoveOperObject moveangle(pLineAngle);
					pLineAngle->CalPosition();
					//pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
					//g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.datumLineStart"))
		{
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstLineAngle;
			datumPointDlg.m_xPoint = pFirstLineAngle->desStartPos.datumLineStart;
			if(IDOK==datumPointDlg.DoModal())
			{
				datumPointDlg.m_xPoint.UpdatePos(pFirstLineAngle->BelongModel());
				/*sText.Printf("%f",datumPointDlg.m_pPoint->pos.x);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.datumPoint.x"),CString(sText));
				sText.Printf("%f",datumPointDlg.m_pPoint->pos.y);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.datumPoint.y"),CString(sText));
				sText.Printf("%f",datumPointDlg.m_pPoint->pos.z);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.datumPoint.z"),CString(sText));*/
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->desStartPos.datumLineStart=datumPointDlg.m_xPoint;
					pLineAngle->ClearFlag();
					CAngleMoveOperObject moveangle(pLineAngle);
					pLineAngle->CalPosition();
					//pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
					//g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.datumLineEnd"))
		{
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstLineAngle;
			datumPointDlg.m_xPoint = pFirstLineAngle->desStartPos.datumLineEnd;
			if(IDOK==datumPointDlg.DoModal())
			{
				datumPointDlg.m_xPoint.UpdatePos(pFirstLineAngle->BelongModel());
				/*sText.Printf("%f",datumPointDlg.m_pPoint->pos.x);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.datumPoint.x"),CString(sText));
				sText.Printf("%f",datumPointDlg.m_pPoint->pos.y);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.datumPoint.y"),CString(sText));
				sText.Printf("%f",datumPointDlg.m_pPoint->pos.z);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.datumPoint.z"),CString(sText));*/
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->desStartPos.datumLineEnd=datumPointDlg.m_xPoint;
					pLineAngle->ClearFlag();
					CAngleMoveOperObject moveangle(pLineAngle);
					pLineAngle->CalPosition();
					//pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
					//g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.datumLineStart"))
		{
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstLineAngle;
			datumPointDlg.m_xPoint = pFirstLineAngle->desEndPos.datumLineStart;
			if(IDOK==datumPointDlg.DoModal())
			{
				datumPointDlg.m_xPoint.UpdatePos(pFirstLineAngle->BelongModel());
				/*sText.Printf("%f",datumPointDlg.m_pPoint->pos.x);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.datumPoint.x"),CString(sText));
				sText.Printf("%f",datumPointDlg.m_pPoint->pos.y);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.datumPoint.y"),CString(sText));
				sText.Printf("%f",datumPointDlg.m_pPoint->pos.z);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.datumPoint.z"),CString(sText));*/
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->desEndPos.datumLineStart=datumPointDlg.m_xPoint;
					pLineAngle->ClearFlag();
					CAngleMoveOperObject moveangle(pLineAngle);
					pLineAngle->CalPosition();
					//pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
					//g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.datumLineEnd"))
		{
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstLineAngle;
			datumPointDlg.m_xPoint = pFirstLineAngle->desEndPos.datumLineEnd;
			if(IDOK==datumPointDlg.DoModal())
			{
				datumPointDlg.m_xPoint.UpdatePos(pFirstLineAngle->BelongModel());
				/*sText.Printf("%f",datumPointDlg.m_pPoint->pos.x);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.datumPoint.x"),CString(sText));
				sText.Printf("%f",datumPointDlg.m_pPoint->pos.y);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.datumPoint.y"),CString(sText));
				sText.Printf("%f",datumPointDlg.m_pPoint->pos.z);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.datumPoint.z"),CString(sText));*/
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->desEndPos.datumLineEnd=datumPointDlg.m_xPoint;
					pLineAngle->ClearFlag();
					CAngleMoveOperObject moveangle(pLineAngle);
					pLineAngle->CalPosition();
					//pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
					//g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.face_offset_norm")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.x")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.y")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.z"))
		{	
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Plane Offset Normal";
#else
			vecDlg.m_sCaption = "ƽ���淨��";
#endif
			vecDlg.m_fVectorX = pFirstLineAngle->desStartPos.face_offset_norm.x;
			vecDlg.m_fVectorY = pFirstLineAngle->desStartPos.face_offset_norm.y;
			vecDlg.m_fVectorZ = pFirstLineAngle->desStartPos.face_offset_norm.z;
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.z"),CString(sText));
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					pLineAngle->desStartPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_xEndBeforeLift"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "End Before Pre-Lift";
#else
			vecDlg.m_sCaption = "Ԥ��ǰ�Ǹ��յ�����";
#endif
			vecDlg.m_fVectorX = pFirstLineAngle->EndBeforeLift().x;
			vecDlg.m_fVectorY = pFirstLineAngle->EndBeforeLift().y;
			vecDlg.m_fVectorZ = pFirstLineAngle->EndBeforeLift().z;
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_xEndBeforeLift.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_xEndBeforeLift.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_xEndBeforeLift.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			vecDlg.DoModal();
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("endPos"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pointDlg.m_sCaption = "Ridge End Position";
#else
			pointDlg.m_sCaption = "�Ǹ������յ�����";
#endif
			pointDlg.m_bEnableLock=TRUE;
			pointDlg.m_fPointX = pFirstLineAngle->End().x;
			pointDlg.m_fPointY = pFirstLineAngle->End().y;
			pointDlg.m_fPointZ = pFirstLineAngle->End().z;
			pointDlg.m_bLockPoint=pFirstLineAngle->IsEndPosLocked();
			if(pFirstLineAngle->desEndPos.jgber_cal_style<1)
				pointDlg.m_bReadOnly=TRUE;
			if(pointDlg.DoModal()==IDOK)
			{
				sText.Printf("%f",pointDlg.m_fPointX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("endPos.x"),CString(sText));
				sText.Printf("%f",pointDlg.m_fPointY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("endPos.y"),CString(sText));
				sText.Printf("%f",pointDlg.m_fPointZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("endPos.z"),CString(sText));
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("desEndPos.datum_to_ber_style"),pointDlg.m_bLockPoint);
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->SetEnd(f3dPoint(pointDlg.m_fPointX,pointDlg.m_fPointY,pointDlg.m_fPointZ));
					if(pointDlg.m_bLockPoint)
						pLineAngle->LockEndPos();
					else 
						pLineAngle->UnlockEndPos();
				}
			}
			else
				return FALSE;
			//�����ն����λ���Ƿ�Ϊ����״̬�����ն�λ�ò����Ƿ�Ϊֻ��״̬
			if(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd)
			{	//����������Ϊֻ������
				SetPropItemReadOnly(pFirstLineAngle,"endPos.x",TRUE);
				SetPropItemReadOnly(pFirstLineAngle,"endPos.y",TRUE);
				SetPropItemReadOnly(pFirstLineAngle,"endPos.z",TRUE);
				SetPropItemReadOnly(pFirstLineAngle,"desEndPos.wing_x_offset.offsetDist",TRUE);
				SetPropItemReadOnly(pFirstLineAngle,"desEndPos.wing_y_offset.offsetDist",TRUE);
			}
			else
			{	//δ����ʱ���ݶ�λ��ʽ����ֻ������
				SetPropItemReadOnly(pFirstLineAngle,"endPos.x",pFirstLineAngle->desEndPos.jgber_cal_style<1);
				SetPropItemReadOnly(pFirstLineAngle,"endPos.y",pFirstLineAngle->desEndPos.jgber_cal_style<1);
				SetPropItemReadOnly(pFirstLineAngle,"endPos.z",pFirstLineAngle->desEndPos.jgber_cal_style<1);
				//δ����ʱ����׼����������ֻ������
				SetPropItemReadOnly(pFirstLineAngle,"desEndPos.wing_x_offset.offsetDist",pFirstLineAngle->desEndPos.wing_x_offset.gStyle!=4);
				SetPropItemReadOnly(pFirstLineAngle,"desEndPos.wing_y_offset.offsetDist",pFirstLineAngle->desEndPos.wing_y_offset.gStyle!=4);
			}
			SetPropItemReadOnly(pFirstLineAngle,"hEnd",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"oddEnd",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"oddEnd.m_iOddCalStyle",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"oddEnd.m_fCollideDist",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"oddEnd.m_hRefPart1",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"oddEnd.m_hRefPart2",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.jgber_cal_style",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datum_jg_h",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.origin_pos_style",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.cDatumAngleWing",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datumWingOffset",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datumWingOffset.operStyle",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datumWingOffset.gStyle",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datumWingOffset.offsetDist",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datumPoint",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datumPoint.x",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datumPoint.y",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datumPoint.z",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.spatialOperationStyle",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datum_offset_dist",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.len_offset_type",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.len_offset_dist",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.offset_zhun",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.SimpleOffsetZhunju",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.huoqu_jg_h",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.huoqu_dist",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.coordCtrlType",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.cMainCoordCtrlWing",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.ctrlWing",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.ctrlWing.operStyle",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.ctrlWing.gStyle",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.ctrlWing.offsetDist",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.otherWing",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.otherWing.operStyle",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.otherWing.gStyle",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.otherWing.offsetDist",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.fLenOffsetOfCoordWing",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.hDatumLine",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datumLineStart",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			/*SetPropItemReadOnly(pFirstLineAngle,"desEndPos.start_jg_h",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.iStartJgBerStyle",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.fStartJgLengOffsetX",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.fStartJgLengOffsetY",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);*/
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datumLineEnd",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			/*SetPropItemReadOnly(pFirstLineAngle,"desEndPos.end_jg_h",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.iEndJgBerStyle",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.fEndJgLengOffsetX",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.fEndJgLengOffsetY",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);*/
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.cFaceOffsetDatumLine",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.face_offset_norm",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.face_offset_norm.x",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.face_offset_norm.y",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.face_offset_norm.z",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datum_to_ber_style",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.cWorkWingStyle",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.wing_x_offset",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.wing_x_offset.gStyle",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.wing_x_offset.offsetDist",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.wing_y_offset",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.wing_y_offset.gStyle",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.wing_y_offset.offsetDist",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd);
		} 
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("actualEndPos"))
		{
#ifdef AFX_TARG_ENU_ENGLISH
			pointDlg.m_sCaption = "Actual Ridge End Position";
#else
			pointDlg.m_sCaption = "�Ǹ������յ�ʵ������";
#endif
			pointDlg.m_bEnableLock=FALSE;
			pointDlg.m_bReadOnly=TRUE;
			f3dPoint actualEnd,vec=pFirstLineAngle->End()-pFirstLineAngle->Start();
			normalize(vec);
			actualEnd = pFirstLineAngle->End()+vec*pFirstLineAngle->endOdd();
			pointDlg.m_fPointX = actualEnd.x;
			pointDlg.m_fPointY = actualEnd.y;
			pointDlg.m_fPointZ = actualEnd.z;
			pointDlg.DoModal();
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.datumPoint"))
		{
			CDatumPointPropertyDlg datumPointDlg;
			datumPointDlg.m_pCurEditObj=pFirstLineAngle;
			datumPointDlg.m_xPoint = pFirstLineAngle->desEndPos.datumPoint;
			if(IDOK==datumPointDlg.DoModal())
			{
				datumPointDlg.m_xPoint.UpdatePos(pFirstLineAngle->BelongModel());
				sText.Printf("%f",datumPointDlg.m_xPoint.Position().x);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.datumPoint.x"),CString(sText));
				CPropTreeItem *pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.x"),NULL);
				if(pFindItem)
					pFindItem->SetReadOnly(datumPointDlg.m_xPoint.datum_pos_style!=0);
				sText.Printf("%f",datumPointDlg.m_xPoint.Position().y);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.datumPoint.y"),CString(sText));
				pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.y"),NULL);
				if(pFindItem)
					pFindItem->SetReadOnly(datumPointDlg.m_xPoint.datum_pos_style!=0);
				sText.Printf("%f",datumPointDlg.m_xPoint.Position().z);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.datumPoint.z"),CString(sText));
				pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.z"),NULL);
					pFindItem->SetReadOnly(datumPointDlg.m_xPoint.datum_pos_style!=0);
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->desEndPos.datumPoint=datumPointDlg.m_xPoint;
					pLineAngle->ClearFlag();
					CAngleMoveOperObject moveangle(pLineAngle);
					pLineAngle->CalPosition();
					//pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
					//g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.face_offset_norm")||
			    pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.x")||
			    pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.y")||
			    pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.z"))
		{	
#ifdef AFX_TARG_ENU_ENGLISH
			vecDlg.m_sCaption = "Plane Offset Normal";
#else
			vecDlg.m_sCaption = "ƽ���淨��";
#endif
			vecDlg.m_fVectorX = pFirstLineAngle->desEndPos.face_offset_norm.x;
			vecDlg.m_fVectorY = pFirstLineAngle->desEndPos.face_offset_norm.y;
			vecDlg.m_fVectorZ = pFirstLineAngle->desEndPos.face_offset_norm.z;
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.x"),&pParentItem);
			vecDlg.m_bReadOnlyX = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.y"),&pParentItem);
			vecDlg.m_bReadOnlyY = pFindItem->IsReadOnly();
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.z"),&pParentItem);
			vecDlg.m_bReadOnlyZ = pFindItem->IsReadOnly();
			if(vecDlg.DoModal()==IDOK)
			{
				sText.Printf("%f",vecDlg.m_fVectorX);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.x"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorY);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.y"),CString(sText));
				sText.Printf("%f",vecDlg.m_fVectorZ);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.z"),CString(sText));
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					pLineAngle->desEndPos.face_offset_norm.Set(vecDlg.m_fVectorX,vecDlg.m_fVectorY,vecDlg.m_fVectorZ);
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddStart"))
		{
			odddlg.m_hRefPart1.Format("0X%X",pFirstLineAngle->desStartOdd.m_hRefPart1);
			odddlg.m_hRefPart2.Format("0X%X",pFirstLineAngle->desStartOdd.m_hRefPart2);
			odddlg.m_fCollideDist = pFirstLineAngle->desStartOdd.m_fCollideDist;
			odddlg.m_iOddCalStyle = pFirstLineAngle->desStartOdd.m_iOddCalStyle;
			if(odddlg.DoModal()==IDOK)
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->desStartOdd.m_hRefPart1=HexStrToLong(odddlg.m_hRefPart1);//��ΪHexStrToLong����Ч����sscanf������ַ���ʱ���޸�����ֵ
					pLineAngle->desStartOdd.m_hRefPart2=HexStrToLong(odddlg.m_hRefPart2);
					pLineAngle->desStartOdd.m_fCollideDist = odddlg.m_fCollideDist;
					pLineAngle->desStartOdd.m_iOddCalStyle = odddlg.m_iOddCalStyle;
					pLineAngle->CalStartOddment();
					if(odddlg.m_iOddCalStyle==2)
					{	//����������ͷ��������޸���ײ��϶��Ӧ���¼����н���Ϣ wht 09-12-29
						CLDSLineAngle *pMainAngle=(CLDSLineAngle*)console.FromPartHandle(pLineAngle->desStartOdd.m_hRefPart1,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
						CLDSLineAngle *pCutAngle2=(CLDSLineAngle*)console.FromPartHandle(pLineAngle->desStartOdd.m_hRefPart2,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
						if(pMainAngle||pCutAngle2)
							pLineAngle->CalCutAngleInfo(pMainAngle,pCutAngle2,false,false,ftoi(odddlg.m_fCollideDist));
					}
				}
				char valueStr[MAX_TEMP_CHAR_100+1]="";
				if(pFirstLineAngle->GetPropValueStr(pItem->m_idProp,valueStr,101)>0)
					pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
			}
			//������������
			CXhChar100 itemValue;
			CPropTreeItem* pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("oddStart.m_iOddCalStyle"),NULL);
			if(pFirstLineAngle->GetPropValueStr(pFindItem->m_idProp,itemValue))
				pPropList->SetItemPropValue(pFindItem->m_idProp,itemValue);
			UpdateLineAngleOddProperty(pPropList,pFindItem,TRUE);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddStart.m_hRefPart1") ||
			pItem->m_idProp==CLDSLineAngle::GetPropID("oddStart.m_hRefPart2"))
		{
			int iRefPart1Or2=1;
			if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddStart.m_hRefPart1"))
				iRefPart1Or2=1;
			else
				iRefPart1Or2=2;
			static CModifyPartHandleDlg modifyhdlg;
			if(iRefPart1Or2==1)
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->desStartOdd.m_hRefPart1);
			else
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->desStartOdd.m_hRefPart2);
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Reference Rod";
			modifyhdlg.m_sTitle.Format("Reference Rod %d",iRefPart1Or2);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�/�˼��İ�ť
			cmdStr.Format("Please select reference rod %d:",iRefPart1Or2);
#else 
			modifyhdlg.m_sCaption="�޸Ĳ��ո˼�";
			modifyhdlg.m_sTitle.Format("���ո˼�%d",iRefPart1Or2);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�/�˼��İ�ť
			cmdStr.Format("��ѡ����ո˼�%d:",iRefPart1Or2);
#endif
			modifyhdlg.SetCmdPickPromptStr(cmdStr);
			modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
			if(modifyhdlg.DoModal()!=IDOK)
				return FALSE;
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL|| !pPart->IsAngle())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				MessageBox(NULL,"The handle is error,so can't find the rod��",sSoftName,MB_OK);
#else 
				MessageBox(NULL,"�����������Ҳ�����Ӧ�˼���",sSoftName,MB_OK);
#endif
				return FALSE;
			}
			CUndoOperObject undo(&Ta);
			CAngleMoveOperObject oddmodified;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				if(iRefPart1Or2==1)
					pLineAngle->desStartOdd.m_hRefPart1=modifyhdlg.m_nHandle;
				else
					pLineAngle->desStartOdd.m_hRefPart2=modifyhdlg.m_nHandle;
				pLineAngle->CalStartOddment();
				pLineAngle->SyncMirProp("oddStart");
			}
			//��������ֵ	
			valueStr.Format("0X%X",pPart->handle);
			pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
			//
			long idProp=CLDSLineAngle::GetPropID("oddStart");
			if(pFirstLineAngle->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			g_pSolidDraw->SetEntSnapStatus(pPart->handle,FALSE);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddStartExtra"))
		{
			CExtraAngleOddSetDlg dlg;
			dlg.m_pAngle = pFirstLineAngle;
			dlg.start0_end1=0;
			if(dlg.DoModal()==IDOK)
			{
				CXhChar100 valueStr;
				if(pFirstLineAngle->GetPropValueStr(pFirstLineAngle,pItem->m_idProp,valueStr)>0)
					pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddEnd"))
		{
			odddlg.m_hRefPart1.Format("0X%X",pFirstLineAngle->desEndOdd.m_hRefPart1);
			odddlg.m_hRefPart2.Format("0X%X",pFirstLineAngle->desEndOdd.m_hRefPart2);
			odddlg.m_fCollideDist = pFirstLineAngle->desEndOdd.m_fCollideDist;
			odddlg.m_iOddCalStyle = pFirstLineAngle->desEndOdd.m_iOddCalStyle;
			if(odddlg.DoModal()==IDOK)
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->desEndOdd.m_hRefPart1=HexStrToLong(odddlg.m_hRefPart1);//��ΪHexStrToLong����Ч����sscanf������ַ���ʱ���޸�����ֵ
					pLineAngle->desEndOdd.m_hRefPart2=HexStrToLong(odddlg.m_hRefPart2);
					pLineAngle->desEndOdd.m_fCollideDist = odddlg.m_fCollideDist;
					pLineAngle->desEndOdd.m_iOddCalStyle = odddlg.m_iOddCalStyle;
					pLineAngle->CalEndOddment();
					if(odddlg.m_iOddCalStyle==2)
					{	//����������ͷ��������޸���ײ��϶��Ӧ���¼����н���Ϣ wht 09-12-29
						CLDSLineAngle *pMainAngle=(CLDSLineAngle*)console.FromPartHandle(pLineAngle->desEndOdd.m_hRefPart1,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
						CLDSLineAngle *pCutAngle2=(CLDSLineAngle*)console.FromPartHandle(pLineAngle->desEndOdd.m_hRefPart2,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
						if(pMainAngle||pCutAngle2)
							pLineAngle->CalCutAngleInfo(pMainAngle,pCutAngle2,false,false,ftoi(odddlg.m_fCollideDist));
					}
				}
				char valueStr[MAX_TEMP_CHAR_100+1]="";
				if(pFirstLineAngle->GetPropValueStr(pItem->m_idProp,valueStr)>0)
					pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
			}
			//������������
			CXhChar100 itemValue;
			CPropTreeItem* pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("oddEnd.m_iOddCalStyle"),NULL);
			if(pFirstLineAngle->GetPropValueStr(pFindItem->m_idProp,itemValue))
				pPropList->SetItemPropValue(pFindItem->m_idProp,itemValue);
			UpdateLineAngleOddProperty(pPropList,pFindItem,FALSE);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddEnd.m_hRefPart1") ||
			pItem->m_idProp==CLDSLineAngle::GetPropID("oddEnd.m_hRefPart2"))
		{
			int iRefPart1Or2=1;
			if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddEnd.m_hRefPart1"))
				iRefPart1Or2=1;
			else
				iRefPart1Or2=2;
			static CModifyPartHandleDlg modifyhdlg;
			if(iRefPart1Or2==1)
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->desEndOdd.m_hRefPart1);
			else
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->desEndOdd.m_hRefPart2);
			modifyhdlg.m_sCaption="�޸Ĳ��ո˼�";
			modifyhdlg.m_sTitle.Format("���ո˼�%d:",iRefPart1Or2);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�/�˼��İ�ť
			cmdStr.Format("��ѡ����ո˼�%d:",iRefPart1Or2);
			modifyhdlg.SetCmdPickPromptStr(cmdStr);
			modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
			if(modifyhdlg.DoModal()!=IDOK)
				return FALSE;
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)//|| !pPart->IsAngle())
			{
				MessageBox(NULL,"�����������Ҳ�����Ӧ������",sSoftName,MB_OK);
				return FALSE;
			}
			CUndoOperObject undo(&Ta);
			CAngleMoveOperObject oddmodified;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				if(iRefPart1Or2==1)
					pLineAngle->desEndOdd.m_hRefPart1=modifyhdlg.m_nHandle;
				else
					pLineAngle->desEndOdd.m_hRefPart2=modifyhdlg.m_nHandle;
				pLineAngle->CalEndOddment();
				pLineAngle->SyncMirProp("oddEnd");
			}
			//��������ֵ
			valueStr.Format("0X%X",pPart->handle);
			pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
			//
			long idProp=CLDSLineAngle::GetPropID("oddEnd");
			if(pFirstLineAngle->GetPropValueStr(idProp,sText)>0)
				pPropList->SetItemPropValue(idProp,sText);
			g_pSolidDraw->SetEntSnapStatus(pPart->handle,FALSE);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddEndExtra"))
		{
			CExtraAngleOddSetDlg dlg;
			dlg.m_pAngle = pFirstLineAngle;
			dlg.start0_end1=1;
			if(dlg.DoModal()==IDOK)
			{
				CXhChar100 valueStr;
				if(pFirstLineAngle->GetPropValueStr(pFirstLineAngle,pItem->m_idProp,valueStr)>0)
					pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("norm_x_wing"))
		{
			CDesignVectorPropDlg design_norm;
			design_norm.m_hCurEditPart=pFirstLineAngle->handle;
			design_norm.m_bSpecAngleWingNormStyle=TRUE;
			design_norm.m_hCurEditPart=pFirstLineAngle->handle;
			if(pFirstLineAngle->des_norm_x.IsSpecNormActual())
			{	//ָ������
				design_norm.m_xVector.norm_style=0;
				design_norm.m_xVector=pFirstLineAngle->des_norm_x.spec_norm;
				design_norm.m_xVector.vector=pFirstLineAngle->get_norm_x_wing();
			}
			else if(pFirstLineAngle->des_norm_x.IsByOtherWingActual())//�ɽǸ���һ֫ȷ������
			{
				design_norm.m_xVector.norm_style=6;
				design_norm.m_xVector.vector=pFirstLineAngle->get_norm_x_wing();
				design_norm.m_xVector.hVicePart=pFirstLineAngle->handle;
			}
			else if(pFirstLineAngle->des_norm_x.hViceJg>0x20)//�ɲ��սǸֽ�����ȷ��
			{
				design_norm.m_xVector.norm_style=2;
				design_norm.m_xVector.hVicePart=pFirstLineAngle->handle;
				design_norm.m_xVector.hCrossPart=pFirstLineAngle->des_norm_x.hViceJg;
				design_norm.m_xVector.nearVector=pFirstLineAngle->des_norm_x.near_norm;
			}
			if(design_norm.DoModal()==IDOK)
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->SetDirectMasterAngle();
					if(design_norm.m_xVector.norm_style==0)
					{	//ָ������
						pLineAngle->des_norm_x.bSpecific=TRUE;
						pLineAngle->des_norm_x.bByOtherWing=FALSE;
						pLineAngle->des_norm_x.spec_norm=design_norm.m_xVector;
						pLineAngle->des_norm_x.hViceJg=0;
						pLineAngle->set_norm_x_wing(design_norm.m_xVector.vector);
					}
					else if(design_norm.m_xVector.norm_style==6)//�ɽǸ���һ֫ȷ������
					{
						pLineAngle->des_norm_x.bSpecific=FALSE;
						pLineAngle->des_norm_x.bByOtherWing=TRUE;
						pLineAngle->des_norm_x.hViceJg=0;
						pLineAngle->cal_x_wing_norm();
					}
					else if(design_norm.m_xVector.norm_style==2)//�ɲ��սǸֽ�����ȷ��
					{
						pLineAngle->des_norm_x.bSpecific=FALSE;
						pLineAngle->des_norm_x.bByOtherWing=FALSE;
						pLineAngle->des_norm_x.hViceJg=design_norm.m_xVector.hCrossPart;
						pLineAngle->des_norm_x.near_norm=design_norm.m_xVector.nearVector;
						pLineAngle->cal_x_wing_norm();
					}
					else
					{
						pLineAngle->des_norm_x.bSpecific=TRUE;
						pLineAngle->des_norm_x.spec_norm=design_norm.m_xVector;
						pLineAngle->des_norm_x.hViceJg=0;
					}
				}
				if(pFirstLineAngle->GetPropValueStr(pItem->m_idProp,sText)>0)
					pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
				if(pFirstLineAngle->GetPropValueStr(CLDSLineAngle::GetPropID("norm_y_wing.hViceJg"),sText)>0)
					pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("norm_x_wing.hViceJg"),CString(sText));
				sText.Printf("%f",pFirstLineAngle->get_norm_x_wing().x);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("norm_x_wing.x"),CString(sText));
				sText.Printf("%f",pFirstLineAngle->get_norm_x_wing().y);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("norm_x_wing.y"),CString(sText));
				sText.Printf("%f",pFirstLineAngle->get_norm_x_wing().z);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("norm_x_wing.z"),CString(sText));
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("norm_x_wing.hViceJg"))
		{
			static CModifyPartHandleDlg modifyhdlg;
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->des_norm_x.hViceJg);
			modifyhdlg.m_sCaption="�޸Ľ�����ո˼�";
			modifyhdlg.m_sTitle="���ո˼�";
			modifyhdlg.m_bEnableMir=FALSE;
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�/�˼��İ�ť
			modifyhdlg.SetCmdPickPromptStr("��ѡ�񽻲���ո˼�");
			modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
			g_pSolidSet->SetOperType(OPER_OTHER);
			if(modifyhdlg.DoModal()!=IDOK)
				return FALSE;
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
				return FALSE;
			g_pSolidDraw->SetEntSnapStatus(modifyhdlg.m_nHandle,FALSE);
			g_pSolidDraw->SetEntSnapStatus(pFirstLineAngle->handle,TRUE);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->SetDirectMasterAngle();
				pLineAngle->des_norm_x.hViceJg=modifyhdlg.m_nHandle;
				pLineAngle->des_norm_x.bByOtherWing=FALSE;
				pLineAngle->des_norm_x.bSpecific=FALSE;
				pLineAngle->cal_x_wing_norm();
			}
			//��������ֵ
			if(pPart)
			{
				if(pFirstLineAngle->GetPropValueStr(pItem->m_idProp,sText)>0)
					pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("norm_x_wing.hViceJg"),CString(sText));
				sText.Printf("��0X%X����",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("norm_x_wing"),CString(sText));
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("norm_y_wing"))
		{
			CDesignVectorPropDlg design_norm;
			design_norm.m_hCurEditPart=pFirstLineAngle->handle;
			design_norm.m_bSpecAngleWingNormStyle=TRUE;
			design_norm.m_hCurEditPart=pFirstLineAngle->handle;
			if(pFirstLineAngle->des_norm_y.IsSpecNormActual())
			{	//ָ������
				design_norm.m_xVector.norm_style=0;
				design_norm.m_xVector=pFirstLineAngle->des_norm_y.spec_norm;
				design_norm.m_xVector.vector=pFirstLineAngle->get_norm_y_wing();
			}
			else if(pFirstLineAngle->des_norm_y.IsByOtherWingActual())//�ɽǸ���һ֫ȷ������
			{
				design_norm.m_xVector.norm_style=6;
				design_norm.m_xVector.hVicePart=pFirstLineAngle->handle;
				design_norm.m_xVector.vector=pFirstLineAngle->get_norm_y_wing();
			}
			else if(pFirstLineAngle->des_norm_y.hViceJg>0x20)//�ɲ��սǸֽ�����ȷ��
			{
				design_norm.m_xVector.norm_style=2;
				design_norm.m_xVector.hVicePart=pFirstLineAngle->handle;
				design_norm.m_xVector.hCrossPart=pFirstLineAngle->des_norm_y.hViceJg;
				design_norm.m_xVector.nearVector=pFirstLineAngle->des_norm_y.near_norm;
			}
			if(design_norm.DoModal()==IDOK)
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->SetDirectMasterAngle();
					if(design_norm.m_xVector.norm_style==0)
					{	//ָ������
						pLineAngle->des_norm_y.bSpecific=TRUE;
						pLineAngle->des_norm_y.bByOtherWing=FALSE;
						pLineAngle->des_norm_y.spec_norm=design_norm.m_xVector;
						pLineAngle->des_norm_y.hViceJg=0;
						pLineAngle->set_norm_y_wing(design_norm.m_xVector.vector);
					}
					else if(design_norm.m_xVector.norm_style==6)//�ɽǸ���һ֫ȷ������
					{
						pLineAngle->des_norm_y.bSpecific=FALSE;
						pLineAngle->des_norm_y.bByOtherWing=TRUE;
						pLineAngle->des_norm_y.hViceJg=0;
						pLineAngle->cal_y_wing_norm();
					}
					else if(design_norm.m_xVector.norm_style==2)//�ɲ��սǸֽ�����ȷ��
					{
						pLineAngle->des_norm_y.bSpecific=FALSE;
						pLineAngle->des_norm_y.bByOtherWing=FALSE;
						pLineAngle->des_norm_y.hViceJg=design_norm.m_xVector.hCrossPart;
						pLineAngle->des_norm_y.near_norm=design_norm.m_xVector.nearVector;
						pLineAngle->cal_y_wing_norm();
					}
					else
					{
						pLineAngle->des_norm_y.bSpecific=TRUE;
						pLineAngle->des_norm_y.spec_norm=design_norm.m_xVector;
						pLineAngle->des_norm_y.hViceJg=0;
					}
				}
				if(pFirstLineAngle->GetPropValueStr(pItem->m_idProp,sText)>0)
					pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
				if(pFirstLineAngle->GetPropValueStr(CLDSLineAngle::GetPropID("norm_y_wing.hViceJg"),sText)>0)
					pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("norm_y_wing.hViceJg"),CString(sText));
				sText.Printf("%f",pFirstLineAngle->get_norm_y_wing().x);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("norm_y_wing.x"),CString(sText));
				sText.Printf("%f",pFirstLineAngle->get_norm_y_wing().y);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("norm_y_wing.y"),CString(sText));
				sText.Printf("%f",pFirstLineAngle->get_norm_y_wing().z);
				SimplifiedNumString(sText);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("norm_y_wing.z"),CString(sText));
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("norm_y_wing.hViceJg"))
		{
			static CModifyPartHandleDlg modifyhdlg;
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->des_norm_y.hViceJg);
			modifyhdlg.m_sCaption="�޸Ľ�����ո˼�";
			modifyhdlg.m_sTitle="���ո˼�";
			modifyhdlg.m_bEnableMir=FALSE;
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�/�˼��İ�ť
			modifyhdlg.SetCmdPickPromptStr("��ѡ�񽻲���ո˼�");
			modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
			g_pSolidSet->SetOperType(OPER_OTHER);
			if(modifyhdlg.DoModal()!=IDOK)
				return FALSE;
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
				return FALSE;
			g_pSolidDraw->SetEntSnapStatus(modifyhdlg.m_nHandle,FALSE);
 			g_pSolidDraw->SetEntSnapStatus(pFirstLineAngle->handle,TRUE);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->SetDirectMasterAngle();
				pLineAngle->des_norm_y.hViceJg=modifyhdlg.m_nHandle;
				pLineAngle->des_norm_y.bByOtherWing=FALSE;
				pLineAngle->des_norm_y.bSpecific=FALSE;
				pLineAngle->cal_y_wing_norm();
			}
			//��������ֵ
			if(pPart)
			{
				if(pFirstLineAngle->GetPropValueStr(pItem->m_idProp,sText)>0)
					pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("norm_y_wing.hViceJg"),CString(sText));
				sText.Printf("��0X%X����",modifyhdlg.m_nHandle);
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("norm_y_wing"),CString(sText));
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("work_wing.hViceJg"))
		{
			static CModifyPartHandleDlg modifyhdlg;
			if(pFirstLineAngle->GetWorkWingX0Y1()==0)
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->des_norm_x.hViceJg);
			else if(pFirstLineAngle->GetWorkWingX0Y1()==1)
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->des_norm_y.hViceJg);
			modifyhdlg.m_sCaption="�޸Ľ�����ո˼�";
			modifyhdlg.m_sTitle="���ո˼�";
			modifyhdlg.m_bEnableMir=FALSE;
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�/�˼��İ�ť
			modifyhdlg.SetCmdPickPromptStr("��ѡ�񽻲���ո˼�");
			modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
			g_pSolidSet->SetOperType(OPER_OTHER);
			if(modifyhdlg.DoModal()!=IDOK)
				return FALSE;
			CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
			if(pPart==NULL)
				return FALSE;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				if(pLineAngle->GetWorkWingX0Y1()==0)
					pLineAngle->des_norm_x.hViceJg=modifyhdlg.m_nHandle;
				else if(pLineAngle->GetWorkWingX0Y1()==1)
					pLineAngle->des_norm_y.hViceJg=modifyhdlg.m_nHandle;
				pLineAngle->SetDirectMasterAngle();
				pLineAngle->cal_x_wing_norm();
				pLineAngle->cal_y_wing_norm();
			}
			//��������ֵ	
			valueStr.Format("0X%X",pPart->handle);
			pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("start_wing_plane")
			||pItem->m_idProp==CLDSLineAngle::GetPropID("end_wing_plane"))
		{
			CPartWorkPlaneDlg wingPlaneDlg;
			if(pItem->m_idProp==CLDSLineAngle::GetPropID("start_wing_plane"))
				wingPlaneDlg.m_bStart=TRUE;
			else
				wingPlaneDlg.m_bStart=FALSE;
			wingPlaneDlg.m_pDatumPart=pFirstLineAngle;
			wingPlaneDlg.DoModal();
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("teGTable")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("wingXG")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("wingYG"))
		{
			dlg.m_nXG  = pFirstLineAngle->xWingXZhunJu.g;
			dlg.m_nXG1 = pFirstLineAngle->xWingXZhunJu.g1;
			dlg.m_nXG2 = pFirstLineAngle->xWingXZhunJu.g2;
			dlg.m_nXG3 = pFirstLineAngle->xWingXZhunJu.g3;
			dlg.m_nYG  = pFirstLineAngle->xWingYZhunJu.g;
			dlg.m_nYG1 = pFirstLineAngle->xWingYZhunJu.g1;
			dlg.m_nYG2 = pFirstLineAngle->xWingYZhunJu.g2;
			dlg.m_nYG3 = pFirstLineAngle->xWingYZhunJu.g3;
			if(dlg.DoModal()==IDOK)
			{
				for(CLDSLineAngle *pAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pAngle;pAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pAngle->xWingXZhunJu.g  = dlg.m_nXG ;
					pAngle->xWingXZhunJu.g1 = dlg.m_nXG1;
					pAngle->xWingXZhunJu.g2 = dlg.m_nXG2;
					pAngle->xWingXZhunJu.g3 = dlg.m_nXG3;
					pAngle->xWingYZhunJu.g  = dlg.m_nYG ;
					pAngle->xWingYZhunJu.g1 = dlg.m_nYG1;
					pAngle->xWingYZhunJu.g2 = dlg.m_nYG2;
					pAngle->xWingYZhunJu.g3 = dlg.m_nYG3;
					pAngle->SetModified();
					JGZJ jgzj;
					getjgzj(jgzj,pAngle->GetWidth());
					if( jgzj.g==pAngle->xWingXZhunJu.g&&jgzj.g1==pAngle->xWingXZhunJu.g1&&
						jgzj.g2==pAngle->xWingXZhunJu.g2&&jgzj.g3==pAngle->xWingXZhunJu.g3&&
						jgzj.g==pAngle->xWingYZhunJu.g&&jgzj.g1==pAngle->xWingYZhunJu.g1&&
						jgzj.g2==pAngle->xWingYZhunJu.g2&&jgzj.g3==pAngle->xWingYZhunJu.g3)
						pAngle->m_bEnableTeG=FALSE;
					else
						pAngle->m_bEnableTeG=TRUE;
				}
				long idPropWingXG=CLDSLineAngle::GetPropID("wingXG");
				long idPropWingYG=CLDSLineAngle::GetPropID("wingYG");
				if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,idPropWingXG,sText)>0)
					pPropList->SetItemPropValue(idPropWingXG,CString(sText));
				if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,idPropWingYG,sText)>0)
					pPropList->SetItemPropValue(idPropWingYG,CString(sText));
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("startCutWing"))
		{
			cutdlg.m_iCutXWingAngle = pFirstLineAngle->cut_wing[0];
			if(cutdlg.m_iCutXWingAngle==1||cutdlg.m_iCutXWingAngle==2)
			{
				cutdlg.m_fCutWingWide = pFirstLineAngle->cut_wing_para[0][1];
				cutdlg.m_fCutWingLen  = pFirstLineAngle->cut_wing_para[0][0];
				cutdlg.m_fCutWingLen2 = pFirstLineAngle->cut_wing_para[0][2];
			}
			
			cutdlg.m_fCutAngleXLen		 = pFirstLineAngle->cut_angle[0][0];
			cutdlg.m_fCutAngleXWide	 = pFirstLineAngle->cut_angle[0][1];
			cutdlg.m_fCutAngleYLen		 = pFirstLineAngle->cut_angle[1][0];
			cutdlg.m_fCutAngleYWide	 = pFirstLineAngle->cut_angle[1][1];
			cutdlg.m_bUserSpecCutInfo = pFirstLineAngle->m_bUserSpecStartCutAngle;	//�û�ָ���н���Ϣ wht 11-07-04
			if(cutdlg.DoModal()==IDOK)
			{
				CModifiedPartsOperObject updator;
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->m_bUserSpecStartCutAngle = cutdlg.m_bUserSpecCutInfo;
					pLineAngle->cut_wing[0] = cutdlg.m_iCutXWingAngle;
					if(fabs(cutdlg.m_fCutWingWide)<EPS&&fabs(cutdlg.m_fCutWingLen)<EPS&&fabs(cutdlg.m_fCutWingLen2)<EPS)
						pLineAngle->cut_wing[0]=0;	//��֫���ݾ�Ϊ0ʱ��������֫���� wht 11-01-19
					if(cutdlg.m_iCutXWingAngle==1||cutdlg.m_iCutXWingAngle==2)
					{
						pLineAngle->cut_wing_para[0][1] = cutdlg.m_fCutWingWide;
						pLineAngle->cut_wing_para[0][0] = cutdlg.m_fCutWingLen ;
						pLineAngle->cut_wing_para[0][2] = cutdlg.m_fCutWingLen2;
					}
					pLineAngle->cut_angle[0][0] = cutdlg.m_fCutAngleXLen;
					pLineAngle->cut_angle[0][1] = cutdlg.m_fCutAngleXWide;
					pLineAngle->cut_angle[1][0] = cutdlg.m_fCutAngleYLen;
					pLineAngle->cut_angle[1][1] = cutdlg.m_fCutAngleYWide;
					pLineAngle->SyncMirProp("oddStartOnSameLabel",0,LABEL_PROP::ANGLE_CUT_START);
					pLineAngle->SetModified();
				}
				//����ʼ���н���֫��ʾ wht 11-01-11
				if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,pItem->m_idProp,sText)>0)
					pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
				long item_id=CLDSLineAngle::GetPropID("startCutWingX");
				if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>0)
					pPropList->SetItemPropValue(item_id,CString(sText));
				if((pFindItem=pPropList->FindItemByPropId(item_id,NULL))!=NULL)
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp=!pFirstLineAngle->m_bUserSpecStartCutAngle;
				item_id=CLDSLineAngle::GetPropID("startCutWingY");
				if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>0)
					pPropList->SetItemPropValue(item_id,CString(sText));
				if((pFindItem=pPropList->FindItemByPropId(item_id,NULL))!=NULL)
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp=!pFirstLineAngle->m_bUserSpecStartCutAngle;
				//�����н���֫��ʾ����
				/*
				long item_id=CLDSLineAngle::GetPropID("start_cut_angle_type");
				if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>0)
				{
					pPropList->SetItemPropValue(item_id,CString(sText));
					pFindItem=pPropList->FindItemByPropId(item_id,NULL);
					if(pFindItem)
						ModifyLineAngleProperty(pPropList,pFindItem,CString(sText));
				}
				//�����ֶ�ָ������
				long idProp = CLDSLineAngle::GetPropID("m_bUserSpecStartCutAngle");
				if(pFirstLineAngle->GetPropValueStr(idProp,sText)>0)
					pPropList->SetItemPropValue(idProp,CString(sText));
				//��������ֻ������ wht 11-07-04
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("start_cut_angle_type"),!pFirstLineAngle->m_bUserSpecStartCutAngle);
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[0][0]"),!pFirstLineAngle->m_bUserSpecStartCutAngle);
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[0][1]"),!pFirstLineAngle->m_bUserSpecStartCutAngle);
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[1][0]"),!pFirstLineAngle->m_bUserSpecStartCutAngle);
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[1][1]"),!pFirstLineAngle->m_bUserSpecStartCutAngle);
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_wing_para[0][0]"),!pFirstLineAngle->m_bUserSpecStartCutAngle);
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_wing_para[0][1]"),!pFirstLineAngle->m_bUserSpecStartCutAngle);
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_wing_para[0][2]"),!pFirstLineAngle->m_bUserSpecStartCutAngle);
				*/
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("endCutWing"))
		{
			cutdlg.m_iCutXWingAngle = pFirstLineAngle->cut_wing[1];
			if(cutdlg.m_iCutXWingAngle==1||cutdlg.m_iCutXWingAngle==2)
			{
				cutdlg.m_fCutWingWide = pFirstLineAngle->cut_wing_para[1][1];
				cutdlg.m_fCutWingLen  = pFirstLineAngle->cut_wing_para[1][0];
				cutdlg.m_fCutWingLen2 = pFirstLineAngle->cut_wing_para[1][2];
			}
			
			cutdlg.m_fCutAngleXLen		 = pFirstLineAngle->cut_angle[2][0];
			cutdlg.m_fCutAngleXWide	 = pFirstLineAngle->cut_angle[2][1];
			cutdlg.m_fCutAngleYLen		 = pFirstLineAngle->cut_angle[3][0];
			cutdlg.m_fCutAngleYWide	 = pFirstLineAngle->cut_angle[3][1];
			cutdlg.m_bUserSpecCutInfo = pFirstLineAngle->m_bUserSpecEndCutAngle;	//�û�ָ���н���Ϣ wht 11-07-04
			if(cutdlg.DoModal()==IDOK)
			{
				CModifiedPartsOperObject updator;
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->m_bUserSpecEndCutAngle = cutdlg.m_bUserSpecCutInfo;
					pLineAngle->cut_wing[1] = cutdlg.m_iCutXWingAngle;
					if(fabs(cutdlg.m_fCutWingWide)<EPS&&fabs(cutdlg.m_fCutWingLen)<EPS&&fabs(cutdlg.m_fCutWingLen2)<EPS)
						pLineAngle->cut_wing[1]=0;	//��֫���ݾ�Ϊ0ʱ��������֫���� wht 11-01-19
					if(cutdlg.m_iCutXWingAngle==1||cutdlg.m_iCutXWingAngle==2)
					{
						pLineAngle->cut_wing_para[1][1] = cutdlg.m_fCutWingWide;
						pLineAngle->cut_wing_para[1][0] = cutdlg.m_fCutWingLen ;
						pLineAngle->cut_wing_para[1][2] = cutdlg.m_fCutWingLen2;
					}
					pLineAngle->cut_angle[2][0] = cutdlg.m_fCutAngleXLen;
					pLineAngle->cut_angle[2][1] = cutdlg.m_fCutAngleXWide;
					pLineAngle->cut_angle[3][0] = cutdlg.m_fCutAngleYLen;
					pLineAngle->cut_angle[3][1] = cutdlg.m_fCutAngleYWide;
					pLineAngle->SyncMirProp("oddEndOnSameLabel",0,LABEL_PROP::ANGLE_CUT_END);
					pLineAngle->SetModified();
				}
				//����ʼ���н���֫��ʾ wht 11-01-11
				if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,pItem->m_idProp,sText)>0)
					pPropList->SetItemPropValue(pItem->m_idProp,CString(sText));
				long item_id=CLDSLineAngle::GetPropID("endCutWingX");
				if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>0)
					pPropList->SetItemPropValue(item_id,CString(sText));
				if((pFindItem=pPropList->FindItemByPropId(item_id,NULL))!=NULL)
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp=!pFirstLineAngle->m_bUserSpecEndCutAngle;
				item_id=CLDSLineAngle::GetPropID("endCutWingY");
				if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>0)
					pPropList->SetItemPropValue(item_id,CString(sText));
				if((pFindItem=pPropList->FindItemByPropId(item_id,NULL))!=NULL)
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp=!pFirstLineAngle->m_bUserSpecEndCutAngle;
				//�����н���֫��ʾ����
				/*
				long item_id=CLDSLineAngle::GetPropID("end_cut_angle_type");
				if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>0)
				{
					pPropList->SetItemPropValue(item_id,CString(sText));
					pFindItem=pPropList->FindItemByPropId(item_id,NULL);
					if(pFindItem)
						ModifyLineAngleProperty(pPropList,pFindItem,CString(sText));
				}
				//�����ֶ�ָ������
				long idProp = CLDSLineAngle::GetPropID("m_bUserSpecEndCutAngle");
				if(pFirstLineAngle->GetPropValueStr(idProp,sText)>0)
					pPropList->SetItemPropValue(idProp,CString(sText));
				//��������ֻ������ wht 11-07-04
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("end_cut_angle_type"),!pFirstLineAngle->m_bUserSpecEndCutAngle);
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[2][0]"),!pFirstLineAngle->m_bUserSpecEndCutAngle);
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[2][1]"),!pFirstLineAngle->m_bUserSpecEndCutAngle);
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[3][0]"),!pFirstLineAngle->m_bUserSpecEndCutAngle);
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[3][1]"),!pFirstLineAngle->m_bUserSpecEndCutAngle);
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_wing_para[1][0]"),!pFirstLineAngle->m_bUserSpecEndCutAngle);
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_wing_para[1][1]"),!pFirstLineAngle->m_bUserSpecEndCutAngle);
				pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_wing_para[1][2]"),!pFirstLineAngle->m_bUserSpecEndCutAngle);
				*/
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cfgword"))
		{
			cfgdlg.cfgword = pFirstLineAngle->cfgword; 
			if(pFirstLineAngle->IsBodyObj())
				cfgdlg.cfg_style=CFG_BODY_NO;
			else
				cfgdlg.cfg_style=CFG_LEG_NO;
			if(cfgdlg.DoModal()==IDOK)
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->cfgword = cfgdlg.cfgword;		//�ڴ˴����µ�һ���ڵ������Ϻ�
					pLineAngle->SetModified(TRUE,FALSE);
					pLineAngle->SyncMirProp("cfgword");
				}
			}
			if(pFirstLineAngle->GetPropValueStr(pItem->m_idProp,sText))	//������ĺ�
				pItem->m_lpNodeInfo->m_strPropValue=sText;
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("startAttachBolts")||pItem->m_idProp==CLDSLineAngle::GetPropID("midAttachBolts")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("endAttachBolts"))
		{
			int start1_end2_mid0 = 0;
			if(pItem->m_idProp==CLDSLineAngle::GetPropID("startAttachBolts"))
				start1_end2_mid0 = 1;
			else if(pItem->m_idProp==CLDSLineAngle::GetPropID("endAttachBolts"))
				start1_end2_mid0 = 2;
			else if(pItem->m_idProp==CLDSLineAngle::GetPropID("midAttachBolts"))
				start1_end2_mid0 = 0;
			g_pLsRefDlg->InitLsRefDlg(pFirstLineAngle,start1_end2_mid0);
		}
#if defined(__TSA_)||defined(__LDS_)
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_uStatMatNo"))
		{
			UINT iNo=Ta.GetNewStatNo();
			valueStr.Format("%d",iNo);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->m_uStatMatNo = iNo;
			pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.pRefPole"))
		{
			static CModifyPartHandleDlg modifyhdlg;
			int update_type=0;	//0.�ڵ㸸�˼� 1.�ڵ���������1 2.�ڵ���������2
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Reference Rod";
			modifyhdlg.m_sTitle="Reference Rod:";
#else
			modifyhdlg.m_sCaption="�޸Ĳ��ո˼�";
			modifyhdlg.m_sTitle="���ո˼�:";
#endif
			if(pFirstLineAngle->CalLenCoef.hRefPole>0x20)
			{
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->CalLenCoef.hRefPole);
				g_pSolidDraw->SetEntSnapStatus(pFirstLineAngle->CalLenCoef.hRefPole,TRUE);
			}
			else
				modifyhdlg.m_sHandle="0X0";
			modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�/�˼��İ�ť
			modifyhdlg.m_bEnableMir=FALSE;		//������Գ�
			BOOL bRetOk=FALSE;
			if(modifyhdlg.DoModal()==IDOK)
			{
				CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
				if(pPart==NULL||(pPart&&!pPart->IsLinePart()&&!pPart->IsArcPart()))
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part��","LDS",MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ�˼���","LDS",MB_OK);
#endif
				else
				{
					g_pSolidDraw->SetEntSnapStatus(pPart->handle,FALSE);
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("CalLenCoef.pRefPole"),valueStr);
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
						pLineAngle->CalLenCoef.hRefPole=modifyhdlg.m_nHandle;
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.pHorizPole"))
		{
			static CModifyPartHandleDlg modifyhdlg;
			int update_type=0;	//0.�ڵ㸸�˼� 1.�ڵ���������1 2.�ڵ���������2
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Horizontal Join Rod";
			modifyhdlg.m_sTitle="Horizontal Join Rod:";
#else
			modifyhdlg.m_sCaption="�޸ĺ����˼�";
			modifyhdlg.m_sTitle="�����˼�:";
#endif
			if(pFirstLineAngle->CalLenCoef.hHorizPole>0x20)
			{
				modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->CalLenCoef.hHorizPole);
				g_pSolidDraw->SetEntSnapStatus(pFirstLineAngle->CalLenCoef.hHorizPole,TRUE);
			}
			else
				modifyhdlg.m_sHandle="0X0";
			modifyhdlg.SetPickObjTypeId(CLS_LINEPART);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�/�˼��İ�ť
			if(modifyhdlg.DoModal()==IDOK)
			{
				CLDSPart *pPart=console.FromPartHandle(modifyhdlg.m_nHandle);
				if(pPart==NULL||(pPart&&!pPart->IsLinePart()&&!pPart->IsArcPart()))
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part��","LDS",MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ�˼���","LDS",MB_OK);
#endif
				else
				{
					g_pSolidDraw->SetEntSnapStatus(pPart->handle,FALSE);
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("CalLenCoef.pHorizPole"),valueStr);
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
						pLineAngle->CalLenCoef.hHorizPole=modifyhdlg.m_nHandle;
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.minR.hStartNode"))
		{
			static CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Start Node for Minimum Axis Calculates Length ";
			modifyhdlg.m_sTitle="Start Node:";
#else
			modifyhdlg.m_sCaption="�޸���С����㳤����ʼ�ڵ�";
			modifyhdlg.m_sTitle="��ʼ�ڵ�:";
#endif
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->CalLenCoef.minR.hStartNode);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select the start node for minimum axis calculates length:");
#else
			modifyhdlg.SetCmdPickPromptStr("��ѡ����С����㳤����ʼ�ڵ�:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part��",sSoftName,MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("CalLenCoef.minR.hStartNode"),valueStr);
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
						pLineAngle->CalLenCoef.minR.hStartNode=pNode->handle;
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.minR.hEndNode"))
		{
			static CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify End Node for Minimum Axis Calculates Length ";
			modifyhdlg.m_sTitle="End Node:";
#else
			modifyhdlg.m_sCaption="�޸���С����㳤����ֹ�ڵ�";
			modifyhdlg.m_sTitle="��ֹ�ڵ�:";
#endif
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->CalLenCoef.minR.hEndNode);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select the end node for minimum axis calculates length:");
#else
			modifyhdlg.SetCmdPickPromptStr("��ѡ����С����㳤����ֹ�ڵ�:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part��",sSoftName,MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("CalLenCoef.minR.hEndNode"),valueStr);
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
						pLineAngle->CalLenCoef.minR.hEndNode=pNode->handle;
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.paraR.hStartNode"))
		{
			static CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify Start Node for Parallel Axis Calculates Length";
			modifyhdlg.m_sTitle="Start Node:";
#else
			modifyhdlg.m_sCaption="�޸�ƽ������㳤����ʼ�ڵ�";
			modifyhdlg.m_sTitle="��ʼ�ڵ�:";
#endif
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->CalLenCoef.paraR.hStartNode);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select the end node for parallel axis calculates length:");
#else
			modifyhdlg.SetCmdPickPromptStr("��ѡ��ƽ������㳤����ʼ�ڵ�:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part��",sSoftName,MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("CalLenCoef.paraR.hStartNode"),valueStr);
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
						pLineAngle->CalLenCoef.paraR.hStartNode=pNode->handle;
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.paraR.hEndNode"))
		{
			static CModifyPartHandleDlg modifyhdlg;
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.m_sCaption="Modify End Node for Parallel Axis Calculates Length";
			modifyhdlg.m_sTitle="End Node:";
#else
			modifyhdlg.m_sCaption="�޸�ƽ������㳤����ֹ�ڵ�";
			modifyhdlg.m_sTitle="��ֹ�ڵ�:";
#endif
			modifyhdlg.m_sHandle.Format("0X%X",pFirstLineAngle->CalLenCoef.paraR.hEndNode);
			modifyhdlg.m_bDispSelectBtn=TRUE;//��ʾѡ��ڵ�İ�ť
#ifdef AFX_TARG_ENU_ENGLISH
			modifyhdlg.SetCmdPickPromptStr("Please select the end node for parallel axis calculates length:");
#else
			modifyhdlg.SetCmdPickPromptStr("��ѡ��ƽ������㳤����ֹ�ڵ�:");
#endif
			modifyhdlg.SetPickObjTypeId(CLS_NODE);
			if(IDOK==modifyhdlg.DoModal())
			{
				CLDSNode *pNode=console.FromNodeHandle(modifyhdlg.m_nHandle);
				if(pNode==NULL)
#ifdef AFX_TARG_ENU_ENGLISH
					MessageBox(NULL,"Inputted handle error,can't find related part��",sSoftName,MB_OK);
#else
					MessageBox(NULL,"�����������Ҳ�����Ӧ�ڵ㣡",sSoftName,MB_OK);
#endif
				else
				{
					valueStr.Format("0X%X",modifyhdlg.m_nHandle);
					pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("CalLenCoef.paraR.hEndNode"),valueStr);
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
						pLineAngle->CalLenCoef.paraR.hEndNode=pNode->handle;
				}
			}
		}
#endif
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("relativeObjs"))
		{	
			CRelativeObjectDlg relativeDlg;
			relativeDlg.m_pWorkObj = pFirstLineAngle;
			if(relativeDlg.DoModal()==IDOK)
			{
				CXhChar200 sText;
				pFirstLineAngle->GetPropValueStr(pItem->m_idProp,sText);
				pPropList->SetItemPropValue(pItem->m_idProp,sText);
			}
		}	
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("shadowInfo"))
		{
			CDesignShadowDlg shadowdlg;
			shadowdlg.SetShadowObj((CLDSObject*)pFirstLineAngle);
			shadowdlg.DoModal();
		}
		else if(pItem->m_idProp>=CLDSLineAngle::GetPropID("OpenSlot1")&&pItem->m_idProp>=CLDSLineAngle::GetPropID("OpenSlot10"))
		{
			int index=pItem->m_idProp-CLDSLineAngle::GetPropID("OpenSlot1");
			CLDSLineAngle::OPENING_CUT* pOpenCut=pFirstLineAngle->xarrOpenCuts.GetAt(index);
			if(pOpenCut==NULL)
			{
				MessageBox(NULL,"��������",sSoftName,MB_OK);
				return FALSE;
			}
			COpenSlotDlg dlg;
			dlg.m_pCurJg=pFirstLineAngle;
			dlg.m_iCutWing=(pOpenCut->dfLateralBtmXorY<0)?0:1;
			dlg.m_fOffLen=pOpenCut->dfLenPosFrom;
			dlg.m_fSlotW=pOpenCut->dfLenPosTo-pOpenCut->dfLenPosFrom;
			dlg.m_fSlotH=pFirstLineAngle->size_wide-fabs(pOpenCut->dfLateralBtmXorY);
			dlg.m_fDeltaS=pOpenCut->dfTipDeltaS;
			dlg.m_fDeltaE=pOpenCut->dfTipDeltaE;
			if(dlg.DoModal()==IDOK)
			{
				pOpenCut->dfLateralBtmXorY=pFirstLineAngle->size_wide-dlg.m_fSlotH;
				if(dlg.m_iCutWing==0)
					pOpenCut->dfLateralBtmXorY*=-1;
				pOpenCut->dfLenPosFrom=dlg.m_fOffLen;
				pOpenCut->dfLenPosTo=dlg.m_fOffLen+dlg.m_fSlotW;
				pOpenCut->dfTipDeltaS=dlg.m_fDeltaS;
				pOpenCut->dfTipDeltaE=dlg.m_fDeltaE;
			}
		}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(pFirstLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle *pGroupLineAngle=(CLDSGroupLineAngle*)pFirstLineAngle;
			if(pItem->m_idProp==CLDSGroupLineAngle::GetPropID("group_style"))
			{
				/*CGroupJgStyleDlg group_jg_style_dlg;
				group_jg_style_dlg.m_iJgGroupStyle=pGroupLineAngle->group_style;
				group_jg_style_dlg.m_fFillThick=pGroupLineAngle->jg_space;
				
				if(group_jg_style_dlg.DoModal()==IDOK)
				{
					pGroupLineAngle->group_style=group_jg_style_dlg.m_iJgGroupStyle;
					pGroupLineAngle->jg_space=group_jg_style_dlg.m_fFillThick;
				}
				if(pGroupLineAngle->GetPropValueStr(CLDSGroupLineAngle::GetPropID("group_style"),sText)>0)
					pPropList->SetItemPropValue(CLDSGroupLineAngle::GetPropID("group_style"),CString(sText));
				if(pGroupLineAngle->GetPropValueStr(CLDSGroupLineAngle::GetPropID("jg_space"),sText)>0)
					pPropList->SetItemPropValue(CLDSGroupLineAngle::GetPropID("jg_space"),CString(sText));*/
			}
			else
				return FALSE;
		}
#endif
		else
			return FALSE;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->SetModified();
			pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
			g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
		}
		g_pSolidDraw->Draw();
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
	return TRUE;
}

static CString GetLineHandleStr()
{
	CString sLineHandleArr,sValue;
	for(CLDSLine* pLine=Ta.Line.GetFirst();pLine;pLine=Ta.Line.GetNext())
	{
		sValue.Format("0X%X|",pLine->handle);
		sLineHandleArr+=sValue;
	}
#ifdef AFX_TARG_ENU_ENGLISH
	sLineHandleArr+="<New...>|";
	sLineHandleArr+="<Local Define...>";
#else
	sLineHandleArr+="<�½�...>|";
	sLineHandleArr+="<�ֲ�����...>";
#endif
	return sLineHandleArr;
}
BOOL ModifyLineAngleProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr);
void UpdateLineAngle_DesPos_DatumLine_TreeItem(CPropertyList* pPropList,CPropTreeItem* pParentItem,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem,*pSonPropItem;
	CItemInfo *lpInfo=NULL;
	CXhChar100 sText;
	pPropList->DeleteAllSonItems(pParentItem);
	if(bStart)
	{
		if(pFirstLineAngle->desStartPos.hDatumLine>0X20)
		{	//��׼����Ϣ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			//lpInfo->m_cmbType=CBS_DROPDOWNLIST;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.None|1.X Symmetry|2.Y Symmetry|3.Z Symmetry";
			lpInfo->m_strPropName = "Symmetry Type";
			lpInfo->m_strPropHelp = "Symmetry Type based on specified datum line";
#else
			lpInfo->m_cmbItems="0.��|1.X��Գ�|2.Y��Գ�|3.Z��Գ�";
			lpInfo->m_strPropName = "���նԳ�����";
			lpInfo->m_strPropHelp = "����ָ����׼�ߵĲ��նԳ�����";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("desStartPos.cMirTransType");
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Line Start";
			lpInfo->m_strPropHelp = "Coordinate of datum line's start point";
#else
			lpInfo->m_strPropName = "��׼���������";
			lpInfo->m_strPropHelp = "��׼���������";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("desStartPos.datumLine.startPos");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//���X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X��";
			lpInfo->m_strPropHelp = "X coordinate of datum line's start point";
#else
			lpInfo->m_strPropName = "X���꣺";
			lpInfo->m_strPropHelp = "��׼�����X����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("desStartPos.datumLine.startPos.x");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//���Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y��";
			lpInfo->m_strPropHelp = "Y coordinate of datum line's start point";
#else
			lpInfo->m_strPropName = "Y���꣺";
			lpInfo->m_strPropHelp = "��׼�����Y����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("desStartPos.datumLine.startPos.y");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//���Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z��";
			lpInfo->m_strPropHelp = "Z coordinate of datum line's start point";
#else
			lpInfo->m_strPropName = "Z���꣺";
			lpInfo->m_strPropHelp = "��׼�����Z����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("desStartPos.datumLine.startPos.z");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//�յ�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Line End";
			lpInfo->m_strPropHelp = "Coordinate of datum line's end point";
#else
			lpInfo->m_strPropName = "��׼���յ�����";
			lpInfo->m_strPropHelp = "��׼���յ�����";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("desStartPos.datumLine.endPos");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//�յ�X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X��";
			lpInfo->m_strPropHelp = "X coordinate of datum line's end point";
#else
			lpInfo->m_strPropName = "X���꣺";
			lpInfo->m_strPropHelp = "��׼���յ�X����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("desStartPos.datumLine.endPos.x");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//�յ�Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y��";
			lpInfo->m_strPropHelp = "Y coordinate of datum line's end point";
#else
			lpInfo->m_strPropName = "Y���꣺";
			lpInfo->m_strPropHelp = "��׼���յ�Y����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("desStartPos.datumLine.endPos.y");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//�յ�Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z��";
			lpInfo->m_strPropHelp = "Z coordinate of datum line's end point";
#else
			lpInfo->m_strPropName = "Z���꣺";
			lpInfo->m_strPropHelp = "��׼���յ�Z����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("desStartPos.datumLine.endPos.z");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		else
		{	//�ֲ����壺�����׼�������յ�
			//ͶӰ��׼����㶨��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Datum Line Start";
			lpInfo->m_strPropHelp = "Define the projecting datum line's start point";
#else
			lpInfo->m_strPropName = "ͶӰ��׼�����";
			lpInfo->m_strPropHelp = "ͶӰ��׼����㶨��";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("desStartPos.datumLineStart");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ͶӰ��׼���յ㶨��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Datum Line End";
			lpInfo->m_strPropHelp = "Define the projecting datum line's end point";
#else
			lpInfo->m_strPropName = "ͶӰ��׼���յ�";
			lpInfo->m_strPropHelp = "ͶӰ��׼���յ㶨��";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("desStartPos.datumLineEnd");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
	}
	else
	{
		if(pFirstLineAngle->desEndPos.hDatumLine>0X20)
		{	//��׼����Ϣ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType=CBS_DROPDOWNLIST;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.None|1.X Symmetry|2.Y Symmetry|3.Z Symmetry";
			lpInfo->m_strPropName = "Symmetry Type";
			lpInfo->m_strPropHelp = "Symmetry Type based on specified datum line";
#else
			lpInfo->m_cmbItems="0.��|1.X��Գ�|2.Y��Գ�|3.Z��Գ�";
			lpInfo->m_strPropName = "���նԳ�����";
			lpInfo->m_strPropHelp = "����ָ����׼�ߵĲ��նԳ�����";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("desEndPos.cMirTransType");
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Line Start";
			lpInfo->m_strPropHelp = "Coordinate of datum line's start point";
#else
			lpInfo->m_strPropName = "��׼���������";
			lpInfo->m_strPropHelp = "��׼���������";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("desEndPos.datumLine.startPos");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//���X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X��";
			lpInfo->m_strPropHelp = "X coordinate of datum line's start point";
#else
			lpInfo->m_strPropName = "X���꣺";
			lpInfo->m_strPropHelp = "��׼�����X����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("desEndPos.datumLine.startPos.x");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//���Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y��";
			lpInfo->m_strPropHelp = "Y coordinate of datum line's start point";
#else
			lpInfo->m_strPropName = "Y���꣺";
			lpInfo->m_strPropHelp = "��׼�����Y����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("desEndPos.datumLine.startPos.y");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//���Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z��";
			lpInfo->m_strPropHelp = "Z coordinate of datum line's start point";
#else
			lpInfo->m_strPropName = "Z���꣺";
			lpInfo->m_strPropHelp = "��׼�����Z����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("desEndPos.datumLine.startPos.z");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//�յ�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Line End";
			lpInfo->m_strPropHelp = "Coordinate of datum line's end point";
#else
			lpInfo->m_strPropName = "��׼���յ�����";
			lpInfo->m_strPropHelp = "��׼���յ�����";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("desEndPos.datumLine.endPos");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//�յ�X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X��";
			lpInfo->m_strPropHelp = "X coordinate of datum line's end point";
#else
			lpInfo->m_strPropName = "X���꣺";
			lpInfo->m_strPropHelp = "��׼���յ�X����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("desEndPos.datumLine.endPos.x");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//�յ�Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y��";
			lpInfo->m_strPropHelp = "Y coordinate of datum line's end point";
#else
			lpInfo->m_strPropName = "Y���꣺";
			lpInfo->m_strPropHelp = "��׼���յ�Y����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("desEndPos.datumLine.endPos.y");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//�յ�Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z��";
			lpInfo->m_strPropHelp = "Z coordinate of datum line's end point";
#else
			lpInfo->m_strPropName = "Z���꣺";
			lpInfo->m_strPropHelp = "��׼���յ�Z����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("desEndPos.datumLine.endPos.z");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly();
			pSonPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
		else
		{	//�ֲ����壺�����׼�������յ�
			//ͶӰ��׼����㶨��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Datum Line Start";
			lpInfo->m_strPropHelp = "Define the projecting datum line's start point";
#else
			lpInfo->m_strPropName = "ͶӰ��׼�����";
			lpInfo->m_strPropHelp = "ͶӰ��׼����㶨��";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("desEndPos.datumLineStart");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
			//ͶӰ��׼���յ㶨��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Project Datum Line End";
			lpInfo->m_strPropHelp = "Define the projecting datum line's end point";
#else
			lpInfo->m_strPropName = "ͶӰ��׼���յ�";
			lpInfo->m_strPropHelp = "ͶӰ��׼���յ㶨��";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,true);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("desEndPos.datumLineEnd");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=pParentItem->m_dwPropGroup;
		}
	}
}
void UpdateLinaAngle_DesPos_TwoCtrlWing_SonItem(CPropertyList *pPropList,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	if(pFirstLineAngle==NULL)
		return; 
	CXhChar500 sText;
	int idProp,idMainCtrlProp,idOtherCtrlProp;
	CMultiOffsetPos des_pos;	//���Ǵ���ͶӰ֫���ƽ��֫����������ƫ����
	if(bStart)
	{
		des_pos.ctrlWing.operStyle=pFirstLineAngle->desStartPos.ctrlWing.operStyle;
		des_pos.otherWing.operStyle=pFirstLineAngle->desStartPos.otherWing.operStyle;
		idProp=CLDSLineAngle::GetPropID("desStartPos.fLenOffsetOfCoordWing");
		idMainCtrlProp=CLDSLineAngle::GetPropID("desStartPos.ctrlWing.operStyle");
		idOtherCtrlProp=CLDSLineAngle::GetPropID("desStartPos.otherWing.operStyle");
	}
	else
	{
		des_pos.ctrlWing.operStyle=pFirstLineAngle->desEndPos.ctrlWing.operStyle;
		des_pos.otherWing.operStyle=pFirstLineAngle->desEndPos.otherWing.operStyle;
		idProp=CLDSLineAngle::GetPropID("desEndPos.fLenOffsetOfCoordWing");
		idMainCtrlProp=CLDSLineAngle::GetPropID("desEndPos.ctrlWing.operStyle");
		idOtherCtrlProp=CLDSLineAngle::GetPropID("desEndPos.otherWing.operStyle");
	}
	pPropList->DeleteItemByPropId(idProp);
	//��TMAV5.1.3.0��ʼȡ�����ص㶨λ��ʽ���� wjh-2016.6.4
	//if((des_pos.ctrlWing.operStyle==0&&des_pos.otherWing.operStyle!=0)||
	//	(des_pos.ctrlWing.operStyle!=0&&des_pos.otherWing.operStyle==0))
#ifdef __NEW_ANGLE_LOCA_MODE_
	CMultiOffsetPos* pDesPos=bStart?&pFirstLineAngle->desStartPos:&pFirstLineAngle->desEndPos;
	if(des_pos.otherWing.operStyle==0&&!pDesPos->blLegayDualFairWingCtrlPoint)
#else
	if(des_pos.otherWing.operStyle==0)
#endif
	{
		CPropTreeItem *pGroupItem=NULL,*pParentItem=NULL;
		//if(des_pos.ctrlWing.operStyle==0)
		//	pGroupItem=pPropList->FindItemByPropId(idMainCtrlProp,&pParentItem);
		//else
			pGroupItem=pPropList->FindItemByPropId(idOtherCtrlProp,&pParentItem);
		//
		CItemInfo *lpInfo= new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Offset Dist Of Coord_Wing";
		lpInfo->m_strPropHelp = "Offset Distance Of Coord_Wing";
#else
		lpInfo->m_strPropName = "���������λƫ��";
		lpInfo->m_strPropHelp = "������֫��λ�������������Ķ����λƫ������";
#endif
		CPropTreeItem *pSonPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,true);
		pSonPropItem->m_idProp = idProp;
		pSonPropItem->m_dwPropGroup=pGroupItem->m_dwPropGroup;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		else
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
	}
}
void UpdateLineAngle_DesPos_DatumPosStyle_TreeItem(CPropertyList *pPropList,CPropTreeItem *pLeaderItem,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	CPropTreeItem *pGroupItem,*pPropItem,*pSonPropItem,*pParentItem=NULL;
	int idProp;
	CItemInfo *lpInfo;
	if(pFirstLineAngle==NULL||pLeaderItem==NULL)
		return;
	CMultiOffsetPos *pDesPos;
	const int GROUP_GENERAL=1,GROUP_STATMAT=2,GROUP_START=3,GROUP_END=4,GROUP_DESIGN=5;
	DWORD dwPropGroup=0;
	if(bStart)
	{
		dwPropGroup=GetSingleWord(GROUP_START);
		pDesPos=&pFirstLineAngle->desStartPos;
	}
	else
	{
		dwPropGroup=GetSingleWord(GROUP_END);
		pDesPos=&pFirstLineAngle->desEndPos;
	}
	CXhChar500 sText;
	//���㶨λ��ʽ
	if(pDesPos->spatialOperationStyle!=0)
	{	//��ָ����׼��
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.z"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.z"));
		}
	}
	else //if(pDesPos->spatialOperationStyle==0)
	{	//��ʼ��Ϊ��׼��
		//ָ�����㶨λ�����Ϣ
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datumPoint");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datumPoint");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Point";
			lpInfo->m_strPropHelp = "Datum point's position";
#else
			lpInfo->m_strPropName = "����λ��";
			lpInfo->m_strPropHelp = "����λ��";
#endif
			pLeaderItem->m_bHideChildren=FALSE;
			pPropItem = pPropList->InsertItem(pLeaderItem ,lpInfo,0,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));

		pDesPos->datumPoint.UpdatePos(pFirstLineAngle->BelongModel());
		if(//(bStart&&pFirstLineAngle->bHuoQuStart&&pFirstLineAngle->desStartPos.spatialOperationStyle==0&&pFirstLineAngle->desStartPos.datumPoint.datum_pos_style==1)||
			(!bStart&&&pFirstLineAngle->bHuoQuEnd&&pFirstLineAngle->desEndPos.spatialOperationStyle==0&&pFirstLineAngle->desEndPos.datumPoint.datum_pos_style==1))
		{
			CLDSLineAngle* pDatumAngle=NULL;
			if(bStart)
			{
				pDatumAngle=(CLDSLineAngle*)pFirstLineAngle->BelongModel()->FromPartHandle(pFirstLineAngle->desStartPos.datumPoint.des_para.RODEND.hRod,CLS_LINEANGLE);
				idProp=CLDSLineAngle::GetPropID("desStartPos.bendingLenOffsetComplement");
			}
			else
			{
				pDatumAngle=(CLDSLineAngle*)pFirstLineAngle->BelongModel()->FromPartHandle(pFirstLineAngle->desEndPos.datumPoint.des_para.RODEND.hRod,CLS_LINEANGLE);
				idProp=CLDSLineAngle::GetPropID("desEndPos.bendingLenOffsetComplement");
			}
			if(pDatumAngle->huoqu_handle!=pFirstLineAngle->huoqu_handle&&pFirstLineAngle->huoqu_handle>0)
			{
				pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
				if(pSonPropItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Bend Point Length Offset";
					lpInfo->m_strPropHelp = "������Ի�׼�Ǹ����Ӷ���㣬�ػ�׼�Ǹ���������һ�����Ӷε�����ƫ����";
#else
					lpInfo->m_strPropName = "���������ƫ����";
					lpInfo->m_strPropHelp = "������Ի�׼�Ǹ����Ӷ���㣬�ػ�׼�Ǹ���������һ�����Ӷε�����ƫ����";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,true);
					pSonPropItem->m_idProp = idProp;
					pSonPropItem->m_dwPropGroup=dwPropGroup;
				}
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			}
		}
		//����λ��X�������
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datumPoint.x");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datumPoint.x");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X";
			lpInfo->m_strPropHelp = "X coordinate of datum position";
#else
			lpInfo->m_strPropName = "X����";
			lpInfo->m_strPropHelp = "����λ��X����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,true);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart) 
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->desStartPos.datumPoint.datum_pos_style!=0||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		else
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->desEndPos.datumPoint.datum_pos_style!=0||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		//����λ��Y�������
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datumPoint.y");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datumPoint.y");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y";
			lpInfo->m_strPropHelp = "Y coordinate of datum position";
#else
			lpInfo->m_strPropName = "Y����";
			lpInfo->m_strPropHelp = "����λ��Y����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,1,true);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->desStartPos.datumPoint.datum_pos_style!=0||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		else
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->desEndPos.datumPoint.datum_pos_style!=0||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		//����λ��Z�������
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datumPoint.z");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datumPoint.z");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z";
			lpInfo->m_strPropHelp = "Z coordinate of datum position";
#else
			lpInfo->m_strPropName = "Z����";
			lpInfo->m_strPropHelp = "����λ��Z����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,2,true);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->desStartPos.datumPoint.datum_pos_style!=0||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		else
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->desEndPos.datumPoint.datum_pos_style!=0||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
	}
	if(pDesPos->spatialOperationStyle>0&&pDesPos->spatialOperationStyle<15)
	{	//��Ӽ���
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datum_jg_h");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datum_jg_h");
		pGroupItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pGroupItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Lap-Joint Position Datum Rod";
			lpInfo->m_strPropHelp = "Datum rod on lap-joint position, when it defaults 0X0, take end node's parent rod as datum rod.";
#else
			lpInfo->m_strPropName = "���λ�������˼�";
			lpInfo->m_strPropHelp = "���λ�������˼���Ĭ��0X0ʱ���Զ˽ڵ�ĸ��˼�Ϊ��׼�˼���";
#endif
			pGroupItem = pPropList->InsertItem(pLeaderItem,lpInfo,0,true);
			pGroupItem->m_idProp = idProp;
			pGroupItem->m_dwPropGroup=dwPropGroup;
		}
		pGroupItem->m_bHideChildren=FALSE;
		pGroupItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pGroupItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pGroupItem->m_idProp,sText)>0)
			pGroupItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pGroupItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pGroupItem->m_idProp));
		else
			pGroupItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pGroupItem->m_idProp));
#ifdef __NEW_ANGLE_LOCA_MODE_
		//����ͶӰʱ��֧���û�ָ�����׼��
		if (pDesPos->spatialOperationStyle == 2 || pDesPos->spatialOperationStyle == 3||
			pDesPos->spatialOperationStyle == 5 || pDesPos->spatialOperationStyle == 6)
		{
			if (bStart)
				idProp = CLDSLineAngle::GetPropID("desStartPos.LappedDatumWingG");
			else
				idProp = CLDSLineAngle::GetPropID("desEndPos.LappedDatumWingG");
			pPropItem = pPropList->FindItemByPropId(idProp, &pParentItem);
			if (pPropItem == NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_POPMENUEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems = "Automatic Decide";
				lpInfo->m_strPropName = "Datum Plane Offset Type";
				lpInfo->m_strPropHelp = "Datum point's offset type on working datum plane";
#else
				lpInfo->m_cmbItems = "�Զ��ж�";
				lpInfo->m_strPropName = "����ľ�";
				lpInfo->m_strPropHelp = "��׼������֫�ϴ�ӻ����ִ���ľ�";
#endif
				pPropItem = pPropList->InsertItem(pGroupItem, lpInfo, 0, true);
				pPropItem->m_idProp = idProp;
				pPropItem->m_dwPropGroup = dwPropGroup;
			}
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = (pDesPos->datumWingOffset.gStyle != 4) || !CLDSLineAngle::IsPropEqual(pView->selectObjs, pPropItem->m_idProp);
			if (pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp, sText) > 0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			if (bStart)
				pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked() || pFirstLineAngle->bHuoQuStart ||
					pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			else
				pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked() || pFirstLineAngle->bHuoQuEnd ||
					pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		}
#endif
		//����ƫ������
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.len_offset_type");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.len_offset_type");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Length Extend|1.Parallel Offset|2.X|3.Y|4.Z";
			lpInfo->m_strPropName = "Datum Plane Offset Type";
			lpInfo->m_strPropHelp = "Datum point's offset type on working datum plane";
#else
			lpInfo->m_cmbItems="0.��������|1.����ƽ��ƫ��|2.X�������|3.Y�������|4.Z�������";
			lpInfo->m_strPropName = "����ƫ������";
			lpInfo->m_strPropHelp = "�����ڹ��������ڵ�ƫ��������";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,1,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//�������췽��ƫ����
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.len_offset_dist");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.len_offset_dist");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Plane Offset";
			lpInfo->m_strPropHelp = "Datum point's offset on working datum plane";
#else
			lpInfo->m_strPropName = "����ƫ����";
			lpInfo->m_strPropHelp = "�����ڹ��������ڵ�ƫ����";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,2,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));

		if(pDesPos->IsInHuoQuLine()||pDesPos->IsInOffsetZhunLine())
		{
			//������׼�Ǹ�
			if(bStart)
				idProp=CLDSLineAngle::GetPropID("desStartPos.huoqu_jg_h");
			else
				idProp=CLDSLineAngle::GetPropID("desEndPos.huoqu_jg_h");
			pGroupItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pGroupItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Bending Datum Angle";
				lpInfo->m_strPropHelp = "Bending datum angle";
#else
				lpInfo->m_strPropName = "�����߻�׼�Ǹ�";
				lpInfo->m_strPropHelp = "�����߻�׼�Ǹ�";
#endif
				pGroupItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
				pGroupItem->m_idProp = idProp;
				pGroupItem->m_dwPropGroup=dwPropGroup;
			}
			pGroupItem->m_bHideChildren=FALSE;
			pGroupItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pGroupItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pGroupItem->m_idProp,sText)>0)
				pGroupItem->m_lpNodeInfo->m_strPropValue = sText;
			if(bStart)
				pGroupItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pGroupItem->m_idProp));
			else
				pGroupItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pGroupItem->m_idProp));
			if(bStart)
				idProp=CLDSLineAngle::GetPropID("desStartPos.datum_offset_dist");
			else
				idProp=CLDSLineAngle::GetPropID("desEndPos.datum_offset_dist");
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Datum Point Normal Offset";
				lpInfo->m_strPropHelp = "Datum point normal offset";
#else
				lpInfo->m_strPropName = "���㷨��ƫ����";
				lpInfo->m_strPropHelp = "���㷨��ƫ����";
#endif
				pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,true);
				pPropItem->m_idProp = idProp;
				pPropItem->m_dwPropGroup=dwPropGroup;
			}
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			if(bStart)
				pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			else
				pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			//������׼�Ǹֵ�ǰ����֫
			if(bStart)
				idProp=CLDSLineAngle::GetPropID("desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2");
			else
				idProp=CLDSLineAngle::GetPropID("desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2");
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
				lpInfo->m_cmbType = CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Work Leg";
				lpInfo->m_strPropHelp = "Work leg of bending datum angle";
				lpInfo->m_cmbItems = "Default|X Leg|Y Leg";
#else
				lpInfo->m_strPropName = "��ǰ����֫";
				lpInfo->m_strPropHelp = "��ǰ����֫";
				lpInfo->m_cmbItems = "0.�Զ��ж�|1.X֫|2.Y֫";
#endif
				pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,true);
				pPropItem->m_idProp = idProp;
				pPropItem->m_dwPropGroup=dwPropGroup;
			}
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			if(bStart)
				pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			else
				pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			//�����߼�϶
			if(bStart)
				idProp=CLDSLineAngle::GetPropID("desStartPos.huoqu_dist");
			else
				idProp=CLDSLineAngle::GetPropID("desEndPos.huoqu_dist");
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Bend Line Space";
				lpInfo->m_strPropHelp = "Bend line space";
#else
				lpInfo->m_strPropName = "�����߼�϶";
				lpInfo->m_strPropHelp = "�����߼�϶";
#endif
				pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,true);
				pPropItem->m_idProp = idProp;
				pPropItem->m_dwPropGroup=dwPropGroup;
			}
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			if(bStart)
				pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			else
				pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		}
		else
		{
			if(bStart)
			{
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.huoqu_jg_h"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.huoqu_dist"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datum_offset_dist"));
			}
			else
			{
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.huoqu_jg_h"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.huoqu_dist"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datum_offset_dist"));
			}
		}
		if(pDesPos->IsInOffsetZhunLine())
		{
			//ƫ��׼��
			if(bStart)
				idProp=CLDSLineAngle::GetPropID("desStartPos.offset_zhun");
			else
				idProp=CLDSLineAngle::GetPropID("desEndPos.offset_zhun");
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Offset G";
				lpInfo->m_strPropHelp = "Offset stadia";
#else
				lpInfo->m_strPropName = "ƫ��׼��";
				lpInfo->m_strPropHelp = "ƫ��׼��";
#endif
				pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,true);
				pPropItem->m_idProp = idProp;
				pPropItem->m_dwPropGroup=dwPropGroup;
			}
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			if(bStart)
				pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			else
				pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		}
		else
		{
			if(bStart)
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.offset_zhun"));
			else
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.offset_zhun"));
		}
	}
	else
	{
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datum_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datum_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.len_offset_type"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.len_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.huoqu_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.huoqu_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.offset_zhun"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datum_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datum_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.len_offset_type"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.len_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.huoqu_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.huoqu_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.offset_zhun"));
		}
	}
#ifndef __NEW_ANGLE_LOCA_MODE_
	if(pDesPos->spatialOperationStyle==15)
#else
	if (pDesPos->spatialOperationStyle == 15 ||	//��ģʽ��������Ƶ�ͶӰ/ƽ�ƹ鲢����֫������������� wjh-2019
		pDesPos->spatialOperationStyle == 18 || pDesPos->spatialOperationStyle == 19)
#endif
	{	//��֫�������
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.cMainCoordCtrlWing");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.cMainCoordCtrlWing");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Main Leg,Controls Coordinate";
			lpInfo->m_strPropHelp = "Main leg,controls coordinate is the leg which can control any two of X,Y,Z coordinate";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems = "X Leg|Y Leg";
#else
			lpInfo->m_strPropName = "������Ƶ�";
			lpInfo->m_strPropHelp = "������Ƶ���ָ���Կ��ƶ����X��Y��Z�������������������������Ĺؼ��㡣";
			lpInfo->m_cmbType=1;
#ifndef __NEW_ANGLE_LOCA_MODE_
			lpInfo->m_cmbItems = "X֫���ߵ�|Y֫���ߵ�";
#else
			if (pDesPos->spatialOperationStyle == 15)
				lpInfo->m_cmbItems = "X֫���ߵ�|Y֫���ߵ�|ͶӰ���ߵ�|ƽ�����ߵ�";
			else //if (pDesPos->spatialOperationStyle == 18 || pDesPos->spatialOperationStyle == 19)
				lpInfo->m_cmbItems = "X֫���ߵ�|Y֫���ߵ�|ͶӰ���ߵ�|ƽ�����ߵ�";
#endif
#endif
			pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,0,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));

		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.coordCtrlType");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.coordCtrlType");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Coordinate,Main Control Leg Controls";
			lpInfo->m_strPropHelp = "Coordinate which key point on main control leg controls.";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems = "0.X,Z|1:Y,Z|2.X,Y";
#else
			lpInfo->m_strPropName = "����֫��������";
			lpInfo->m_strPropHelp = "����֫�ؼ�����Ƶ����������";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems = "0.X,Z����|1:Y,Z����|2.X,Y����";
#endif
			pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,1,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
#ifdef __NEW_ANGLE_LOCA_MODE_
		if (pDesPos->blLegayDualFairWingCtrlPoint)
		{
			pPropItem->m_lpNodeInfo->m_strPropName = "X֫����ƫ��";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "X֫����ƫ��";
			pPropItem->m_lpNodeInfo->m_strPropValue="";
			pPropItem->SetReadOnly();
		}
#endif

		/*if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.ctrlWing.operStyle");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.ctrlWing.operStyle");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Main Control Point Position Style";
			lpInfo->m_strPropHelp = "Position style of key point on main control leg.";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems = "0.Project|1.X-Y Plane Offset|2.Y-Z Plane Offset|3.X-Z Plane Offset";
#else
			lpInfo->m_strPropName = "���ص㶨λ��ʽ";
			lpInfo->m_strPropHelp = "����֫�ؼ��㶨λ��ʽ��";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems = "0.����ͶӰ|1.X-Yƽ��ƽ��|2.Y-Zƽ��ƽ��|3.X-Zƽ��ƽ��";
#endif
			pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		*/
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.ctrlWing.gStyle");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.ctrlWing.gStyle");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Main Control Line Stadia Type";
			lpInfo->m_strPropHelp = "Main control line's stadia type.";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems = "g|g1|g2|g3|User-Defined";
#else
			lpInfo->m_strPropName = "������׼������";
			lpInfo->m_strPropHelp = "������׼�����͡�";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems = "g|g1|g2|g3|�Զ���";
#endif
#ifdef __NEW_ANGLE_LOCA_MODE_
			if (pDesPos->blLegayDualFairWingCtrlPoint)
			{
				lpInfo->m_strPropName = "׼������";
				lpInfo->m_strPropHelp = "׼�����͡�";
			}
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		else
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.ctrlWing.offsetDist");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.ctrlWing.offsetDist");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Main Control Line Stadia";
			lpInfo->m_strPropHelp = "Main control line's stadia.";
#else
			lpInfo->m_strPropName = "������׼��";
			lpInfo->m_strPropHelp = "������׼��ֵ��";
#endif
#ifdef __NEW_ANGLE_LOCA_MODE_
			if (pDesPos->blLegayDualFairWingCtrlPoint)
			{
				lpInfo->m_strPropName = "׼��";
				lpInfo->m_strPropHelp = "׼��ֵ��";
			}
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||pDesPos->ctrlWing.gStyle!=4||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		else
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||pDesPos->ctrlWing.gStyle!=4||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));

		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.otherWing.operStyle");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.otherWing.operStyle");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Auxiliary Control Point Position Style";
			lpInfo->m_strPropHelp = "Position style of key point on auxiliary control leg.";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems = "0.Project|1.X-Y Plane Offset|2.Y-Z Plane Offset|3.X-Z Plane Offset";
#else
			lpInfo->m_strPropName = "���ص㶨λ��ʽ";
			lpInfo->m_strPropHelp = "����֫�ؼ��㶨λ��ʽ��";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems = "0.����ͶӰ|1.X-Yƽ��ƽ��|2.Y-Zƽ��ƽ��|3.X-Zƽ��ƽ��";
#endif
			pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly();	//�ݲ����Ÿ��ص�ƽ�Ʒ�ʽ
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
#ifdef __NEW_ANGLE_LOCA_MODE_
		if (pDesPos->blLegayDualFairWingCtrlPoint)
		{
			pPropItem->m_lpNodeInfo->m_strPropName = "Y֫����ƫ��";
			pPropItem->m_lpNodeInfo->m_strPropHelp = "Y֫����ƫ��";
			pPropItem->m_lpNodeInfo->m_strPropValue="";
			pPropItem->SetReadOnly();	//�ݲ����Ÿ��ص�ƽ�Ʒ�ʽ
		}
#endif

		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.otherWing.gStyle");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.otherWing.gStyle");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Main Control Line Stadia Type";
			lpInfo->m_strPropHelp = "Main control line's stadia type.";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems = "g|g1|g2|g3|User-Defined";
#else
			lpInfo->m_strPropName = "������׼������";
			lpInfo->m_strPropHelp = "������׼�����͡�";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems = "g|g1|g2|g3|�Զ���";
#endif
#ifdef __NEW_ANGLE_LOCA_MODE_
			if (pDesPos->blLegayDualFairWingCtrlPoint)
			{
				lpInfo->m_strPropName = "׼������";
				lpInfo->m_strPropHelp = "׼�����͡�";
			}
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		else
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.otherWing.offsetDist");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.otherWing.offsetDist");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Main Control Line Stadia";
			lpInfo->m_strPropHelp = "Main control line's stadia.";
#else
			lpInfo->m_strPropName = "������׼��";
			lpInfo->m_strPropHelp = "������׼��ֵ��";
#endif
#ifdef __NEW_ANGLE_LOCA_MODE_
			if (pDesPos->blLegayDualFairWingCtrlPoint)
			{
				lpInfo->m_strPropName = "׼��";
				lpInfo->m_strPropHelp = "׼��ֵ��";
			}
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,true);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||pDesPos->otherWing.gStyle!=4||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		else
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||pDesPos->otherWing.gStyle!=4||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		//��֫�������ʱ���迼�Ǵ���ͶӰ֫���ƽ��֫����������ƫ����
		UpdateLinaAngle_DesPos_TwoCtrlWing_SonItem(pPropList,bStart);
	}
	else
	{
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.coordCtrlType"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.cMainCoordCtrlWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.ctrlWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.ctrlWing.operStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.ctrlWing.gStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.ctrlWing.offsetDist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.otherWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.otherWing.operStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.otherWing.gStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.otherWing.offsetDist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.fLenOffsetOfCoordWing"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.coordCtrlType"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.cMainCoordCtrlWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.ctrlWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.ctrlWing.operStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.ctrlWing.gStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.ctrlWing.offsetDist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.otherWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.otherWing.operStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.otherWing.gStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.otherWing.offsetDist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.fLenOffsetOfCoordWing"));
		}
	}
#ifndef __NEW_ANGLE_LOCA_MODE_
	if(pDesPos->spatialOperationStyle==17||pDesPos->spatialOperationStyle==19)
#else
	if (pDesPos->spatialOperationStyle == 17)
#endif
	{	//��ָ����ƽ��
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.cFaceOffsetDatumLine");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.cFaceOffsetDatumLine");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		BOOL bReadOnly = FALSE;
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Plane Offset Datum Line";
			lpInfo->m_strPropHelp = "Plane offset plane's datum line";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems="Ridge|X Leg|Y Leg";
#else
			lpInfo->m_strPropName = "ƽ�����׼��";
			lpInfo->m_strPropHelp = "ƽ�����׼��";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems="����|X֫����|Y֫����";
#endif
			pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,3,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	}
	else
	{
		if(bStart)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.cFaceOffsetDatumLine"));
		else
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.cFaceOffsetDatumLine"));
	}
	if((pDesPos->spatialOperationStyle>3&&pDesPos->spatialOperationStyle<7)||
		(pDesPos->spatialOperationStyle>9&&pDesPos->spatialOperationStyle<13)||
#ifndef __NEW_ANGLE_LOCA_MODE_
		pDesPos->spatialOperationStyle==17||pDesPos->spatialOperationStyle==19)
#else
		pDesPos->spatialOperationStyle == 17)
#endif
	{	//ƽ��
		//ƽ���淨�߶���
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.face_offset_norm");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.face_offset_norm");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		BOOL bReadOnly = FALSE;
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Plane Offset Normal";
			lpInfo->m_strPropHelp = "Define the normal of plane offset plane";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems="X-Y Plane|X-Z Plane|Y-Z Plane|User-Defined";
#else
			lpInfo->m_strPropName = "ƽ���淨��";
			lpInfo->m_strPropHelp = "ƽ���淨�߶���";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems="X-Yƽ��|X-Zƽ��|Y-Zƽ��|�Զ���";
#endif
			pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			else
				pPropItem->m_bHideChildren = TRUE;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
#ifdef AFX_TARG_ENU_ENGLISH
		if(strcmp(sText,"User-Defined")!=0)
#else
		if(strcmp(sText,"�Զ���")!=0)
#endif
			bReadOnly = TRUE;
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//ƽ���淨��X����
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.x");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.x");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pPropItem);
		if(pPropItem==NULL)
			return;
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X";
			lpInfo->m_strPropHelp = "X coordinate of plane offset plane's normal";
#else
			lpInfo->m_strPropName = "X����";
			lpInfo->m_strPropHelp = "ƽ���淨��X����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,true);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||bReadOnly||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		else
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||bReadOnly||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		//ƽ���淨��Y����
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.y");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.y");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pPropItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y";
			lpInfo->m_strPropHelp = "Y coordinate of plane offset plane's normal";
#else
			lpInfo->m_strPropName = "Y����";
			lpInfo->m_strPropHelp = "ƽ���淨��Z����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,1,true);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||bReadOnly||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		else
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||bReadOnly||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		//ƽ���淨��Z����
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.z");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.z");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pPropItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z";
			lpInfo->m_strPropHelp = "Z coordinate of plane offset plane's normal";
#else
			lpInfo->m_strPropName = "Z����";
			lpInfo->m_strPropHelp = "ƽ���淨��Z����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,2,true);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||bReadOnly||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		else
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||bReadOnly||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
	}
	else
	{
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.z"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.z"));
		}
	}
	if(pDesPos->spatialOperationStyle==16||pDesPos->spatialOperationStyle==17)
	{	//��ָ��ͶӰ��ƽ��
		//ͶӰ��׼�߶��巽ʽ
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.hDatumLine");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.hDatumLine");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Line";
			lpInfo->m_strPropHelp = "Define datum line";
#else
			lpInfo->m_strPropName = "��׼�߶���";
			lpInfo->m_strPropHelp = "��׼�߶���";
#endif
			lpInfo->m_cmbItems=GetLineHandleStr();
			pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,0,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		UpdateLineAngle_DesPos_DatumLine_TreeItem(pPropList,pPropItem,bStart);
	}
	else
	{
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.hDatumLine"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.startPos"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.startPos.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.startPos.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.startPos.z"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.endPos"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.endPos.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.endPos.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.endPos.z"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLineStart"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLineEnd"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.hDatumLine"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.startPos"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.startPos.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.startPos.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.startPos.z"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.endPos"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.endPos.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.endPos.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.endPos.z"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLineStart"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLineEnd"));
		}
	}
}
void UpdateLineAngle_DesPos_OrgPosStyle_TreeItem(CPropertyList *pPropList,CPropTreeItem *pLeaderItem,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem,*pSonPropItem,*pParentItem=NULL;
	int idProp;
	CItemInfo *lpInfo;
	if(pFirstLineAngle==NULL||pLeaderItem==NULL)
		return;
	const int GROUP_GENERAL=1,GROUP_STATMAT=2,GROUP_START=3,GROUP_END=4,GROUP_DESIGN=5;
	CMultiOffsetPos *pDesPos=NULL;
	DWORD dwPropGroup=0;
	if(bStart)
	{
		dwPropGroup=GetSingleWord(GROUP_START);
		pDesPos=&pFirstLineAngle->desStartPos;
	}
	else
	{
		dwPropGroup=GetSingleWord(GROUP_END);
		pDesPos=&pFirstLineAngle->desEndPos;
	}

	CXhChar500 sText;
	if(pDesPos->origin_pos_style!=1)
	{	//�ǻ�׼��
		if(bStart)
		{
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datum_jg_h",pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
								pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("desStartPos.datum_jg_h")));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.z"));
		}
		else
		{
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datum_jg_h",pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
								pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("desEndPos.datum_jg_h")));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.z"));
		}
	}
	else //if(pDesPos->origin_pos_style==1)
	{	//��ʼ��Ϊ��׼��
		pPropItem=NULL;
		if(bStart)
		{
			SetPropItemReadOnly(pFirstLineAngle,"desStartPos.datum_jg_h",true);
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datum_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.len_offset_type"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.len_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.offset_zhun"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.huoqu_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.huoqu_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.z"));
		}
		else
		{
			SetPropItemReadOnly(pFirstLineAngle,"desEndPos.datum_jg_h",true);
			pPropItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datum_jg_h"),&pParentItem);
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datum_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.len_offset_type"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.len_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.offset_zhun"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.huoqu_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.huoqu_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.z"));
		}
		//ָ�����㶨λ�����Ϣ
		pDesPos->datumPoint.UpdatePos(pFirstLineAngle->BelongModel());
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datumPoint");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datumPoint");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Point";
			lpInfo->m_strPropHelp = "Datum point's position";
#else
			lpInfo->m_strPropName = "����λ��";
			lpInfo->m_strPropHelp = "����λ��";
#endif
			pLeaderItem->m_bHideChildren=FALSE;
			pPropItem = pPropList->InsertItem(pLeaderItem ,lpInfo,0,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//����λ��X�������
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datumPoint.x");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datumPoint.x");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X";
			lpInfo->m_strPropHelp = "X coordinate of datum point's position";
#else
			lpInfo->m_strPropName = "X����";
			lpInfo->m_strPropHelp = "����λ��X����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,true);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->desStartPos.datumPoint.datum_pos_style!=0||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		else
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->desEndPos.datumPoint.datum_pos_style!=0||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		//����λ��Y�������
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datumPoint.y");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datumPoint.y");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y";
			lpInfo->m_strPropHelp = "Y coordinate of datum point's position";
#else
			lpInfo->m_strPropName = "Y����";
			lpInfo->m_strPropHelp = "����λ��Y����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,1,true);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->desStartPos.datumPoint.datum_pos_style!=0||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		else
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->desEndPos.datumPoint.datum_pos_style!=0||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		//����λ��Z�������
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datumPoint.z");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datumPoint.z");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z";
			lpInfo->m_strPropHelp = "Z coordinate of datum point's position";
#else
			lpInfo->m_strPropName = "Z����";
			lpInfo->m_strPropHelp = "����λ��Z����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,2,true);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=dwPropGroup;
		}
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->desStartPos.datumPoint.datum_pos_style!=0||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		else
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->desEndPos.datumPoint.datum_pos_style!=0||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
	}
	if(pDesPos->origin_pos_style!=2)
	{	//�ǽǸֹؼ���
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.cDatumAngleWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumWingOffset"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.operStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.gStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.offsetDist"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.cDatumAngleWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumWingOffset"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.operStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.gStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.offsetDist"));
		}
	}
	else if(pDesPos->origin_pos_style==2)
	{	//ԭʼ��Ϊ��׼�Ǹֹؼ������
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.operStyle");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.operStyle");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Key Point Calculates Style";
			lpInfo->m_strPropHelp = "The calculative style of Datum angle's key point relative to currently angle's end node ";
			lpInfo->m_cmbItems = "0.Project|1.X-Y Plane Offset|2.Y-Z Plane Offset|3.X-Z Plane Offset";//�˴���֧�ֲ����õġ��Զ���ƽ���桱
#else
			lpInfo->m_strPropName = "�ؼ�����㷽ʽ";
			lpInfo->m_strPropHelp = "��׼�Ǹֹؼ�������ڵ�ǰ�Ǹֶ˽ڵ�ļ��㷽ʽ��";
			lpInfo->m_cmbItems = "0.����ͶӰ|1.X-Yƽ��ƽ��|2.Y-Zƽ��ƽ��|3.X-Zƽ��ƽ��";//�˴���֧�ֲ����õġ��Զ���ƽ���桱
#endif
			pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo, -1,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_dwPropGroup = dwPropGroup;
		}
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.cDatumAngleWing");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.cDatumAngleWing");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Key Point Belong Leg";
			lpInfo->m_strPropHelp = "The position of Angle's key point as start point.";
			lpInfo->m_cmbItems = "Ridge|X Leg|Y Leg";
#else
			lpInfo->m_strPropName = "�ؼ������֫";
			lpInfo->m_strPropHelp = "��Ϊ��ʼ��ĽǸֹؼ�������λ�á�";
			lpInfo->m_cmbItems = "���ߵ�|X֫���ߵ�|Y֫���ߵ�";
#endif
			pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo, -1,true);
			pPropItem->m_idProp = idProp ;
		}
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup = dwPropGroup;

		if(pDesPos->cDatumAngleWing=='X'||pDesPos->cDatumAngleWing=='Y')
		{
			if(bStart)
				idProp=CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.gStyle");
			else
				idProp=CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.gStyle");
			pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pSonPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Center Line G-Style";
				lpInfo->m_strPropHelp = "When key point on angle's leg center line,the offset style of ridge line to center line.";
				lpInfo->m_cmbItems = "g|g1|g2|g3|User-Defined";
#else
				lpInfo->m_strPropName = "����׼������";
				lpInfo->m_strPropHelp = "�ؼ����ڽǸ�֫����ʱ�����ߵ����ߵ�ƫ�������͡�";
				lpInfo->m_cmbItems = "g|g1|g2|g3|�Զ���";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,true);
				pSonPropItem->m_idProp = idProp;
			}
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			if(CLDSLineAngle::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pSonPropItem->m_idProp);
			if(bStart)
				pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			else
				pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup = dwPropGroup;
			if(bStart)
				idProp=CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.offsetDist");
			else
				idProp=CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.offsetDist");
			pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pSonPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Center Line Offset";
				lpInfo->m_strPropHelp = "When key point on angle's leg center line,actual offset of ridge line to center line";
#else
				lpInfo->m_strPropName = "����׼��ֵ";
				lpInfo->m_strPropHelp = "�ؼ����ڽǸ�֫����ʱ�����ߵ����ߵ�ʵ��ƫ������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,true);
				pSonPropItem->m_idProp = idProp;
				pSonPropItem->m_dwPropGroup = dwPropGroup;
			}
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			if(bStart)
				pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			else
				pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				
		}
		else
		{
			if(bStart)
			{
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.operStyle"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.gStyle"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.offsetDist"));
			}
			else
			{
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.operStyle"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.gStyle"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.offsetDist"));
			}
		}
	}
	if(pDesPos->origin_pos_style==0||pDesPos->origin_pos_style==5)
	{	//��ʼ��Ϊ�ڵ�λ�û������ж�
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datum_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.z"));

			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datum_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.len_offset_type"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.len_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.offset_zhun"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.huoqu_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.huoqu_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.z"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datum_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.z"));

			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datum_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.len_offset_type"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.len_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.offset_zhun"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.huoqu_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.huoqu_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.z"));
		}
	}
}
void UpdateLineAngle_DesPos_DatumToBerStyle_TreeItem(CPropertyList *pPropList,CPropTreeItem *pLeaderItem,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem,*pParentItem=NULL;
	int idProp;
	CItemInfo *lpInfo;
	if(pFirstLineAngle==NULL||pLeaderItem==NULL)
		return;
	const int GROUP_GENERAL=1,GROUP_STATMAT=2,GROUP_START=3,GROUP_END=4,GROUP_DESIGN=5;
	CMultiOffsetPos *pDesPos=NULL;
	DWORD dwPropGroup=0;
	pPropList->SetItemReadOnly(pLeaderItem->m_idProp,true);
	
	if(bStart)
	{
		dwPropGroup=GetSingleWord(GROUP_START);
		pDesPos=&pFirstLineAngle->desStartPos;
		//���ڶ̽Ǹַſ�������->��㶨λ��ʽ������
		/*if(pFirstLineAngle->pStart==NULL)
		{
			pPropList->DeleteItemByPropId(pLeaderItem->m_idProp);
			return;
		}*/
	}
	else
	{
		dwPropGroup=GetSingleWord(GROUP_END);
		pDesPos=&pFirstLineAngle->desEndPos;
		//���ڶ̽Ǹַſ�������->��㶨λ��ʽ������
		/*if(pFirstLineAngle->pEnd==NULL)
		{
			pPropList->DeleteItemByPropId(pLeaderItem->m_idProp);
			return;
		}*/
	}
	CXhChar500 sText;
	if(bStart)
		idProp=CLDSLineAngle::GetPropID("desStartPos.cWorkWingStyle");
	else
		idProp=CLDSLineAngle::GetPropID("desEndPos.cWorkWingStyle");
	pPropItem=pPropList->FindItemByPropId(idProp,NULL);
	if(pPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		//lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Position Connect Leg";
		lpInfo->m_strPropHelp = "Datum connecting leg of offset direction when angle offset along its own leg normal,default as the working leg of working plane which combine with end node's parent rod";
		lpInfo->m_cmbType=1;
		lpInfo->m_cmbItems="Default|-|X Leg|Y Leg|X&Y Leg";
#else
		lpInfo->m_strPropName = "��λ����֫";
		lpInfo->m_strPropHelp = "�Ǹ�������֫���߷���ƫ��ʱƫ�Ʒ���Ļ�׼����֫��Ĭ��Ϊ��˽ڵ㸸�˼�����ɹ�����Ĺ���֫��";
		lpInfo->m_cmbType=1;
		lpInfo->m_cmbItems="�����ж�|-|X֫��λ|Y֫��λ|˫֫��λ";
#endif
		pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
		pPropItem->m_idProp = idProp;
		pPropItem->m_dwPropGroup=dwPropGroup;
	}
	if(pDesPos->cWorkWingStyle==0)	//�����ж�
		pPropItem->m_lpNodeInfo->SetCheck(1);
	else
		pPropItem->m_lpNodeInfo->SetCheck(-1);
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = (pDesPos->cWorkWingStyle==0)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	//X֫������ƫ�Ʒ�ʽ
	if(bStart)
		idProp=CLDSLineAngle::GetPropID("desStartPos.wing_x_offset.gStyle");
	else
		idProp=CLDSLineAngle::GetPropID("desEndPos.wing_x_offset.gStyle");
	pPropItem=pPropList->FindItemByPropId(idProp,NULL);
	if(pPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Leg Normal Offsets Style";
		lpInfo->m_strPropHelp = "X leg normal offsets style";
		lpInfo->m_cmbType=1;
		lpInfo->m_cmbItems="g|g1|g2|g3|User-Defined";
#else
		lpInfo->m_strPropName = "X֫������ƫ�Ʒ�ʽ";
		lpInfo->m_strPropHelp = "X֫������ƫ�Ʒ�ʽ";
		lpInfo->m_cmbType=1;
		lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���";
#endif
		pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
		pPropItem->m_idProp = idProp;
		pPropItem->m_dwPropGroup=dwPropGroup;
	}
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	if(bStart)
		pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
		pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	else
		pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
		pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	//X֫������ƫ��
	if(bStart)
		idProp=CLDSLineAngle::GetPropID("desStartPos.wing_x_offset.offsetDist");
	else
		idProp=CLDSLineAngle::GetPropID("desEndPos.wing_x_offset.offsetDist");
	pPropItem=pPropList->FindItemByPropId(idProp,NULL);
	if(pPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Leg Normal Offset";
		lpInfo->m_strPropHelp = "X leg normal's offset";
#else
		lpInfo->m_strPropName = "X֫������ƫ����";
		lpInfo->m_strPropHelp = "X֫������ƫ����";
#endif
		pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
		pPropItem->m_idProp = idProp;
		pPropItem->m_dwPropGroup=dwPropGroup;
	}
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	if(bStart)
		pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
		pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	else
		pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
		pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	if(pDesPos->wing_x_offset.gStyle!=4)
		pPropItem->SetReadOnly();
	//Y֫������ƫ�Ʒ�ʽ
	if(bStart)
		idProp=CLDSLineAngle::GetPropID("desStartPos.wing_y_offset.gStyle");
	else
		idProp=CLDSLineAngle::GetPropID("desEndPos.wing_y_offset.gStyle");
	pPropItem=pPropList->FindItemByPropId(idProp,NULL);
	if(pPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Leg Normal Offsets Style";
		lpInfo->m_strPropHelp = "Y leg normal offsets Style";
		lpInfo->m_cmbType=1;
		lpInfo->m_cmbItems="g|g1|g2|g3|User-Defined";
#else
		lpInfo->m_strPropName = "Y֫������ƫ�Ʒ�ʽ";
		lpInfo->m_strPropHelp = "Y֫������ƫ�Ʒ�ʽ";
		lpInfo->m_cmbType=1;
		lpInfo->m_cmbItems="g|g1|g2|g3|�Զ���";
#endif
		pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
		pPropItem->m_idProp = idProp;
		pPropItem->m_dwPropGroup=dwPropGroup;
	}
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	if(bStart)
		pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
		pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	else
		pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
		pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	//Y֫������ƫ��
	if(bStart)
		idProp=CLDSLineAngle::GetPropID("desStartPos.wing_y_offset.offsetDist");
	else
		idProp=CLDSLineAngle::GetPropID("desEndPos.wing_y_offset.offsetDist");
	pPropItem=pPropList->FindItemByPropId(idProp,NULL);
	if(pPropItem==NULL)
	{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Leg Normal Offset";
		lpInfo->m_strPropHelp = "Y leg normal's offset";
#else
		lpInfo->m_strPropName = "Y֫������ƫ����";
		lpInfo->m_strPropHelp = "Y֫������ƫ����";
#endif
		pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
		pPropItem->m_idProp = idProp;
		pPropItem->m_dwPropGroup=dwPropGroup;
	}
	pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
	if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	if(bStart)
		pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
		pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	else
		pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
		pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	if(pDesPos->wing_y_offset.gStyle!=4)
		pPropItem->SetReadOnly();

	CLDSLinePart *pDatumLinePart=NULL;
	if(bStart)
		pDatumLinePart=pFirstLineAngle->GetStartDatumPart();
	else 
		pDatumLinePart=pFirstLineAngle->GetEndDatumPart();
	if(pDatumLinePart&&pDatumLinePart->GetClassTypeId()==CLS_LINETUBE&&(
		pDesPos->spatialOperationStyle==1||pDesPos->spatialOperationStyle==4))	
	{	//��ֹ�����ͶӰ/ƽ��
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.fEccentricDist");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.fEccentricDist");
		pPropItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Eccentric Distance";
			lpInfo->m_strPropHelp = "Eccentric distance";
#else
			lpInfo->m_strPropName = "ƫ�ľ�";
			lpInfo->m_strPropHelp = "ƫ�ľ�";
#endif
			pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
	}
	else
	{
		if(bStart)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.fEccentricDist"));
		else
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.fEccentricDist"));
	}
}

void UpdateLineAngle_ArcLift_TreeItem(CPropertyList *pPropList,CPropTreeItem *pArcLiftItem,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem=NULL,*pParentItem=NULL;
	int idProp;
	CItemInfo *lpInfo;
	if(pFirstLineAngle==NULL||pArcLiftItem==NULL)
		return;
	char sText[MAX_TEMP_CHAR_100+1]="";
	const int GROUP_GENERAL=1,GROUP_STATMAT=2,GROUP_START=3,GROUP_END=4,GROUP_DESIGN=5;
	DWORD dwPropGroup=0;
	if(bStart)
	{
		dwPropGroup=GetSingleWord(GROUP_START);
		//��Ԥ������㶨λ��ʽΪָ�����꣬��ʾԤ��ǰʼ������
		if(Ta.ArcLift.FromHandle(pFirstLineAngle->ArcLiftHandle())!=NULL && pFirstLineAngle->desStartPos.jgber_cal_style!=0)
		{
			//ʼ��Ԥ��ǰ����
			idProp=CLDSLineAngle::GetPropID("m_xStartBeforeLift");
			pParentItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pParentItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Start Before Pre-Lift";
				lpInfo->m_strPropHelp = "Start coordinate before pre-lift";
#else
				lpInfo->m_strPropName = "ʼ��Ԥ��ǰ����";
				lpInfo->m_strPropHelp = "ʼ��Ԥ��ǰ����";
#endif
				pParentItem = pPropList->InsertItem(pArcLiftItem,lpInfo,-1,TRUE);
				pParentItem->m_bHideChildren = FALSE;
				pParentItem->m_idProp = idProp;

			}
			if(CLDSLineAngle::GetPropStatus(pParentItem->m_idProp)!=-1)
				pParentItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pParentItem->m_idProp);
			pParentItem->m_dwPropGroup = dwPropGroup;
			//X
			idProp=CLDSLineAngle::GetPropID("m_xStartBeforeLift.x");
			pPropItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "X coordinate of start before pre-lift";
#else
				lpInfo->m_strPropHelp = "ʼ��Ԥ��ǰX�����������";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
				pPropItem->m_idProp = idProp;
			}		
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			pPropItem->m_dwPropGroup = dwPropGroup;
			//Y
			idProp=CLDSLineAngle::GetPropID("m_xStartBeforeLift.y");
			pPropItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "Y coordinate of start before pre-lift";
#else
				lpInfo->m_strPropHelp = "ʼ��Ԥ��ǰY�����������";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
				pPropItem->m_idProp = idProp;
			}		
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			pPropItem->m_dwPropGroup = dwPropGroup;		
			//Z
			idProp=CLDSLineAngle::GetPropID("m_xStartBeforeLift.z");
			pPropItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "Z coordinate of start before pre-lift";
#else
				lpInfo->m_strPropHelp = "ʼ��Ԥ��ǰZ�����������";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
				pPropItem->m_idProp = idProp;
			}		
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			pPropItem->m_dwPropGroup = dwPropGroup;
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_xStartBeforeLift"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_xStartBeforeLift.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_xStartBeforeLift.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_xStartBeforeLift.z"));
		}
	}
	else
	{
		dwPropGroup=GetSingleWord(GROUP_END);
		//��Ԥ������㶨λ��ʽΪָ�����꣬��ʾԤ��ǰ�ն�����
		if(Ta.ArcLift.FromHandle(pFirstLineAngle->ArcLiftHandle())!=NULL && pFirstLineAngle->desEndPos.jgber_cal_style!=0)
		{
			//�ն�Ԥ��ǰ����
			idProp=CLDSLineAngle::GetPropID("m_xEndBeforeLift");
			pParentItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pParentItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "End Before Pre-Lift";
				lpInfo->m_strPropHelp = "End coordinate before pre-lift";
#else
				lpInfo->m_strPropName = "�ն�Ԥ��ǰ����";
				lpInfo->m_strPropHelp = "�ն�Ԥ��ǰ����";
#endif
				pParentItem = pPropList->InsertItem(pArcLiftItem,lpInfo,-1,TRUE);
				pParentItem->m_bHideChildren = FALSE;
				pParentItem->m_idProp = idProp;

			}
			if(CLDSLineAngle::GetPropStatus(pParentItem->m_idProp)!=-1)
				pParentItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pParentItem->m_idProp);
			pParentItem->m_dwPropGroup = dwPropGroup;
			//X
			idProp=CLDSLineAngle::GetPropID("m_xEndBeforeLift.x");
			pPropItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "X coordinate of end before pre-lift";
#else
				lpInfo->m_strPropHelp = "�ն�Ԥ��ǰX�����������";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
				pPropItem->m_idProp = idProp;
			}
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			pPropItem->m_dwPropGroup = dwPropGroup;
			//Y
			idProp=CLDSLineAngle::GetPropID("m_xEndBeforeLift.y");
			pPropItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "Y coordinate of start before pre-lift";
#else
				lpInfo->m_strPropHelp = "�ն�Ԥ��ǰY�����������";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
				pPropItem->m_idProp = idProp;
			}
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			pPropItem->m_dwPropGroup = dwPropGroup;
			//Z
			idProp=CLDSLineAngle::GetPropID("m_xEndBeforeLift.z");
			pPropItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "Z coordinate of start before pre-lift";
#else
				lpInfo->m_strPropHelp = "�ն�Ԥ��ǰZ�����������";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
				pPropItem->m_idProp = idProp;
			}
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly();
			pPropItem->m_dwPropGroup = dwPropGroup;
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_xEndBeforeLift"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_xEndBeforeLift.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_xEndBeforeLift.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_xEndBeforeLift.z"));
		}
	}		
}
void UpdateLineAngle_DesPos_SimpleHuoquJg_TreeItem(CPropertyList *pPropList,CPropTreeItem *pLeaderItem,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	CPropTreeItem *pSonItem,*pLeafItem=NULL,*pParentItem=NULL;
	int idProp;
	CItemInfo *lpInfo;
	if(pFirstLineAngle==NULL||pLeaderItem==NULL)
		return;
	CXhChar500 sText;
	CMultiOffsetPos *pDesPos=bStart ? &pFirstLineAngle->desStartPos : &pFirstLineAngle->desEndPos;
	const int GROUP_GENERAL=1,GROUP_STATMAT=2,GROUP_START=3,GROUP_END=4,GROUP_DESIGN=5;
	DWORD dwPropGroup=bStart ? GetSingleWord(GROUP_START) : GetSingleWord(GROUP_END);
	if(pDesPos->IsInHuoQuLine()||pDesPos->IsInOffsetZhunLine())
	{
		//�����߻�׼�Ǹ�
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.SimpleBendLineAlongAngle");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.SimpleBendLineAlongAngle");
		pSonItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pSonItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Bending Datum Angle";
			lpInfo->m_strPropHelp = "Bending Datum Angle";
#else
			lpInfo->m_strPropName = "�����߻�׼�Ǹ�";
			lpInfo->m_strPropHelp = "";
#endif
			pSonItem = pPropList->InsertItem(pLeaderItem,lpInfo,2,true);
			pSonItem->m_idProp = idProp;
			pSonItem->m_dwPropGroup=dwPropGroup;
		}
		pSonItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		BOOL defaultReadOnlyState=pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp);
		if(bStart)
			pSonItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||defaultReadOnlyState);
		else
			pSonItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||defaultReadOnlyState);
		//��ǰ����֫
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.SimpleBendAngleWing");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.SimpleBendAngleWing");
		pLeafItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Work Leg";
			lpInfo->m_strPropHelp = "Work leg of bending datum angle";
			lpInfo->m_cmbItems = "Default|X Leg|Y Leg";
#else
			lpInfo->m_strPropName = "��ǰ����֫";
			lpInfo->m_strPropHelp = "��ǰ����֫";
			lpInfo->m_cmbItems = "0.�Զ��ж�|1.X֫|2.Y֫";
#endif
			pLeafItem = pPropList->InsertItem(pSonItem,lpInfo,-1,true);
			pLeafItem->m_idProp = idProp;
			pLeafItem->m_dwPropGroup=dwPropGroup;
		}
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		defaultReadOnlyState=pFirstLineAngle->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp);
		if(bStart)
			pLeafItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||defaultReadOnlyState);
		else
			pLeafItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||defaultReadOnlyState);
		//������϶
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.SimpleBendClearance");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.SimpleBendClearance");
		pLeafItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Normal Offset";
			lpInfo->m_strPropHelp = "X normal's offset";
#else
			lpInfo->m_strPropName = "������϶";
			lpInfo->m_strPropHelp = "�ع���ƽ�淨��ƫ����������ڽǸ�ʱ�����Ǹִ��֫����Ϊ���򣬴���ڸֹ�ʱ��ǰ����֫����Ϊ����";
#endif
			pLeafItem = pPropList->InsertItem(pSonItem,lpInfo,-1,true);
			pLeafItem->m_idProp = idProp;
			pLeafItem->m_dwPropGroup=dwPropGroup;
		}
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		defaultReadOnlyState=pFirstLineAngle->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp);
		if(bStart)
			pLeafItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||defaultReadOnlyState);
		else
			pLeafItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||defaultReadOnlyState);
		if(pDesPos->IsInOffsetZhunLine())
		{	//ƫ��׼��
			if(bStart)
				idProp=CLDSLineAngle::GetPropID("desStartPos.SimpleOffsetZhunju");
			else
				idProp=CLDSLineAngle::GetPropID("desEndPos.SimpleOffsetZhunju");
			pLeafItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pLeafItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Offset G";
				lpInfo->m_strPropHelp = "Offset stadia";
#else
				lpInfo->m_strPropName = "ƫ��׼��";
				lpInfo->m_strPropHelp = "ƫ��׼��";
#endif
				pLeafItem = pPropList->InsertItem(pSonItem,lpInfo,-1,true);
				pLeafItem->m_idProp = idProp;
				pLeafItem->m_dwPropGroup=dwPropGroup;
			}
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
			defaultReadOnlyState=pFirstLineAngle->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp);
			if(bStart)
				pLeafItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||defaultReadOnlyState);
			else
				pLeafItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||defaultReadOnlyState);
		}
		else
		{
			if(bStart)
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleOffsetZhunju"));
			else
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleOffsetZhunju"));
		}

	}
	else
	{
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleBendLineAlongAngle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleBendAngleWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleBendClearance"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleOffsetZhunju"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleBendLineAlongAngle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleBendAngleWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleBendClearance"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleOffsetZhunju"));
		}
	}
}
void UpdateLineAngle_DesPos_SimpleDatumPoint_TreeItem(CPropertyList *pPropList,CPropTreeItem *pLeaderItem,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem,*pSonItem,*pLeafItem=NULL,*pParentItem=NULL;
	int idProp;
	CItemInfo *lpInfo;
	if(pFirstLineAngle==NULL||pLeaderItem==NULL)
		return;
	CMultiOffsetPos *pDesPos=bStart ? &pFirstLineAngle->desStartPos : &pFirstLineAngle->desEndPos;
	const int GROUP_GENERAL=1,GROUP_STATMAT=2,GROUP_START=3,GROUP_END=4,GROUP_DESIGN=5;
	DWORD dwPropGroup=bStart ? GetSingleWord(GROUP_START) : GetSingleWord(GROUP_END);
	CXhChar500 sText;
	pPropList->DeleteAllSonItems(pLeaderItem);
	if(pDesPos->m_iSimpleDatumPoint==0)
	{	//ָ�����㶨λ�����Ϣ
		pDesPos->datumPoint.UpdatePos(pFirstLineAngle->BelongModel());
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datumPoint");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datumPoint");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Point";
			lpInfo->m_strPropHelp = "Datum point's position";
#else
			lpInfo->m_strPropName = "����λ��";
			lpInfo->m_strPropHelp = "����λ��";
#endif
			pLeaderItem->m_bHideChildren=FALSE;
			pPropItem = pPropList->InsertItem(pLeaderItem ,lpInfo,0,true);
			pPropItem->m_idProp = idProp;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//����λ��X�������
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datumPoint.x");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datumPoint.x");
		pSonItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X";
			lpInfo->m_strPropHelp = "X coordinate of datum point's position";
#else
			lpInfo->m_strPropName = "X����";
			lpInfo->m_strPropHelp = "����λ��X����";
#endif
			pSonItem = pPropList->InsertItem(pPropItem,lpInfo,0,true);
			pSonItem->m_idProp = idProp;
			pSonItem->m_dwPropGroup=dwPropGroup;
		}
		pSonItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
			pFirstLineAngle->desStartPos.datumPoint.datum_pos_style!=0||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp));
		else
			pSonItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
			pFirstLineAngle->desEndPos.datumPoint.datum_pos_style!=0||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp));
		//����λ��Y�������
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datumPoint.y");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datumPoint.y");
		pSonItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y";
			lpInfo->m_strPropHelp = "Y coordinate of datum point's position";
#else
			lpInfo->m_strPropName = "Y����";
			lpInfo->m_strPropHelp = "����λ��Y����";
#endif
			pSonItem = pPropList->InsertItem(pPropItem,lpInfo,1,true);
			pSonItem->m_idProp = idProp;
			pSonItem->m_dwPropGroup=dwPropGroup;
		}
		pSonItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
			pFirstLineAngle->desStartPos.datumPoint.datum_pos_style!=0||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp));
		else
			pSonItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
			pFirstLineAngle->desEndPos.datumPoint.datum_pos_style!=0||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp));
		//����λ��Z�������
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datumPoint.z");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datumPoint.z");
		pSonItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Z";
			lpInfo->m_strPropHelp = "Z coordinate of datum point's position";
#else
			lpInfo->m_strPropName = "Z����";
			lpInfo->m_strPropHelp = "����λ��Z����";
#endif
			pSonItem = pPropList->InsertItem(pPropItem,lpInfo,2,true);
			pSonItem->m_idProp = idProp;
			pSonItem->m_dwPropGroup=dwPropGroup;
		}
		pSonItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pSonItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
			pFirstLineAngle->desStartPos.datumPoint.datum_pos_style!=0||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp));
		else
			pSonItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
			pFirstLineAngle->desEndPos.datumPoint.datum_pos_style!=0||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp));
	}
	else if(pDesPos->m_iSimpleDatumPoint==1)
	{	//���Ƶ�
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleDenpendWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleDependRod"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleLenOffsetType"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleLenOffset"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleNormOffset"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleBendLineAlongAngle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleBendAngleWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleBendClearance"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleOffsetZhunju"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleDenpendWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleDependRod"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleLenOffsetType"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleLenOffset"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleNormOffset"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleBendLineAlongAngle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleBendAngleWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleBendClearance"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleOffsetZhunju"));
		}
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.SimpleWingXTipCtrlPoint");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.SimpleWingXTipCtrlPoint");
		pSonItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pSonItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "depend rod";
			lpInfo->m_strPropHelp = "depend rod handle";
			lpInfo->m_cmbItems="Project|X,Z��Y,Z|X,Y|X|Y|Z";
#else
			lpInfo->m_strPropName = "X(��)֫��������";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems="�����ж�|-|�������|X,Z����|Y,Z����|X,Y����|X����|Y����|Z����";
#endif
			pSonItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
			pSonItem->m_idProp = idProp;
			pSonItem->m_dwPropGroup=dwPropGroup;
		}
		if(!pDesPos->bUdfCtrlCoordsMode)	//�����ж�ģʽʱ,����ʵ�ʿ���������ʲô��ֻ�ڵ�һ���˵����
			pSonItem->m_lpNodeInfo->SetCheck(1);
		else	//ָ����������ʱ,��ֱ���ڵ�ǰģʽ�˵����
			pSonItem->m_lpNodeInfo->SetCheck(-1);
		pSonItem->m_lpNodeInfo->m_bMutiObjsProp = (!pDesPos->bUdfCtrlCoordsMode)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		BOOL defaultReadOnlyState=pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp);
		if(bStart)
			pSonItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||defaultReadOnlyState);
		else
			pSonItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||defaultReadOnlyState);
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.SimpleWingYTipCtrlPoint");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.SimpleWingYTipCtrlPoint");
		pSonItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pSonItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "depend rod";
			lpInfo->m_strPropHelp = "depend rod handle";
			lpInfo->m_cmbItems="Project|X,Z��Y,Z|X,Y|X|Y|Z";
#else
			lpInfo->m_strPropName = "Y(��)֫��������";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems="�����ж�|-|�������|X,Z����|Y,Z����|X,Y����|X����|Y����|Z����";
#endif
			pSonItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
			pSonItem->m_idProp = idProp;
			pSonItem->m_dwPropGroup=dwPropGroup;
		}
		if(!pDesPos->bUdfCtrlCoordsMode)	//�����ж�ģʽʱ,����ʵ�ʿ���������ʲô��ֻ�ڵ�һ���˵����
			pSonItem->m_lpNodeInfo->SetCheck(1);
		else	//ָ����������ʱ,��ֱ���ڵ�ǰģʽ�˵����
			pSonItem->m_lpNodeInfo->SetCheck(-1);
		pSonItem->m_lpNodeInfo->m_bMutiObjsProp = (!pDesPos->bUdfCtrlCoordsMode)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		defaultReadOnlyState=pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp);
		if(bStart)
			pSonItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||defaultReadOnlyState);
		else
			pSonItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||defaultReadOnlyState);
	}
	else if(pDesPos->m_iSimpleDatumPoint==2)
	{
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleWingXTipCtrlPoint"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleWingYTipCtrlPoint"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleWingXTipCtrlPoint"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleWingYTipCtrlPoint"));
		}
		//Ŀ���׼��
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.SimpleDestLineMode");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.SimpleDestLineMode");
		pSonItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pSonItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "depend rod";
			lpInfo->m_strPropHelp = "depend rod handle";
			lpInfo->m_cmbItems="Auto|-|Ridge line|Axis line|Bend line|Offset axis";
#else
			lpInfo->m_strPropName = "Ŀ���׼��";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems="�����ж�|-|��������|��������|���ڻ�����|����ƫ��׼��";
#endif
			pSonItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
			pSonItem->m_idProp = idProp;
			pSonItem->m_dwPropGroup=dwPropGroup;
		}
		if(pDesPos->ciDestLineMode==0)	//�����ж�
			pSonItem->m_lpNodeInfo->SetCheck(1);
		else
			pSonItem->m_lpNodeInfo->SetCheck(-1);
		pSonItem->m_lpNodeInfo->m_bMutiObjsProp = (pDesPos->ciDestLineMode==0)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		BOOL defaultReadOnlyState=pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp);
		if(bStart)
			pSonItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||defaultReadOnlyState);
		else
			pSonItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||defaultReadOnlyState);
		//��������˼�
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.SimpleDependRod");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.SimpleDependRod");
		pSonItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pSonItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "depend rod";
			lpInfo->m_strPropHelp = "depend rod handle";
#else
			lpInfo->m_strPropName = "��������˼�";
			lpInfo->m_strPropHelp = "�������ʱ������ӵ������˼���ʶ���";
#endif
			pSonItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
			pSonItem->m_idProp = idProp;
			pSonItem->m_dwPropGroup=dwPropGroup;
		}
		pSonItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		defaultReadOnlyState=pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp);
		if(bStart)
			pSonItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||defaultReadOnlyState);
		else
			pSonItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||defaultReadOnlyState);
		//�������֫
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.SimpleDenpendWing");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.SimpleDenpendWing");
		pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Length Extend|1.Parallel Offset|2.X|3.Y|4.Z";
			lpInfo->m_strPropName = "Datum Plane Offset Type";
			lpInfo->m_strPropHelp = "Datum point's offset type on working datum plane";
#else
			lpInfo->m_cmbItems="�����ж�|-|X֫|Y֫";
			lpInfo->m_strPropName = "�������֫";
			lpInfo->m_strPropHelp = "�����ڹ��������ڵ�ƫ��������";
#endif
			pLeafItem = pPropList->InsertItem(pSonItem,lpInfo,-1,true);
			pLeafItem->m_idProp = idProp;
			pLeafItem->m_dwPropGroup=dwPropGroup;
		}
		if(!pDesPos->bUdfLappedDatumWing)
			pLeafItem->m_lpNodeInfo->SetCheck(1);
		else
			pLeafItem->m_lpNodeInfo->SetCheck(-1);
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = (!pDesPos->bUdfLappedDatumWing)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pLeafItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
		else
			pLeafItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
#ifdef __NEW_ANGLE_LOCA_MODE_
		//��ӻ����ִ���ľ�
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.LappedDatumWingG");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.LappedDatumWingG");
		pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_POPMENUEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="Automatic Decide";
			lpInfo->m_strPropName = "Datum Plane Offset Type";
			lpInfo->m_strPropHelp = "Datum point's offset type on working datum plane";
#else
			lpInfo->m_cmbItems = "�Զ��ж�";
			lpInfo->m_strPropName = "����ľ�";
			lpInfo->m_strPropHelp = "��׼������֫�ϴ�ӻ����ִ���ľ�";
#endif
			pLeafItem = pPropList->InsertItem(pSonItem,lpInfo,-1,true);
			pLeafItem->m_idProp = idProp;
			pLeafItem->m_dwPropGroup=dwPropGroup;
		}
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = (pDesPos->datumWingOffset.gStyle!=4)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pLeafItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
		else
			pLeafItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
#endif
		//��ӻ���ƫ������
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.SimpleLenOffsetType");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.SimpleLenOffsetType");
		pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Length Extend|1.Parallel Offset|2.X|3.Y|4.Z";
			lpInfo->m_strPropName = "Datum Plane Offset Type";
			lpInfo->m_strPropHelp = "Datum point's offset type on working datum plane";
#else
			lpInfo->m_cmbItems="0.��������|1.����ƽ��ƫ��|2.X�������|3.Y�������|4.Z�������";
			lpInfo->m_strPropName = "����ƫ������";
			lpInfo->m_strPropHelp = "�����ڹ��������ڵ�ƫ��������";
#endif
			pLeafItem = pPropList->InsertItem(pSonItem,lpInfo,-1,true);
			pLeafItem->m_idProp = idProp;
			pLeafItem->m_dwPropGroup=dwPropGroup;
		}
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pLeafItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
		else
			pLeafItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
		//�������췽��ƫ����
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.SimpleLenOffset");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.SimpleLenOffset");
		pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Plane Offset";
			lpInfo->m_strPropHelp = "Datum point's offset on working datum plane";
#else
			lpInfo->m_strPropName = "����ƫ����";
			lpInfo->m_strPropHelp = "�����ڹ��������ڵ�ƫ����";
#endif
			pLeafItem = pPropList->InsertItem(pSonItem,lpInfo,-1,true);
			pLeafItem->m_idProp = idProp;
			pLeafItem->m_dwPropGroup=dwPropGroup;
		}
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pLeafItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
		else
			pLeafItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
			pFirstLineAngle->GetObjPropReadOnlyDefaultState(pLeafItem->m_idProp));
		UpdateLineAngle_DesPos_SimpleHuoquJg_TreeItem(pPropList,pLeaderItem,bStart);
	}
	else
		pPropList->DeleteAllSonItems(pLeaderItem);
	//
	if(pDesPos->m_iSimpleDatumPoint==0||pDesPos->m_iSimpleDatumPoint==2)
	{	//������֫
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.SimpleSelfWorkWing");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.SimpleSelfWorkWing");
		pSonItem=pPropList->FindItemByPropId(idProp,NULL);
		if(pSonItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "depend rod";
			lpInfo->m_strPropHelp = "depend rod handle";
			lpInfo->m_cmbItems="Auto|-|Axis line|Bend line|Offset axis";
#else
			lpInfo->m_strPropName = "��λ����֫";
			lpInfo->m_strPropHelp = "";
			lpInfo->m_cmbItems="�����ж�|-|X֫|Y֫|˫֫";
#endif
			pSonItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
			pSonItem->m_idProp = idProp;
			pSonItem->m_dwPropGroup=dwPropGroup;
		}
		if(pDesPos->cWorkWingStyle==0)	//�����ж�
			pSonItem->m_lpNodeInfo->SetCheck(1);
		else
			pSonItem->m_lpNodeInfo->SetCheck(-1);
		pSonItem->m_lpNodeInfo->m_bMutiObjsProp = (pDesPos->cWorkWingStyle==0)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pSonItem->m_idProp,sText)>0)
			pSonItem->m_lpNodeInfo->m_strPropValue = sText;
		BOOL defaultReadOnlyState=pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp);
		if(bStart)
			pSonItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||defaultReadOnlyState);
		else
			pSonItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||defaultReadOnlyState);
		char cSelfWorkWing=pFirstLineAngle->GetSelfWorkWing(bStart,NULL);
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.SimpleNormOffset");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.SimpleNormOffset");
		pSonItem=pPropList->FindItemByPropId(idProp,NULL);
		//if(cSelfWorkWing=='X'||cSelfWorkWing=='Y')	//˫֫ƫ��ʱҲӦ�����빤��ƽ�淨��ƫ��
		{	//����ƽ�淨��ƫ��
			if(pSonItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
	#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "X Normal Offset";
				lpInfo->m_strPropHelp = "X normal's offset";
	#else
				lpInfo->m_strPropName = "��������ƫ����";
				lpInfo->m_strPropHelp = "�ع���ƽ�淨��ƫ����������ڽǸ�ʱ�����Ǹִ��֫����Ϊ���򣬴���ڸֹ�ʱ��ǰ����֫����Ϊ����";
	#endif
				pSonItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
				pSonItem->m_idProp = idProp;
				pSonItem->m_dwPropGroup=dwPropGroup;
			}
			pSonItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pSonItem->m_idProp,sText)>0)
				pSonItem->m_lpNodeInfo->m_strPropValue = sText;
			defaultReadOnlyState=pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonItem->m_idProp);
			if(bStart)
				pSonItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||defaultReadOnlyState);
			else
				pSonItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||defaultReadOnlyState);
		}
		//else if(pSonItem)
		//	pPropList->DeleteItemByPropId(pSonItem->m_idProp);
	}
}
void UpdateLineAngle_DesPos_JgCalStyle_TreeItem(CPropertyList *pPropList,CPropTreeItem *pLeaderItem,BOOL bStart)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	CPropTreeItem *pPropItem,*pLeafItem=NULL,*pParentItem=NULL;
	int idProp;
	CItemInfo *lpInfo;
	if(pFirstLineAngle==NULL||pLeaderItem==NULL)
		return;
	CMultiOffsetPos *pDesPos=bStart ? &pFirstLineAngle->desStartPos : &pFirstLineAngle->desEndPos;
	const int GROUP_GENERAL=1,GROUP_STATMAT=2,GROUP_START=3,GROUP_END=4,GROUP_DESIGN=5;
	DWORD dwPropGroup=bStart ? GetSingleWord(GROUP_START) : GetSingleWord(GROUP_END);
	CXhChar500 sText;
	if(pDesPos->jgber_cal_style>0)
	{	//ָ������
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datum_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.origin_pos_style"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.cDatumAngleWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumWingOffset"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.operStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.gStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.offsetDist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumPoint.z"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.spatialOperationStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datum_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.len_offset_type"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.len_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.offset_zhun"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.huoqu_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.huoqu_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.z"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.cFaceOffsetDatumLine"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.hDatumLine"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.startPos"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.startPos.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.startPos.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.startPos.z"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.endPos"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.endPos.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.endPos.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLine.endPos.z"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLineStart"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datumLineEnd"));
			//����spatialOperationStyle=15ʱ��Ч
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.coordCtrlType"));		//0:X,Z����;1:Y,Z����;X,Y����
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.cMainCoordCtrlWing"));	//�Ǹ�����֫ 'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.ctrlWing"));			//�Ǹ���Ҫ����֫(�����������������֫)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.ctrlWing.operStyle"));  //�Ǹ���Ҫ����֫(�����������������֫)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.ctrlWing.gStyle"));		//�Ǹ���Ҫ����֫(�����������������֫)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.ctrlWing.offsetDist"));	//�Ǹ���Ҫ����֫(�����������������֫)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.otherWing"));			//�Ǹָ�������֫(����һ�����������֫)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.otherWing.operStyle"));	//�Ǹָ�������֫(����һ�����������֫)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.otherWing.gStyle"));	//�Ǹָ�������֫(����һ�����������֫)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.otherWing.offsetDist"));//�Ǹָ�������֫(����һ�����������֫);
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.fLenOffsetOfCoordWing"));
			/*pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.start_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.iStartJgBerStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.fStartJgLengOffsetX"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.fStartJgLengOffsetY"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.projectDatumLineEndInfo"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.end_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.iEndJgBerStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.fEndJgLengOffsetX"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.fEndJgLengOffsetY"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.bByOtherEndOffset1"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm2"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm2.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm2.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm2.z"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.bByOtherEndOffset2"));*/
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.datum_to_ber_style"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.cWorkWingStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.wing_x_offset.gStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.wing_x_offset.offsetDist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.wing_y_offset.gStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.wing_y_offset.offsetDist"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datum_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.origin_pos_style"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.cDatumAngleWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumWingOffset"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.operStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.gStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.offsetDist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumPoint.z"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.spatialOperationStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datum_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.len_offset_type"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.len_offset_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.offset_zhun"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.huoqu_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.huoqu_dist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.z"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.fEccentricDist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.cFaceOffsetDatumLine"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.hDatumLine"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.startPos"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.startPos.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.startPos.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.startPos.z"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.endPos"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.endPos.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.endPos.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLine.endPos.z"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLineStart"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datumLineEnd"));
			//����spatialOperationStyle=15ʱ��Ч
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.coordCtrlType"));		//0:X,Z����;1:Y,Z����;X,Y����
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.cMainCoordCtrlWing"));	//�Ǹ�����֫ 'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.ctrlWing"));			//�Ǹ���Ҫ����֫(�����������������֫)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.ctrlWing.operStyle"));  //�Ǹ���Ҫ����֫(�����������������֫)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.ctrlWing.gStyle"));		//�Ǹ���Ҫ����֫(�����������������֫)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.ctrlWing.offsetDist"));	//�Ǹ���Ҫ����֫(�����������������֫)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.otherWing"));			//�Ǹָ�������֫(����һ�����������֫)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.otherWing.operStyle"));	//�Ǹָ�������֫(����һ�����������֫)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.otherWing.gStyle"));	//�Ǹָ�������֫(����һ�����������֫)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.otherWing.offsetDist"));//�Ǹָ�������֫(����һ�����������֫);
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.fLenOffsetOfCoordWing"));
			/*pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.start_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.iStartJgBerStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.fStartJgLengOffsetX"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.fStartJgLengOffsetY"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.projectDatumLineEndInfo"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.end_jg_h"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.iEndJgBerStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.fEndJgLengOffsetX"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.fEndJgLengOffsetY"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.bByOtherEndOffset1"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm2"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm2.x"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm2.y"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm2.z"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.bByOtherEndOffset2"));*/
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.datum_to_ber_style"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.cWorkWingStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.wing_x_offset.gStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.wing_x_offset.offsetDist"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.wing_y_offset.gStyle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.wing_y_offset.offsetDist"));
		}
		if(pDesPos->jgber_cal_style==1)
		{
			if(bStart)
			{
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleDatumPoint"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleDenpendWing"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleDependRod"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleLenOffsetType"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleLenOffset"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleNormOffset"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleWingXTipCtrlPoint"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleWingYTipCtrlPoint"));
			}
			else
			{
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleDatumPoint"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleDenpendWing"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleDependRod"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleLenOffsetType"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleLenOffset"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleNormOffset"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleWingXTipCtrlPoint"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleWingYTipCtrlPoint"));
			}
		}
		else if(pDesPos->jgber_cal_style==2)
		{
			//for(CLDSLineAngle* pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			//	bStart?pLineAngle->IntelliUpdateStartSimplePosParam():pLineAngle->IntelliUpdateEndSimplePosParam();
			//���㶨λ����
			if(bStart)
				idProp=CLDSLineAngle::GetPropID("desStartPos.SimpleDatumPoint");
			else
				idProp=CLDSLineAngle::GetPropID("desEndPos.SimpleDatumPoint");
			pPropItem=pPropList->FindItemByPropId(idProp,NULL);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_cmbItems="Auto|-|Specified|By Node|Lapped|Co-linear";
				lpInfo->m_strPropName = "depend rod";
				lpInfo->m_strPropHelp = "depend rod handle";
#else
				lpInfo->m_cmbItems="�����ж�|-|ָ������|�ڵ����|��Ӷ�λ|���߶�λ";
				lpInfo->m_strPropName = "��λ����";
				lpInfo->m_strPropHelp = "�Ǹ���㶨λ����";
#endif
				pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
				pPropItem->m_idProp = idProp;
				pPropItem->m_dwPropGroup=dwPropGroup;
			}
			if(!pDesPos->bUdfDatumPointMode)
				pPropItem->m_lpNodeInfo->SetCheck(1);
			else
				pPropItem->m_lpNodeInfo->SetCheck(-1);
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = (!pDesPos->bUdfDatumPointMode)||!CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			UpdateLineAngle_DesPos_SimpleDatumPoint_TreeItem(pPropList,pPropItem,bStart);
		}
	}
	else //if(pDesPos->jgber_cal_style==0)
	{	//��������λ
		if(bStart)
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleDatumPoint"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleDenpendWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleDependRod"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleLenOffsetType"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleLenOffset"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleNormOffset"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleBendLineAlongAngle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleBendAngleWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleBendClearance"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleOffsetZhunju"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleWingXTipCtrlPoint"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desStartPos.SimpleWingYTipCtrlPoint"));
		}
		else
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleDatumPoint"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleDenpendWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleDependRod"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleLenOffsetType"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleLenOffset"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleNormOffset"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleBendLineAlongAngle"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleBendAngleWing"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleBendClearance"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleOffsetZhunju"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleWingXTipCtrlPoint"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("desEndPos.SimpleWingYTipCtrlPoint"));
		}
		//���λ�������˼�
		//A. ��ʼ�㶨λ
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.origin_pos_style");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.origin_pos_style");
		if(pDesPos->spatialOperationStyle==16||pDesPos->spatialOperationStyle==17)
		{
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Origin Position Style";
				lpInfo->m_strPropHelp = "Confirm calculation style of origin position point which angle end datum point position need";
				lpInfo->m_cmbItems = "Node|Intelligent";//1.��׼��|2.��׼�Ǹֹؼ���";//|3.��һ�˽ڵ�����|4.��һ���������";
#else
				lpInfo->m_strPropName = "��ʼ��λ��";
				lpInfo->m_strPropHelp = "ȷ���Ǹֶ˻���λ�������ʼ��λ��ļ��㷽ʽ";
				lpInfo->m_cmbItems = "�ڵ�λ��|����ʶ��";//1.��׼��|2.��׼�Ǹֹؼ���";//|3.��һ�˽ڵ�����|4.��һ���������";
#endif
				pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo, 0,true);
				pPropItem->m_idProp = idProp;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
				pPropItem->m_dwPropGroup = dwPropGroup;
			}
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
				pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			if( (bStart&&pFirstLineAngle->pStart&&pFirstLineAngle->pStart->m_cPosCalType==4) ||
				(!bStart&&pFirstLineAngle->pEnd&&pFirstLineAngle->pEnd->m_cPosCalType==4))
				pPropItem->SetReadOnly();	//�����ǿ��Ϊ����ʶ������
			UpdateLineAngle_DesPos_OrgPosStyle_TreeItem(pPropList,pPropItem,bStart);
		}
		else
			pPropList->DeleteItemByPropId(idProp);
		//B. ���㶨λ
		//���㶨λ��ʽ
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.spatialOperationStyle");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.spatialOperationStyle");
		pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Datum Point Position Style";
			lpInfo->m_strPropHelp = "The spatial operation style from origin point to datum point.";
			lpInfo->m_cmbType=1;
			lpInfo->m_cmbItems = "Specify|Ridge Project|X Leg Project|Y Leg Project|Ridge Plane Offset|X Leg Plane Offset|Y Leg Plane Offset|Ridge Bend Line Project|X Leg Bend Line Project|Y Leg Bend Line Project|Ridge Bend Line Plane Offset|X Leg Bend Line Plane Offset|Y Leg Bend Line Plane Offset|X Leg Offset Line|Y Leg Offset Line|Legs Coordinate Control|Project To Line |Plane Offset To Line|Node Project|Node Plane Offset";
#else
			lpInfo->m_strPropName = "���㶨λ��ʽ";
			lpInfo->m_strPropHelp = "�ɳ�ʼ��->����Ŀռ���㶨λ��ʽ";
			lpInfo->m_cmbType=1;
#ifndef	__NEW_ANGLE_LOCA_MODE_
			lpInfo->m_cmbItems = "ָ����׼��|����(��)��ͶӰ|X֫����ͶӰ|Y֫����ͶӰ|������ƽ��|X֫����ƽ��|Y֫����ƽ��|�����߻�����ͶӰ|X֫���߻�����ͶӰ|Y֫���߻�����ͶӰ|�����߻�����ƽ��|X֫���߻�����ƽ��|Y֫���߻�����ƽ��|X֫ƫ��׼��|Y֫ƫ��׼��|��֫�������|ָ����ͶӰ|ָ����ƽ��|������Ƶ�ͶӰ|������Ƶ�ƽ��";
#else
			lpInfo->m_cmbItems = "ָ����׼��|����(��)��ͶӰ|X֫����ͶӰ|Y֫����ͶӰ|������ƽ��|X֫����ƽ��|Y֫����ƽ��|�����߻�����ͶӰ|X֫���߻�����ͶӰ|Y֫���߻�����ͶӰ|�����߻�����ƽ��|X֫���߻�����ƽ��|Y֫���߻�����ƽ��|X֫ƫ��׼��|Y֫ƫ��׼��|�ڵ��������|ָ����ͶӰ|ָ����ƽ��";
#endif
#endif
			pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
			pPropItem->m_idProp = idProp;
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=dwPropGroup;
		}
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		if(bStart)
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		else
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		UpdateLineAngle_DesPos_DatumPosStyle_TreeItem(pPropList,pPropItem,bStart);
		//
		int spatialOperationStyle=pFirstLineAngle->desEndPos.spatialOperationStyle;
		if(bStart)
			spatialOperationStyle=pFirstLineAngle->desStartPos.spatialOperationStyle;
		if(bStart)
			idProp=CLDSLineAngle::GetPropID("desStartPos.datum_to_ber_style");
		else
			idProp=CLDSLineAngle::GetPropID("desEndPos.datum_to_ber_style");
#ifndef __NEW_ANGLE_LOCA_MODE_
		if(spatialOperationStyle>17||spatialOperationStyle<15)
#else
		if (spatialOperationStyle!=15&& spatialOperationStyle != 18 && spatialOperationStyle != 19)
#endif
		{	//����->��㶨λ��ʽ
			pPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Datum To Ridge Point Position";
				lpInfo->m_strPropHelp = "The vector from datum point to ridge point offset position style";
#else
				lpInfo->m_strPropName = "����->��㶨λ��ʽ";
				lpInfo->m_strPropHelp = "�ɻ���->���ʸ��ƫ�ƶ�λ��ʽ";
#endif
				pPropItem = pPropList->InsertItem(pLeaderItem,lpInfo,-1,true);
				pPropItem->m_idProp = idProp;
				pPropItem->m_dwPropGroup=dwPropGroup;
			}
			pPropItem->SetReadOnly(TRUE);
			if(bStart)
				pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			else
				pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			UpdateLineAngle_DesPos_DatumToBerStyle_TreeItem(pPropList,pPropItem,bStart);
		}
		else 
			pPropList->DeleteItemByPropId(idProp);
	}
	
	if(bStart)
	{	
		if(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart)
		{	//�������Ϊֻ������
			SetPropItemReadOnly(pFirstLineAngle,"startPos.x",TRUE);
			SetPropItemReadOnly(pFirstLineAngle,"startPos.y",TRUE);
			SetPropItemReadOnly(pFirstLineAngle,"startPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ���ж�
			SetPropItemReadOnly(pFirstLineAngle,"startPos.x",pFirstLineAngle->desStartPos.jgber_cal_style<1||
								pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("startPos.x")));
			SetPropItemReadOnly(pFirstLineAngle,"startPos.y",pFirstLineAngle->desStartPos.jgber_cal_style<1||
								pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("startPos.y")));
			SetPropItemReadOnly(pFirstLineAngle,"startPos.z",pFirstLineAngle->desStartPos.jgber_cal_style<1||
								pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("startPos.z")));
		}
	}
	else
	{
		if(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd)
		{	//�������Ϊֻ������
			SetPropItemReadOnly(pFirstLineAngle,"endPos.x",TRUE);
			SetPropItemReadOnly(pFirstLineAngle,"endPos.y",TRUE);
			SetPropItemReadOnly(pFirstLineAngle,"endPos.z",TRUE);
		}
		else
		{	//δ����ʱ���ݶ�λ��ʽ���ж�
			SetPropItemReadOnly(pFirstLineAngle,"endPos.x",pFirstLineAngle->desEndPos.jgber_cal_style<1||
								pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("endPos.x")));
			SetPropItemReadOnly(pFirstLineAngle,"endPos.y",pFirstLineAngle->desEndPos.jgber_cal_style<1||
								pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("endPos.y")));
			SetPropItemReadOnly(pFirstLineAngle,"endPos.z",pFirstLineAngle->desEndPos.jgber_cal_style<1||
								pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("endPos.z")));
		}
	}
}
void UpdateLineAngleOddProperty(CPropertyList* pPropList,CPropTreeItem* pParentItem,BOOL bStart/*=TRUE*/)
{
	CXhChar500 sText;
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	pPropList->DeleteAllSonItems(pParentItem);
	//��ײ��϶
	CItemInfo* lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_buttonType = BDT_POPMENUEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Collision clearance";
	if(bStart)
		lpInfo->m_strPropHelp = "The collision clearance of angle start oddment.";
	else
		lpInfo->m_strPropHelp = "The collision clearance of angle end oddment.";
	lpInfo->m_cmbItems="Ends calculate|Edge Cutting|Cutting by clearance";
#else
	lpInfo->m_strPropName = "��ײ��϶";
	if(bStart)
		lpInfo->m_strPropHelp = "�Ǹ�ʼ������ͷ����ײ��϶";
	else
		lpInfo->m_strPropHelp = "�Ǹ��ն�����ͷ����ײ��϶";
	lpInfo->m_cmbItems="��������ͷ|�����н�|����϶�н�";
#endif
	CPropTreeItem* pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
	if(bStart)
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("oddStart.m_fCollideDist");
	else
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("oddEnd.m_fCollideDist");
	if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
	//��˨�������ͷʱ����һ����ײ���ո˼��������нǻ�׼�Ǹֵ����ã��ʲ�����Ϊֻ�� wjh-2015.5.6
	//if(bStart)
	//	pSonPropItem->SetReadOnly(pFirstLineAngle->desStartOdd.m_iOddCalStyle==1);
	//else
	//	pSonPropItem->SetReadOnly(pFirstLineAngle->desEndOdd.m_iOddCalStyle==1);
	//���ո˼�1
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_BUTTON;
	lpInfo->m_buttonType  = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Reference Part 1";
	lpInfo->m_strPropHelp = "Reference Part 1 to calculate angle ends.";
#else
	lpInfo->m_strPropName = "���չ���1";
	lpInfo->m_strPropHelp = "����Ǹ�����ͷ�Ĳ��ո˼�1";
#endif
	pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
	if(bStart)
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("oddStart.m_hRefPart1");
	else
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("oddEnd.m_hRefPart1");
	if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
	//��˨�������ͷʱ����һ����ײ���ո˼��������нǻ�׼�Ǹֵ����ã��ʲ�����Ϊֻ�� wjh-2015.5.6
	//if(bStart)
	//	pSonPropItem->SetReadOnly(pFirstLineAngle->desStartOdd.m_iOddCalStyle==1);
	//else
	//	pSonPropItem->SetReadOnly(pFirstLineAngle->desEndOdd.m_iOddCalStyle==1);
	//���ո˼�2
	lpInfo = new CItemInfo();
	lpInfo->m_controlType=PIT_BUTTON;
	lpInfo->m_buttonType=BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Reference Rod 2";
	lpInfo->m_strPropHelp = "Reference rod 2 to calculate angle oddment.";
#else
	lpInfo->m_strPropName = "���ո˼�2";
	lpInfo->m_strPropHelp = "����Ǹ�����ͷ�Ĳ��ո˼�2";
#endif
	pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo,-1,TRUE);
	if(bStart)
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("oddStart.m_hRefPart2");
	else
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("oddEnd.m_hRefPart2");
	if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
		pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
	pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
	pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
	//��˨�������ͷʱ����һ����ײ���ո˼��������нǻ�׼�Ǹֵ����ã��ʲ�����Ϊֻ�� wjh-2015.5.6
	//if(bStart)
	//	pSonPropItem->SetReadOnly(pFirstLineAngle->desStartOdd.m_iOddCalStyle==1);
	//else
	//	pSonPropItem->SetReadOnly(pFirstLineAngle->desEndOdd.m_iOddCalStyle==1);
}

BYTE IsOnlyDisplayWorkWingNormInfo(int *wing_x0_y1_all2)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	if(pView->selectObjs.GetNodeNum()<=1)
		return FALSE;
	CLDSLineAngle *pSelJg=NULL;
	int work_wing=-1;
	BOOL bHasSpecNorm=FALSE;
	for(pSelJg=(CLDSLineAngle*)pView->selectObjs.GetFirst();pSelJg;pSelJg=(CLDSLineAngle*)pView->selectObjs.GetNext())
	{
		if(pSelJg->des_norm_x.IsSpecNormActual() || pSelJg->des_norm_y.IsSpecNormActual())
			bHasSpecNorm=TRUE;
		int cur_wing=-1;
		if(pSelJg->des_norm_x.IsByViceRodActual())
			cur_wing=0;
		else if(pSelJg->des_norm_y.IsByViceRodActual())
			cur_wing=1;
		else 
			return FALSE;
		if(work_wing==-1)
			work_wing=cur_wing;
		else if(work_wing!=cur_wing)
			work_wing=2;
	}
	if(wing_x0_y1_all2!=NULL)
		*wing_x0_y1_all2=work_wing;
	if(bHasSpecNorm&&work_wing!=0&&work_wing!=1)
		return FALSE;
	else
		return TRUE;
}
void UpdateLineAngleWingNormProperty(CPropertyList* pPropList,CPropTreeItem* pParentItem)
{
	CXhChar500 sText;
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	CItemInfo* lpInfo=NULL;
	CPropTreeItem* pPropItem=NULL,*pSonPropItem=NULL;
	int wing_x0_y1_all2=0;
	if(!IsOnlyDisplayWorkWingNormInfo(&wing_x0_y1_all2))
	{	
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("work_wing"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("work_wing.hViceJg"));
		if(pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("norm_x_wing"),NULL))
		{
			SetPartMutiObjsProp(pFirstLineAngle,"norm_x_wing");
			SetPartMutiObjsProp(pFirstLineAngle,"norm_x_wing.x");
			SetPartMutiObjsProp(pFirstLineAngle,"norm_x_wing.y");
			SetPartMutiObjsProp(pFirstLineAngle,"norm_x_wing.z");
			SetPartMutiObjsProp(pFirstLineAngle,"norm_x_wing.hViceJg");
		}
		else
		{	//X֫���߷���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "X Leg Normal";
			lpInfo->m_strPropHelp = "Angle's X leg normal";
#else
			lpInfo->m_strPropName = "X(��)֫���߷���";
			lpInfo->m_strPropHelp = "�Ǹ�X֫���߷���";
#endif
			//lpInfo->m_cmbItems = "0.����˼�|1.�û�����";
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("norm_x_wing");
			pPropItem->m_bHideChildren = TRUE;
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "X coordinate of angle's X leg normal";
#else
			lpInfo->m_strPropHelp = "�Ǹ�X֫����X�᷽�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("norm_x_wing.x");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "Y coordinate of angle's X leg normal";
#else
			lpInfo->m_strPropHelp = "�Ǹ�X֫����Y�᷽�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("norm_x_wing.y");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "Z coordinate of angle's X leg normal";
#else
			lpInfo->m_strPropHelp = "�Ǹ�X֫����Z�᷽�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("norm_x_wing.z");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			//������ո˼�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Work WingX Normal";
			lpInfo->m_strPropHelp = "Angle's Work WingX normal";
#else
			lpInfo->m_strPropName = "X֫��������ո˼�";
			lpInfo->m_strPropHelp = "X֫��������ո˼�";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("norm_x_wing.hViceJg");
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0&&!pFirstLineAngle->des_norm_x.IsSpecNormActual()&&!pFirstLineAngle->des_norm_x.IsByOtherWingActual())
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		}
		if(pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("norm_y_wing"),NULL))
		{
			SetPartMutiObjsProp(pFirstLineAngle,"norm_y_wing");
			SetPartMutiObjsProp(pFirstLineAngle,"norm_y_wing.x");
			SetPartMutiObjsProp(pFirstLineAngle,"norm_y_wing.y");
			SetPartMutiObjsProp(pFirstLineAngle,"norm_y_wing.z");
			SetPartMutiObjsProp(pFirstLineAngle,"norm_y_wing.hViceJg");
		}
		else
		{	//Y֫���߷���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Y Leg Normal";
			lpInfo->m_strPropHelp = "Angle's Y leg normal";
#else
			lpInfo->m_strPropName = "Y(��)֫���߷���";
			lpInfo->m_strPropHelp = "�Ǹ�Y֫���߷���";
#endif
			//lpInfo->m_cmbItems = "0.����˼�|1.�û�����";
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("norm_y_wing");
			pPropItem->m_bHideChildren = TRUE;
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "X coordinate of angle's Y leg normal";
#else
			lpInfo->m_strPropHelp = "�Ǹ�Y֫����X�᷽�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("norm_y_wing.x");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "Y coordinate of angle's Y leg normal";
#else
			lpInfo->m_strPropHelp = "�Ǹ�Y֫����Y�᷽�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("norm_y_wing.y");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "Z coordinate of angle's Y leg normal";
#else
			lpInfo->m_strPropHelp = "�Ǹ�Y֫����Z�᷽�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("norm_y_wing.z");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			//������ո˼�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Work WingY Normal";
			lpInfo->m_strPropHelp = "Angle's Work WingY normal";
#else
			lpInfo->m_strPropName = "Y֫��������ո˼�";
			lpInfo->m_strPropHelp = "Y֫��������ո˼�";
#endif
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("norm_y_wing.hViceJg");
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0&&!pFirstLineAngle->des_norm_y.IsSpecNormActual()&&!pFirstLineAngle->des_norm_y.IsByOtherWingActual())
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		}
	}
	else
	{	
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("norm_x_wing"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("norm_x_wing.x"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("norm_x_wing.y"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("norm_x_wing.z"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("norm_x_wing.hViceJg"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("norm_y_wing"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("norm_y_wing.x"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("norm_y_wing.y"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("norm_y_wing.z"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("norm_y_wing.hViceJg"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("work_wing"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("work_wing.hViceJg"));
		//ȫ��ɾ�����²���,����֫ͬΪX֫��Y֫ʱӦ��ʾ��һ֫��Ʋ������������ wht 16-10-21
		{	
#ifdef AFX_TARG_ENU_ENGLISH
			CXhChar50 propNameArr[3]={"X Leg Normal","Y Leg Normal","Work Wing Normal"};
			CXhChar50 propHelpArr[3]={"Angle's X leg normal","Angle's Y leg normal","Angle's Work Wing normal"};
#else
			CXhChar50 propNameArr[3]={"X(��)֫���߷���","Y(��)֫���߷���","����֫���߷���"};
			CXhChar50 propHelpArr[3]={"�Ǹ�X֫���߷���","�Ǹ�Y֫���߷���","�Ǹֹ���֫���߷���"};
#endif
			CXhChar16 idStrArr[3]={"norm_x_wing","norm_y_wing","work_wing"};
			if(wing_x0_y1_all2<0||wing_x0_y1_all2>2)
				wing_x0_y1_all2=2;
			//����֫����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = propNameArr[wing_x0_y1_all2];
			lpInfo->m_strPropHelp = propHelpArr[wing_x0_y1_all2];
			pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID(idStrArr[wing_x0_y1_all2]);
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			pPropItem->SetReadOnly(TRUE);
			//������ո˼�
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Work Wing Normal";
			lpInfo->m_strPropHelp = "Angle's Work Wing normal";
#else
			lpInfo->m_strPropName = "������ո˼�";
			lpInfo->m_strPropHelp = "������ո˼�";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID(CXhChar50("%s.hViceJg",(char*)idStrArr[wing_x0_y1_all2]));
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
			pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;

			if(wing_x0_y1_all2==0||wing_x0_y1_all2==1)
			{	//��ʾ����һ֫����
				//Y֫���߷���
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_strPropName = propNameArr[1-wing_x0_y1_all2];
				lpInfo->m_strPropHelp = propHelpArr[1-wing_x0_y1_all2];
				//lpInfo->m_cmbItems = "0.����˼�|1.�û�����";
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1,TRUE);
				pPropItem->m_idProp = CLDSLineAngle::GetPropID(idStrArr[1-wing_x0_y1_all2]);
				pPropItem->m_bHideChildren = TRUE;
				if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
				if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
				pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
				//X
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "X coordinate of angle's leg normal";
#else
				lpInfo->m_strPropHelp = "֫����X�᷽�������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
				pSonPropItem->m_idProp = CLDSLineAngle::GetPropID(CXhChar50("%s.x",(char*)idStrArr[1-wing_x0_y1_all2]));
				if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
				//Y
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "Y coordinate of angle's leg normal";
#else
				lpInfo->m_strPropHelp = "֫����Y�᷽�������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
				pSonPropItem->m_idProp = CLDSLineAngle::GetPropID(CXhChar50("%s.y",(char*)idStrArr[1-wing_x0_y1_all2]));
				if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
				//Z
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "Z";
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropHelp = "Z coordinate of angle's leg normal";
#else
				lpInfo->m_strPropHelp = "֫����Z�᷽�������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1,TRUE);
				pSonPropItem->m_idProp = CLDSLineAngle::GetPropID(CXhChar50("%s.z",(char*)idStrArr[1-wing_x0_y1_all2]));
				if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
			}
		}
	}
}

BOOL ModifyLineAnglePositionSubProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	CLDSLineAngle *pLineAngle = NULL;
	CPropTreeItem *pFindItem, *pParentItem;

	int idProp;
	CString tempStr, tempStr_1;
	char tem_str[MAX_TEMP_CHAR_100+1]="";
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	const int GROUP_GENERAL=1,GROUP_STATMAT=2,GROUP_START=3,GROUP_END=4,GROUP_DESIGN=5;
	CXhChar500 sText;
	if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.jgber_cal_style"))
	{
		int jgber_cal_style=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Specify")==0)
			jgber_cal_style=1;
		else if(valueStr.CompareNoCase("Parametric")==0)
			jgber_cal_style=0;
		else if(valueStr.CompareNoCase("Simple")==0)
#else
		if(valueStr.CompareNoCase("ָ������")==0)
			jgber_cal_style=1;
		else if(valueStr.CompareNoCase("��������λ")==0)
			jgber_cal_style=0;
		else if(valueStr.CompareNoCase("�򵥶�λ")==0)
#endif
			jgber_cal_style=2;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			if(pLineAngle->desStartPos.jgber_cal_style==0&&jgber_cal_style==2)
			{
				//if( pLineAngle->desStartPos.spatialOperationStyle==0||
				//	pLineAngle->desStartPos.spatialOperationStyle==16||pLineAngle->desStartPos.spatialOperationStyle==17)
					pLineAngle->desStartPos.bUdfDatumPointMode=TRUE;
			}
			pLineAngle->desStartPos.jgber_cal_style=jgber_cal_style;
			pLineAngle->SetModified();
			if(jgber_cal_style==2)
			{
				CAngleMoveOperObject moveangle(pLineAngle);
				pLineAngle->CalPosition();
			}
		}
		UpdateLineAngle_DesPos_JgCalStyle_TreeItem(pPropList,pItem,TRUE);
		//��Ԥ��ʱ����ʾԤ��ǰʼ������
		UpdateLineAngle_ArcLift_TreeItem(pPropList,pItem,TRUE);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.origin_pos_style"))
	{
		BYTE origin_pos_style=0;
		if(valueStr[0]>='0'&&valueStr[0]<='9')	//����ʹ��isdigit(valueStr[0])������������ASCII��ʱ��ASSERT����
			origin_pos_style=valueStr[0]-'0';
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Node")==0)
			origin_pos_style=0;
		else if(valueStr.CompareNoCase("Intelligent")==0)
#else
		else if(valueStr.CompareNoCase("�ڵ�λ��")==0)
			origin_pos_style=0;
		else if(valueStr.CompareNoCase("����ʶ��")==0)
#endif
			origin_pos_style=5;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.origin_pos_style=origin_pos_style;
			pLineAngle->SetModified();
		}
		UpdateLineAngle_DesPos_OrgPosStyle_TreeItem(pPropList,pItem,TRUE);
	}
	/*else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.cDatumAngleWing"))
	{	//�Ǹ�'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
		BYTE cDatumAngleWing;
		if(valueStr[0]=='X')
			cDatumAngleWing='X';
		else if(valueStr[0]=='Y')
			cDatumAngleWing='Y';
		else
			cDatumAngleWing=0;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.cDatumAngleWing=cDatumAngleWing;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.operStyle"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.datumWingOffset.operStyle=valueStr[0]-'0';
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.gStyle"))
	{
		BYTE gStyle=0;
		if(valueStr.CompareNoCase("g")==0)
			gStyle=0;
		else if(valueStr.CompareNoCase("g1")==0)
			gStyle=1;
		else if(valueStr.CompareNoCase("g2")==0)
			gStyle=2;
		else if(valueStr.CompareNoCase("g3")==0)
			gStyle=3;
		else if(valueStr.CompareNoCase("�Զ���")==0)
			gStyle=4;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.datumWingOffset.gStyle=gStyle;
		idProp=CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.offsetDist");
		pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pFindItem)
		{
			if(pFirstLineAngle->GetPropValueStr(idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue=sText;
				pFindItem->SetReadOnly(gStyle!=4||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.datumWingOffset.offsetDist"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.datumWingOffset.offsetDist=atof(valueStr);
	}*/
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.datumPoint.x"))
	{	
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.datumPoint.SetPositionX(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.datumPoint.y"))
	{	
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.datumPoint.SetPositionY(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.datumPoint.z"))
	{	
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.datumPoint.SetPositionZ(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.spatialOperationStyle"))
	{	//������ƫ��
		int spatialOperationStyle=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Specify")==0)
			spatialOperationStyle=0;
		else if(valueStr.CompareNoCase("Ridge Project")==0)
			spatialOperationStyle=1;
		else if(valueStr.CompareNoCase("X Leg Project")==0)
			spatialOperationStyle=2;
		else if(valueStr.CompareNoCase("Y Leg Project")==0)
			spatialOperationStyle=3;
		else if(valueStr.CompareNoCase("Ridge Plane Offset")==0)
			spatialOperationStyle=4;
		else if(valueStr.CompareNoCase("X Leg Plane Offset")==0)
			spatialOperationStyle=5;
		else if(valueStr.CompareNoCase("Y Leg Plane Offset")==0)
			spatialOperationStyle=6;
		else if(valueStr.CompareNoCase("Ridge Bend Line Project")==0)
			spatialOperationStyle=7;
		else if(valueStr.CompareNoCase("X Leg Bend Line Project")==0)
			spatialOperationStyle=8;				 
		else if(valueStr.CompareNoCase("Y Leg Bend Line Project")==0)
			spatialOperationStyle=9;
		else if(valueStr.CompareNoCase("Ridge Bend Line Plane Offset")==0)
			spatialOperationStyle=10;
		else if(valueStr.CompareNoCase("X Leg Bend Line Plane Offset")==0)
			spatialOperationStyle=11;				 
		else if(valueStr.CompareNoCase("Y Leg Bend Line Plane Offset")==0)
			spatialOperationStyle=12;
		else if(valueStr.CompareNoCase("X Leg Offset Line")==0)
			spatialOperationStyle=13;
		else if(valueStr.CompareNoCase("Y Leg Offset Line")==0)
			spatialOperationStyle=14;
		else if(valueStr.CompareNoCase("Legs Coordinate Control")==0)
			spatialOperationStyle=15;
		else if(valueStr.CompareNoCase("Project To Line ")==0)
			spatialOperationStyle=16;
		else if(valueStr.CompareNoCase("Plane Offset To Line")==0)
			spatialOperationStyle=17;
		else if(valueStr.CompareNoCase("Node Project")==0)
			spatialOperationStyle=18;
		else if(valueStr.CompareNoCase("Node Plane Offset")==0)
#else
		if(valueStr.CompareNoCase("ָ����׼��")==0)
			spatialOperationStyle=0;
		else if(valueStr.CompareNoCase("����(��)��ͶӰ")==0)
			spatialOperationStyle=1;
		else if(valueStr.CompareNoCase("X֫����ͶӰ")==0)
			spatialOperationStyle=2;
		else if(valueStr.CompareNoCase("Y֫����ͶӰ")==0)
			spatialOperationStyle=3;
		else if(valueStr.CompareNoCase("������ƽ��")==0)
			spatialOperationStyle=4;
		else if(valueStr.CompareNoCase("X֫����ƽ��")==0)
			spatialOperationStyle=5;
		else if(valueStr.CompareNoCase("Y֫����ƽ��")==0)
			spatialOperationStyle=6;
		else if(valueStr.CompareNoCase("�����߻�����ͶӰ")==0)
			spatialOperationStyle=7;
		else if(valueStr.CompareNoCase("X֫���߻�����ͶӰ")==0)
			spatialOperationStyle=8;				 
		else if(valueStr.CompareNoCase("Y֫���߻�����ͶӰ")==0)
			spatialOperationStyle=9;
		else if(valueStr.CompareNoCase("�����߻�����ƽ��")==0)
			spatialOperationStyle=10;
		else if(valueStr.CompareNoCase("X֫���߻�����ƽ��")==0)
			spatialOperationStyle=11;				 
		else if(valueStr.CompareNoCase("Y֫���߻�����ƽ��")==0)
			spatialOperationStyle=12;
		else if(valueStr.CompareNoCase("X֫ƫ��׼��")==0)
			spatialOperationStyle=13;
		else if(valueStr.CompareNoCase("Y֫ƫ��׼��")==0)
			spatialOperationStyle=14;
		else if(valueStr.CompareNoCase("��֫�������")==0||valueStr.CompareNoCase("�ڵ��������")==0)
			spatialOperationStyle=15;
		else if(valueStr.CompareNoCase("ָ����ͶӰ")==0)
			spatialOperationStyle=16;
		else if(valueStr.CompareNoCase("ָ����ƽ��")==0)
			spatialOperationStyle=17;
		else if(valueStr.CompareNoCase("������Ƶ�ͶӰ")==0)
			spatialOperationStyle=18;
		else if(valueStr.CompareNoCase("������Ƶ�ƽ��")==0)
#endif
			spatialOperationStyle=19;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			//��������ת������׼����ʽʱ��ӦĬ�϶˽ڵ�����Ϊ��׼������(���ڻ�׼����ָ����������㸳ֵ��ʼ��������� wjh-2014.9.2
			if( pLineAngle->desStartPos.spatialOperationStyle!=spatialOperationStyle&&spatialOperationStyle==0&&
				pLineAngle->desStartPos.datumPoint.Position().IsZero()&&pLineAngle->pStart!=NULL)	//SetPosition��ͬʱ����CLDSPoint::datum_pos��CLDSPoint::pos -wjh 2016.6.15
				pLineAngle->desStartPos.datumPoint.SetPosition(pLineAngle->pStart->Position(false));
			pLineAngle->desStartPos.spatialOperationStyle=spatialOperationStyle;
			if(pLineAngle->desStartPos.spatialOperationStyle==1)
			{	//�û����Ǹ���㶨λ������Ϊ������ͶӰʱ��������Ϊϣ�����������ǰ�����㣨����֫����ƫ�ƣ� wjh-2015.12.16
				pLineAngle->desStartPos.wing_x_offset.gStyle=pLineAngle->desStartPos.wing_y_offset.gStyle=4;
				pLineAngle->desStartPos.wing_x_offset.offsetDist=pLineAngle->desStartPos.wing_y_offset.offsetDist=0;
			}
			pLineAngle->SetModified();
		}
		UpdateLineAngle_DesPos_DatumPosStyle_TreeItem(pPropList,pItem,TRUE);
		//���»��㵽���Ķ�λ���� wht 12-05-30
		long idProp=CLDSLineAngle::GetPropID("desStartPos.datum_to_ber_style");
		if(spatialOperationStyle>17||spatialOperationStyle<15)
		{	//����->��㶨λ��ʽ
			pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pFindItem==NULL)
			{
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Datum To Ridge Point Position";
				lpInfo->m_strPropHelp = "The vector from datum point to ridge point offset position style";
#else
				lpInfo->m_strPropName = "����->��㶨λ��ʽ";
				lpInfo->m_strPropHelp = "�ɻ���->���ʸ��ƫ�ƶ�λ��ʽ";
#endif
				pFindItem = pPropList->InsertItem(pItem->m_pParent,lpInfo,-1,true);
				pFindItem->m_idProp = idProp;
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			pFindItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
			UpdateLineAngle_DesPos_DatumToBerStyle_TreeItem(pPropList,pFindItem,TRUE);
		}
		else 
			pPropList->DeleteItemByPropId(idProp);
	}
	/*else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.start_jg_h"))
	{
		long h;
		sscanf(tem_str,"%X",&h);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.start_jg_h=h;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.iStartJgBerStyle"))
	{
		int iStyle=0;
		if(stricmp(tem_str,"�ն����")==0)
			iStyle=1;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.iStartJgBerStyle=iStyle;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.fStartJgLengOffsetX"))
	{
		double offset=atof(tem_str);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.fStartJgLengOffsetX=offset;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.fStartJgLengOffsetY"))
	{
		double offset=atof(tem_str);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.fStartJgLengOffsetY=offset;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.end_jg_h"))
	{
		long h;
		sscanf(tem_str,"%X",&h);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.end_jg_h=h;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.iEndJgBerStyle"))
	{
		int iStyle=0;
		if(stricmp(tem_str,"�ն����")==0)
			iStyle=1;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.iEndJgBerStyle=iStyle;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.fEndJgLengOffsetX"))
	{
		double offset=atof(tem_str);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.fEndJgLengOffsetX=offset;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.fEndJgLengOffsetY"))
	{
		double offset=atof(tem_str);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.fEndJgLengOffsetY=offset;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.datum_to_ber_style"))
	{	//����->��㶨λ��ʽ
		if(valueStr.CompareNoCase("ͶӰ")==0)
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->desStartPos.datum_to_ber_style=0;
		}
		else
		{
			CFaceOffsetDlg offsetDlg;
			if(pFirstLineAngle->desStartPos.bByOtherEndOffset)
				offsetDlg.m_iFaceOffsetDatumPosStyle=1;
			if(pFirstLineAngle->desStartPos.cFaceOffsetDatumLine=='X')
				offsetDlg.m_iFaceOffsetDatumLine=1;
			else if(pFirstLineAngle->desStartPos.cFaceOffsetDatumLine=='Y')
				offsetDlg.m_iFaceOffsetDatumLine=2;
			else
				offsetDlg.m_iFaceOffsetDatumLine=0;
			offsetDlg.m_fFaceOffsetNormX=pFirstLineAngle->desStartPos.face_offset_norm.x;
			offsetDlg.m_fFaceOffsetNormY=pFirstLineAngle->desStartPos.face_offset_norm.y;
			offsetDlg.m_fFaceOffsetNormZ=pFirstLineAngle->desStartPos.face_offset_norm.z;
			double x=fabs(offsetDlg.m_fFaceOffsetNormX);
			double y=fabs(offsetDlg.m_fFaceOffsetNormY);
			double z=fabs(offsetDlg.m_fFaceOffsetNormZ);
			if(x>EPS&&y<EPS&&z<EPS)
				offsetDlg.m_iType=0;
			else if(x<EPS&&y>EPS&&z<EPS)
				offsetDlg.m_iType=1;
			else if(x<EPS&&y<EPS&&z>EPS)
				offsetDlg.m_iType=2;
			else
				offsetDlg.m_iType=3;
			if(offsetDlg.DoModal()!=IDOK)
				return FALSE;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				if(offsetDlg.m_iFaceOffsetDatumPosStyle!=0)
					pLineAngle->desStartPos.bByOtherEndOffset=TRUE;
				else
					pLineAngle->desStartPos.bByOtherEndOffset=FALSE;
				if(offsetDlg.m_iFaceOffsetDatumLine==1)
					pLineAngle->desStartPos.cFaceOffsetDatumLine='X';
				else if(offsetDlg.m_iFaceOffsetDatumLine==2)
					pLineAngle->desStartPos.cFaceOffsetDatumLine='Y';
				else
					pLineAngle->desStartPos.cFaceOffsetDatumLine=0;
				pLineAngle->desStartPos.face_offset_norm.Set(
					offsetDlg.m_fFaceOffsetNormX,
					offsetDlg.m_fFaceOffsetNormY,
					offsetDlg.m_fFaceOffsetNormZ);
				pLineAngle->desStartPos.datum_to_ber_style=1;
			}
		}
		UpdateLineAngle_DesPos_DatumToBerStyle_TreeItem(pPropList,pItem,TRUE);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.bByOtherEndOffset2"))
	{	//������һ�˽���ƽ�Ƽ���
		if(valueStr.CompareNoCase("�˽ڵ�")==0)
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->desStartPos.bByOtherEndOffset=FALSE;
		}
		else
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->desStartPos.bByOtherEndOffset=TRUE;
		}
	}*/
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleDenpendWing"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			//TODO:��Ҫ��
			pLineAngle->desStartPos.cSimpleDatumAngleWing=valueStr[0]-'0';
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleDenpendWing"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			//TODO:��Ҫ��
			pLineAngle->desEndPos.cSimpleDatumAngleWing=valueStr[0]-'0';
			pLineAngle->SetModified();
		}
	}
	else if (pItem->m_idProp == CLDSLineAngle::GetPropID("desStartPos.LappedDatumWingG")||
			pItem->m_idProp == CLDSLineAngle::GetPropID("desEndPos.LappedDatumWingG"))
	{
		bool bDesStart = (pItem->m_idProp == CLDSLineAngle::GetPropID("desStartPos.LappedDatumWingG")) ? true : false;
		for (pLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst(); pLineAngle; pLineAngle = (CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			CMultiOffsetPos* pDesPosPara = bDesStart ? &pLineAngle->desStartPos : &pLineAngle->desEndPos;
			pDesPosPara->datumWingOffset.gStyle = 4;
			pDesPosPara->datumWingOffset.offsetDist = atof(valueStr);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.datum_jg_h")||pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleDependRod"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.datum_jg_h=HexStrToLong(tem_str);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.huoqu_jg_h")||
		pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleBendLineAlongAngle"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.huoqu_jg_h=HexStrToLong(tem_str);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.huoqu_dist")||
		pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleBendClearance"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.huoqu_dist=atof(tem_str);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2")||
		pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleBendAngleWing"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(stricmp(tem_str,"X Leg")==0)
			pFirstLineAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2=1;
		else if(stricmp(tem_str,"Y Leg")==0)
			pFirstLineAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2=2;
		else
			pFirstLineAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2=0;
#else
		if(stricmp(tem_str,"1.X֫")==0)
			pFirstLineAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2=1;
		else if(stricmp(tem_str,"2.Y֫")==0)
			pFirstLineAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2=2;
		else
			pFirstLineAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2=0;
#endif
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2=pFirstLineAngle->desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2;
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2")||
		pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleBendAngleWing"))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(stricmp(tem_str,"X Leg")==0)
			pFirstLineAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2=1;
		else if(stricmp(tem_str,"Y Leg")==0)
			pFirstLineAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2=2;
		else
			pFirstLineAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2=0;
#else
		if(stricmp(tem_str,"1.X֫")==0)
			pFirstLineAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2=1;
		else if(stricmp(tem_str,"2.Y֫")==0)
			pFirstLineAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2=2;
		else
			pFirstLineAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2=0;
#endif
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2=pFirstLineAngle->desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2;
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.offset_zhun")||
		pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleOffsetZhunju"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.offset_zhun=atof(tem_str);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.datum_offset_dist"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.datum_offset_dist=atof(tem_str);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleNormOffset"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			char cSelfWorkWing=pLineAngle->GetSelfWorkWing(TRUE,NULL);
			if(cSelfWorkWing!='Y')
			{
				pLineAngle->desStartPos.wing_x_offset.gStyle=4;
				CLDSLinePart* pFatherRod=pLineAngle->GetStartDatumPart();
				if(pFatherRod&&pFatherRod->IsAngle())
				{
					CLDSLineAngle* pFatherAngle=(CLDSLineAngle*)pFatherRod;
					char cDatumAngleWing=pLineAngle->GetLappedWorkWing(TRUE,pLineAngle->vxWingNorm);
					if( (cDatumAngleWing=='X'&&(pFatherAngle->vxWingNorm*pLineAngle->vxWingNorm)<0)||
						(cDatumAngleWing=='Y'&&(pFatherAngle->vyWingNorm*pLineAngle->vxWingNorm)<0))
						pLineAngle->desStartPos.wing_x_offset.offsetDist=-atof(tem_str);
					else
						pLineAngle->desStartPos.wing_x_offset.offsetDist= atof(tem_str);
				}
				else	//��ӻ�׼�˼��ǽǸ�ʱ�������߼�Ϊ������֫����
					pLineAngle->desStartPos.wing_x_offset.offsetDist= atof(tem_str);
				if(cSelfWorkWing!='X')	//��֫�������
					pLineAngle->desStartPos.wing_y_offset=pLineAngle->desStartPos.wing_x_offset;
				pLineAngle->SetModified();
			}
			else //if(cSelfWorkWing=='Y')
			{
				pLineAngle->desStartPos.wing_y_offset.gStyle=4;
				CLDSLinePart* pFatherRod=pLineAngle->GetStartDatumPart();
				if(pFatherRod&&pFatherRod->IsAngle())
				{
					CLDSLineAngle* pFatherAngle=(CLDSLineAngle*)pFatherRod;
					char cDatumAngleWing=pLineAngle->GetLappedWorkWing(TRUE,pLineAngle->vyWingNorm);
					if( (cDatumAngleWing=='X'&&(pFatherAngle->vxWingNorm*pLineAngle->vyWingNorm)<0)||
						(cDatumAngleWing=='Y'&&(pFatherAngle->vyWingNorm*pLineAngle->vyWingNorm)<0))
						pLineAngle->desStartPos.wing_y_offset.offsetDist=-atof(tem_str);
					else
						pLineAngle->desStartPos.wing_y_offset.offsetDist= atof(tem_str);
				}
				else	//��ӻ�׼�˼��ǽǸ�ʱ�������߼�Ϊ������֫����
					pLineAngle->desStartPos.wing_y_offset.offsetDist= atof(tem_str);
				pLineAngle->SetModified();
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.len_offset_type")||
		pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleLenOffsetType"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			if(tem_str[0]=='0')
				pLineAngle->desStartPos.len_offset_type=0;
			else if(tem_str[0]=='1')
				pLineAngle->desStartPos.len_offset_type=1;
			else if(tem_str[0]>'1')
				pLineAngle->desStartPos.len_offset_type='X'+tem_str[0]-'2';
			else
				pLineAngle->desStartPos.len_offset_type=0;
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.len_offset_dist")||
		pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.SimpleLenOffset"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.len_offset_dist=atof(tem_str);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.coordCtrlType"))
	{	//0:X,Z����;1:Y,Z����;X,Y����
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.coordCtrlType=valueStr[0]-'0';
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.cMainCoordCtrlWing"))
	{	//
		BYTE cMainCoordCtrlWing;//�Ǹ�����֫��'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
		BYTE spatialOperationStyle=0;
		if(valueStr.CompareNoCase("Y֫���ߵ�")==0)
			cMainCoordCtrlWing='Y';
		else if(valueStr.CompareNoCase("X֫���ߵ�")==0)
			cMainCoordCtrlWing='X';
		else if(valueStr.CompareNoCase("ͶӰ���ߵ�")==0)
			spatialOperationStyle=18;
		else if (valueStr.CompareNoCase("ƽ�����ߵ�")==0)
			spatialOperationStyle=19;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			if(spatialOperationStyle==18)
				pLineAngle->desStartPos.spatialOperationStyle=18;
			else if(spatialOperationStyle==19)
			{
				pLineAngle->desStartPos.spatialOperationStyle=19;
				pLineAngle->desStartPos.cFaceOffsetDatumLine=0;
			}
			else //if (spatialOperationStyle==0)
			{
				pLineAngle->desStartPos.cMainCoordCtrlWing=cMainCoordCtrlWing;
				if(pLineAngle->desStartPos.spatialOperationStyle==19)
					pLineAngle->desStartPos.cFaceOffsetDatumLine=cMainCoordCtrlWing;
				else
					pLineAngle->desStartPos.spatialOperationStyle=15;
			}
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.ctrlWing.operStyle"))
	{	//�Ǹ���Ҫ����֫(�����������������֫)
		/*BYTE operStyle=0;
		if(valueStr.CompareNoCase("0.����ͶӰ")==0)
			operStyle=0;
		else if(valueStr.CompareNoCase("1.X-Yƽ��ƽ��")==0)
			operStyle=1;
		else if(valueStr.CompareNoCase("2.Y-Zƽ��ƽ��")==0)
			operStyle=2;
		else if(valueStr.CompareNoCase("3.X-Zƽ��ƽ��")==0)
			operStyle=3;*/
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.ctrlWing.operStyle=valueStr[0]-'0';
			pLineAngle->SetModified();
		}
		UpdateLinaAngle_DesPos_TwoCtrlWing_SonItem(pPropList,TRUE);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.ctrlWing.gStyle"))
	{
		BYTE gStyle=0;
		if(valueStr.CompareNoCase("g")==0)
			gStyle=0;
		else if(valueStr.CompareNoCase("g1")==0)
			gStyle=1;
		else if(valueStr.CompareNoCase("g2")==0)
			gStyle=2;
		else if(valueStr.CompareNoCase("g3")==0)
			gStyle=3;
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("User-Defined")==0)
#else
		else if(valueStr.CompareNoCase("�Զ���")==0)
#endif
			gStyle=4;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.xMasterWing.gStyle=gStyle;
#ifdef __NEW_ANGLE_LOCA_MODE_
			if (pLineAngle->desStartPos.spatialOperationStyle == 18 || pLineAngle->desStartPos.spatialOperationStyle == 19)
			{
				if (pLineAngle->desStartPos.cFaceOffsetDatumLine == 'X')
					pLineAngle->desStartPos.wing_y_offset.gStyle=gStyle;
				else if (pLineAngle->desStartPos.cFaceOffsetDatumLine == 'Y')
					pLineAngle->desStartPos.wing_x_offset.gStyle=gStyle;
			}
#endif
			pLineAngle->SetModified();
		}
		idProp=CLDSLineAngle::GetPropID("desStartPos.ctrlWing.offsetDist");
		pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pFindItem)
		{
			if(pFirstLineAngle->GetPropValueStr(idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue=sText;
			pFindItem->SetReadOnly(gStyle!=4||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.ctrlWing.offsetDist"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.xMasterWing.offsetDist=atof(valueStr);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.otherWing.operStyle"))
	{	//�Ǹָ�������֫(����һ�����������֫)
		/*BYTE operStyle=0;
		if(valueStr.CompareNoCase("0.����ͶӰ")==0)
			operStyle=0;
		else if(valueStr.CompareNoCase("1.X-Yƽ��ƽ��")==0)
			operStyle=1;
		else if(valueStr.CompareNoCase("2.Y-Zƽ��ƽ��")==0)
			operStyle=2;
		else if(valueStr.CompareNoCase("3.X-Zƽ��ƽ��")==0)
			operStyle=3;*/
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.xSlaveWing.operStyle=valueStr[0]-'0';
			pLineAngle->SetModified();
		}
		UpdateLinaAngle_DesPos_TwoCtrlWing_SonItem(pPropList,TRUE);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.otherWing.gStyle"))
	{
		BYTE gStyle=0;
		if(valueStr.CompareNoCase("g")==0)
			gStyle=0;
		else if(valueStr.CompareNoCase("g1")==0)
			gStyle=1;
		else if(valueStr.CompareNoCase("g2")==0)
			gStyle=2;
		else if(valueStr.CompareNoCase("g3")==0)
			gStyle=3;
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("User-Defined")==0)
#else
		else if(valueStr.CompareNoCase("�Զ���")==0)
#endif
			gStyle=4;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.xSlaveWing.gStyle=gStyle;
			pLineAngle->SetModified();
		}
		idProp=CLDSLineAngle::GetPropID("desStartPos.otherWing.offsetDist");
		pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pFindItem)
		{
			if(pFirstLineAngle->GetPropValueStr(idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue=sText;
			pFindItem->SetReadOnly(gStyle!=4||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.otherWing.offsetDist"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.xSlaveWing.offsetDist=atof(valueStr);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.fLenOffsetOfCoordWing"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.fLenOffsetOfCoordWing=atof(valueStr);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.cFaceOffsetDatumLine"))
	{
		char cFaceOffsetDatumLine;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("X Leg")==0)
			cFaceOffsetDatumLine='X';
		else if(valueStr.CompareNoCase("Y Leg")==0)
#else
		if(valueStr.CompareNoCase("X֫����")==0)
			cFaceOffsetDatumLine='X';
		else if(valueStr.CompareNoCase("Y֫����")==0)
#endif
			cFaceOffsetDatumLine='Y';
		else
			cFaceOffsetDatumLine=0;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.cFaceOffsetDatumLine=cFaceOffsetDatumLine;
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.face_offset_norm"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			BOOL bReadOnly = TRUE;
			pLineAngle->SetModified();
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("X-Y Plane")==0)
#else
			if(valueStr.CompareNoCase("X-Yƽ��")==0)
#endif
			{
				pLineAngle->desStartPos.face_offset_norm.x=0;
				pLineAngle->desStartPos.face_offset_norm.y=0;
				pLineAngle->desStartPos.face_offset_norm.z=1;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(valueStr.CompareNoCase("X-Z Plane")==0)
#else
			else if(valueStr.CompareNoCase("X-Zƽ��")==0)
#endif
			{
				pLineAngle->desStartPos.face_offset_norm.x=0;
				pLineAngle->desStartPos.face_offset_norm.y=1;
				pLineAngle->desStartPos.face_offset_norm.z=0;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(valueStr.CompareNoCase("Y-Z Plane")==0)
#else
			else if(valueStr.CompareNoCase("Y-Zƽ��")==0)
#endif
			{
				pLineAngle->desStartPos.face_offset_norm.x=1;
				pLineAngle->desStartPos.face_offset_norm.y=0;
				pLineAngle->desStartPos.face_offset_norm.z=0;
			}
			else //if(valueStr.CompareNoCase("�Զ���")==0)
				bReadOnly = FALSE;
			tempStr.Format("%f",pLineAngle->desStartPos.face_offset_norm.x);
			SimplifiedNumString(tempStr);
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.x"),bReadOnly||
				pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.x")));
			pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.x"),tempStr);

			tempStr.Format("%f",pLineAngle->desStartPos.face_offset_norm.y);
			SimplifiedNumString(tempStr);
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.y"),bReadOnly||
				pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.y")));
			pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.y"),tempStr);

			tempStr.Format("%f",pLineAngle->desStartPos.face_offset_norm.z);
			SimplifiedNumString(tempStr);
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.z"),bReadOnly||
				pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.z")));
			pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.z"),tempStr);
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.x"))
	{	
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.face_offset_norm.x=atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.y"))
	{	
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.face_offset_norm.y=atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.face_offset_norm.z"))
	{	
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desStartPos.face_offset_norm.z=atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.cWorkWingStyle"))
	{
		int work_wing_auto0_x1_y2_both3=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Default")==0)
			work_wing_auto0_x1_y2_both3=0;
		else if(valueStr.CompareNoCase("X Leg")==0)
			work_wing_auto0_x1_y2_both3=1;
		else if(valueStr.CompareNoCase("Y Leg")==0)
			work_wing_auto0_x1_y2_both3=2;
		else if(valueStr.CompareNoCase("X&Y Leg")==0)
#else
		if(valueStr.CompareNoCase("�Զ��ж�")==0)
			work_wing_auto0_x1_y2_both3=0;
		else if(valueStr.CompareNoCase("X֫��λ")==0)
			work_wing_auto0_x1_y2_both3=1;
		else if(valueStr.CompareNoCase("Y֫��λ")==0)
			work_wing_auto0_x1_y2_both3=2;
		else if(valueStr.CompareNoCase("˫֫��λ")==0)
#endif
			work_wing_auto0_x1_y2_both3=3;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.cWorkWingStyle=work_wing_auto0_x1_y2_both3;
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.wing_x_offset.gStyle"))
	{
		int offset_style;
		if(valueStr.CompareNoCase("g")==0)
			offset_style=0;
		else if(valueStr.CompareNoCase("g1")==0)
			offset_style=1;
		else if(valueStr.CompareNoCase("g2")==0)
			offset_style=2;
		else if(valueStr.CompareNoCase("g3")==0)
			offset_style=3;
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("User-Defined")==0)
#else
		else if(valueStr.CompareNoCase("�Զ���")==0)
#endif
			offset_style=4;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.wing_x_offset.gStyle=offset_style;
			pLineAngle->SetModified();
		}
		idProp=CLDSLineAngle::GetPropID("desStartPos.wing_x_offset.offsetDist");
		pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pFindItem)
		{
			if(pFirstLineAngle->GetPropValueStr(idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue=sText;
			pFindItem->SetReadOnly(offset_style!=4||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.wing_x_offset.offsetDist"))
	{
		double offset_dist=atof(valueStr);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.wing_x_offset.offsetDist=offset_dist;
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.wing_y_offset.gStyle"))
	{
		int offset_style;
		if(valueStr.CompareNoCase("g")==0)
			offset_style=0;
		else if(valueStr.CompareNoCase("g1")==0)
			offset_style=1;
		else if(valueStr.CompareNoCase("g2")==0)
			offset_style=2;
		else if(valueStr.CompareNoCase("g3")==0)
			offset_style=3;
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("User-Defined")==0)
#else
		else if(valueStr.CompareNoCase("�Զ���")==0)
#endif
			offset_style=4;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.wing_y_offset.gStyle=offset_style;
			pLineAngle->SetModified();
		}
		idProp=CLDSLineAngle::GetPropID("desStartPos.wing_y_offset.offsetDist");
		pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pFindItem)
		{
			if(pFirstLineAngle->GetPropValueStr(idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue=sText;
			pFindItem->SetReadOnly(offset_style!=4||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.wing_y_offset.offsetDist"))
	{
		double offset_dist=atof(valueStr);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.wing_y_offset.offsetDist=offset_dist;
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.fEccentricDist"))
	{
		double fEccentricDist=atof(valueStr);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.fEccentricDist=fEccentricDist;
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.jgber_cal_style"))
	{
		int jgber_cal_style=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Parametric")==0)
			jgber_cal_style=0;
		else if(valueStr.CompareNoCase("Specify")==0)
			jgber_cal_style=1;
		else if(valueStr.CompareNoCase("Simple")==0)
#else
		if(valueStr.CompareNoCase("��������λ")==0)
			jgber_cal_style=0;
		else if(valueStr.CompareNoCase("ָ������")==0)
			jgber_cal_style=1;
		else if(valueStr.CompareNoCase("�򵥶�λ")==0)
#endif
			jgber_cal_style=2;
		/*else if(valueStr.CompareNoCase("����ƽ��")==0)
			jgber_cal_style=2;
		else if(valueStr.CompareNoCase("ָ������")==0)
			jgber_cal_style=3;*/
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			if(pLineAngle->desEndPos.jgber_cal_style==0&&jgber_cal_style==2)
			{
				//if( pLineAngle->desEndPos.spatialOperationStyle==0||
				//	pLineAngle->desEndPos.spatialOperationStyle==16||pLineAngle->desStartPos.spatialOperationStyle==17)
					pLineAngle->desEndPos.bUdfDatumPointMode=TRUE;
			}
			pLineAngle->desEndPos.jgber_cal_style=jgber_cal_style;
			pLineAngle->SetModified();
			CAngleMoveOperObject moveangle(pLineAngle);
			if(jgber_cal_style==2)
				pLineAngle->CalPosition();
		}
		UpdateLineAngle_DesPos_JgCalStyle_TreeItem(pPropList,pItem,FALSE);
		//��Ԥ��ʱ����ʾԤ��ǰ�ն�����
		UpdateLineAngle_ArcLift_TreeItem(pPropList,pItem,FALSE);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.origin_pos_style"))
	{
		BYTE origin_pos_style=0;
		if(valueStr[0]>='0'&&valueStr[0]<='9')	//����ʹ��isdigit(valueStr[0])������������ASCII��ʱ��ASSERT����
			origin_pos_style=valueStr[0]-'0';
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("Node")==0)
			origin_pos_style=0;
		else if(valueStr.CompareNoCase("Intelligent")==0)
#else
		else if(valueStr.CompareNoCase("�ڵ�λ��")==0)
			origin_pos_style=0;
		else if(valueStr.CompareNoCase("����ʶ��")==0)
#endif
			origin_pos_style=5;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.origin_pos_style=origin_pos_style;
			pLineAngle->SetModified();
		}
		UpdateLineAngle_DesPos_OrgPosStyle_TreeItem(pPropList,pItem,FALSE);
	}
	/*else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.cDatumAngleWing"))
	{	//�Ǹ�'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
		BYTE cDatumAngleWing;
		if(valueStr[0]=='X')
			cDatumAngleWing='X';
		else if(valueStr[0]=='Y')
			cDatumAngleWing='Y';
		else
			cDatumAngleWing=0;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.cDatumAngleWing=cDatumAngleWing;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.operStyle"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.datumWingOffset.operStyle=valueStr[0]-'0';
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.gStyle"))
	{
		BYTE gStyle=0;
		if(valueStr.CompareNoCase("g")==0)
			gStyle=0;
		else if(valueStr.CompareNoCase("g1")==0)
			gStyle=1;
		else if(valueStr.CompareNoCase("g2")==0)
			gStyle=2;
		else if(valueStr.CompareNoCase("g3")==0)
			gStyle=3;
		else if(valueStr.CompareNoCase("�Զ���")==0)
			gStyle=4;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.datumWingOffset.gStyle=gStyle;
		idProp=CLDSLineAngle::GetPropID("desEndPos.datumWingOffset.offsetDist");
		pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pFindItem)
		{
			if(pFirstLineAngle->GetPropValueStr(idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue=sText;
				pFindItem->SetReadOnly(gStyle!=4||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		}
	}*/
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.spatialOperationStyle"))
	{	//������ƫ��
		int spatialOperationStyle=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Specify")==0)
			spatialOperationStyle=0;
		else if(valueStr.CompareNoCase("Ridge Project")==0)
			spatialOperationStyle=1;
		else if(valueStr.CompareNoCase("X Leg Project")==0)
			spatialOperationStyle=2;
		else if(valueStr.CompareNoCase("Y Leg Project")==0)
			spatialOperationStyle=3;
		else if(valueStr.CompareNoCase("Ridge Plane Offset")==0)
			spatialOperationStyle=4;
		else if(valueStr.CompareNoCase("X Leg Plane Offset")==0)
			spatialOperationStyle=5;
		else if(valueStr.CompareNoCase("Y Leg Plane Offset")==0)
			spatialOperationStyle=6;
		else if(valueStr.CompareNoCase("Ridge Bend Line Project")==0)
			spatialOperationStyle=7;
		else if(valueStr.CompareNoCase("X Leg Bend Line Project")==0)
			spatialOperationStyle=8;				 
		else if(valueStr.CompareNoCase("Y Leg Bend Line Project")==0)
			spatialOperationStyle=9;
		else if(valueStr.CompareNoCase("Ridge Bend Line Plane Offset")==0)
			spatialOperationStyle=10;
		else if(valueStr.CompareNoCase("X Leg Bend Line Plane Offset")==0)
			spatialOperationStyle=11;				 
		else if(valueStr.CompareNoCase("Y Leg Bend Line Plane Offset")==0)
			spatialOperationStyle=12;
		else if(valueStr.CompareNoCase("X Leg Offset Line")==0)
			spatialOperationStyle=13;
		else if(valueStr.CompareNoCase("Y Leg Offset Line")==0)
			spatialOperationStyle=14;
		else if(valueStr.CompareNoCase("Legs Coordinate Control")==0)
			spatialOperationStyle=15;
		else if(valueStr.CompareNoCase("Project To Line ")==0)
			spatialOperationStyle=16;
		else if(valueStr.CompareNoCase("Plane Offset To Line")==0)
			spatialOperationStyle=17;
		else if(valueStr.CompareNoCase("Node Project")==0)
			spatialOperationStyle=18;
		else if(valueStr.CompareNoCase("Node Plane Offset")==0)
#else
		if(valueStr.CompareNoCase("ָ����׼��")==0)
			spatialOperationStyle=0;
		else if(valueStr.CompareNoCase("����(��)��ͶӰ")==0)
			spatialOperationStyle=1;
		else if(valueStr.CompareNoCase("X֫����ͶӰ")==0)
			spatialOperationStyle=2;
		else if(valueStr.CompareNoCase("Y֫����ͶӰ")==0)
			spatialOperationStyle=3;
		else if(valueStr.CompareNoCase("������ƽ��")==0)
			spatialOperationStyle=4;
		else if(valueStr.CompareNoCase("X֫����ƽ��")==0)
			spatialOperationStyle=5;
		else if(valueStr.CompareNoCase("Y֫����ƽ��")==0)
			spatialOperationStyle=6;
		else if(valueStr.CompareNoCase("�����߻�����ͶӰ")==0)
			spatialOperationStyle=7;
		else if(valueStr.CompareNoCase("X֫���߻�����ͶӰ")==0)
			spatialOperationStyle=8;				 
		else if(valueStr.CompareNoCase("Y֫���߻�����ͶӰ")==0)
			spatialOperationStyle=9;
		else if(valueStr.CompareNoCase("�����߻�����ƽ��")==0)
			spatialOperationStyle=10;
		else if(valueStr.CompareNoCase("X֫���߻�����ƽ��")==0)
			spatialOperationStyle=11;				 
		else if(valueStr.CompareNoCase("Y֫���߻�����ƽ��")==0)
			spatialOperationStyle=12;
		else if(valueStr.CompareNoCase("X֫ƫ��׼��")==0)
			spatialOperationStyle=13;
		else if(valueStr.CompareNoCase("Y֫ƫ��׼��")==0)
			spatialOperationStyle=14;
		else if(valueStr.CompareNoCase("��֫�������")==0||valueStr.CompareNoCase("�ڵ��������")==0)
			spatialOperationStyle=15;
		else if(valueStr.CompareNoCase("ָ����ͶӰ")==0)
			spatialOperationStyle=16;
		else if(valueStr.CompareNoCase("ָ����ƽ��")==0)
			spatialOperationStyle=17;
		else if(valueStr.CompareNoCase("������Ƶ�ͶӰ")==0)
			spatialOperationStyle=18;
		else if(valueStr.CompareNoCase("������Ƶ�ƽ��")==0)
#endif
			spatialOperationStyle=19;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			//��������ת������׼����ʽʱ��ӦĬ�϶˽ڵ�����Ϊ��׼������(���ڻ�׼����ָ����������㸳ֵ��ʼ��������� wjh-2014.9.2
			if( pLineAngle->desEndPos.spatialOperationStyle!=spatialOperationStyle&&spatialOperationStyle==0&&
				pLineAngle->desEndPos.datumPoint.Position().IsZero()&&pLineAngle->pEnd!=NULL)
				pLineAngle->desEndPos.datumPoint.SetPosition(pLineAngle->pEnd->Position(false));
			pLineAngle->desEndPos.spatialOperationStyle=spatialOperationStyle;
			if(pLineAngle->desEndPos.spatialOperationStyle==1)
			{	//�û����Ǹ���㶨λ������Ϊ������ͶӰʱ��������Ϊϣ�����������ǰ�����㣨����֫����ƫ�ƣ� wjh-2015.12.16
				pLineAngle->desEndPos.wing_x_offset.gStyle=pLineAngle->desEndPos.wing_y_offset.gStyle=4;
				pLineAngle->desEndPos.wing_x_offset.offsetDist=pLineAngle->desEndPos.wing_y_offset.offsetDist=0;
			}
			pLineAngle->SetModified();
		}
		UpdateLineAngle_DesPos_DatumPosStyle_TreeItem(pPropList,pItem,FALSE);
		//���»��㵽���Ķ�λ���� wht 12-05-30
		long idProp=CLDSLineAngle::GetPropID("desEndPos.datum_to_ber_style");
		if(spatialOperationStyle>17||spatialOperationStyle<15)
		{	//����->��㶨λ��ʽ
			pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pFindItem==NULL)
			{
				CItemInfo *lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Datum To Ridge Point Position";
				lpInfo->m_strPropHelp = "The vector from datum point to ridge point offset position style";
#else
				lpInfo->m_strPropName = "����->��㶨λ��ʽ";
				lpInfo->m_strPropHelp = "�ɻ���->���ʸ��ƫ�ƶ�λ��ʽ";
#endif
				pFindItem = pPropList->InsertItem(pItem->m_pParent,lpInfo,-1,true);
				pFindItem->m_idProp = idProp;
				pFindItem->m_dwPropGroup=pItem->m_dwPropGroup;
			}
			pFindItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
			UpdateLineAngle_DesPos_DatumToBerStyle_TreeItem(pPropList,pFindItem,FALSE);
		}
		else 
			pPropList->DeleteItemByPropId(idProp);
	}
	/*else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.start_jg_h"))
	{
		long h;
		sscanf(tem_str,"%X",&h);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.start_jg_h=h;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.iStartJgBerStyle"))
	{
		int iStyle=0;
		if(stricmp(tem_str,"�ն����")==0)
			iStyle=1;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.iStartJgBerStyle=iStyle;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.fStartJgLengOffsetX"))
	{
		double offset=atof(tem_str);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.fStartJgLengOffsetX=offset;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.fStartJgLengOffsetY"))
	{
		double offset=atof(tem_str);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.fStartJgLengOffsetY=offset;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.end_jg_h"))
	{
		long h;
		sscanf(tem_str,"%X",&h);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.end_jg_h=h;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.iEndJgBerStyle"))
	{
		int iStyle=0;
		if(stricmp(tem_str,"�ն����")==0)
			iStyle=1;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.iEndJgBerStyle=iStyle;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.fEndJgLengOffsetX"))
	{
		double offset=atof(tem_str);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.fEndJgLengOffsetX=offset;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.fEndJgLengOffsetY"))
	{
		double offset=atof(tem_str);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.fEndJgLengOffsetY=offset;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.datum_to_ber_style"))
	{	//����->��㶨λ��ʽ
		if(valueStr.CompareNoCase("ͶӰ")==0)
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->desEndPos.datum_to_ber_style=0;
		}
		else
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->desEndPos.datum_to_ber_style=1;
		}
		UpdateLineAngle_DesPos_DatumToBerStyle_TreeItem(pPropList,pItem,FALSE);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.bByOtherEndOffset2"))
	{	//������һ�˽���ƽ�Ƽ���
		if(valueStr.CompareNoCase("�˽ڵ�")==0)
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->desEndPos.bByOtherEndOffset=FALSE;
		}
		else
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->desEndPos.bByOtherEndOffset=TRUE;
		}
	}*/
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.datum_jg_h")||pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleDependRod"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.datum_jg_h=HexStrToLong(tem_str);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.huoqu_jg_h")||
		pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleBendLineAlongAngle"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.huoqu_jg_h=HexStrToLong(tem_str);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.huoqu_dist")||
		pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleBendClearance"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.huoqu_dist=atof(tem_str);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.offset_zhun")||
		pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleOffsetZhunju"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.offset_zhun=atof(tem_str);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.datum_offset_dist"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.datum_offset_dist=atof(tem_str);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleNormOffset"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			char cSelfWorkWing=pLineAngle->GetSelfWorkWing(FALSE,NULL);
			if(cSelfWorkWing!='Y')
			{
				pLineAngle->desEndPos.wing_x_offset.gStyle=4;
				CLDSLinePart* pFatherRod=pLineAngle->GetEndDatumPart();
				if(pFatherRod&&pFatherRod->IsAngle())
				{
					CLDSLineAngle* pFatherAngle=(CLDSLineAngle*)pFatherRod;
					char cDatumAngleWing=pLineAngle->GetLappedWorkWing(FALSE,pLineAngle->vxWingNorm);
					if( (cDatumAngleWing=='X'&&(pFatherAngle->vxWingNorm*pLineAngle->vxWingNorm)<0)||
						(cDatumAngleWing=='Y'&&(pFatherAngle->vyWingNorm*pLineAngle->vxWingNorm)<0))
						pLineAngle->desEndPos.wing_x_offset.offsetDist=-atof(tem_str);
					else
						pLineAngle->desEndPos.wing_x_offset.offsetDist= atof(tem_str);
				}
				else	//��ӻ�׼�˼��ǽǸ�ʱ�������߼�Ϊ������֫����
					pLineAngle->desEndPos.wing_x_offset.offsetDist= atof(tem_str);
				if(cSelfWorkWing!='X')	//��֫�������
					pLineAngle->desEndPos.wing_y_offset=pLineAngle->desEndPos.wing_x_offset;
				pLineAngle->SetModified();
			}
			else //if(cSelfWorkWing=='Y')
			{
				pLineAngle->desEndPos.wing_y_offset.gStyle=4;
				CLDSLinePart* pFatherRod=pLineAngle->GetEndDatumPart();
				if(pFatherRod&&pFatherRod->IsAngle())
				{
					CLDSLineAngle* pFatherAngle=(CLDSLineAngle*)pFatherRod;
					char cDatumAngleWing=pLineAngle->GetLappedWorkWing(FALSE,pLineAngle->vyWingNorm);
					if( (cDatumAngleWing=='X'&&(pFatherAngle->vxWingNorm*pLineAngle->vyWingNorm)<0)||
						(cDatumAngleWing=='Y'&&(pFatherAngle->vyWingNorm*pLineAngle->vyWingNorm)<0))
						pLineAngle->desEndPos.wing_y_offset.offsetDist=-atof(tem_str);
					else
						pLineAngle->desEndPos.wing_y_offset.offsetDist= atof(tem_str);
				}
				else	//��ӻ�׼�˼��ǽǸ�ʱ�������߼�Ϊ������֫����
					pLineAngle->desEndPos.wing_y_offset.offsetDist= atof(tem_str);
				pLineAngle->SetModified();
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.len_offset_type")||
		pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleLenOffsetType"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			if(tem_str[0]=='0')
				pLineAngle->desEndPos.len_offset_type=0;
			else if(tem_str[0]=='1')
				pLineAngle->desEndPos.len_offset_type=1;
			else if(tem_str[0]>'1')
				pLineAngle->desEndPos.len_offset_type='X'+tem_str[0]-'2';
			else
				pLineAngle->desEndPos.len_offset_type=0;
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.len_offset_dist")||
		pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.SimpleLenOffset"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.len_offset_dist=atof(tem_str);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.coordCtrlType"))
	{	//0:X,Z����;1:Y,Z����;X,Y����
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.coordCtrlType=valueStr[0]-'0';
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.cMainCoordCtrlWing"))
	{	//
		BYTE cMainCoordCtrlWing;//�Ǹ�����֫��'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
		BYTE spatialOperationStyle=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Y Leg")==0)
#else
		if(valueStr.CompareNoCase("Y֫���ߵ�")==0)
#endif
			cMainCoordCtrlWing='Y';
		else if(valueStr.CompareNoCase("X֫���ߵ�")==0)
			cMainCoordCtrlWing='X';
		else if(valueStr.CompareNoCase("ͶӰ���ߵ�")==0)
			spatialOperationStyle=18;
		else if(valueStr.CompareNoCase("ƽ�����ߵ�")==0)
			spatialOperationStyle=19;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			if(spatialOperationStyle==18)
				pLineAngle->desEndPos.spatialOperationStyle=18;
			else if(spatialOperationStyle==19)
			{
				pLineAngle->desEndPos.spatialOperationStyle=19;
				pLineAngle->desEndPos.cFaceOffsetDatumLine=0;
			}
			else //if (spatialOperationStyle==0)
			{
				pLineAngle->desEndPos.cMainCoordCtrlWing=cMainCoordCtrlWing;
				if(pLineAngle->desEndPos.spatialOperationStyle==19)
					pLineAngle->desEndPos.cFaceOffsetDatumLine=cMainCoordCtrlWing;
				else
					pLineAngle->desEndPos.spatialOperationStyle=15;
			}
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.ctrlWing.operStyle"))
	{	//�Ǹ���Ҫ����֫(�����������������֫)
		/*BYTE operStyle=0;
		if(valueStr.CompareNoCase("0.����ͶӰ")==0)
			operStyle=0;
		else if(valueStr.CompareNoCase("1.X-Yƽ��ƽ��")==0)
			operStyle=1;
		else if(valueStr.CompareNoCase("2.Y-Zƽ��ƽ��")==0)
			operStyle=2;
		else if(valueStr.CompareNoCase("3.X-Zƽ��ƽ��")==0)
			operStyle=3;*/
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.xMasterWing.operStyle=valueStr[0]-'0';
			pLineAngle->SetModified();
		}
		UpdateLinaAngle_DesPos_TwoCtrlWing_SonItem(pPropList,FALSE);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.ctrlWing.gStyle"))
	{
		BYTE gStyle=0;
		if(valueStr.CompareNoCase("g")==0)
			gStyle=0;
		else if(valueStr.CompareNoCase("g1")==0)
			gStyle=1;
		else if(valueStr.CompareNoCase("g2")==0)
			gStyle=2;
		else if(valueStr.CompareNoCase("g3")==0)
			gStyle=3;
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("User-Defined")==0)
#else
		else if(valueStr.CompareNoCase("�Զ���")==0)
#endif
			gStyle=4;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.xMasterWing.gStyle=gStyle;
#ifdef __NEW_ANGLE_LOCA_MODE_
			if (pLineAngle->desEndPos.spatialOperationStyle == 18 || pLineAngle->desEndPos.spatialOperationStyle == 19)
			{
				if (pLineAngle->desEndPos.cFaceOffsetDatumLine == 'X')
					pLineAngle->desEndPos.wing_y_offset.gStyle=gStyle;
				else if (pLineAngle->desEndPos.cFaceOffsetDatumLine == 'Y')
					pLineAngle->desEndPos.wing_x_offset.gStyle=gStyle;
			}
#endif
			pLineAngle->SetModified();
		}
		idProp=CLDSLineAngle::GetPropID("desEndPos.ctrlWing.offsetDist");
		pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pFindItem)
		{
			if(pFirstLineAngle->GetPropValueStr(idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue=sText;
			pFindItem->SetReadOnly(gStyle!=4||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.ctrlWing.offsetDist"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.xMasterWing.offsetDist=atof(valueStr);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.otherWing.operStyle"))
	{	//�Ǹָ�������֫(����һ�����������֫)
		/*BYTE operStyle=0;
		if(valueStr.CompareNoCase("0.����ͶӰ")==0)
			operStyle=0;
		else if(valueStr.CompareNoCase("1.X-Yƽ��ƽ��")==0)
			operStyle=1;
		else if(valueStr.CompareNoCase("2.Y-Zƽ��ƽ��")==0)
			operStyle=2;
		else if(valueStr.CompareNoCase("3.X-Zƽ��ƽ��")==0)
			operStyle=3;*/
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.xSlaveWing.operStyle=valueStr[0]-'0';
			pLineAngle->SetModified();
		}
		UpdateLinaAngle_DesPos_TwoCtrlWing_SonItem(pPropList,FALSE);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.otherWing.gStyle"))
	{
		BYTE gStyle=0;
		if(valueStr.CompareNoCase("g")==0)
			gStyle=0;
		else if(valueStr.CompareNoCase("g1")==0)
			gStyle=1;
		else if(valueStr.CompareNoCase("g2")==0)
			gStyle=2;
		else if(valueStr.CompareNoCase("g3")==0)
			gStyle=3;
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("User-Defined")==0)
#else
		else if(valueStr.CompareNoCase("�Զ���")==0)
#endif
			gStyle=4;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.xSlaveWing.gStyle=gStyle;
			pLineAngle->SetModified();
		}
		idProp=CLDSLineAngle::GetPropID("desEndPos.otherWing.offsetDist");
		pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pFindItem)
		{
			if(pFirstLineAngle->GetPropValueStr(idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue=sText;
			pFindItem->SetReadOnly(gStyle!=4||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.otherWing.offsetDist"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.xSlaveWing.offsetDist=atof(valueStr);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.fLenOffsetOfCoordWing"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.fLenOffsetOfCoordWing=atof(valueStr);
			pLineAngle->SetModified();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.bendingLenOffsetComplement"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			if(pLineAngle->bHuoQuStart&&pLineAngle->desStartPos.spatialOperationStyle==0&&pLineAngle->desStartPos.datumPoint.datum_pos_style==1)
			{
				pLineAngle->desStartPos.datumPoint.des_para.RODEND.len_offset_dist=atof(valueStr);
				pLineAngle->desStartPos.datumPoint.UpdatePos(pLineAngle->BelongModel());
				pLineAngle->bHuoQuStart=FALSE;
				BOOL locked=pLineAngle->IsEndPosLocked();
				pLineAngle->UnlockEndPos();
				CAngleMoveOperObject moveangle(pLineAngle);
				pLineAngle->CalPosition();
				pLineAngle->bHuoQuStart=TRUE;
				if(locked)
					pLineAngle->LockStartPos();
				//���µ�ǰ���ӵ���һ���ӽǸֵ��������λ��
				for(CLDSPolyJoint* pJoint=(CLDSPolyJoint*)pLineAngle->BelongModel()->EnumPartFirst(CLS_POLYJOINT);pJoint;
					pJoint=(CLDSPolyJoint*)pLineAngle->BelongModel()->EnumPartNext(CLS_POLYJOINT))
				{
					if(pJoint->pPrevLinePart==pLineAngle&&pJoint->pNextLinePart&&pJoint->pNextLinePart->GetClassTypeId()==CLS_LINEANGLE)
					{
						CLDSLineAngle* pAngle=(CLDSLineAngle*)pJoint->pNextLinePart;
						if(pAngle->End()==pLineAngle->Start())
						{
							BOOL huoqu=pAngle->bHuoQuEnd;
							pAngle->bHuoQuEnd=FALSE;
							locked=pAngle->IsEndPosLocked();
							pAngle->UnlockEndPos();
							pAngle->SetEnd(pLineAngle->Start());
							CAngleMoveOperObject moveangle(pAngle);
							pAngle->CalPosition();
							pAngle->bHuoQuEnd=huoqu;
							if(locked)
								pAngle->LockEndPos();
						}
						else //if(pAngle->Start()==pLineAngle->Start())
						{
							BOOL huoqu=pAngle->bHuoQuStart;
							pAngle->bHuoQuStart=FALSE;
							locked=pAngle->IsStartPosLocked();
							pAngle->UnlockStartPos();
							pAngle->SetStart(pLineAngle->Start());
							CAngleMoveOperObject moveangle(pAngle);
							pAngle->CalPosition();
							pAngle->bHuoQuStart=huoqu;
							if(locked)
								pAngle->LockStartPos();
						}
						pAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
						g_pSolidDraw->NewSolidPart(pAngle->GetSolidPartObject());
						pJoint->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
						g_pSolidDraw->NewSolidPart(pJoint->GetSolidPartObject());
						break;
					}
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.bendingLenOffsetComplement"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			if(pLineAngle->bHuoQuEnd&&pLineAngle->desEndPos.spatialOperationStyle==0&&pLineAngle->desEndPos.datumPoint.datum_pos_style==1)
			{
				pLineAngle->desEndPos.datumPoint.des_para.RODEND.len_offset_dist=atof(valueStr);
				pLineAngle->desEndPos.datumPoint.UpdatePos(pLineAngle->BelongModel());
				pLineAngle->bHuoQuEnd=FALSE;
				BOOL locked=pLineAngle->IsEndPosLocked();
				pLineAngle->UnlockEndPos();
				CAngleMoveOperObject moveangle(pLineAngle);
				pLineAngle->CalPosition();
				moveangle.Terminate();
				pLineAngle->bHuoQuEnd=TRUE;
				if(locked)
					pLineAngle->LockEndPos();
				//���µ�ǰ���ӵ���һ���ӽǸֵ��������λ��
				for(CLDSPolyJoint* pJoint=(CLDSPolyJoint*)pLineAngle->BelongModel()->EnumPartFirst(CLS_POLYJOINT);pJoint;
					pJoint=(CLDSPolyJoint*)pLineAngle->BelongModel()->EnumPartNext(CLS_POLYJOINT))
				{
					if(pJoint->pPrevLinePart==pLineAngle&&pJoint->pNextLinePart&&pJoint->pNextLinePart->GetClassTypeId()==CLS_LINEANGLE)
					{
						CLDSLineAngle* pAngle=(CLDSLineAngle*)pJoint->pNextLinePart;
						if(pAngle->End()==pLineAngle->End())
						{
							BOOL huoqu=pAngle->bHuoQuEnd;
							pAngle->bHuoQuEnd=FALSE;
							locked=pAngle->IsEndPosLocked();
							pAngle->UnlockEndPos();
							pAngle->SetEnd(pLineAngle->End());
							CAngleMoveOperObject sonmoveangle(pAngle);
							pAngle->CalPosition();
							pAngle->bHuoQuEnd=huoqu;
							if(locked)
								pAngle->LockEndPos();
						}
						else //if(pAngle->Start()==pLineAngle->End())
						{
							BOOL huoqu=pAngle->bHuoQuStart;
							pAngle->bHuoQuStart=FALSE;
							locked=pAngle->IsStartPosLocked();
							pAngle->UnlockStartPos();
							pAngle->SetStart(pLineAngle->End());
							CAngleMoveOperObject moveangle(pAngle);
							pAngle->CalPosition();
							pAngle->bHuoQuStart=huoqu;
							if(locked)
								pAngle->LockStartPos();
						}
						pAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
						g_pSolidDraw->NewSolidPart(pAngle->GetSolidPartObject());
						pJoint->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
						g_pSolidDraw->NewSolidPart(pJoint->GetSolidPartObject());
						break;
					}
				}
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.datumPoint.x"))
	{	
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.datumPoint.SetPositionX(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.datumPoint.y"))
	{	
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.datumPoint.SetPositionY(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.datumPoint.z"))
	{	
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.datumPoint.SetPositionZ(atof(valueStr));
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.cFaceOffsetDatumLine"))
	{
		char cFaceOffsetDatumLine;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("X Leg")==0)
			cFaceOffsetDatumLine='X';
		else if(valueStr.CompareNoCase("Y Leg")==0)
#else
		if(valueStr.CompareNoCase("X֫����")==0)
			cFaceOffsetDatumLine='X';
		else if(valueStr.CompareNoCase("Y֫����")==0)
#endif
			cFaceOffsetDatumLine='Y';
		else
			cFaceOffsetDatumLine=0;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.cFaceOffsetDatumLine=cFaceOffsetDatumLine;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.face_offset_norm"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			BOOL bReadOnly = TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("X-Y Plane")==0)
#else
			if(valueStr.CompareNoCase("X-Yƽ��")==0)
#endif
			{
				pLineAngle->desEndPos.face_offset_norm.x=0;
				pLineAngle->desEndPos.face_offset_norm.y=0;
				pLineAngle->desEndPos.face_offset_norm.z=1;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(valueStr.CompareNoCase("X-Z Plane")==0)
#else
			else if(valueStr.CompareNoCase("X-Zƽ��")==0)
#endif
			{
				pLineAngle->desEndPos.face_offset_norm.x=0;
				pLineAngle->desEndPos.face_offset_norm.y=1;
				pLineAngle->desEndPos.face_offset_norm.z=0;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			else if(valueStr.CompareNoCase("Y-Z Plane")==0)
#else
			else if(valueStr.CompareNoCase("Y-Zƽ��")==0)
#endif
			{
				pLineAngle->desEndPos.face_offset_norm.x=1;
				pLineAngle->desEndPos.face_offset_norm.y=0;
				pLineAngle->desEndPos.face_offset_norm.z=0;
			}
			else //if(valueStr.CompareNoCase("�Զ���")==0)
				bReadOnly = FALSE;
			tempStr.Format("%f",pLineAngle->desEndPos.face_offset_norm.x);
			SimplifiedNumString(tempStr);
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.x"),bReadOnly||
				pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.x")));
			pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.x"),tempStr);
			
			tempStr.Format("%f",pLineAngle->desEndPos.face_offset_norm.y);
			SimplifiedNumString(tempStr);
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.y"),bReadOnly||
				pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.y")));
			pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.y"),tempStr);
			
			tempStr.Format("%f",pLineAngle->desEndPos.face_offset_norm.z);
			SimplifiedNumString(tempStr);
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.z"),bReadOnly||
				pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.z")));
			pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.z"),tempStr);
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.x"))
	{	
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.face_offset_norm.x=atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.y"))
	{	
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.face_offset_norm.y=atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.face_offset_norm.z"))
	{	
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.face_offset_norm.z=atof(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.cWorkWingStyle"))
	{
		int work_wing_auto0_x1_y2_both3=0;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Default")==0)
			work_wing_auto0_x1_y2_both3=0;
		else if(valueStr.CompareNoCase("X Leg")==0)
			work_wing_auto0_x1_y2_both3=1;
		else if(valueStr.CompareNoCase("Y Leg")==0)
			work_wing_auto0_x1_y2_both3=2;
		else if(valueStr.CompareNoCase("X&Y Leg")==0)
#else
		if(valueStr.CompareNoCase("�Զ��ж�")==0)
			work_wing_auto0_x1_y2_both3=0;
		else if(valueStr.CompareNoCase("X֫��λ")==0)
			work_wing_auto0_x1_y2_both3=1;
		else if(valueStr.CompareNoCase("Y֫��λ")==0)
			work_wing_auto0_x1_y2_both3=2;
		else if(valueStr.CompareNoCase("˫֫��λ")==0)
#endif
			work_wing_auto0_x1_y2_both3=3;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.cWorkWingStyle=work_wing_auto0_x1_y2_both3;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.wing_x_offset.gStyle"))
	{
		int offset_style;
		if(valueStr.CompareNoCase("g")==0)
			offset_style=0;
		else if(valueStr.CompareNoCase("g1")==0)
			offset_style=1;
		else if(valueStr.CompareNoCase("g2")==0)
			offset_style=2;
		else if(valueStr.CompareNoCase("g3")==0)
			offset_style=3;
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("User-Defined")==0)
#else
		else if(valueStr.CompareNoCase("�Զ���")==0)
#endif
			offset_style=4;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.wing_x_offset.gStyle=offset_style;
		idProp=CLDSLineAngle::GetPropID("desEndPos.wing_x_offset.offsetDist");
		pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pFindItem)
		{
			if(pFirstLineAngle->GetPropValueStr(idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue=sText;
			pFindItem->SetReadOnly(offset_style!=4||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.wing_x_offset.offsetDist"))
	{
		double offset_dist=atof(valueStr);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.wing_x_offset.offsetDist=offset_dist;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.wing_y_offset.gStyle"))
	{
		int offset_style;
		if(valueStr.CompareNoCase("g")==0)
			offset_style=0;
		else if(valueStr.CompareNoCase("g1")==0)
			offset_style=1;
		else if(valueStr.CompareNoCase("g2")==0)
			offset_style=2;
		else if(valueStr.CompareNoCase("g3")==0)
			offset_style=3;
#ifdef AFX_TARG_ENU_ENGLISH
		else if(valueStr.CompareNoCase("User-Defined")==0)
#else
		else if(valueStr.CompareNoCase("�Զ���")==0)
#endif
			offset_style=4;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.wing_y_offset.gStyle=offset_style;
		idProp=CLDSLineAngle::GetPropID("desEndPos.wing_y_offset.offsetDist");
		pFindItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pFindItem)
		{
			if(pFirstLineAngle->GetPropValueStr(idProp,sText)>0)
				pFindItem->m_lpNodeInfo->m_strPropValue=sText;
			pFindItem->SetReadOnly(offset_style!=4||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.wing_y_offset.offsetDist"))
	{
		double offset_dist=atof(valueStr);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.wing_y_offset.offsetDist=offset_dist;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.fEccentricDist"))
	{
		double fEccentricDist=atof(valueStr);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->desEndPos.fEccentricDist=fEccentricDist;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.hDatumLine"))
	{
		THANDLE hDatumLine=pFirstLineAngle->desStartPos.hDatumLine;
		CLDSLine* pLine=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("<Local Define...>")==0)
			hDatumLine=0;
		else if(valueStr.Compare("<New...>")==0)
#else
		if(valueStr.Compare("<�ֲ�����...>")==0)
			hDatumLine=0;
		else if(valueStr.Compare("<�½�...>")==0)
#endif
		{
			CDatumLinePropertyDlg datumLineDlg;
			if(IDOK==datumLineDlg.DoModal())
			{
				pLine=console.AppendLine();
				CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(MODEL_DATUM_LINE_GROUP);
				pLine->iNo=pNoGroup->EnumIdleNo();
				pNoGroup->SetNoState(pLine->iNo);
				sprintf(pLine->description,"%s",datumLineDlg.m_datumLine.description);
				pLine->hPointArr[0]=datumLineDlg.m_datumLine.hPointArr[0];
				pLine->hPointArr[1]=datumLineDlg.m_datumLine.hPointArr[1];
				hDatumLine=pLine->handle;
				pItem->m_lpNodeInfo->m_cmbItems=GetLineHandleStr();
			}
		}
		else
			sscanf(tem_str,"%X",&hDatumLine);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desStartPos.hDatumLine=hDatumLine;
			if(hDatumLine>0X20)
			{
				pLineAngle->ClearFlag();
				CAngleMoveOperObject moveangle(pLineAngle);
				pLineAngle->CalPosition();
				//pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				//g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
			}
		}
		if(hDatumLine==0)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr.Format("%s","<Local Define>...");
#else
			valueStr.Format("%s","<�ֲ�����>...");
#endif
		else
			valueStr.Format("0X%X",hDatumLine);
		pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
		UpdateLineAngle_DesPos_DatumLine_TreeItem(pPropList,pItem,TRUE);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.hDatumLine"))
	{
		THANDLE hDatumLine=pFirstLineAngle->desEndPos.hDatumLine;
		CLDSLine* pLine=NULL;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.Compare("<Local Define...>")==0)
			hDatumLine=0;
		else if(valueStr.Compare("<New...>")==0)
#else
		if(valueStr.Compare("<�ֲ�����...>")==0)
			hDatumLine=0;
		else if(valueStr.Compare("<�½�...>")==0)
#endif
		{
			CDatumLinePropertyDlg datumLineDlg;
			if(IDOK==datumLineDlg.DoModal())
			{
				pLine=console.AppendLine();
				CObjNoGroup *pNoGroup=Ta.NoManager.FromGroupID(MODEL_DATUM_LINE_GROUP);
				pLine->iNo=pNoGroup->EnumIdleNo();
				pNoGroup->SetNoState(pLine->iNo);
				sprintf(pLine->description,"%s",datumLineDlg.m_datumLine.description);
				pLine->hPointArr[0]=datumLineDlg.m_datumLine.hPointArr[0];
				pLine->hPointArr[1]=datumLineDlg.m_datumLine.hPointArr[1];
				hDatumLine=pLine->handle;
				pItem->m_lpNodeInfo->m_cmbItems=GetLineHandleStr();
			}
		}
		else
			sscanf(tem_str,"%X",&hDatumLine);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->desEndPos.hDatumLine=hDatumLine;
			if(hDatumLine>0X20)
			{
				pLineAngle->ClearFlag();
				CAngleMoveOperObject moveangle(pLineAngle);
				pLineAngle->CalPosition();
				//pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				//g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
			}
		}
		if(hDatumLine==0)
#ifdef AFX_TARG_ENU_ENGLISH
			valueStr.Format("%s","<Local Define>...");
#else
			valueStr.Format("%s","<�ֲ�����>...");
#endif
		else
			valueStr.Format("0X%X",hDatumLine);
		pPropList->SetItemPropValue(pItem->m_idProp,valueStr);
		UpdateLineAngle_DesPos_DatumLine_TreeItem(pPropList,pItem,FALSE);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.cMirTransType"))
	{
		if(valueStr[0]=='0')
			pFirstLineAngle->desStartPos.cMirTransType=0;
		else
			pFirstLineAngle->desStartPos.cMirTransType=valueStr[0]-'1'+'X';
		pFirstLineAngle->SetModified();
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			if((pLineAngle->desStartPos.spatialOperationStyle==16||pLineAngle->desStartPos.spatialOperationStyle==17)&&
				pLineAngle->desStartPos.hDatumLine>0x20)
			{
				pLineAngle->desStartPos.cMirTransType=pFirstLineAngle->desStartPos.cMirTransType;
				pLineAngle->SetModified();
			}
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.cMirTransType"))
	{
		if(valueStr[0]=='0')
			pFirstLineAngle->desEndPos.cMirTransType=0;
		else
			pFirstLineAngle->desEndPos.cMirTransType=valueStr[0]-'1'+'X';
		pFirstLineAngle->SetModified();
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			if((pLineAngle->desEndPos.spatialOperationStyle==16||pLineAngle->desEndPos.spatialOperationStyle==17)&&
				pLineAngle->desEndPos.hDatumLine>0x20)
			{
				pLineAngle->desEndPos.cMirTransType=pFirstLineAngle->desEndPos.cMirTransType;
				pLineAngle->SetModified();
			}
		}
	}
	else
		return FALSE;
	return TRUE;
}
void UpdateLineAngleCalLenCoefPropItem(CLDSLineAngle *pFirstAngle,CPropertyList *pPropList,CLDSView *pView,int iSectionType);
BOOL ModifyLineAngleDesignSubProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	CLDSLineAngle *pLineAngle = NULL;
	CString tempStr, tempStr_1;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	const int GROUP_GENERAL=1,GROUP_STATMAT=2,GROUP_START=3,GROUP_END=4,GROUP_DESIGN=5;
	CXhChar500 sText;
#ifdef __LDS_
	if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_uStatMatNo"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->m_uStatMatNo = atoi(valueStr);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_fWeightRaiseCoef"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->m_fWeightRaiseCoef = atof(valueStr);
			if(pLineAngle->GetClassTypeId()==CLS_LINEANGLE)
				((CLDSGroupLineAngle*)pLineAngle)->UpdateSonJgBasicInfo();
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_iElemType"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("0.�Զ��ж�")==0)
				pLineAngle->m_iElemType = 0;
			else if(valueStr.CompareNoCase("1.������")==0)
				pLineAngle->m_iElemType = 1;
			else
				pLineAngle->m_iElemType = 2;
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("sel_mat_method"))
	{
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			if(valueStr.CompareNoCase("�Զ����")==0)
				pLineAngle->sel_mat_method=AUX_AUTODESIGN;
			else if(valueStr.CompareNoCase("����ʼ��")==0)
				pLineAngle->sel_mat_method=AUX_BY_START;
			else if(valueStr.CompareNoCase("�����ն�")==0)
				pLineAngle->sel_mat_method=AUX_BY_END;
			else if(valueStr.CompareNoCase("����б��")==0)
				pLineAngle->sel_mat_method=AUX_ONLY_PRIMARY;
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_bNeedSelMat"))
	{
		BOOL bJustify=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			bJustify=TRUE;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->m_bNeedSelMat=bJustify;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_bNeedSelSize"))
	{
		BOOL bJustify=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			bJustify=TRUE;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->m_bNeedSelSize=bJustify;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("sel_mat_method"))
	{
		pFirstLineAngle->sel_mat_method=AUX_AUTODESIGN;
		if(valueStr.CompareNoCase("�Զ����")==0)
			pFirstLineAngle->sel_mat_method=AUX_AUTODESIGN;
		if(valueStr.CompareNoCase("����ʼ��")==0)
			pFirstLineAngle->sel_mat_method=AUX_BY_START;
		else if(valueStr.CompareNoCase("�����ն�")==0)
			pFirstLineAngle->sel_mat_method=AUX_BY_END;
		else if(valueStr.CompareNoCase("����б��")==0)
			pFirstLineAngle->sel_mat_method=AUX_ONLY_PRIMARY;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->sel_mat_method=pFirstLineAngle->sel_mat_method;
		/*if(sel_mat_method==AUX_AUTODESIGN)
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("bear_load_coef"));
		else
		{
			CPropTreeItem *pFindItem,*pParentItem;
			pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("bear_load_coef"),&pParentItem);
			if(pFindItem==NULL)
			{
				//������֧����ϵ��
				CItemInfo* lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "������֧����ϵ��";
				lpInfo->m_strPropHelp = "������֧����ϵ��";
				pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,TRUE);
				pFindItem->m_idProp = CLDSLineAngle::GetPropID("bear_load_coef");
				if(pFirstLineAngle->GetPropValueStr(pFindItem->m_idProp,sText)>0)
					pFindItem->m_lpNodeInfo->m_strPropValue = sText;
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				pPropList->SetModifyValueFunc(ModifyLineAngleProperty);
				pFindItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
			}
		}*/
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("bear_load_coef"))
	{
		double bear_load_coef=atof(valueStr);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->bear_load_coef=bear_load_coef;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("hole_out"))
	{
		double hole_out=atof(valueStr);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->hole_out=hole_out;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_bAutoCalHoleOutCoef"))
	{
		BOOL bJustify=FALSE;
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Yes")==0)
#else
		if(valueStr.CompareNoCase("��")==0)
#endif
			bJustify=TRUE;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->m_bAutoCalHoleOutCoef=bJustify;
		CPropTreeItem *pFindItem,*pParentItem;
		pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("hole_out"),&pParentItem);
		if(pFindItem)
			pPropList->SetItemReadOnly(pFindItem->m_idProp,bJustify||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pFindItem->m_idProp));
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.iTypeNo"))
	{
		int iTypeNo=0;
		if(valueStr.CompareNoCase("0.�����ж�")==0)
			iTypeNo=0;
		else if(valueStr.CompareNoCase("1.Vб��(��Ų�����)")==0)
			iTypeNo=1;
		else if(valueStr.CompareNoCase("2.Vб��(V������)")==0)
			iTypeNo=2;
		else if(valueStr.CompareNoCase("3.Vб��3�ڼ��������")==0)
			iTypeNo=3;
		else if(valueStr.CompareNoCase("4.Vб��4�ڼ��������")==0)
			iTypeNo=4;
		else if(valueStr.CompareNoCase("5.ָ���˼����㳤��")==0)
			iTypeNo=5;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->CalLenCoef.iTypeNo=iTypeNo;
		UpdateLineAngleCalLenCoefPropItem(pFirstLineAngle,pPropList,pView,3);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.pRefPole"))
	{
		char *endptr;
		long h=strtol(tem_str,&endptr,16);
		CLDSPart *pPart=console.FromPartHandle(h);
		if(pPart&&pPart->IsLinePart())
			pFirstLineAngle->CalLenCoef.hRefPole=pPart->handle;
		else
			pFirstLineAngle->CalLenCoef.hRefPole=0;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->CalLenCoef.hRefPole = pFirstLineAngle->CalLenCoef.hRefPole;
			pLineAngle->SyncMirProp("DesignInfo");
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.pHorizPole"))
	{
		char *endptr;
		long h=strtol(tem_str,&endptr,16);
		CLDSPart *pPart=console.FromPartHandle(h);
		if(pPart&&pPart->IsLinePart())
			pFirstLineAngle->CalLenCoef.hHorizPole=pPart->handle;
		else
			pFirstLineAngle->CalLenCoef.hRefPole=0;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->CalLenCoef.hHorizPole = pFirstLineAngle->CalLenCoef.hHorizPole;
			pLineAngle->SyncMirProp("DesignInfo");
		}
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.minR.hStartNode"))
	{
		char *endptr;
		pFirstLineAngle->CalLenCoef.minR.hStartNode=strtol(tem_str,&endptr,16);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->CalLenCoef.minR.hStartNode = pFirstLineAngle->CalLenCoef.minR.hStartNode;
			pLineAngle->SyncMirProp("DesignInfo");
		}
		long idProp=CLDSLinePart::GetPropID("CalLenCoef.spec_callenParaR");
		if(pFirstLineAngle->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.minR.hEndNode"))
	{
		char *endptr;
		pFirstLineAngle->CalLenCoef.minR.hEndNode=strtol(tem_str,&endptr,16);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->CalLenCoef.minR.hEndNode = pFirstLineAngle->CalLenCoef.minR.hEndNode;
			pLineAngle->SyncMirProp("DesignInfo");
		}
		long idProp=CLDSLinePart::GetPropID("CalLenCoef.spec_callenParaR");
		if(pFirstLineAngle->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.minR.coef"))
	{
		pFirstLineAngle->CalLenCoef.minR.coef=atof(valueStr);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->CalLenCoef.minR.coef = pFirstLineAngle->CalLenCoef.minR.coef;
			pLineAngle->SyncMirProp("DesignInfo");
		}
		long idProp=CLDSLinePart::GetPropID("CalLenCoef.spec_callenMinR");
		if(pFirstLineAngle->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.paraR.hStartNode"))
	{
		char *endptr;
		pFirstLineAngle->CalLenCoef.paraR.hStartNode=strtol(tem_str,&endptr,16);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->CalLenCoef.paraR.hStartNode = pFirstLineAngle->CalLenCoef.paraR.hStartNode;
			pLineAngle->SyncMirProp("DesignInfo");
		}
		long idProp=CLDSLinePart::GetPropID("CalLenCoef.spec_callenParaR");
		if(pFirstLineAngle->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.paraR.hEndNode"))
	{
		char *endptr;
		pFirstLineAngle->CalLenCoef.paraR.hEndNode=strtol(tem_str,&endptr,16);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->CalLenCoef.paraR.hEndNode = pFirstLineAngle->CalLenCoef.paraR.hEndNode;
			pLineAngle->SyncMirProp("DesignInfo");
		}
		long idProp=CLDSLinePart::GetPropID("CalLenCoef.spec_callenParaR");
		if(pFirstLineAngle->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.paraR.coef"))
	{
		pFirstLineAngle->CalLenCoef.paraR.coef=atof(valueStr);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			pLineAngle->CalLenCoef.paraR.coef = pFirstLineAngle->CalLenCoef.paraR.coef;
			pLineAngle->SyncMirProp("DesignInfo");
		}
		long idProp=CLDSLinePart::GetPropID("CalLenCoef.spec_callenParaR");
		if(pFirstLineAngle->GetPropValueStr(idProp,tem_str))
			pPropList->SetItemPropValue(idProp,tem_str);
	}
	/*else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.spec_callenMinR"))
	{
		double spec_callenMinR=atof(valueStr);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->CalLenCoef.spec_callenMinR=spec_callenMinR;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("CalLenCoef.spec_callenParaR"))
	{
		double spec_callenParaR=atof(valueStr);
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->CalLenCoef.spec_callenParaR=spec_callenParaR;
	}*/
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("start_joint_type"))
	{
		JOINT_TYPE joint_type=JOINT_AUTOJUSTIFY;
		if(valueStr.CompareNoCase("�½�")==0)
			joint_type=JOINT_HINGE;
		else if(valueStr.CompareNoCase("�ս�")==0)
			joint_type=JOINT_RIGID;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->start_joint_type=joint_type;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("start_force_type"))
	{
		FORCE_TYPE force_type=CENTRIC_FORCE;
		if(valueStr.CompareNoCase("ƫ������")==0)
			force_type=ECCENTRIC_FORCE;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->start_force_type=force_type;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("end_joint_type"))
	{
		JOINT_TYPE joint_type=JOINT_AUTOJUSTIFY;
		if(valueStr.CompareNoCase("�½�")==0)
			joint_type=JOINT_HINGE;
		else if(valueStr.CompareNoCase("�ս�")==0)
			joint_type=JOINT_RIGID;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->end_joint_type=joint_type;
	}
	else if(pItem->m_idProp==CLDSLineAngle::GetPropID("end_force_type"))
	{
		FORCE_TYPE force_type=CENTRIC_FORCE;
		if(valueStr.CompareNoCase("ƫ������")==0)
			force_type=ECCENTRIC_FORCE;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			pLineAngle->end_force_type=force_type;
	}
	else
#endif
		return FALSE;
	return TRUE;
}
BOOL ModifyLineAngleProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr);
#ifdef __LDS_
void UpdateAuxLineAnglePropItem(CLDSLineAngle *pFirstAngle,CPropertyList *pPropList,CLDSView *pView,int iSectionType)
{
	CItemInfo *lpInfo;
	CPropTreeItem *pGroupItem,*pPropItem,*pParentItem;
	char sText[MAX_TEMP_CHAR_100+1]="";
	if(pFirstAngle->IsAuxPole())
	{
		pGroupItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("designInfo"),&pParentItem);
		int idProp=CLDSLineAngle::GetPropID("sel_mat_method");
		pPropItem = pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_strPropName = "�������ϼ�������";
			lpInfo->m_strPropHelp = "�������ϼ�������";
			lpInfo->m_cmbItems ="�Զ����|����ʼ��|�����ն�|����б��";
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,1,TRUE);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("sel_mat_method");
			pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);	
		}
		if(pFirstAngle->GetPropValueStr(idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp=!CLDSLineAngle::IsPropEqual(pView->selectObjs,idProp);

		/*if(pFirstAngle->sel_mat_method!=AUX_AUTODESIGN)
		{
			idProp=CLDSLineAngle::GetPropID("bear_load_coef");
			CPropTreeItem* pSonPropItem = pPropList->FindItemByPropId(idProp,&pParentItem);
			if(pSonPropItem==NULL)
			{
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "������֧����ϵ��";
				lpInfo->m_strPropHelp = "������֧����ϵ��";
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,2,TRUE);
				pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("bear_load_coef");
				pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);	
			}
			if(pFirstAngle->GetPropValueStr(idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp=!CLDSLineAngle::IsPropEqual(pView->selectObjs,idProp);
		}*/
	}
	else
	{
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("sel_mat_method"));
		//pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("bear_load_coef"));
	}
}
void UpdateLineAngleCalLenCoefPropItem(CLDSLineAngle *pFirstAngle,CPropertyList *pPropList,CLDSView *pView,int iSectionType)
{
	int idProp;
	CItemInfo *lpInfo;
	CXhChar500 sText;
	CPropTreeItem *pPropItem,*pSonPropItem,*pParentItem;
	pPropItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("CalLenCoef.iTypeNo"),&pParentItem);
	if(pPropItem==NULL)
		return;	//LDS��˫�����л��������뽨ģ��ʱ���ܵ���pPropItemΪ�գ�����ִ�л����� wjh-2017.5.4
	if(pFirstAngle->CalLenCoef.iTypeNo==0)
	{
		idProp=CLDSLineAngle::GetPropID("CalLenCoef.pRefPole");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Reference Pole,Press At The Same Time";
			lpInfo->m_strPropHelp = "Reference pole,press at the same time";
#else
			lpInfo->m_strPropName = "ͬʱ��ѹ���ո˼�";
			lpInfo->m_strPropHelp = "ͬʱ��ѹ���ո˼�";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,TRUE);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
		}
		if(pFirstAngle->GetPropValueStr(idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp=!CLDSLineAngle::IsPropEqual(pView->selectObjs,idProp);
	}
	else
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("CalLenCoef.pRefPole"));
	if(pFirstAngle->CalLenCoef.iTypeNo==3||pFirstAngle->CalLenCoef.iTypeNo==4)
	{
		idProp=CLDSLineAngle::GetPropID("CalLenCoef.pHorizPole");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Reference Horizontal Join Pole";
			lpInfo->m_strPropHelp = "Reference horizontal join pole to calculate length";
#else
			lpInfo->m_strPropName = "���պ�����";
			lpInfo->m_strPropHelp = "���㳤�Ȳ��պ�����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,0,TRUE);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
		}
		if(pFirstAngle->GetPropValueStr(idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp=!CLDSLineAngle::IsPropEqual(pView->selectObjs,idProp);
	}
	else
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("CalLenCoef.pHorizPole"));
	if(pFirstAngle->CalLenCoef.iTypeNo==5)
	{
		idProp=CLDSLineAngle::GetPropID("CalLenCoef.spec_callenMinR");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Minimum Axis Length";
			lpInfo->m_strPropHelp = "Minimum axis length,unit(mm)";
#else
			lpInfo->m_strPropName = "��С�᳤��";
			lpInfo->m_strPropHelp = "��С�᳤��,��λ(mm)";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
		}
		if(pFirstAngle->GetPropValueStr(idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp=!CLDSLineAngle::IsPropEqual(pView->selectObjs,idProp);

		idProp=CLDSLineAngle::GetPropID("CalLenCoef.minR.hStartNode");
		CPropTreeItem* pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Start";
			lpInfo->m_strPropHelp = "The start node handle of specified length";
#else
			lpInfo->m_strPropName = "��ʼ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ʼ�ڵ���";
#endif
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.minR.hStartNode");
			pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
		}
		if(pFirstAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);

		idProp=CLDSLineAngle::GetPropID("CalLenCoef.minR.hEndNode");
		pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "End";
			lpInfo->m_strPropHelp = "The end node handle of specified length";
#else
			lpInfo->m_strPropName = "��ֹ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ֹ�ڵ���";
#endif
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.minR.hEndNode");
			pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
		}
		if(pFirstAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);

		idProp=CLDSLineAngle::GetPropID("CalLenCoef.minR.coef");
		pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Convert Coef";
			lpInfo->m_strPropHelp = "Convert coefficient to calculate length";
#else
			lpInfo->m_strPropName = "����ϵ��";
			lpInfo->m_strPropHelp = "���㳤�ȵ�����ϵ��";
#endif
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.minR.coef");
			pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
		}
		if(pFirstAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
		
		idProp=CLDSLineAngle::GetPropID("CalLenCoef.spec_callenParaR");
		pSonPropItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pSonPropItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Parallel Axis Length";
			lpInfo->m_strPropHelp = "Parallel axis length,unit(mm)";
#else
			lpInfo->m_strPropName = "ƽ���᳤��";
			lpInfo->m_strPropHelp = "ƽ���᳤��,��λ(mm)";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1,TRUE);
			pSonPropItem->m_idProp = idProp;
			pSonPropItem->m_dwPropGroup=GetSingleWord(iSectionType);
		}
		if(pFirstAngle->GetPropValueStr(idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue=sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp=!CLDSLineAngle::IsPropEqual(pView->selectObjs,idProp);

		idProp=CLDSLineAngle::GetPropID("CalLenCoef.paraR.hStartNode");
		pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Start";
			lpInfo->m_strPropHelp = "The start node handle of specified length";
#else
			lpInfo->m_strPropName = "��ʼ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ʼ�ڵ���";
#endif
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.paraR.hStartNode");
			pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
		}
		if(pFirstAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);

		idProp=CLDSLineAngle::GetPropID("CalLenCoef.paraR.hEndNode");
		pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "End";
			lpInfo->m_strPropHelp = "The end node handle of specified length";
#else
			lpInfo->m_strPropName = "��ֹ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ֹ�ڵ���";
#endif
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.paraR.hEndNode");
			pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
		}
		if(pFirstAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);

		idProp=CLDSLineAngle::GetPropID("CalLenCoef.paraR.coef");
		pLeafItem=pPropList->FindItemByPropId(idProp,&pParentItem);
		if(pLeafItem==NULL)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Convert Coef";
			lpInfo->m_strPropHelp = "Convert coefficient to calculate length";
#else
			lpInfo->m_strPropName = "����ϵ��";
			lpInfo->m_strPropHelp = "���㳤�ȵ�����ϵ��";
#endif
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.paraR.coef");
			pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
		}
		if(pFirstAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
			pLeafItem->m_lpNodeInfo->m_strPropValue = sText;
		pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pLeafItem->m_idProp);
	}
	else
	{
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("CalLenCoef.spec_callenMinR"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("CalLenCoef.minR.hStartNode"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("CalLenCoef.minR.hEndNode"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("CalLenCoef.minR.coef"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("CalLenCoef.spec_callenParaR"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("CalLenCoef.paraR.hStartNode"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("CalLenCoef.paraR.hEndNode"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("CalLenCoef.paraR.coef"));
	}
}
#endif
/*
void UpdateLineAngleCutAngleInfoPropItem(CLDSLineAngle *pFirstLineAngle,CPropertyList *pPropList,
										 BOOL bStart,CLDSView *pView,CString valueStr,BOOL bUpdate)
{
	if(pFirstLineAngle==NULL||pPropList==NULL||pView==NULL)
		return;
	long idParentItem=0;
	if(bStart)	
		idParentItem=CLDSLineAngle::GetPropID("startCutWing");
	else
		idParentItem=CLDSLineAngle::GetPropID("endCutWing");
	CPropTreeItem *pFindItem = pPropList->FindItemByPropId(idParentItem,NULL);
	if(pFindItem==NULL)
		return;
	char sText[100]="";
	int cut_wing=0;
	double x_wing_cut_angle=0,y_wing_cut_angle=0;
	BOOL bUserSpecCutAngle=FALSE;
	if(bStart)	
	{
		cut_wing=pFirstLineAngle->cut_wing[0];	//ʼ����֫���� 0.����֫ 1.X֫��֫ 2.Y֫��֫
		x_wing_cut_angle=pFirstLineAngle->cut_angle[0][0];
		y_wing_cut_angle=pFirstLineAngle->cut_angle[1][0];
		bUserSpecCutAngle=pFirstLineAngle->m_bUserSpecStartCutAngle;
		//
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[0][0]"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[0][1]"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[1][0]"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[1][1]"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_wing_para[0][0]"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_wing_para[0][1]"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_wing_para[0][2]"));
		//��������ֻ������ 
		pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("start_cut_angle_type"),!pFirstLineAngle->m_bUserSpecStartCutAngle||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("start_cut_angle_type")));
	}
	else		
	{
		cut_wing=pFirstLineAngle->cut_wing[1];	//�ն���֫���� 0.����֫ 1.X֫��֫ 2.Y֫��֫
		x_wing_cut_angle=pFirstLineAngle->cut_angle[2][0];
		y_wing_cut_angle=pFirstLineAngle->cut_angle[3][0];
		bUserSpecCutAngle=pFirstLineAngle->m_bUserSpecEndCutAngle;
		//
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[2][0]"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[2][1]"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[3][0]"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[3][1]"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_wing_para[1][0]"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_wing_para[1][1]"));
		pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_wing_para[1][2]"));
		//��������ֻ������ 
		pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("end_cut_angle_type"),!pFirstLineAngle->m_bUserSpecEndCutAngle||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("end_cut_angle_type")));
	}
	CItemInfo *lpInfo=NULL;
	CPropTreeItem *pPropItem=NULL;
	if(cut_wing==1||valueStr[0]=='3'||cut_wing==2||valueStr[0]=='4')
	{	
		//��һ֫��ȡ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "��֫���";
		if(cut_wing==1)
			lpInfo->m_strPropHelp = "X֫��֫,��һ֫��ȡ���";
		else 
			lpInfo->m_strPropHelp = "Y֫��֫,��һ֫��ȡ���";
		pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,bUpdate);
		if(bStart)
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[0][1]");
		else
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[1][1]");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = pFindItem->m_dwPropGroup;
		pPropItem->SetReadOnly(!bUserSpecCutAngle||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//�Ǹ�������ȡ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "������֫����";
		if(cut_wing==1)
			lpInfo->m_strPropHelp = "X֫��֫,�Ǹ�������ȡ����";
		else 
			lpInfo->m_strPropHelp = "Y֫��֫,�Ǹ�������ȡ����";
		pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,bUpdate);
		if(bStart)
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[0][0]");
		else 
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[1][0]");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = pFindItem->m_dwPropGroup;
		pPropItem->SetReadOnly(!bUserSpecCutAngle||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//֫������ȡ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "֫������ȡ����";
		if(cut_wing==1)
			lpInfo->m_strPropHelp = "X֫��֫,֫������ȡ����";
		else 
			lpInfo->m_strPropHelp = "Y֫��֫,֫������ȡ����";
		pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,bUpdate);
		if(bStart)
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[0][2]");
		else 
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[1][2]");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = pFindItem->m_dwPropGroup;
		pPropItem->SetReadOnly(!bUserSpecCutAngle||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	}
	if(x_wing_cut_angle>0||valueStr[0]=='2')
	{	//֫���нǿ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "X֫�нǿ��";
		lpInfo->m_strPropHelp = "X֫֫���нǿ��";
		pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,bUpdate);
		if(bStart)
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[0][1]");
		else 
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[2][1]");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = pFindItem->m_dwPropGroup;
		pPropItem->SetReadOnly(!bUserSpecCutAngle||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//�����нǳ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "X֫�нǳ���";
		lpInfo->m_strPropHelp = "X֫�����нǳ���";
		pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,bUpdate);
		if(bStart)
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[0][0]");
		else 
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[2][0]");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = pFindItem->m_dwPropGroup;
		pPropItem->SetReadOnly(!bUserSpecCutAngle||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	}
	if(y_wing_cut_angle>0||valueStr[0]=='2')
	{	//Y֫���нǿ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Y֫�нǿ��";
		lpInfo->m_strPropHelp = "Y֫֫���нǿ��";
		pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,bUpdate);
		if(bStart)
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[1][1]");
		else
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[3][1]");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = pFindItem->m_dwPropGroup;
		pPropItem->SetReadOnly(!bUserSpecCutAngle||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		//Y�����нǳ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
		lpInfo->m_strPropName = "Y֫�нǳ���";
		lpInfo->m_strPropHelp = "Y֫�����нǳ���";
		pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,bUpdate);
		if(bStart)
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[1][0]");
		else 
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[3][0]");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = pFindItem->m_dwPropGroup;
		pPropItem->SetReadOnly(!bUserSpecCutAngle||pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
	}
	//����ʼ���н���֫��ʾ
	if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,idParentItem,sText)>=0)
		pPropList->SetItemPropValue(idParentItem,CString(sText));
}
*/
long RetrieveBlockHandle(const char* blockIdName);
static void UpdateGroupAngleSolid(CLDSGroupLineAngle* pGroupAngle, CLDSView* pWorkView)
{
	pGroupAngle->UpdateSonJgPos();
	pGroupAngle->UpdateSonJgWingNorm();
	for (int i = 0; i < 4; i++)
	{
		if (pGroupAngle->son_jg_h[i] <= 0)
			continue;
		CLDSLineAngle* pSonAngle = (CLDSLineAngle*)Ta.FromPartHandle(pGroupAngle->son_jg_h[i], CLS_LINEANGLE);
		pSonAngle->Create3dSolidModel();
		g_pSolidDraw->NewSolidPart(pSonAngle->GetSolidPartObject());
		if(pWorkView)
			pWorkView->RedrawAngleFrameDrawing(pSonAngle);
	}
}

BOOL ModifyLineAngleProperty(CPropertyList *pPropList,CPropTreeItem* pItem, CString &valueStr)
{
	CTaskLogErrorLife loglife;
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	CLDSLineAngle *pLineAngle = NULL;
	CPropTreeItem *pFindItem, *pParentItem, *pPropItem,*pInsertItem;
	CItemInfo *lpInfo;
	CString tempStr, tempStr_1;
	char tem_str[MAX_TEMP_CHAR_100+1];
	_snprintf(tem_str,MAX_TEMP_CHAR_100,"%s",valueStr);
	if(pView->selectObjs.GetNodeNum()<0)
		return FALSE;
	bool bReCalAnglePos=true;
	const int GROUP_GENERAL=1,GROUP_STATMAT=2,GROUP_START=3,GROUP_END=4,GROUP_DESIGN=5;
	try{
		CXhChar500 sText;
		if(pItem->m_idProp==CLDSLineAngle::GetPropID("layer"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{	
				pLineAngle->SetLayer(tem_str);
				pLineAngle->SyncMirProp("layer");
				if(pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
					((CLDSGroupLineAngle*)pLineAngle)->UpdateSonJgBasicInfo();
			}
			//UpdateAuxLineAnglePropItem(pFirstLineAngle,pPropList,pView,3);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cMaterial"))
		{	//�Ǹֲ���
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cMaterial = QuerySteelBriefMatMark(tem_str);
				pLineAngle->SetModified(TRUE,FALSE);
				pLineAngle->SyncMirProp("cMaterial");
				if(pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
					((CLDSGroupLineAngle*)pLineAngle)->UpdateSonJgBasicInfo();
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_cQualityLevel"))
		{	//�Ǹ������ȼ�
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Default")==0)
					pLineAngle->blDefaultQualityLevel=true;
				else
#else
				if(valueStr.CompareNoCase("Ĭ��")==0)
					pLineAngle->blDefaultQualityLevel=true;
				else
#endif
					pLineAngle->m_cQualityLevel = valueStr[0];
				pLineAngle->SetModified(TRUE,FALSE);
				pLineAngle->SyncMirProp("m_cQualityLevel");
				if(pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
					((CLDSGroupLineAngle*)pLineAngle)->UpdateSonJgBasicInfo();
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_hBlock"))
		{
			long hBlock=RetrieveBlockHandle(valueStr);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->m_hBlock=hBlock;
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("specification")) 
		{	//�Ǹֹ��
			double wing_wide=pFirstLineAngle->size_wide,wing_thick=pFirstLineAngle->size_thick,wing_height=pFirstLineAngle->size_height;
			restore_JG_guige(tem_str,wing_wide,wing_height,wing_thick);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				if(pLineAngle->m_bEnableTeG&&wing_wide!=pLineAngle->GetWidth())
					pLineAngle->m_bEnableTeG=FALSE;	//�Ǹ���������׼��ʱ�����ȷ����仯��������������׼�� wht 16-07-10
				pLineAngle->set_wing_wide(wing_wide);
				pLineAngle->set_y_wing_width(wing_height);
				pLineAngle->set_wing_thick(wing_thick);

				if(pLineAngle->size_idClassType==CLS_LINEANGLE||pLineAngle->size_idClassType==CLS_GROUPLINEANGLE)
				{
					pLineAngle->connectStart.d=SelBoltDByWingWide(pLineAngle->size_wide,pLineAngle->connectStart.grade);
					pLineAngle->connectEnd.d=SelBoltDByWingWide(pLineAngle->size_wide,pLineAngle->connectEnd.grade);
					if(pLineAngle==pFirstLineAngle)
					{
						SetPartMutiObjsProp(pFirstLineAngle,"connectStart.d");
						SetPartMutiObjsProp(pFirstLineAngle,"connectStart.grade");
						SetPartMutiObjsProp(pFirstLineAngle,"connectEnd.d");
						SetPartMutiObjsProp(pFirstLineAngle,"connectEnd.grade");
					}
				}
				if(pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
					((CLDSGroupLineAngle*)pLineAngle)->UpdateSonJgBasicInfo();
				for(RELATIVE_OBJECT *pMirObj=pLineAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pLineAngle->relativeObjs.GetNext())
				{
					CLDSLineAngle *pMirLineAngle=(CLDSLineAngle*)pLineAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
					if(pMirLineAngle)
					{
						if(wing_height==0)	//���ȱ߽Ǹֲ�֧�ֶԳ�֫���
							pMirLineAngle->set_wing_wide(wing_wide);
						pMirLineAngle->set_wing_thick(wing_thick);
						if(pMirLineAngle->size_idClassType==CLS_LINEANGLE||pMirLineAngle->size_idClassType==CLS_GROUPLINEANGLE)
						{
							pMirLineAngle->connectStart.d=SelBoltDByWingWide(pMirLineAngle->size_wide,pMirLineAngle->connectStart.grade);
							pMirLineAngle->connectEnd.d=SelBoltDByWingWide(pMirLineAngle->size_wide,pMirLineAngle->connectEnd.grade);
						}
						if(pMirLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
							((CLDSGroupLineAngle*)pMirLineAngle)->UpdateSonJgBasicInfo();
					}
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("iSeg"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->iSeg = SEGI(valueStr);
				pLineAngle->SyncMirProp("iSeg");
				if(pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
					((CLDSGroupLineAngle*)pLineAngle)->UpdateSonJgBasicInfo();
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("sPartNo"))
		{
			bool parsed=false;
			SEGI iSeg;
			if(CLDSPart::UPDATE_PARTNO_AND_SEG&&ParsePartNo(tem_str,&iSeg,NULL))
			{
				parsed=true;
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("iSeg"),iSeg.ToString());
			}
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				if(parsed)
					pLineAngle->iSeg=iSeg;
				pLineAngle->SetPartNo(tem_str);
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cfgword"))
		{	//�ø��¹��ĵ�һ�������е���ĺŸ�����������������Ϻ�
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cfgword = pFirstLineAngle->cfgword;
				pLineAngle->SyncMirProp("cfgword");
				if(pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
					((CLDSGroupLineAngle*)pLineAngle)->UpdateSonJgBasicInfo();
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_uStatMatCoef"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->m_uStatMatCoef = atoi(valueStr);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_bSyncShadowPartNo"))
		{
			bool sync=false;
			if(valueStr.Compare("��")==0)
				sync=true;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->m_bSyncShadowPartNo = sync;
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_hPartWeldParent"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->m_hPartWeldParent=HexStrToLong(tem_str);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_bDisplayHole"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
					pLineAngle->m_bDisplayHole = TRUE;
				else if(valueStr.CompareNoCase("No")==0)
#else
				if(valueStr.CompareNoCase("��")==0)
					pLineAngle->m_bDisplayHole = TRUE;
				else if(valueStr.CompareNoCase("��")==0)
#endif
					pLineAngle->m_bDisplayHole = FALSE;
				pLineAngle->SyncMirProp("m_bDisplayHole");
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_bVirtualPart"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
					pLineAngle->m_bVirtualPart = TRUE;
				else if(valueStr.CompareNoCase("No")==0)
#else
				if(valueStr.CompareNoCase("��")==0)
					pLineAngle->m_bVirtualPart = TRUE;
				else if(valueStr.CompareNoCase("��")==0)
#endif
					pLineAngle->m_bVirtualPart = FALSE;
				//���⹹�����Բ��ܶԳ�,�����ڻ�������ʱ�ǳ�������(ֻ��1����,����3����Ҳ��Ҫ��ģ) wjh-2015.4.30
				//for(RELATIVE_OBJECT *pMirObj=pLineAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pLineAngle->relativeObjs.GetNext())
				//{
				//	CLDSLineAngle *pMirLineAngle=(CLDSLineAngle*)pLineAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
				//	if(pMirLineAngle)
				//		pMirLineAngle->m_bVirtualPart=pLineAngle->m_bVirtualPart;
				//}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_bFootNail"))
		{	//�Ŷ�
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
					pLineAngle->m_bFootNail = TRUE;
				else if(valueStr.CompareNoCase("No")==0)
#else
				if(valueStr.CompareNoCase("��")==0)
					pLineAngle->m_bFootNail = TRUE;
				else if(valueStr.CompareNoCase("��")==0)
#endif
					pLineAngle->m_bFootNail = FALSE;
				for(RELATIVE_OBJECT *pMirObj=pLineAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pLineAngle->relativeObjs.GetNext())
				{
					CLDSLineAngle *pMirLineAngle=(CLDSLineAngle*)pLineAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
					if(pMirLineAngle)
						pMirLineAngle->m_bFootNail=pLineAngle->m_bFootNail;
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_bEnableTeG"))
		{	//����׼��
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
					pLineAngle->m_bEnableTeG = TRUE;
				else if(valueStr.CompareNoCase("No")==0)
#else
				if(valueStr.CompareNoCase("��")==0)
					pLineAngle->m_bEnableTeG = TRUE;
				else if(valueStr.CompareNoCase("��")==0)
#endif
					pLineAngle->m_bEnableTeG = FALSE;
				pLineAngle->SyncMirProp("m_bEnableTeG");
				if(pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
					((CLDSGroupLineAngle*)pLineAngle)->UpdateSonJgBasicInfo();
			}
			if((pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("wingXG"),&pParentItem))!=NULL)
			{
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bEnableTeG;
			}
			if((pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("wingYG"),&pParentItem))!=NULL)
			{
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bEnableTeG;
			}
			pPropList->SetItemRedrawState(CLDSLineAngle::GetPropID("wingXG"));
			pPropList->SetItemRedrawState(CLDSLineAngle::GetPropID("wingYG"));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("XorYMirLayStyle"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				if(valueStr.GetLength()>0)
					pLineAngle->MirLayStyle=valueStr[0]-'0';
				else
					pLineAngle->MirLayStyle=CLDSLineAngle::MIRXorY_UNCHAHNGE;
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_cPosCtrlDatumLineType"))
		{	//��Ӷ�λ��׼
			BYTE cPosCtrlDatumLineType=0;
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.CompareNoCase("X Leg")==0)
				cPosCtrlDatumLineType = 'X';
			else if(valueStr.CompareNoCase("Y Leg")==0)
#else
			if(valueStr.CompareNoCase("X֫����")==0)
				cPosCtrlDatumLineType = 'X';
			else if(valueStr.CompareNoCase("Y֫����")==0)
#endif
				cPosCtrlDatumLineType = 'Y';
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->m_cPosCtrlDatumLineType = cPosCtrlDatumLineType;
			if(pFirstLineAngle->m_cPosCtrlDatumLineType=='X'||pFirstLineAngle->m_cPosCtrlDatumLineType=='Y')
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_cPosCtrlType"),&pParentItem);
				if(pFindItem==NULL)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Position Control Style";
					lpInfo->m_strPropHelp = "Position control style";
					lpInfo->m_cmbItems = "0.Project|1.X-Y Plane Offset|2.Y-Z Plane Offset|3.X-Z Plane Offset";
#else
					lpInfo->m_strPropName = "��ӵ㶨λ��ʽ";
					lpInfo->m_strPropHelp = "��ӵ㶨λ��ʽ";
					lpInfo->m_cmbItems = "0.����ͶӰ|1.X-Yƽ��ƽ��|2.Y-Zƽ��ƽ��|3.X-Zƽ��ƽ��";
#endif
					pPropItem = pPropList->InsertItem(pItem,lpInfo, -1,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_cPosCtrlType");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
				}
			}
			else 
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_cPosCtrlType"));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_cPosCtrlType"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->m_cPosCtrlType = valueStr[0]-'0';
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_bUserSpecColor"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
					pLineAngle->m_bUserSpecColor = TRUE;
				else if(valueStr.CompareNoCase("No")==0)
#else
				if(valueStr.CompareNoCase("��")==0)
					pLineAngle->m_bUserSpecColor = TRUE;
				else if(valueStr.CompareNoCase("��")==0)
#endif
					pLineAngle->m_bUserSpecColor = FALSE;
				for(RELATIVE_OBJECT *pMirObj=pLineAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pLineAngle->relativeObjs.GetNext())
				{
					CLDSLineAngle *pMirLineAngle=(CLDSLineAngle*)pLineAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEANGLE);
					if(pMirLineAngle)
						pMirLineAngle->m_bUserSpecColor=pLineAngle->m_bUserSpecColor;
				}
			}
			if(pFirstLineAngle->m_bUserSpecColor)
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("crMaterial"),NULL);
				if(pFindItem==NULL)
				{
					CItemInfo *lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
					lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Color";
					lpInfo->m_strPropHelp = "Part color";
#else
					lpInfo->m_strPropName = "������ɫ";
					lpInfo->m_strPropHelp = "������ɫ";
#endif
					pFindItem = pPropList->InsertItem(pItem,lpInfo,-1,true);
					pFindItem->m_idProp = CLDSLineAngle::GetPropID("crMaterial");
					pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
					if(pFirstLineAngle->GetPropValueStr(pFindItem->m_idProp,sText)>0)
						pFindItem->m_lpNodeInfo->m_strPropValue = sText;
					pFindItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				}
			}
			else
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("crMaterial"));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("crMaterial"))
		{
			COLORREF crCurSel=0;
			memmove(tem_str, tem_str+3, 97);	//����RGB
			sscanf(tem_str,"%X",&crCurSel);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->crMaterial = crCurSel;
				pLineAngle->SyncMirProp("crMaterial");
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_fHeJiaoReservedWidth"))
		{
			double reservedWidth=atof(valueStr);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->m_fHeJiaoReservedWidth = reservedWidth;
				pLineAngle->SetModified();
				for(RELATIVE_OBJECT *pMirObj=pLineAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pLineAngle->relativeObjs.GetNext())
				{
					CLDSLineAngle *pMirLineAngle=(CLDSLineAngle*)pLineAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEANGLE);
					if(pMirLineAngle)
					{
						pMirLineAngle->m_fHeJiaoReservedWidth=pLineAngle->m_fHeJiaoReservedWidth;
						pMirLineAngle->SetModified();
					}
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_cHeJiaoReservedWidthType"))
		{
			char cType='I';
#ifdef AFX_TARG_ENU_ENGLISH
			if(valueStr.Compare("External Width")==0)
#else
			if(valueStr.Compare("���")==0)
#endif
				cType='O';
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->m_cHeJiaoReservedWidthType = cType;
				pLineAngle->SetModified();
				for(RELATIVE_OBJECT *pMirObj=pLineAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pLineAngle->relativeObjs.GetNext())
				{
					CLDSLineAngle *pMirLineAngle=(CLDSLineAngle*)pLineAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEANGLE);
					if(pMirLineAngle)
					{
						pMirLineAngle->m_cHeJiaoReservedWidthType=cType;
						pMirLineAngle->SetModified();
					}
				}
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("sNotes"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{	
				_snprintf(pLineAngle->sNotes,MAX_TEMP_CHAR_50,"%s",valueStr);
				/*for(RELATIVE_OBJECT *pMirObj=pLineAngle->relativeObjs.GetFirst();pMirObj;pMirObj=pLineAngle->relativeObjs.GetNext())
				{
					CLDSLineAngle *pMirLineAngle=(CLDSLineAngle*)pLineAngle->BelongModel()->FromPartHandle(pMirObj->hObj,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
					if(pMirLineAngle)
						strcpy(pMirLineAngle->sNotes,pLineAngle->sNotes);
				}*/
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("wingXG"))
		{
			int g=atoi(valueStr);
			pItem->m_lpNodeInfo->m_bMutiObjsProp = false;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{	
				JGZJ jgzj;
				getjgzj(jgzj,pLineAngle->GetWidth());
				if(!pLineAngle->m_bEnableTeG&&g!=jgzj.g)
				{
					pLineAngle->xWingXZhunJu=jgzj;
					pLineAngle->xWingXZhunJu.g=g;
					pLineAngle->m_bEnableTeG=TRUE;
				}
				else if(pLineAngle->m_bEnableTeG)
				{
					pLineAngle->xWingXZhunJu.g=g;
					if( jgzj.g==pLineAngle->xWingXZhunJu.g&&jgzj.g1==pLineAngle->xWingXZhunJu.g1&&
						jgzj.g2==pLineAngle->xWingXZhunJu.g2&&jgzj.g3==pLineAngle->xWingXZhunJu.g3&&
						jgzj.g==pLineAngle->xWingYZhunJu.g&&jgzj.g1==pLineAngle->xWingYZhunJu.g1&&
						jgzj.g2==pLineAngle->xWingYZhunJu.g2&&jgzj.g3==pLineAngle->xWingYZhunJu.g3)
						pLineAngle->m_bEnableTeG=FALSE;
				}
				pLineAngle->SetModified();
				//
				if (pLineAngle->GetClassTypeId() == CLS_GROUPLINEANGLE)
					((CLDSGroupLineAngle*)pLineAngle)->UpdateSonJgBasicInfo();
			}
			if((pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_bEnableTeG"),&pParentItem))!=NULL)
				pPropList->SetItemPropValue(pFindItem->m_idProp, "��");
			if((pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("wingYG"),&pParentItem))!=NULL)
			{
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bEnableTeG;
				pPropList->SetItemRedrawState(CLDSLineAngle::GetPropID("wingYG"));
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("wingYG"))
		{
			int g=atoi(valueStr);
			pItem->m_lpNodeInfo->m_bMutiObjsProp = false;

			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{	
				JGZJ jgzj;
				getjgzj(jgzj,pLineAngle->GetWidth());
				if(!pLineAngle->m_bEnableTeG&&g!=jgzj.g)
				{
					pLineAngle->xWingYZhunJu=jgzj;
					pLineAngle->xWingYZhunJu.g=g;
					pLineAngle->m_bEnableTeG=TRUE;
				}
				else if(pLineAngle->m_bEnableTeG)
				{
					pLineAngle->xWingYZhunJu.g=g;
					if( jgzj.g==pLineAngle->xWingXZhunJu.g&&jgzj.g1==pLineAngle->xWingXZhunJu.g1&&
						jgzj.g2==pLineAngle->xWingXZhunJu.g2&&jgzj.g3==pLineAngle->xWingXZhunJu.g3&&
						jgzj.g==pLineAngle->xWingYZhunJu.g&&jgzj.g1==pLineAngle->xWingYZhunJu.g1&&
						jgzj.g2==pLineAngle->xWingYZhunJu.g2&&jgzj.g3==pLineAngle->xWingYZhunJu.g3)
						pLineAngle->m_bEnableTeG=FALSE;
				}
				pLineAngle->SetModified();
				//
				if (pLineAngle->GetClassTypeId() == CLS_GROUPLINEANGLE)
					((CLDSGroupLineAngle*)pLineAngle)->UpdateSonJgBasicInfo();
			}
			if((pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_bEnableTeG"),&pParentItem))!=NULL)
				pPropList->SetItemPropValue(pFindItem->m_idProp, "��");
			if((pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("wingXG"),&pParentItem))!=NULL)
			{
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pFindItem->m_idProp);
				pFindItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bEnableTeG;
				pPropList->SetItemRedrawState(CLDSLineAngle::GetPropID("wingXG"));
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("startPos.x"))
		{
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("startPos.y"), tempStr);
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("startPos.z"), tempStr_1);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->SetStart(f3dPoint(atof(valueStr),atof(tempStr),atof(tempStr_1)));
				pLineAngle->desStartPos.datumPoint.SetPosition(pLineAngle->Start());
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("startPos.y"))
		{
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("startPos.x"), tempStr);
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("startPos.z"), tempStr_1);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->SetStart(f3dPoint(atof(tempStr),atof(valueStr),atof(tempStr_1)));
				pLineAngle->desStartPos.datumPoint.SetPosition(pLineAngle->Start());
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("startPos.z"))
		{
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("startPos.x"), tempStr);
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("startPos.y"), tempStr_1);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->SetStart(f3dPoint(atof(tempStr),atof(tempStr_1),atof(valueStr)));
				pLineAngle->desStartPos.datumPoint.SetPosition(pLineAngle->Start());
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("endPos.x"))
		{
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("endPos.y"), tempStr);
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("endPos.z"), tempStr_1);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->SetEnd(f3dPoint(atof(valueStr),atof(tempStr),atof(tempStr_1)));
				pLineAngle->desEndPos.datumPoint.SetPosition(pLineAngle->End());
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("endPos.y"))
		{
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("endPos.x"), tempStr);
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("endPos.z"), tempStr_1);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->SetEnd(f3dPoint(atof(tempStr),atof(valueStr),atof(tempStr_1)));
				pLineAngle->desEndPos.datumPoint.SetPosition(pLineAngle->End());
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("endPos.z"))
		{
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("endPos.x"), tempStr);
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("endPos.y"), tempStr_1);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->SetEnd(f3dPoint(atof(tempStr),atof(tempStr_1),atof(valueStr)));
				pLineAngle->desEndPos.datumPoint.SetPosition(pLineAngle->End());
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddStart"))
		{	//����ͷ	//�����Ƿ�Ϊ������ �ж��Ƿ������������ͷ�� δ���...  
			CLDSGroupLineAngle *pGroupLineAngle=NULL;
			CUndoOperObject undo(&Ta);
			CAngleMoveOperObject oddmodified;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{	//����ֶ������˸˼�����ͷӦ���ȣ�������ͷ�ļ��㷽ʽ�޸�Ϊ������
				//�����������ø˼���صĸְ�ʱ�������¼�������ͷ��ֵ	wht 09-09-02
				pLineAngle->desStartOdd.m_iOddCalStyle=2;
				pLineAngle->SetStartOdd(atof(valueStr));
				pLineAngle->SyncMirProp("oddStart");
				if(pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
				{	//�޸���ϽǸ�����ͷ��,��Ϊ���ø����ӽǸ�����ͷ		wht 15-09-16
					pGroupLineAngle=(CLDSGroupLineAngle*)pLineAngle;
					pGroupLineAngle->m_bUpdateSonAngleOddment=TRUE;
				}
				else if(pGroupLineAngle=(CLDSGroupLineAngle*)console.FromPartHandle(pLineAngle->group_father_jg_h,CLS_GROUPLINEANGLE))
					pGroupLineAngle->m_bUpdateSonAngleOddment=FALSE;	//�޸��ӽǸ�����ͷ�󣬹رո����ӽǸ�����ͷ
			}
			//������������Ϣ
			CXhChar100 itemValue;
			CPropTreeItem* pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("oddStart.m_iOddCalStyle"),NULL);
			if(pFirstLineAngle->GetPropValueStr(pFindItem->m_idProp,itemValue))
				pPropList->SetItemPropValue(pFindItem->m_idProp,itemValue);
			UpdateLineAngleOddProperty(pPropList,pFindItem,TRUE);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddStart.m_iOddCalStyle"))
		{
			CUndoOperObject undo(&Ta);
			CAngleMoveOperObject oddmodified;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->desStartOdd.m_iOddCalStyle=valueStr[0]-'0';
				double fOldOdd=pLineAngle->startOdd();
				pLineAngle->CalStartOddment();
				if(fabs(fOldOdd-pLineAngle->startOdd())>CLDSLinePart::ODDMENT_MINIMAL_MODIFIED)
					pLineAngle->SyncMirProp("oddStart");
			}
			//
			UpdateLineAngleOddProperty(pPropList,pItem,TRUE);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddStart.m_fCollideDist"))
		{
			CUndoOperObject undo(&Ta);
			CAngleMoveOperObject oddmodified;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				double oldDist=pLineAngle->desStartOdd.m_fCollideDist=atof(valueStr);
				double fOldOdd=pLineAngle->startOdd();
				pLineAngle->CalStartOddment();
				if(fabs(fOldOdd-pLineAngle->startOdd())>CLDSLinePart::ODDMENT_MINIMAL_MODIFIED)
					pLineAngle->SyncMirProp("oddStart");
				if(pLineAngle->desStartOdd.m_fCollideDist!=oldDist)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox(CXhChar100("Because the angle cutting algorithms limited to inner arc, collision gap automatic correction for %g!",pLineAngle->desStartOdd.m_fCollideDist));
#else
					AfxMessageBox(CXhChar100("���ڽǸ��н�(֫)��������Բ�����ƣ���ײ��϶�Զ�����Ϊ%g",pLineAngle->desStartOdd.m_fCollideDist));
#endif
					valueStr.Format("%g",pLineAngle->desStartOdd.m_fCollideDist);
				}
			}
			//
			CXhChar100 valueStr;
			long idItem=CLDSLineAngle::GetPropID("oddStart");
			if(pFirstLineAngle->GetPropValueStr(idItem,valueStr)>0)
				pPropList->SetItemPropValue(idItem,valueStr);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddStart.m_hRefPart1"))
		{
			CUndoOperObject undo(&Ta);
			CAngleMoveOperObject oddmodified;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{	
				pLineAngle->desStartOdd.m_hRefPart1=HexStrToLong(valueStr);
				//double fOldOdd=pLineAngle->startOdd();
				pLineAngle->CalStartOddment();
				//if(fabs(fOldOdd-pLineAngle->startOdd())>CLDSLinePart::ODDMENT_MINIMAL_MODIFIED)
					pLineAngle->SyncMirProp("oddStart");
			}
			//
			CXhChar100 valueStr;
			long idItem=CLDSLineAngle::GetPropID("oddStart");
			if(pFirstLineAngle->GetPropValueStr(idItem,valueStr)>0)
				pPropList->SetItemPropValue(idItem,valueStr);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddStart.m_hRefPart2"))
		{
			CUndoOperObject undo(&Ta);
			CAngleMoveOperObject oddmodified;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{		
				pLineAngle->desStartOdd.m_hRefPart2=HexStrToLong(valueStr);
				//double fOldOdd=pLineAngle->startOdd();
				pLineAngle->CalStartOddment();
				//if(fabs(fOldOdd-pLineAngle->startOdd())>CLDSLinePart::ODDMENT_MINIMAL_MODIFIED)
					pLineAngle->SyncMirProp("oddStart");
			}
			//
			CXhChar100 valueStr;
			long idItem=CLDSLineAngle::GetPropID("oddStart");
			if(pFirstLineAngle->GetPropValueStr(idItem,valueStr)>0)
				pPropList->SetItemPropValue(idItem,valueStr);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddEnd"))
		{
			CLDSGroupLineAngle *pGroupLineAngle=NULL;
			CUndoOperObject undo(&Ta);
			CAngleMoveOperObject oddmodified;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{	//����ֶ������˸˼�����ͷӦ���ȣ�������ͷ�ļ��㷽ʽ�޸�Ϊ������
				//�����������ø˼���صĸְ�ʱ�������¼�������ͷ��ֵ	wht 09-09-02
				pLineAngle->desEndOdd.m_iOddCalStyle=2;
				pLineAngle->SetEndOdd(atof(valueStr));
				pLineAngle->SyncMirProp("oddEnd");
				if(pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
				{	//�޸���ϽǸ�����ͷ��,��Ϊ���ø����ӽǸ�����ͷ		wht 15-09-16
					pGroupLineAngle=(CLDSGroupLineAngle*)pLineAngle;
					pGroupLineAngle->m_bUpdateSonAngleOddment=TRUE;
				}
				else if(pGroupLineAngle=(CLDSGroupLineAngle*)console.FromPartHandle(pLineAngle->group_father_jg_h,CLS_GROUPLINEANGLE))
					pGroupLineAngle->m_bUpdateSonAngleOddment=FALSE;	//�޸��ӽǸ�����ͷ�󣬹رո����ӽǸ�����ͷ
			}
			//������������;
			CXhChar100 itemValue;
			CPropTreeItem* pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("oddEnd.m_iOddCalStyle"),NULL);
			if(pFirstLineAngle->GetPropValueStr(pFindItem->m_idProp,itemValue))
#ifdef AFX_TARG_ENU_ENGLISH
				pPropList->SetItemPropValue(pFindItem->m_idProp,"2.Specify");
#else
				pPropList->SetItemPropValue(pFindItem->m_idProp,"2.ָ����ֵ");
#endif
			UpdateLineAngleOddProperty(pPropList,pFindItem,FALSE);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddEnd.m_iOddCalStyle"))
		{
			CUndoOperObject undo(&Ta);
			CAngleMoveOperObject oddmodified;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{	
				pLineAngle->desEndOdd.m_iOddCalStyle=valueStr[0]-'0';
				double fOldOdd=pLineAngle->endOdd();
				pLineAngle->CalEndOddment();
				if(fabs(fOldOdd-pLineAngle->endOdd())>CLDSLinePart::ODDMENT_MINIMAL_MODIFIED)
					pLineAngle->SyncMirProp("oddEnd");
			}
			//
			UpdateLineAngleOddProperty(pPropList,pItem,FALSE);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddEnd.m_fCollideDist"))
		{
			CUndoOperObject undo(&Ta);
			CAngleMoveOperObject oddmodified;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{	
				double oldDist=pLineAngle->desEndOdd.m_fCollideDist=atof(valueStr);
				double fOldOdd=pLineAngle->endOdd();
				pLineAngle->CalEndOddment();
				if(fabs(fOldOdd-pLineAngle->endOdd())>CLDSLinePart::ODDMENT_MINIMAL_MODIFIED)
					pLineAngle->SyncMirProp("oddEnd");
				if(pLineAngle->desEndOdd.m_fCollideDist!=oldDist)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox(CXhChar100("Because the angle cutting algorithms limited to inner arc, collision gap automatic correction for %g!",pLineAngle->desEndOdd.m_fCollideDist));
#else
					AfxMessageBox(CXhChar100("���ڽǸ��н�(֫)��������Բ�����ƣ���ײ��϶�Զ�����Ϊ%g",pLineAngle->desEndOdd.m_fCollideDist));
#endif
					valueStr.Format("%g",pLineAngle->desEndOdd.m_fCollideDist);
				}
			}
			//
			CXhChar100 valueStr;
			long idItem=CLDSLineAngle::GetPropID("oddEnd");
			if(pFirstLineAngle->GetPropValueStr(idItem,valueStr)>0)
				pPropList->SetItemPropValue(idItem,valueStr);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddEnd.m_hRefPart1"))
		{
			CUndoOperObject undo(&Ta);
			CAngleMoveOperObject oddmodified;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{	
				pLineAngle->desEndOdd.m_hRefPart1=HexStrToLong(valueStr);
				//double fOldOdd=pLineAngle->endOdd();
				pLineAngle->CalEndOddment();
				//if(fabs(fOldOdd-pLineAngle->endOdd())>CLDSLinePart::ODDMENT_MINIMAL_MODIFIED)
					pLineAngle->SyncMirProp("oddEnd");
			}
			//
			CXhChar100 valueStr;
			long idItem=CLDSLineAngle::GetPropID("oddEnd");
			if(pFirstLineAngle->GetPropValueStr(idItem,valueStr)>0)
				pPropList->SetItemPropValue(idItem,valueStr);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddEnd.m_hRefPart2"))
		{
			CUndoOperObject undo(&Ta);
			CAngleMoveOperObject oddmodified;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{	
				pLineAngle->desEndOdd.m_hRefPart2=HexStrToLong(valueStr);
				//double fOldOdd=pLineAngle->endOdd();
				pLineAngle->CalEndOddment();
				//if(fabs(fOldOdd-pLineAngle->endOdd())>CLDSLinePart::ODDMENT_MINIMAL_MODIFIED)
					pLineAngle->SyncMirProp("oddEnd");
			}
			//
			CXhChar100 valueStr;
			long idItem=CLDSLineAngle::GetPropID("oddEnd");
			if(pFirstLineAngle->GetPropValueStr(idItem,valueStr)>0)
				pPropList->SetItemPropValue(idItem,valueStr);
		}
		//��˨
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("connectStart.grade"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				StrCopy(pLineAngle->connectStart.grade,tem_str,5);
				pLineAngle->SyncMirProp("linkInfo");
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("connectEnd.grade"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				StrCopy(pLineAngle->connectEnd.grade,tem_str,5);
				pLineAngle->SyncMirProp("linkInfo");
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("connectStart.d"))
		{	
			memmove(tem_str,tem_str+1,99);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->connectStart.d = atol(tem_str);
				pLineAngle->SyncMirProp("linkInfo");
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("connectEnd.d"))
		{
			memmove(tem_str,tem_str+1,99);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->connectEnd.d = atol(tem_str);
				pLineAngle->SyncMirProp("linkInfo");
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("connectStart.m_iConnectWing"))
		{
			pFirstLineAngle->connectStart.m_iConnectWing=tem_str[0]-'0';
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->connectStart.m_iConnectWing = pFirstLineAngle->connectStart.m_iConnectWing;
				pLineAngle->SyncMirProp("linkInfo");
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("connectEnd.m_iConnectWing"))
		{
			pFirstLineAngle->connectEnd.m_iConnectWing=tem_str[0]-'0';
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->connectEnd.m_iConnectWing = pFirstLineAngle->connectEnd.m_iConnectWing;
				pLineAngle->SyncMirProp("linkInfo");
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("connectStart.N"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->connectStart.wnConnBoltN = atoi(valueStr);
				pLineAngle->SyncMirProp("linkInfo");
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("connectEnd.N"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->connectEnd.wnConnBoltN = atoi(valueStr);
				pLineAngle->SyncMirProp("linkInfo");
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("connectStart.summBoltN"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->connectStart.wnSummBoltN = atoi(valueStr);
				pLineAngle->IntelliUpdateConnBoltsBySummBolts();
				pLineAngle->SyncMirProp("linkInfo");
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("connectEnd.summBoltN"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->connectEnd.wnSummBoltN = atoi(valueStr);
				pLineAngle->IntelliUpdateConnBoltsBySummBolts();
				pLineAngle->SyncMirProp("linkInfo");
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("connectStart.rows"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->connectStart.rows = atol(valueStr);
				pLineAngle->SyncMirProp("linkInfo");
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("connectEnd.rows"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->connectEnd.rows = atol(valueStr);
				pLineAngle->SyncMirProp("linkInfo");
			}
		}
		//X֫����
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("norm_x_wing.x"))
		{  
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("norm_x_wing.y"), tempStr);
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("norm_x_wing.z"), tempStr_1);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->set_norm_x_wing(f3dPoint(atof(valueStr),atof(tempStr),atof(tempStr_1)));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("norm_x_wing.y"))
		{
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("norm_x_wing.x"), tempStr);
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("norm_x_wing.z"), tempStr_1);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->set_norm_x_wing(f3dPoint(atof(tempStr),atof(valueStr),atof(tempStr_1)));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("norm_x_wing.z"))
		{
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("norm_x_wing.x"), tempStr);
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("norm_x_wing.y"), tempStr_1);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->set_norm_x_wing(f3dPoint(atof(tempStr),atof(tempStr_1),atof(valueStr)));
		}
		//Y֫����
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("norm_y_wing.x"))
		{
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("norm_y_wing.y"), tempStr);
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("norm_y_wing.z"), tempStr_1);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->set_norm_y_wing(f3dPoint(atof(valueStr),atof(tempStr),atof(tempStr_1)));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("norm_y_wing.y"))
		{
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("norm_y_wing.x"), tempStr);
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("norm_y_wing.z"), tempStr_1);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->set_norm_y_wing(f3dPoint(atof(tempStr),atof(valueStr),atof(tempStr_1)));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("norm_y_wing.z"))
		{
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("norm_y_wing.x"), tempStr);
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("norm_y_wing.y"), tempStr_1);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->set_norm_y_wing(f3dPoint(atof(tempStr),atof(tempStr_1),atof(valueStr)));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("start_push_flat_x1_y2"))
		{	//ѹ�⴦��
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("X Leg")==0)
					pLineAngle->start_push_flat_x1_y2 = 1;
				else if(valueStr.CompareNoCase("Y Leg")==0)
					pLineAngle->start_push_flat_x1_y2 = 2;
#else
				if(valueStr.CompareNoCase("X֫")==0)
					pLineAngle->start_push_flat_x1_y2 = 1;
				else if(valueStr.CompareNoCase("Y֫")==0)
					pLineAngle->start_push_flat_x1_y2 = 2;
#endif
				else //if(valueStr.CompareNoCase("��")==0)
					pLineAngle->start_push_flat_x1_y2 = 0;
			}			
			if(pFirstLineAngle->start_push_flat_x1_y2==1 || pFirstLineAngle->start_push_flat_x1_y2==2)
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("start_push_flat_length"),&pParentItem);
				if(pFindItem==NULL)
				{	//ѹ�ⳤ��
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Push Flat Length";
					lpInfo->m_strPropHelp = "Push flat length";
#else
					lpInfo->m_strPropName = "ѹ�ⳤ��";
					lpInfo->m_strPropHelp = "ѹ�ⳤ��";
#endif
					pPropItem = pPropList->InsertItem(pItem,lpInfo,12,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("start_push_flat_length");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup=GetSingleWord(GROUP_STATMAT);
				}
			}
			else
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("start_push_flat_length"));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("start_push_flat_length"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->start_push_flat_length = atol(valueStr);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("end_push_flat_x1_y2"))
		{	//�ն�ѹ�⴦��
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("X Leg")==0)
					pLineAngle->end_push_flat_x1_y2 = 1;
				else if(valueStr.CompareNoCase("Y Leg")==0)
					pLineAngle->end_push_flat_x1_y2 = 2;
				else if(valueStr.CompareNoCase("None")==0)
#else
				if(valueStr.CompareNoCase("X֫")==0)
					pLineAngle->end_push_flat_x1_y2 = 1;
				else if(valueStr.CompareNoCase("Y֫")==0)
					pLineAngle->end_push_flat_x1_y2 = 2;
				else if(valueStr.CompareNoCase("��")==0)
#endif
					pLineAngle->end_push_flat_x1_y2 = 0;
			}			
			if(pFirstLineAngle->end_push_flat_x1_y2==1 || pFirstLineAngle->end_push_flat_x1_y2==2)
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("end_push_flat_length"),&pParentItem);
				if(pFindItem==NULL)
				{	//ѹ�ⳤ��
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Push Flat Length";
					lpInfo->m_strPropHelp = "Push flat length";
#else
					lpInfo->m_strPropName = "ѹ�ⳤ��";
					lpInfo->m_strPropHelp = "ѹ�ⳤ��";
#endif
					pPropItem = pPropList->InsertItem(pItem,lpInfo,12,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("end_push_flat_length");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup=GetSingleWord(GROUP_STATMAT);
				}
			}
			else
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("end_push_flat_length"));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("end_push_flat_length"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->end_push_flat_length = atol(valueStr);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("mid_push_flat_x1_y2"))
		{	//�м�ѹ�⴦��
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("X Leg")==0)
					pLineAngle->mid_push_flat_x1_y2 = 1;
				else if(valueStr.CompareNoCase("Y Leg")==0)
					pLineAngle->mid_push_flat_x1_y2 = 2;
				else if(valueStr.CompareNoCase("None")==0)
#else
				if(valueStr.CompareNoCase("X֫")==0)
					pLineAngle->mid_push_flat_x1_y2 = 1;
				else if(valueStr.CompareNoCase("Y֫")==0)
					pLineAngle->mid_push_flat_x1_y2 = 2;
				else if(valueStr.CompareNoCase("��")==0)
#endif
					pLineAngle->mid_push_flat_x1_y2 = 0;
			}			
			if(pFirstLineAngle->mid_push_flat_x1_y2==1 || pFirstLineAngle->mid_push_flat_x1_y2==2)
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("mid_push_flat_start_offset"),&pParentItem);
				if(pFindItem==NULL)
				{	//ʼ��ƫ����
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Start Offset";
					lpInfo->m_strPropHelp = "Start Offset";
#else
					lpInfo->m_strPropName = "ʼ��ƫ����";
					lpInfo->m_strPropHelp = "ʼ��ƫ����";
#endif
					pPropItem = pPropList->InsertItem(pItem,lpInfo,12,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("mid_push_flat_start_offset");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup=GetSingleWord(GROUP_STATMAT);
					//�ն�ƫ����
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "End Offset";
					lpInfo->m_strPropHelp = "End Offset";
#else
					lpInfo->m_strPropName = "�ն�ƫ����";
					lpInfo->m_strPropHelp = "�ն�ƫ����";
#endif
					pPropItem = pPropList->InsertItem(pItem,lpInfo,12,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("mid_push_flat_end_offset");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup=GetSingleWord(GROUP_STATMAT);
					//ѹ��λ��ƫ��ʼ�˾��� hxr 2012-10-09
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Push Flat Node";
					lpInfo->m_strPropHelp = "The node handle of pushing flat position";
#else
					lpInfo->m_strPropName = "ѹ��λ�ýڵ�";
					lpInfo->m_strPropHelp = "ѹ��λ�ýڵ�";
#endif
					pPropItem = pPropList->InsertItem(pItem,lpInfo,12,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("mid_push_flat_node_handle");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup=GetSingleWord(GROUP_STATMAT);
				}
			}
			else
			{
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("mid_push_flat_start_offset"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("mid_push_flat_end_offset"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("mid_push_flat_node_handle"));
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("mid_push_flat_start_offset"))
		{
			double mid_push_flat_start_offset = atof(valueStr);
			if(mid_push_flat_start_offset<0)	//��ʱ��֧�ָ��� ��0����,����ʹ��value ���н�����¡�
			{
				valueStr="0"; 
				mid_push_flat_start_offset=0;
			}
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->mid_push_flat_start_offset=mid_push_flat_start_offset;
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("mid_push_flat_end_offset"))
		{
			double mid_push_flat_end_offset=atof(valueStr);
			if(mid_push_flat_end_offset<0)	//��ʱ��֧�ָ��� ��0��������ʹ��value ���н�����¡�
			{	
				valueStr="0";
				mid_push_flat_end_offset=0;
			}
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->mid_push_flat_end_offset = mid_push_flat_end_offset;
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_fCutAngleCoefR"))
		{
			double space_odd=0,fCutAngleCoefR=0;
			if(valueStr.GetLength()>0)
			{
				CExpression expression;
				EXPRESSION_VAR* pVar=expression.varList.Append();
				pVar->fValue=0;
				strcpy(pVar->variableStr,"R");
				space_odd=expression.SolveExpression(valueStr.GetBuffer());	
				pVar->fValue=1.0;
				fCutAngleCoefR=expression.SolveExpression(valueStr.GetBuffer())-space_odd;
			}
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->m_fCutAngleCoefR = fCutAngleCoefR;
				pLineAngle->m_nCutAngleSpaceOdd = (short)space_odd;
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_bCutRoot"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
					pLineAngle->m_bCutRoot = TRUE;
				else if(valueStr.CompareNoCase("No")==0)
#else
				if(valueStr.CompareNoCase("��")==0)
					pLineAngle->m_bCutRoot = TRUE;
				else if(valueStr.CompareNoCase("��")==0)
#endif
					pLineAngle->m_bCutRoot = FALSE;
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_bCutBer"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
					pLineAngle->m_bCutBer = TRUE;
				else if(valueStr.CompareNoCase("No")==0)
#else
				if(valueStr.CompareNoCase("��")==0)
					pLineAngle->m_bCutBer = TRUE;
				else if(valueStr.CompareNoCase("��")==0)
#endif
					pLineAngle->m_bCutBer = FALSE;
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_bWeldPart"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(valueStr.CompareNoCase("Yes")==0)
					pLineAngle->m_bWeldPart = TRUE;
				else if(valueStr.CompareNoCase("No")==0)
#else
				if(valueStr.CompareNoCase("��")==0)
					pLineAngle->m_bWeldPart = TRUE;
				else if(valueStr.CompareNoCase("��")==0)
#endif
					pLineAngle->m_bWeldPart = FALSE;
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("startCutWingX")||pItem->m_idProp==CLDSLineAngle::GetPropID("startCutWingY")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("endCutWingX")||pItem->m_idProp==CLDSLineAngle::GetPropID("endCutWingY"))
		{
			int i;
			CXhChar50 cutStr("%s",valueStr);
			for(i=0;i<cutStr.GetLength();i++){
				if(cutStr[i]!='.'&&(cutStr[i]<'0'||cutStr[i]>'9'))
					cutStr[i]=',';
			}
			i=0;
			int cut_para[3]={0,0,0};
			char* key=strtok(cutStr,",");
			while(key!=NULL)
			{
				cut_para[i]=(int)atof(key);
				key=strtok(NULL,",");
				i++;
			}
			CUndoOperObject undo(&Ta);
			if(i==1||i>3)
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("The format of cutting leg's parameter is wrong");
#else
				AfxMessageBox("�����н���֫������ʽ����");
#endif
			else if(pItem->m_idProp==CLDSLineAngle::GetPropID("startCutWingX"))
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->m_bUserSpecStartCutAngle=TRUE;
					if(i==0)
					{	//����н���֫��Ϣ
						pLineAngle->cut_angle[0][0]=0;	//���߷���
						pLineAngle->cut_angle[0][1]=0;	//֫����
						if(pLineAngle->cut_wing[0]==1)	//X֫��֫
							pLineAngle->cut_wing[0]=0;
						if(pLineAngle->cut_wing[0]==0)
						{
							pLineAngle->cut_wing_para[0][0]=0;
							pLineAngle->cut_wing_para[0][1]=0;
							pLineAngle->cut_wing_para[0][2]=0;
						}
					}
					else if(i==2)
					{	//�����нǲ���
						pLineAngle->cut_angle[0][0]=cut_para[1];	//���߷���
						pLineAngle->cut_angle[0][1]=cut_para[0];	//֫����
						if(pLineAngle->cut_wing[0]==1)	//X֫��֫
							pLineAngle->cut_wing[0]=0;
						if(pLineAngle->cut_wing[0]==0)
						{
							pLineAngle->cut_wing_para[0][0]=0;
							pLineAngle->cut_wing_para[0][1]=0;
							pLineAngle->cut_wing_para[0][2]=0;
						}
					}
					else //if(i==3)
					{	//������֫����
						pLineAngle->cut_angle[0][0]=0;	//���߷���
						pLineAngle->cut_angle[0][1]=0;	//֫����
						if(cut_para[0]+cut_para[1]+cut_para[2]>0)
							pLineAngle->cut_wing[0]=1;		//X֫��֫
						pLineAngle->cut_wing_para[0][0]=cut_para[1];	//��ʾ�Ǹ�������ȡ����
						pLineAngle->cut_wing_para[0][1]=cut_para[0];	//��ʾ��һ֫��ȡ���
						pLineAngle->cut_wing_para[0][2]=cut_para[2];	//��ʾ֫������ȡ����
					}
				}
			}
			else if(pItem->m_idProp==CLDSLineAngle::GetPropID("startCutWingY"))
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->m_bUserSpecStartCutAngle=TRUE;
					if(i==0)
					{	//����н���֫��Ϣ
						pLineAngle->cut_angle[1][0]=0;	//���߷���
						pLineAngle->cut_angle[1][1]=0;	//֫����
						if(pLineAngle->cut_wing[0]==2)	//Y֫��֫
							pLineAngle->cut_wing[0]=0;
						if(pLineAngle->cut_wing[0]==0)
						{	
							pLineAngle->cut_wing_para[0][0]=0;
							pLineAngle->cut_wing_para[0][1]=0;
							pLineAngle->cut_wing_para[0][2]=0;
						}
					}
					else if(i==2)
					{	//�����нǲ���
						pLineAngle->cut_angle[1][0]=cut_para[1];	//���߷���
						pLineAngle->cut_angle[1][1]=cut_para[0];	//֫����
						if(pLineAngle->cut_wing[0]==2)	//Y֫��֫
							pLineAngle->cut_wing[0]=0;
						if(pLineAngle->cut_wing[0]==0)
						{
							pLineAngle->cut_wing_para[0][0]=0;
							pLineAngle->cut_wing_para[0][1]=0;
							pLineAngle->cut_wing_para[0][2]=0;
						}
					}
					else //if(i==3)
					{	//������֫����
						pLineAngle->cut_angle[1][0]=0;	//���߷���
						pLineAngle->cut_angle[1][1]=0;	//֫����
						if(cut_para[0]+cut_para[1]+cut_para[2]>0)
							pLineAngle->cut_wing[0]=2;		//Y֫��֫
						pLineAngle->cut_wing_para[0][0]=cut_para[1];	//��ʾ�Ǹ�������ȡ����
						pLineAngle->cut_wing_para[0][1]=cut_para[0];	//��ʾ��һ֫��ȡ���
						pLineAngle->cut_wing_para[0][2]=cut_para[2];	//��ʾ֫������ȡ����
					}
				}
			}
			else if(pItem->m_idProp==CLDSLineAngle::GetPropID("endCutWingX"))
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->m_bUserSpecEndCutAngle=TRUE;
					if(i==0)
					{	//����н���֫��Ϣ
						pLineAngle->cut_angle[2][0]=0;	//���߷���
						pLineAngle->cut_angle[2][1]=0;	//֫����
						if(pLineAngle->cut_wing[1]==1)	//X֫��֫
							pLineAngle->cut_wing[1]=0;
						if(pLineAngle->cut_wing[1]==0)
						{
							pLineAngle->cut_wing_para[1][0]=0;
							pLineAngle->cut_wing_para[1][1]=0;
							pLineAngle->cut_wing_para[1][2]=0;
						}
					}
					else if(i==2)
					{	//�����нǲ���
						pLineAngle->cut_angle[2][0]=cut_para[1];	//���߷���
						pLineAngle->cut_angle[2][1]=cut_para[0];	//֫����
						if(pLineAngle->cut_wing[1]==1)	//X֫��֫
							pLineAngle->cut_wing[1]=0;
						if(pLineAngle->cut_wing[1]==0)
						{
							pLineAngle->cut_wing_para[1][0]=0;
							pLineAngle->cut_wing_para[1][1]=0;
							pLineAngle->cut_wing_para[1][2]=0;
						}
					}
					else //if(i==3)
					{	//������֫����
						pLineAngle->cut_angle[2][0]=0;	//���߷���
						pLineAngle->cut_angle[2][1]=0;	//֫����
						if(cut_para[0]+cut_para[1]+cut_para[2]>0)
							pLineAngle->cut_wing[1]=1;	//X֫��֫
						pLineAngle->cut_wing_para[1][0]=cut_para[1];	//��ʾ�Ǹ�������ȡ����
						pLineAngle->cut_wing_para[1][1]=cut_para[0];	//��ʾ��һ֫��ȡ���
						pLineAngle->cut_wing_para[1][2]=cut_para[2];	//��ʾ֫������ȡ����
					}
				}
			}
			else if(pItem->m_idProp==CLDSLineAngle::GetPropID("endCutWingY"))
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->m_bUserSpecEndCutAngle=TRUE;
					if(i==0)
					{	//����н���֫��Ϣ
						pLineAngle->cut_angle[3][0]=0;	//���߷���
						pLineAngle->cut_angle[3][1]=0;	//֫����
						if(pLineAngle->cut_wing[1]==2)	//Y֫��֫
							pLineAngle->cut_wing[1]=0;
						if(pLineAngle->cut_wing[1]==0)
						{
							pLineAngle->cut_wing_para[1][0]=0;
							pLineAngle->cut_wing_para[1][1]=0;
							pLineAngle->cut_wing_para[1][2]=0;
						}
					}
					else if(i==2)
					{	//�����нǲ���
						pLineAngle->cut_angle[3][0]=cut_para[1];	//���߷���
						pLineAngle->cut_angle[3][1]=cut_para[0];	//֫����
						if(pLineAngle->cut_wing[1]==2)	//X֫��֫
							pLineAngle->cut_wing[1]=0;
						if(pLineAngle->cut_wing[1]==0)
						{
							pLineAngle->cut_wing_para[1][0]=0;
							pLineAngle->cut_wing_para[1][1]=0;
							pLineAngle->cut_wing_para[1][2]=0;
						}
					}
					else //if(i==3)
					{	//������֫����
						pLineAngle->cut_angle[3][0]=0;	//���߷���
						pLineAngle->cut_angle[3][1]=0;	//֫����
						if(cut_para[0]+cut_para[1]+cut_para[2]>0)
							pLineAngle->cut_wing[1]=2;	//X֫��֫
						pLineAngle->cut_wing_para[1][0]=cut_para[1];	//��ʾ�Ǹ�������ȡ����
						pLineAngle->cut_wing_para[1][1]=cut_para[0];	//��ʾ��һ֫��ȡ���
						pLineAngle->cut_wing_para[1][2]=cut_para[2];	//��ʾ֫������ȡ����
					}
				}
			}
			//����ʼ�ն��н������Զ��ж��нǷ�ʽ
			CString sValue;
			if(pItem->m_idProp==CLDSLineAngle::GetPropID("startCutWingX")||pItem->m_idProp==CLDSLineAngle::GetPropID("startCutWingY"))
			{
				if(pItem->m_idProp==CLDSLineAngle::GetPropID("startCutWingX"))
					pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("startCutWingY"),sValue);
				else
					pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("startCutWingX"),sValue);
				if(sValue.GetLength()<1 && i==0)
					pFirstLineAngle->m_bUserSpecStartCutAngle=FALSE;
			}
			else if(pItem->m_idProp==CLDSLineAngle::GetPropID("endCutWingX")||pItem->m_idProp==CLDSLineAngle::GetPropID("endCutWingY"))
			{
				if(pItem->m_idProp==CLDSLineAngle::GetPropID("endCutWingX"))
					pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("endCutWingY"),sValue);
				else
					pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("endCutWingX"),sValue);
				if(sValue.GetLength()<1 && i==0)
					pFirstLineAngle->m_bUserSpecEndCutAngle=FALSE;
			}
#ifdef AFX_TARG_ENU_ENGLISH
			if(pFirstLineAngle->m_bUserSpecStartCutAngle)
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("startCutWing"),"User-Defined");
			else
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("startCutWing"),"Default");
			if(pFirstLineAngle->m_bUserSpecEndCutAngle)
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("endCutWing"),"User-Defined");
			else
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("endCutWing"),"Default");
#else
			if(pFirstLineAngle->m_bUserSpecStartCutAngle)
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("startCutWing"),"�û�ָ��");
			else
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("startCutWing"),"�Զ�����");
			if(pFirstLineAngle->m_bUserSpecEndCutAngle)
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("endCutWing"),"�û�ָ��");
			else
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("endCutWing"),"�Զ�����");
#endif
			bool bCurrTipStart=false;
			if(pItem->m_idProp==CLDSLineAngle::GetPropID("startCutWingX")||pItem->m_idProp==CLDSLineAngle::GetPropID("startCutWingY"))
				bCurrTipStart=true;
			//else if(pItem->m_idProp==CLDSLineAngle::GetPropID("endCutWingX")||pItem->m_idProp==CLDSLineAngle::GetPropID("endCutWingY"))
			//	bCurrTipStart=false;
			CModifiedPartsOperObject updator;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				if(bCurrTipStart)
					pLineAngle->SyncMirProp("oddStartOnSameLabel",0,LABEL_PROP::ANGLE_CUT_START);
				else
					pLineAngle->SyncMirProp("oddEndOnSameLabel",0,LABEL_PROP::ANGLE_CUT_END);
			}
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("FilletROfSX"))
		{
			double fFilletR=atof(valueStr);
			if(fFilletR<0 || fFilletR>=pFirstLineAngle->GetWidth())
				fFilletR=0;	//�ݲ�֧�ָ����򳬹�֫�����
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->dfFilletROfSX=fFilletR;
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("FilletROfSY"))
		{
			double fFilletR=atof(valueStr);
			if(fFilletR<0 || fFilletR>=pFirstLineAngle->GetWidth())
				fFilletR=0;	//�ݲ�֧�ָ����򳬹�֫�����
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->dfFilletROfSY=fFilletR;
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("FilletROfEX"))
		{
			double fFilletR=atof(valueStr);
			if(fFilletR<0 || fFilletR>=pFirstLineAngle->GetWidth())
				fFilletR=0;	//�ݲ�֧�ָ����򳬹�֫�����
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->dfFilletROfEX=fFilletR;
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("FilletROfEY"))
		{
			double fFilletR=atof(valueStr);
			if(fFilletR<0 || fFilletR>=pFirstLineAngle->GetWidth())
				fFilletR=0;	//�ݲ�֧�ָ����򳬹�֫�����
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->dfFilletROfEY=fFilletR;
		}
		/*
		//�ֶ�ָ����֫�н���Ϣ wht 11-07-04
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_bUserSpecStartCutAngle"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				if(valueStr.CompareNoCase("��")==0)
					pLineAngle->m_bUserSpecStartCutAngle = TRUE;
				else if(valueStr.CompareNoCase("��")==0)
					pLineAngle->m_bUserSpecStartCutAngle = FALSE;
			}
			//��������ֻ������ 
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("start_cut_angle_type"),!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("start_cut_angle_type")));
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[0][0]"),!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("cut_angle[0][0]")));
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[0][1]"),!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("cut_angle[0][1]")));
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[1][0]"),!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("cut_angle[1][0]")));
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[1][1]"),!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("cut_angle[1][1]")));
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_wing_para[0][0]"),!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("cut_wing_para[0][0]")));
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_wing_para[0][1]"),!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("cut_wing_para[0][1]")));
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_wing_para[0][2]"),!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("cut_wing_para[0][2]")));
		}
		//������֫�нǿ����޸ķ�ʽ wht 11-01-11
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("start_cut_angle_type"))
		{	//ʼ���н�����
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[0][0]"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[0][1]"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[1][0]"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[1][1]"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_wing_para[0][0]"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_wing_para[0][1]"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_wing_para[0][2]"));
			if(valueStr.CompareNoCase("0.X֫45���н�")==0)
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					if(!(pLineAngle->cut_angle[0][0]>0
						&&fabs(pLineAngle->cut_angle[0][0]-pLineAngle->cut_angle[0][1])<EPS))
					{	//���нǻ����н���Ϣ��Ϊ45���н�
						pLineAngle->cut_angle[0][0]=pLineAngle->cut_angle[0][1]
							=pLineAngle->GetWidth()-pLineAngle->GetThick();
					}
					pLineAngle->cut_wing[0]=0;	//�����֫��Ϣ
					pLineAngle->cut_angle[1][0]=pLineAngle->cut_angle[1][1]=0;	//���ʼ��Y֫�н���Ϣ
				}
			}
			else if(valueStr.CompareNoCase("1.Y֫45���н�")==0)
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					if(!(pLineAngle->cut_angle[1][0]>0
						&&fabs(pLineAngle->cut_angle[1][0]-pLineAngle->cut_angle[1][1])<EPS))
					{	//���нǻ����н���Ϣ��Ϊ45���н�
						pLineAngle->cut_angle[1][0]=pLineAngle->cut_angle[1][1]
							=pLineAngle->GetWidth()-pLineAngle->GetThick();
					}
					pLineAngle->cut_wing[0]=0;	//���ʼ����֫��Ϣ
					pLineAngle->cut_angle[0][0]=pLineAngle->cut_angle[0][1]=0;	//���ʼ��X֫�н���Ϣ
				}
			}
			else if(valueStr[0]=='2')
			{	//��ͨ�н�
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					pLineAngle->cut_wing[0]=0;	//���ʼ����֫��Ϣ
			}
			else if(valueStr[0]=='3')
			{	//X֫��֫
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->cut_wing[0]=1;	//��Ϊ��X֫
					pLineAngle->cut_angle[0][0]=pLineAngle->cut_angle[0][1]=0;	//���ʼ��X֫�н���Ϣ
					pLineAngle->cut_angle[1][0]=pLineAngle->cut_angle[1][1]=0;	//���ʼ��Y֫�н���Ϣ
				}
			}
			else if(valueStr[0]=='4')
			{	//Y֫��֫
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->cut_wing[0]=2;	//��Ϊ��Y֫
					pLineAngle->cut_angle[0][0]=pLineAngle->cut_angle[0][1]=0;	//���ʼ��X֫�н���Ϣ
					pLineAngle->cut_angle[1][0]=pLineAngle->cut_angle[1][1]=0;	//���ʼ��Y֫�н���Ϣ
				}
			}
			else if(valueStr.CompareNoCase("6.����֫�н�")==0)
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					//��֫��Ϣ
					pLineAngle->cut_wing[0]=0;
					pLineAngle->cut_wing_para[0][0]=pLineAngle->cut_wing_para[0][1]
						=pLineAngle->cut_wing_para[0][2]=pLineAngle->cut_wing[0]=0;
					//�н���Ϣ
					pLineAngle->cut_angle[0][0]=pLineAngle->cut_angle[0][1]
						=pLineAngle->cut_angle[1][0]=pLineAngle->cut_angle[1][1]=0;
				}
			}
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("startCutWing"),&pParentItem);
			if(pFindItem)
			{
				if(pFirstLineAngle->cut_wing[0]==1||valueStr[0]=='3'
					||pFirstLineAngle->cut_wing[0]==2||valueStr[0]=='4')
				{	
					//��һ֫��ȡ���
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "��֫���";
					if(pFirstLineAngle->cut_wing[0]==1)
						lpInfo->m_strPropHelp = "X֫��֫,��һ֫��ȡ���";
					else 
						lpInfo->m_strPropHelp = "Y֫��֫,��һ֫��ȡ���";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,TRUE);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[0][1]");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
					pPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
					//�Ǹ�������ȡ����
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "������֫����";
					if(pFirstLineAngle->cut_wing[0]==1)
						lpInfo->m_strPropHelp = "X֫��֫,�Ǹ�������ȡ����";
					else 
						lpInfo->m_strPropHelp = "Y֫��֫,�Ǹ�������ȡ����";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,TRUE);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[0][0]");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
					pPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
					//֫������ȡ����
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "֫������ȡ����";
					if(pFirstLineAngle->cut_wing[0]==1)
						lpInfo->m_strPropHelp = "X֫��֫,֫������ȡ����";
					else 
						lpInfo->m_strPropHelp = "Y֫��֫,֫������ȡ����";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,TRUE);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[0][2]");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
					pPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
				if(pFirstLineAngle->cut_angle[0][0]>0||valueStr[0]=='2')
				{	//֫���нǿ��
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "X֫�нǿ��";
					lpInfo->m_strPropHelp = "X֫֫���нǿ��";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[0][1]");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
					pPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
					//�����нǳ���
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "X֫�нǳ���";
					lpInfo->m_strPropHelp = "X֫�����нǳ���";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[0][0]");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
					pPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
				if(pFirstLineAngle->cut_angle[1][0]>0||valueStr[0]=='2')
				{	//Y֫���нǿ��
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "Y֫�нǿ��";
					lpInfo->m_strPropHelp = "Y֫֫���нǿ��";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[1][1]");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
					pPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
					//Y�����нǳ���
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "Y֫�нǳ���";
					lpInfo->m_strPropHelp = "Y֫�����нǳ���";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[1][0]");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
					pPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
			}
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("startCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cut_wing_para[0][0]"))
		{
			CString sCutWingType="";
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("start_cut_angle_type"),sCutWingType);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cut_wing_para[0][0]=atof(valueStr);
				//��֫���ݴ��ڲ�ȫΪ0ʱ�ٸı�Ϊ��֫ wht 11-01-19
				if(pLineAngle->cut_wing_para[0][0]>0||pLineAngle->cut_wing_para[0][1]>0||pLineAngle->cut_wing_para[0][2]>0)
				{
					if(sCutWingType.CompareNoCase("3.X֫��֫")==0)		//X֫��֫
						pLineAngle->cut_wing[0]=1;
					else if(sCutWingType.CompareNoCase("4.Y֫��֫")==0)	//Y֫��֫
						pLineAngle->cut_wing[0]=2;
				}
				else 
					pLineAngle->cut_wing[0]=0;
			}
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("startCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));	
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cut_wing_para[0][1]"))
		{
			CString sCutWingType="";
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("start_cut_angle_type"),sCutWingType);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cut_wing_para[0][1]=atof(valueStr);
				//��֫���ݴ��ڲ�ȫΪ0ʱ�ٸı�Ϊ��֫ wht 11-01-19
				if(pLineAngle->cut_wing_para[0][0]>0||pLineAngle->cut_wing_para[0][1]>0||pLineAngle->cut_wing_para[0][2]>0)
				{
					if(sCutWingType.CompareNoCase("3.X֫��֫")==0)		//X֫��֫
						pLineAngle->cut_wing[0]=1;
					else if(sCutWingType.CompareNoCase("4.Y֫��֫")==0)	//Y֫��֫
						pLineAngle->cut_wing[0]=2;
				}
				else 
					pLineAngle->cut_wing[0]=0;
			}
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("startCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cut_wing_para[0][2]"))
		{
			CString sCutWingType="";
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("start_cut_angle_type"),sCutWingType);;
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cut_wing_para[0][2]=atof(valueStr);
				//��֫���ݴ��ڲ�ȫΪ0ʱ�ٸı�Ϊ��֫ wht 11-01-19
				if(pLineAngle->cut_wing_para[0][0]>0||pLineAngle->cut_wing_para[0][1]>0||pLineAngle->cut_wing_para[0][2]>0)
				{
					if(sCutWingType.CompareNoCase("3.X֫��֫")==0)		//X֫��֫
						pLineAngle->cut_wing[0]=1;
					else if(sCutWingType.CompareNoCase("4.Y֫��֫")==0)	//Y֫��֫
						pLineAngle->cut_wing[0]=2;
				}
				else 
					pLineAngle->cut_wing[0]=0;
			}
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("startCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cut_angle[0][0]"))
		{
			tempStr.Empty();
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("start_cut_angle_type"),tempStr);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cut_angle[0][0]=atof(valueStr);
				if(tempStr[0]=='0')	//X֫45���н�
					pLineAngle->cut_angle[0][1]=atof(valueStr);
			}
			if(tempStr[0]=='0')	//X֫45���н� �����нǳ��ȵ����нǿ��
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("cut_angle[0][1]"),valueStr);
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("startCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cut_angle[0][1]"))
		{
			tempStr.Empty();
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("start_cut_angle_type"),tempStr);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cut_angle[0][1]=atof(valueStr);
				if(tempStr[0]=='0')	//X֫45���н�
					pLineAngle->cut_angle[0][0]=atof(valueStr);
			}
			if(tempStr[0]=='0')	//X֫45���н� �����нǳ��ȵ����нǿ��
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("cut_angle[0][0]"),valueStr);
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("startCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cut_angle[1][0]"))
		{
			tempStr.Empty();
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("start_cut_angle_type"),tempStr);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cut_angle[1][0]=atof(valueStr);
				if(tempStr[0]=='1')	//Y֫45���н�
					pLineAngle->cut_angle[1][1]=atof(valueStr);
			}
			if(tempStr[0]=='1')	//Y֫45���н� �����нǳ��ȵ����нǿ��
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("cut_angle[1][1]"),valueStr);
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("startCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cut_angle[1][1]"))
		{
			tempStr.Empty();
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("start_cut_angle_type"),tempStr);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cut_angle[1][1]=atof(valueStr);
				if(tempStr[0]=='1')	//Y֫45���н�
					pLineAngle->cut_angle[1][0]=atof(valueStr);
			}
			if(tempStr[0]=='1')	//Y֫45���н� �����нǳ��ȵ����нǿ��
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("cut_angle[1][0]"),valueStr);
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("startCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		//�ֶ�ָ����֫�н���Ϣ wht 11-07-04
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_bUserSpecEndCutAngle"))
		{
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				if(valueStr.CompareNoCase("��")==0)
					pLineAngle->m_bUserSpecEndCutAngle = TRUE;
				else if(valueStr.CompareNoCase("��")==0)
					pLineAngle->m_bUserSpecEndCutAngle = FALSE;
			}
			//��������ֻ������ 
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("end_cut_angle_type"),!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("end_cut_angle_type")));
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[2][0]"),!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("cut_angle[2][0]")));
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[2][1]"),!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("cut_angle[2][1]")));
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[3][0]"),!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("cut_angle[3][0]")));
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_angle[3][1]"),!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("cut_angle[3][1]")));
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_wing_para[1][0]"),!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("cut_wing_para[1][0]")));
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_wing_para[1][1]"),!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("cut_wing_para[1][1]")));
			pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("cut_wing_para[1][2]"),!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("cut_wing_para[1][2]")));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("end_cut_angle_type"))
		{	//�ն��н�����
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[2][0]"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[2][1]"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[3][0]"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_angle[3][1]"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_wing_para[1][0]"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_wing_para[1][1]"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("cut_wing_para[1][2]"));
			if(valueStr.CompareNoCase("0.X֫45���н�")==0)
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					if(!(pLineAngle->cut_angle[2][0]>0
						&&fabs(pLineAngle->cut_angle[2][0]-pLineAngle->cut_angle[2][1])<EPS))
					{	//���нǻ����н���Ϣ��Ϊ45���н�
						pLineAngle->cut_angle[2][0]=pLineAngle->cut_angle[2][1]
							=pLineAngle->GetWidth()-pLineAngle->GetThick();
					}
					pLineAngle->cut_wing[1]=0;	//����ն���֫��Ϣ
					pLineAngle->cut_angle[3][0]=pLineAngle->cut_angle[3][1]=0;	//����ն�Y֫�н���Ϣ
				}
			}
			else if(valueStr.CompareNoCase("1.Y֫45���н�")==0)
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					if(!(pLineAngle->cut_angle[3][0]>0
						&&fabs(pLineAngle->cut_angle[3][0]-pLineAngle->cut_angle[3][1])<EPS))
					{	//���нǻ����н���Ϣ��Ϊ45���н�
						pLineAngle->cut_angle[3][0]=pLineAngle->cut_angle[3][1]
							=pLineAngle->GetWidth()-pLineAngle->GetThick();
					}
					pLineAngle->cut_wing[1]=0;	//����ն���֫��Ϣ
					pLineAngle->cut_angle[2][0]=pLineAngle->cut_angle[2][1]=0;	//����ն�X֫�н���Ϣ
				}
			}
			else if(valueStr[0]=='2')
			{	//��ͨ�н�
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					pLineAngle->cut_wing[1]=0;	//����ն���֫��Ϣ
			}
			else if(valueStr[0]=='3')
			{	//X֫��֫
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->cut_wing[1]=1;	//��Ϊ��X֫
					pLineAngle->cut_angle[2][0]=pLineAngle->cut_angle[2][1]=0;	//����ն�X֫�н���Ϣ
					pLineAngle->cut_angle[3][0]=pLineAngle->cut_angle[3][1]=0;	//����ն�Y֫�н���Ϣ
				}
			}
			else if(valueStr[0]=='4')
			{	//Y֫��֫
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					pLineAngle->cut_wing[1]=2;	//��Ϊ��Y֫
					pLineAngle->cut_angle[2][0]=pLineAngle->cut_angle[2][1]=0;	//����ն�X֫�н���Ϣ
					pLineAngle->cut_angle[3][0]=pLineAngle->cut_angle[3][1]=0;	//����ն�Y֫�н���Ϣ
				}
			}
			else if(valueStr.CompareNoCase("6.����֫�н�")==0)
			{
				for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				{
					//��֫��Ϣ
					pLineAngle->cut_wing[1]=0;
					pLineAngle->cut_wing_para[1][0]=pLineAngle->cut_wing_para[1][1]
						=pLineAngle->cut_wing_para[1][2]=pLineAngle->cut_wing[1]=0;
					//�н���Ϣ
					pLineAngle->cut_angle[2][0]=pLineAngle->cut_angle[2][1]
						=pLineAngle->cut_angle[3][0]=pLineAngle->cut_angle[3][1]=0;
				}
			}
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("endCutWing"),&pParentItem);
			if(pFindItem)
			{
				if(pFirstLineAngle->cut_wing[1]==1||valueStr[0]=='3'
					||pFirstLineAngle->cut_wing[1]==2||valueStr[0]=='4')
				{	
					//��һ֫��ȡ���
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "��֫���";
					if(pFirstLineAngle->cut_wing[1]==1)
						lpInfo->m_strPropHelp = "X֫��֫,��һ֫��ȡ���";
					else 
						lpInfo->m_strPropHelp = "Y֫��֫,��һ֫��ȡ���";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,TRUE);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[1][1]");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
					pPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
					//�Ǹ�������ȡ����
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "������֫����";
					if(pFirstLineAngle->cut_wing[1]==1)
						lpInfo->m_strPropHelp = "X֫��֫,�Ǹ�������ȡ����";
					else 
						lpInfo->m_strPropHelp = "Y֫��֫,�Ǹ�������ȡ����";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,TRUE);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[1][0]");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
					pPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
					//֫������ȡ����
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "֫������ȡ����";
					if(pFirstLineAngle->cut_wing[1]==1)
						lpInfo->m_strPropHelp = "X֫��֫,֫������ȡ����";
					else 
						lpInfo->m_strPropHelp = "Y֫��֫,֫������ȡ����";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,TRUE);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[1][2]");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
					pPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
				if(pFirstLineAngle->cut_angle[2][0]>0||valueStr.CompareNoCase("2.��ͨ�н�")==0)
				{	//֫���нǿ��
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "X֫�нǿ��";
					lpInfo->m_strPropHelp = "X֫֫���нǿ��";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[2][1]");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
					pPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
					//�����нǳ���
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "X֫�нǳ���";
					lpInfo->m_strPropHelp = "X֫�����нǳ���";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[2][0]");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
					pPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
				if(pFirstLineAngle->cut_angle[3][0]>0||valueStr.CompareNoCase("2.��ͨ�н�")==0)
				{	//Y֫���нǿ��
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "Y֫�нǿ��";
					lpInfo->m_strPropHelp = "Y֫֫���нǿ��";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[3][1]");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
					pPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
					//Y�����нǳ���
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
					lpInfo->m_strPropName = "Y֫�нǳ���";
					lpInfo->m_strPropHelp = "Y֫�����нǳ���";
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[3][0]");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(pView->selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = pItem->m_dwPropGroup;
					pPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
											pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
			}
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("endCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cut_wing_para[1][0]"))
		{
			CString sCutWingType="";
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("end_cut_angle_type"),sCutWingType);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cut_wing_para[1][0]=atof(valueStr);
				//��֫���ݴ��ڲ�ȫΪ0ʱ�ٸı�Ϊ��֫ wht 11-01-19
				if(pLineAngle->cut_wing_para[1][0]>0||pLineAngle->cut_wing_para[1][1]>0||pLineAngle->cut_wing_para[1][2]>0)
				{
					if(sCutWingType.CompareNoCase("3.X֫��֫")==0)		//X֫��֫
						pLineAngle->cut_wing[1]=1;
					else if(sCutWingType.CompareNoCase("4.Y֫��֫")==0)	//Y֫��֫
						pLineAngle->cut_wing[1]=2;
				}
				else 
					pLineAngle->cut_wing[1]=0;
			}
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("endCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cut_wing_para[1][1]"))
		{
			CString sCutWingType="";
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("end_cut_angle_type"),sCutWingType);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cut_wing_para[1][1]=atof(valueStr);
				//��֫���ݴ��ڲ�ȫΪ0ʱ�ٸı�Ϊ��֫ wht 11-01-19
				if(pLineAngle->cut_wing_para[1][0]>0||pLineAngle->cut_wing_para[1][1]>0||pLineAngle->cut_wing_para[1][2]>0)
				{
					if(sCutWingType.CompareNoCase("3.X֫��֫")==0)		//X֫��֫
						pLineAngle->cut_wing[1]=1;
					else if(sCutWingType.CompareNoCase("4.Y֫��֫")==0)	//Y֫��֫
						pLineAngle->cut_wing[1]=2;
				}
				else 
					pLineAngle->cut_wing[1]=0;
			}
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("endCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cut_wing_para[1][2]"))
		{
			CString sCutWingType="";
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("end_cut_angle_type"),sCutWingType);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cut_wing_para[1][2]=atof(valueStr);
				//��֫���ݴ��ڲ�ȫΪ0ʱ�ٸı�Ϊ��֫ wht 11-01-19
				if(pLineAngle->cut_wing_para[1][0]>0||pLineAngle->cut_wing_para[1][1]>0||pLineAngle->cut_wing_para[1][2]>0)
				{
					if(sCutWingType.CompareNoCase("3.X֫��֫")==0)		//X֫��֫
						pLineAngle->cut_wing[1]=1;
					else if(sCutWingType.CompareNoCase("4.Y֫��֫")==0)	//Y֫��֫
						pLineAngle->cut_wing[1]=2;
				}
				else 
					pLineAngle->cut_wing[1]=0;
			}
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("endCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cut_angle[2][0]"))
		{
			tempStr.Empty();
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("end_cut_angle_type"),tempStr);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cut_angle[2][0]=atof(valueStr);
				if(tempStr[0]=='0')	//X֫45���н�
					pLineAngle->cut_angle[2][1]=atof(valueStr);
			}
			if(tempStr[0]=='0')	//X֫45���н� �����нǳ��ȵ����нǿ��
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("cut_angle[2][1]"),valueStr);
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("endCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cut_angle[2][1]"))
		{
			tempStr.Empty();
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("end_cut_angle_type"),tempStr);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cut_angle[2][1]=atof(valueStr);
				if(tempStr[0]=='0')	//X֫45���н�
					pLineAngle->cut_angle[2][0]=atof(valueStr);
			}
			if(tempStr[0]=='0')	//X֫45���н� �����нǳ��ȵ����нǿ��
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("cut_angle[2][0]"),valueStr);
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("endCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cut_angle[3][0]"))
		{
			tempStr.Empty();
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("end_cut_angle_type"),tempStr);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cut_angle[3][0]=atof(valueStr);
				if(tempStr[0]=='1')	//Y֫45���н�
					pLineAngle->cut_angle[3][1]=atof(valueStr);
			}
			if(tempStr[0]=='1')	//Y֫45���н� �����нǳ��ȵ����нǿ��
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("cut_angle[3][1]"),valueStr);
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("endCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("cut_angle[3][1]"))
		{
			tempStr.Empty();
			pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("end_cut_angle_type"),tempStr);
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->cut_angle[3][1]=atof(valueStr);
				if(tempStr[0]=='1')	//Y֫45���н�
					pLineAngle->cut_angle[3][0]=atof(valueStr);
			}
			if(tempStr[0]=='1')	//Y֫45���н� �����нǳ��ȵ����нǿ��
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("cut_angle[3][0]"),valueStr);
			//����ʼ���н���֫��ʾ
			long item_id=CLDSLineAngle::GetPropID("endCutWing");
			if(CLDSLineAngle::GetPropValueStr(pFirstLineAngle,item_id,sText)>=0)
				pPropList->SetItemPropValue(item_id,CString(sText));
		}
		*/
		//Ԥ����
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("m_hArcLift"))
		{
			CLDSArcLift *pLift=NULL;
			for(pLift=Ta.ArcLift.GetFirst();pLift;pLift=Ta.ArcLift.GetNext())
			{
				CString sLiftName;
				sLiftName.Format("%s(%d)",pLift->name,pLift->iNo);
				if(sLiftName.CompareNoCase(valueStr)==0)
					break;
			}
			for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				if(pLift)
					pLineAngle->SetArcLift(pLift->handle);
				else 
					pLineAngle->SetArcLift(0);
				CAngleMoveOperObject moveangle(pLineAngle);
				pLineAngle->CalPosition(true);
				pLineAngle->UpdateRelaObjArcLiftProp();
			}
			pInsertItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desStartPos.jgber_cal_style"),NULL);
			if(pInsertItem)
				UpdateLineAngle_ArcLift_TreeItem(pPropList,pInsertItem,TRUE);
			pInsertItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desEndPos.jgber_cal_style"),NULL);
			if(pInsertItem)
				UpdateLineAngle_ArcLift_TreeItem(pPropList,pInsertItem,FALSE);	
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("StartOddCanAcrossMiddleSect")||
				pItem->m_idProp==CLDSLineAngle::GetPropID("EndOddCanAcrossMiddleSect"))
		{
			long idPropStart=CLDSLineAngle::GetPropID("StartOddCanAcrossMiddleSect");
			long idPropEnd=CLDSLineAngle::GetPropID("EndOddCanAcrossMiddleSect");
			if (pItem->m_idProp==idPropStart)
				pPropList->SetItemPropValue(idPropEnd,tem_str);
			else
				pPropList->SetItemPropValue(idPropStart,tem_str);
			for (pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
			{
				pLineAngle->m_blOddByBoltCanAcrossMiddleSect=(tem_str[0]=='1');
				if (pLineAngle->desStartOdd.m_iOddCalStyle==1)
					pLineAngle->CalStartOddment();
				if (pLineAngle->desEndOdd.m_iOddCalStyle==1)
					pLineAngle->CalEndOddment();
			}
		}
		else if (pItem->m_idProp==CLDSLineAngle::GetPropID("blScatterBoltLocaTolerance"))
		{
			for (pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
				pLineAngle->m_blScatterBoltLocaTolerance=(tem_str[0]=='1');
		}
		else if(!ModifyLineAngleDesignSubProperty(pPropList,pItem,valueStr)&&!ModifyLineAnglePositionSubProperty(pPropList,pItem,valueStr))
		{	//������Ϊ�˽���ǰ�����ֽ�Ϊ���ɸ��̺���
			if(pFirstLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				if(pItem->m_idProp==CLDSGroupLineAngle::GetPropID("group_style"))
				{
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					{
						if (pLineAngle->GetClassTypeId() == CLS_GROUPLINEANGLE)
						{
							CLDSGroupLineAngle* pGroupAngle = (CLDSGroupLineAngle*)pLineAngle;
							WORD wiPrevSectStyle = pGroupAngle->wSectStyle;
							pGroupAngle->group_style = atoi(tem_str);
							if (wiPrevSectStyle == 2 || pGroupAngle->wSectStyle == 2)
								pGroupAngle->AppendSonJg();	//�任ǰ��任���漰ʮ����ƴ�Ǹ�ʱ��һ�����漰���ӽǸ������仯 wjh-2019.5.21
							UpdateGroupAngleSolid(pGroupAngle, pView);
						}
					}
				}
				else if(pItem->m_idProp==CLDSGroupLineAngle::GetPropID("jg_space"))
				{
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					{
						if(pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
							((CLDSGroupLineAngle*)pLineAngle)->jg_space=atof(tem_str);
					}
				}
				else if(pItem->m_idProp==CLDSGroupLineAngle::GetPropID("m_bUpdateSonAngleOddment"))
				{
					BOOL m_bUpdateSonAngleOddment=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("No")==0)
#else
					if(valueStr.CompareNoCase("��")==0)
#endif
						m_bUpdateSonAngleOddment=FALSE;
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					{
						if(pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
						{
							((CLDSGroupLineAngle*)pLineAngle)->m_bUpdateSonAngleOddment=m_bUpdateSonAngleOddment;
							((CLDSGroupLineAngle*)pLineAngle)->UpdateSonJgOddment();
						}
					}
				}
				else if(pItem->m_idProp==CLDSGroupLineAngle::GetPropID("ciMasterIndex"))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Diagonal-I")==0||valueStr.CompareNoCase("T-X")==0)
						((CLDSGroupLineAngle*)pFirstLineAngle)->ciMasterIndex=0;
					else if(valueStr.CompareNoCase("Diagonal-II")==0||valueStr.CompareNoCase("T-Y")==0)
						((CLDSGroupLineAngle*)pFirstLineAngle)->ciMasterIndex=1;
#else
					if(valueStr.CompareNoCase("�Խ�I��")==0||valueStr.CompareNoCase("T-X��")==0)
						((CLDSGroupLineAngle*)pFirstLineAngle)->ciMasterIndex=0;
					else if(valueStr.CompareNoCase("�Խ�II��")==0||valueStr.CompareNoCase("T-Y��")==0)
						((CLDSGroupLineAngle*)pFirstLineAngle)->ciMasterIndex=1;
#endif
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					{
						CLDSGroupLineAngle* pGroupAngle = NULL;
						if (pLineAngle->GetClassTypeId() == CLS_GROUPLINEANGLE)
							pGroupAngle = (CLDSGroupLineAngle*)pLineAngle;
						if(pLineAngle!=pFirstLineAngle&&pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
						{
							pGroupAngle->ciMasterIndex=((CLDSGroupLineAngle*)pFirstLineAngle)->ciMasterIndex;
							//((CLDSGroupLineAngle*)pLineAngle)->UpdateSonJgOddment();
						}
						if (pGroupAngle->wSectStyle == 0)
						{	//�Խ����
							DESIGN_JGWING_NORM media = pGroupAngle->des_norm_x;
							pGroupAngle->des_norm_x = pGroupAngle->des_norm_y;
							pGroupAngle->des_norm_y = media;	//Yi=Xo;
							if (pGroupAngle->ciMasterIndex==0)
							{
								pGroupAngle->des_norm_x.ReverseDirection();	//Xi=-Yo
								GEPOINT vxOldWingNormY = pGroupAngle->vyWingNorm;
								pGroupAngle->SetBriefWingNormY(pGroupAngle->vxWingNorm,false);
								pGroupAngle->SetBriefWingNormX(-vxOldWingNormY);
							}
							else
							{	//Ϊ��֤�л��Ŀ����Խ������ִ��� wjh-2019.10.23
								pGroupAngle->des_norm_y.ReverseDirection();	//Xi=-Yo
								GEPOINT vxOldWingNormY = pGroupAngle->vyWingNorm;
								pGroupAngle->SetBriefWingNormY(-pGroupAngle->vxWingNorm,false);
								pGroupAngle->SetBriefWingNormX(vxOldWingNormY);
							}
							UpdateGroupAngleSolid(pGroupAngle, pView);
						}
						else if (pGroupAngle->wSectStyle == 1)
						{	//T�����
							UpdateGroupAngleSolid(pGroupAngle, pView);
						}
					}
				}
				else if(pItem->m_idProp==CLDSGroupLineAngle::GetPropID("ciMasterDirectIndex"))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					if(valueStr.CompareNoCase("Virtual angle")==0)
						((CLDSGroupLineAngle*)pFirstLineAngle)->ciMasterDirectIndex=0;
					else if(valueStr.CompareNoCase("1# angle")==0)
						((CLDSGroupLineAngle*)pFirstLineAngle)->ciMasterDirectIndex=1;
					else if(valueStr.CompareNoCase("1# angle")==0)
						((CLDSGroupLineAngle*)pFirstLineAngle)->ciMasterDirectIndex=2;
#else
					if(valueStr.CompareNoCase("����Ǹ�")==0)
						((CLDSGroupLineAngle*)pFirstLineAngle)->ciMasterDirectIndex=0;
					else if(valueStr.CompareNoCase("1�ŽǸ�")==0)
						((CLDSGroupLineAngle*)pFirstLineAngle)->ciMasterDirectIndex=1;
					else if(valueStr.CompareNoCase("2�ŽǸ�")==0)
						((CLDSGroupLineAngle*)pFirstLineAngle)->ciMasterDirectIndex=2;
#endif
					for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
					{
						if(pLineAngle!=pFirstLineAngle&&pLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
						{
							((CLDSGroupLineAngle*)pLineAngle)->ciMasterDirectIndex=((CLDSGroupLineAngle*)pFirstLineAngle)->ciMasterDirectIndex;
							//((CLDSGroupLineAngle*)pLineAngle)->UpdateSonJgOddment();
						}
					}
				}
				else
					return FALSE;
			}
			else 
				return FALSE;
		}
#ifdef AFX_TARG_ENU_ENGLISH
		if(valueStr.CompareNoCase("Specify")==0&&(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.spatialOperationStyle")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.spatialOperationStyle")))
#else
		if(valueStr.CompareNoCase("ָ����׼��")==0&&(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.spatialOperationStyle")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.spatialOperationStyle")))
#endif
			bReCalAnglePos=false;	//��ָ����׼��ʱ��һ�����λ�ö��Ǵ�ģ����¼���û������
		CAngleMoveOperObject anglemodified;
		for(pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetFirst();pLineAngle;pLineAngle=(CLDSLineAngle*)pView->selectObjs.GetNext())
		{
			if(bReCalAnglePos)
			{
				//CAngleMoveOperObject moveangle(pLineAngle);
				pLineAngle->CalPosition(true);
				pLineAngle->cal_x_wing_norm();
				pLineAngle->cal_y_wing_norm();
				pLineAngle->SetModified();
				pLineAngle->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength());
				CLDSView::RedrawAngleFrameDrawing(pLineAngle);
			}
			g_pSolidDraw->NewSolidPart(pLineAngle->GetSolidPartObject());
			//�޸�����ͷ���������������˨���Լ���ذ�
			/*if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddStart")||pItem->m_idProp==CLDSLineAngle::GetPropID("oddEnd"))
			{	
				CLsRef *pLsRef=NULL;
				CLDSBolt *pFirstBolt=NULL;
				for(pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
				{
					if(pFirstBolt==NULL)
					{
						if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddStart")&&pLsRef->start1_end2_mid0==1)
							pFirstBolt=pLsRef->GetLsPtr();	//ʼ����˨
						else if(pItem->m_idProp==CLDSLineAngle::GetPropID("oddEnd")&&pLsRef->start1_end2_mid0==2)
							pFirstBolt=pLsRef->GetLsPtr();	//�ն���˨
					}
					(*pLsRef)->SetModified();
					(*pLsRef)->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart((*pLsRef)->GetSolidPartObject());
				}
				if(pFirstBolt)
				{
					CLDSPlate *pPlate=NULL;
					for(pPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pPlate;pPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
					{
						if(pPlate->FindLsByHandle(pFirstBolt->handle))
						{
							pPlate->SetModified();
							pPlate->Create3dSolidModel(g_sysPara.bDisplayAllHole,g_pSolidOper->GetScaleUserToScreen(),g_pSolidSet->GetArcSamplingLength(),g_sysPara.display.nSmoothness);
							g_pSolidDraw->NewSolidPart(pPlate->GetSolidPartObject());
							break;
						}
					}
				}
			}*/
		}
		if(pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.huoqu_jg_h")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.huoqu_dist")||
			pItem->m_idProp==CLDSLineAngle::GetPropID("desStartPos.spatialOperationStyle"))
		{	//����ʼ�˹̶�����ͷ
			strcpy(tem_str,"");
			SetPropItemReadOnly(pFirstLineAngle,"oddStartExtra",
				pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("oddStartExtra")));
			if(pFirstLineAngle->GetPropValueStr(CLDSLineAngle::GetPropID("oddStartExtra"),tem_str)>0)
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("oddStartExtra"),CString(tem_str));
			SetPropItemReadOnly(pFirstLineAngle,"oddStartExtra",TRUE);
		}
		else if(pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.huoqu_jg_h")||
				pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.huoqu_dist")||
				pItem->m_idProp==CLDSLineAngle::GetPropID("desEndPos.spatialOperationStyle"))
		{	//�����ն˹̶�����ͷ
			strcpy(tem_str,"");
			SetPropItemReadOnly(pFirstLineAngle,"oddEndExtra",
				pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("oddEndExtra")));
			if(pFirstLineAngle->GetPropValueStr(CLDSLineAngle::GetPropID("oddEndExtra"),tem_str)>0)
				pPropList->SetItemPropValue(CLDSLineAngle::GetPropID("oddEndExtra"),CString(tem_str));
			SetPropItemReadOnly(pFirstLineAngle,"oddEndExtra",TRUE);
		}
		g_pSolidDraw->Draw();
	}
	catch(char* sError)
	{
		pPropList->SetPromptString(sError);
		AfxMessageBox(sError);
	}
	return TRUE;
}
void UpdateLineAngle_OpenSlot_TreeItem(CPropertyList *pPropList,CPropTreeItem *pParentItem)
{
	CLDSView *pView = ((CMainFrame*)AfxGetMainWnd())->GetLDSView();
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)pView->selectObjs.GetFirst();
	pPropList->DeleteAllSonItems(pParentItem);
	//
	CPropTreeItem *pPropItem=NULL,*pSonItem=NULL;
	for(UINT i=0;i<pFirstLineAngle->xarrOpenCuts.Count;i++)
	{
		CItemInfo *lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName.Format("Open Slot%d",i+1);
		lpInfo->m_strPropHelp.Format("Open Slot%d",i+1);
#else
		lpInfo->m_strPropName.Format("����%d",i+1);
		lpInfo->m_strPropHelp.Format("����%d",i+1);
#endif
		CXhChar50 sKey("OpenSlot%d",i+1);
		pPropItem=pPropList->InsertItem(pParentItem,lpInfo, -1,true);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID(sKey);
		pPropItem->m_dwPropGroup = pParentItem->m_dwPropGroup;
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
	}
}
CString GetBlockListStr();
void CLDSView::DisplayLineAngleProperty(BOOL bRebuild)
{
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem *pPropItem, *pSonPropItem,*pRoot=pPropList->GetRootItem();
	CItemInfo* lpInfo;

	CXhChar500 sText;
	const int GROUP_GENERAL=1,GROUP_STATMAT=2,GROUP_START=3,GROUP_END=4,GROUP_DESIGN=5;
	if(bRebuild)
	{	//��Ҫ�ؽ���������ҳ
		pPropDlg->m_arrPropGroupLabel.RemoveAll();
		pPropDlg->m_arrPropGroupLabel.SetSize(5);
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"General");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_STATMAT-1,"Process");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"Start");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"End");
#else
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_GENERAL-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_STATMAT-1,"����");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_START-1,"ʼ��λ��");
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_END-1,"�ն�λ��");
#endif
#if !defined(__TSA_)&&!defined(__LDS_)
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DESIGN-1,"Bolt");
#else
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DESIGN-1,"��˨");
#endif
#else
#ifdef AFX_TARG_ENU_ENGLISH
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DESIGN-1,"Design");
#else
		pPropDlg->m_arrPropGroupLabel.SetAt(GROUP_DESIGN-1,"���");
#endif
#endif
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		if(pFirstLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
			pPropDlg->RefreshTabCtrl(CLDSGroupLineAngle::m_iCurDisplayPropGroup);
		else 
#endif
			pPropDlg->RefreshTabCtrl(CLDSLineAngle::m_iCurDisplayPropGroup);
		pPropList->CleanTree();
		pPropList->m_nObjClassTypeId = pFirstLineAngle->GetClassTypeId();
		pPropList->SetModifyValueFunc(ModifyLineAngleProperty);
		pPropList->SetButtonClickFunc(LineAngleButtonClick);
		pPropList->SetModifyStatusFunc(ModifyLineAngleItemStatus);
		pPropList->SetPropHelpPromptFunc(LineAnglePropItemHelpLiveString);
		pPropList->SetPopMenuClickFunc(FireAnglePopMenuClick);
		//��ʼ��������ֻ��״̬��ʼֵ,���ڹ�����ӳ�乹�����Բ�����༭ wht 12-10-16
		if(!pFirstLineAngle->BelongModel()->IsTowerModel()||pFirstLineAngle->IsShadowObject())
			CPropTreeItem::m_bDefaultReadOnlyState=TRUE;
		else 
			CPropTreeItem::m_bDefaultReadOnlyState=FALSE;
		//�Ǹֻ�����Ϣ
		lpInfo = new CItemInfo();
		if(pFirstLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Group Angle Basic Info";
			lpInfo->m_strPropHelp = "The basic information of group angle";
#else
			lpInfo->m_strPropName = "��ϽǸֻ�����Ϣ";
			lpInfo->m_strPropHelp = "��ϽǸֻ�����Ϣ";
#endif
		}
		else
		{
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Angle Basic Info";
			lpInfo->m_strPropHelp = "The basic information of angle";
#else
			lpInfo->m_strPropName = "�Ǹֻ�����Ϣ";
			lpInfo->m_strPropHelp = "�Ǹֻ�����Ϣ";
#endif
		}
		CPropTreeItem *pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLineAngle::GetPropID("basicInfo");
		if(CLDSLineAngle::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_STATMAT)
			|GetSingleWord(GROUP_DESIGN)|GetSingleWord(GROUP_START)|GetSingleWord(GROUP_END);
			//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Handle";
		lpInfo->m_strPropHelp = "The handle of angle"; 
#else
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "�Ǹ־��"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("handle");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_STATMAT)
			|GetSingleWord(GROUP_DESIGN)|GetSingleWord(GROUP_START)|GetSingleWord(GROUP_END);
			//ͼ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Layer";
		lpInfo->m_strPropHelp = "The layer angle belongs to"; 
#else
		lpInfo->m_strPropName = "ͼ��";
		lpInfo->m_strPropHelp = "�Ǹ�����ͼ��"; 
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("layer");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
			//��ĺ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Cfgword";
		lpInfo->m_strPropHelp = "Cfgword";
#else
		lpInfo->m_strPropName = "��ĺ�";
		lpInfo->m_strPropHelp = "��ĺ�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("cfgword");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_STATMAT);
			//����	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Material";
		lpInfo->m_strPropHelp = "Angle's material";
#else
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "�Ǹֲ���";
#endif
		lpInfo->m_cmbItems = MakeMaterialMarkSetString();//"Q235|Q345|Q390";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("cMaterial");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_STATMAT);
		//�����ȼ�	
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "m_cQualityLevel";
		lpInfo->m_strPropHelp = "Angle's quality grade";
		lpInfo->m_cmbItems = "Default|A|B|C|D|";
#else
		lpInfo->m_strPropName = "�����ȼ�";
		lpInfo->m_strPropHelp = "�Ǹ������ȼ�";
		lpInfo->m_cmbItems = "Ĭ��|A|B|C|D|";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_cQualityLevel");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_STATMAT);
			//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Spec";
		lpInfo->m_strPropHelp = "Angle's spec(wide \"X\" thickness)";
#else
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "�ǸֵĹ��(֫��X֫��)";
#endif
		lpInfo->m_cmbItems = MakeAngleSetString();
		lpInfo->m_cmbType = CDT_EDIT;
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("specification");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_STATMAT);
			//�ֶκ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Segment Num";
		lpInfo->m_strPropHelp = "Segment number";
#else
		lpInfo->m_strPropName = "�ֶκ�";
		lpInfo->m_strPropHelp = "�ֶκ�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("iSeg");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_STATMAT);
		if(pFirstLineAngle->GetClassTypeId()==CLS_LINEANGLE)
		{	//�������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "PartNo";
			lpInfo->m_strPropHelp = "Part Number";
#else
			lpInfo->m_strPropName = "�������";
			lpInfo->m_strPropHelp = "�������";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("sPartNo");
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_STATMAT);

			if(!pFirstLineAngle->IsShadowObject())
			{	//��������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
	#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Relative Object";
				lpInfo->m_strPropHelp = "The object relate to currently object and mirror style";
	#else
				lpInfo->m_strPropName = "��������";
				lpInfo->m_strPropHelp = "�뵱ǰ����������Ĺ����Լ��ԳƷ�ʽ";
	#endif
				pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp = CLDSLineAngle::GetPropID("relativeObjs");
				if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
			}
			if(pFirstLineAngle->group_father_jg_h>0x20)
			{	//��ϸ��Ǹ־��
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Group Father Angle";
				lpInfo->m_strPropHelp = "The handle of group father angle"; 
#else
				lpInfo->m_strPropName = "��ϸ��Ǹ�";
				lpInfo->m_strPropHelp = "��ϸ��Ǹ־��"; 
#endif
				pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp = CLDSLineAngle::GetPropID("group_father_jg_h");
				if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
			}
		}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else
		{
			CLDSGroupLineAngle *pGroupLineAngle=(CLDSGroupLineAngle*)pFirstLineAngle;
			lpInfo = new CItemInfo();
			if(pGroupLineAngle->wSectStyle<=1)
				lpInfo->m_controlType = PIT_COMBO;
			else
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Group Style";
			lpInfo->m_strPropHelp = "Grout style";
#else
			lpInfo->m_strPropName = "�����ʽ";
			lpInfo->m_strPropHelp = "�����ʽ";
#endif
			lpInfo->m_cmbItems="0.�Խ����|1.T�����";
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSGroupLineAngle::GetPropID("group_style");
			if(pGroupLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSGroupLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
			if(pGroupLineAngle->wSectStyle==2)
				pPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Update Sub-Angle Oddment";
			lpInfo->m_strPropHelp = "Update the oddment of sub-angle";
			lpInfo->m_cmbItems = "Yes|No";
#else
			lpInfo->m_strPropName = "�����ӽǸ�����ͷ";
			lpInfo->m_strPropHelp = "�����ӽǸ�����ͷ";
			lpInfo->m_cmbItems = "��|��";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSGroupLineAngle::GetPropID("m_bUpdateSonAngleOddment");
			if(pGroupLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSGroupLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Same direct angle";
			lpInfo->m_strPropHelp = "Same direct angle";
			if(pGroupLineAngle->wSectStyle==0)
				lpInfo->m_cmbItems = "Diagonal-I|Diagonal-II";
			else if(pGroupLineAngle->wSectStyle==1)
				lpInfo->m_cmbItems = "T-X|T-Y";
#else
			lpInfo->m_strPropName = "����ƴװ����";
			lpInfo->m_strPropHelp = "��������ӽǸּ��ƴװ����";
			if(pGroupLineAngle->wSectStyle==0)
				lpInfo->m_cmbItems = "�Խ�I��|�Խ�II��";
			else if(pGroupLineAngle->wSectStyle==1)
				lpInfo->m_cmbItems = "T-X��|T-Y��";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSGroupLineAngle::GetPropID("ciMasterIndex");
			if(pGroupLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSGroupLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Relations of angles";
			lpInfo->m_strPropHelp = "Relations of angles";
			lpInfo->m_cmbItems = "Virtual angle|1# angle|2# angle";
#else
			lpInfo->m_strPropName = "֫���������";
			lpInfo->m_strPropHelp = "������ӽǸ�֫����ļ������ݽǸ�";
			lpInfo->m_cmbItems = "����Ǹ�|1�ŽǸ�|2�ŽǸ�";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSGroupLineAngle::GetPropID("ciMasterDirectIndex");
			if(pGroupLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSGroupLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Group Space";
			lpInfo->m_strPropHelp = "Group space";
#else
			lpInfo->m_strPropName = "��ϼ�϶";
			lpInfo->m_strPropHelp = "��ϼ�϶";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSGroupLineAngle::GetPropID("jg_space");
			if(pGroupLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSGroupLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);

			if(!pFirstLineAngle->IsShadowObject())
			{	//��������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
	#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Relative Object";
				lpInfo->m_strPropHelp = "The object relate to currently object and mirror style";
	#else
				lpInfo->m_strPropName = "��������";
				lpInfo->m_strPropHelp = "�뵱ǰ����������Ĺ����Լ��ԳƷ�ʽ";
	#endif
				pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
				pPropItem->m_idProp = CLDSLineAngle::GetPropID("relativeObjs");
				if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					lpInfo->m_strPropValue = sText;
				pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
			}
		}
#endif
			//ͳ��ϵ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Stat Material Coef";
		lpInfo->m_strPropHelp = "Stat. material coefficient";
#else
		lpInfo->m_strPropName = "ͳ��ϵ��";
		lpInfo->m_strPropHelp = "ͳ��ϵ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_uStatMatCoef");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//���Ӹ�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType =	PIT_BUTTON;	//PIT_EDIT
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Part Weld To";
		lpInfo->m_strPropHelp = "If current part needs welding to other part,then this property is the handle of current part attached welding part.If current part doesn't need welding,this value is 0X0.";
#else
		lpInfo->m_strPropName = "���Ӹ�����";
		lpInfo->m_strPropHelp = "����˹�����Ҫ���ӵ�����������,��ô�����Ծ��Ǳ������������ĺ��ӹ����ľ��;����˹�������Ҫ����,��ֵΪ0X0��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_hPartWeldParent");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(FALSE);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
			//ʵ�ʳ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Length";
		lpInfo->m_strPropHelp = "Angle's actual length";
#else
		lpInfo->m_strPropName = "ʵ�ʳ���";
		lpInfo->m_strPropHelp = "�Ǹֵ�ʵ�ʳ���";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("length");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL)|GetSingleWord(GROUP_STATMAT);
		//
		if(pFirstLineAngle->IsShadowObject())
		{
			//Ӱ����Ϣ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Shadow Info";
			lpInfo->m_strPropHelp = "Shadow information";
#else
			lpInfo->m_strPropName = "Ӱ����Ϣ";
			lpInfo->m_strPropHelp = "Ӱ����Ϣ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("shadowInfo");
			pPropItem->m_bHideChildren = FALSE;
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			pPropItem->SetReadOnly(FALSE);
			//Ӱ��ĸ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Shadow Mother Object";
			lpInfo->m_strPropHelp = "The shadow mother object's handle of currently part,when it's 0x0 means that the currently object is original object";
#else
			lpInfo->m_strPropName = "Ӱ��ĸ����";
			lpInfo->m_strPropHelp = "��ǰ������Ӧ��Ӱ��ĸ����������þ��Ϊ0x0ʱ��ʾ��ǰ����Ϊԭʼ������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("m_hMotherObj");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			//�������±��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Synchornize part label";
			lpInfo->m_strPropHelp = "Synchornize part label and segment number with mother part.";
			lpInfo->m_cmbItems = "Yes|No";
#else
			lpInfo->m_strPropName = "�������±��";
			lpInfo->m_strPropHelp = "��ǰӰ�乹������Ӱ��ĸ����ͬ�����¹�����š��κ���Ϣ��";
			lpInfo->m_cmbItems = "��|��";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("m_bSyncShadowPartNo");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(FALSE);
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			if (pFirstLineAngle->IsBlockRefShadowObject())
			{
				//������������
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Belong Block Reference";
				lpInfo->m_strPropHelp = "The block reference's handle currently part belong to.";
#else
				lpInfo->m_strPropName = "������������";
				lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("m_hBlockRef");
				if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else 
		{
				//��������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Belong Block";
			lpInfo->m_strPropHelp = "The block's handle currently part belong to.";
#else
			lpInfo->m_strPropName = "��������";
			lpInfo->m_strPropHelp = "��ǰ���������Ĳ��������";
#endif
			lpInfo->m_cmbItems=GetBlockListStr();
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_hBlock");
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
			//�Ŷ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems = "Yes|No";
		lpInfo->m_strPropName = "Foot Nail";
		lpInfo->m_strPropHelp = "Foot nail on Tube";
#else
		lpInfo->m_cmbItems = "��|��";
		lpInfo->m_strPropName = "�Ŷ��Ǹ�";
		lpInfo->m_strPropHelp = "�ֹ��ϽŶ��Ǹ�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_bFootNail");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
			//���⸨������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Virtual Object";
		lpInfo->m_strPropHelp = "Virtual object is only used to assist other part position,but not the statistic material part which structure really has,so didn't participate in the material statistics";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "���⸨������";
		lpInfo->m_strPropHelp = "���⸨������ֻ���ڸ�������������λ�������ǽṹ����ʵ���е�ͳ�Ĺ��������Բ���Ԥ����ͳ�ơ�";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_bVirtualPart");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
			//��ʾ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Hole Visible";
		lpInfo->m_strPropHelp = "Display hole or not";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "��ʾ��";
		lpInfo->m_strPropHelp = "�Ƿ���ʾ��";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_bDisplayHole");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
			//�û�ָ��������ɫ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Specify Color";
		lpInfo->m_strPropHelp = "User specifies the part color or not";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "ָ��������ɫ";
		lpInfo->m_strPropHelp = "������ɫ�Ƿ�Ϊ�û�ָ��";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_bUserSpecColor");
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		if(pFirstLineAngle->m_bUserSpecColor)
		{	//������ɫ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
			lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Color";
			lpInfo->m_strPropHelp = "Part color";
#else
			lpInfo->m_strPropName = "������ɫ";
			lpInfo->m_strPropHelp = "������ɫ";
#endif
			CPropTreeItem *pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("crMaterial");
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
		}
			//��ע
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Notes";
		lpInfo->m_strPropHelp = "Notes";
#else
		lpInfo->m_strPropName = "��ע";
		lpInfo->m_strPropHelp = "��ע";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("sNotes");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);

		//��λ��Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Setup Position Info";
		lpInfo->m_strPropHelp = "The setup position informateion of angle and its lap-joint angles";
#else
		lpInfo->m_strPropName = "��װ��λ��Ϣ";
		lpInfo->m_strPropHelp = "�Ǹּ����ϴ�ӽǸֵİ�װ��λ��Ϣ";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLineAngle::GetPropID("SetupPosInfo");
		pGroupItem->m_bHideChildren = FALSE;
		if(CLDSLineAngle::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//��������׼��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Special G Enable";
		lpInfo->m_strPropHelp = "Start special stadia";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "��������׼��";
		lpInfo->m_strPropHelp = "��������׼��";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem, lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_bEnableTeG");
		if (pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp, sText) > 0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs, pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp, pFirstLineAngle->IsShadowObject()));
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//��ǰִ��׼��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_FILEPATH;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "current g";
		lpInfo->m_strPropHelp = "current boltline g";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "X(��)ִ֫��׼��";
		lpInfo->m_strPropHelp = "�Ǹ�X֫��ǰִ�е�׼��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("wingXG");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bEnableTeG;
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//��ǰִ��׼��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_FILEPATH;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "current g";
		lpInfo->m_strPropHelp = "current boltline g";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "Y(��)ִ֫��׼��";
		lpInfo->m_strPropHelp = "�Ǹ�Y֫��ǰִ�е�׼��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("wingYG");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bEnableTeG;
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//��Ӷ�λ��׼֫
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Position Control Datum Line Type";
		lpInfo->m_strPropHelp = "Position control datum line type";
		lpInfo->m_cmbItems = "X Leg|Y Leg|None";
#else
		lpInfo->m_strPropName = "��Ӷ�λ��׼";
		lpInfo->m_strPropHelp = "��Ӷ�λ��׼";
		lpInfo->m_cmbItems = "X֫����|Y֫����|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_cPosCtrlDatumLineType");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		if(pFirstLineAngle->m_cPosCtrlDatumLineType=='X'||pFirstLineAngle->m_cPosCtrlDatumLineType=='Y')
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Position Control Style";
			lpInfo->m_strPropHelp = "Position control style";
			lpInfo->m_cmbItems = "0.Project|1.X-Y Plane Offset|2.Y-Z Plane Offset|3.X-Z Plane Offset";
#else
			lpInfo->m_strPropName = "��ӵ㶨λ��ʽ";
			lpInfo->m_strPropHelp = "��ӵ㶨λ��ʽ";
			lpInfo->m_cmbItems = "0.����ͶӰ|1.X-Yƽ��ƽ��|2.Y-Zƽ��ƽ��|3.X-Zƽ��ƽ��";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("m_cPosCtrlType");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		}
        //Ԥ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Pre-Lift";
		lpInfo->m_strPropHelp = "Angle's start coordinate before pre-lift";
#else
		lpInfo->m_strPropName = "Ԥ����";
		lpInfo->m_strPropHelp = "�Ǹ�ʼĩ��ִ�е�Ԥ����";
#endif
		lpInfo->m_cmbType=CDT_LIST;
		lpInfo->m_cmbItems=MakeArcLiftSetString();
		CPropTreeItem *pArcLiftItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pArcLiftItem->m_bHideChildren = FALSE;
		pArcLiftItem->m_idProp=CLDSLineAngle::GetPropID("m_hArcLift");
		if(pFirstLineAngle->GetPropValueStr(pArcLiftItem->m_idProp,sText)>0)
			pArcLiftItem->m_lpNodeInfo->m_strPropValue = sText;
		if(CLDSLineAngle::GetPropStatus(pArcLiftItem->m_idProp)!=-1)
			pArcLiftItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pArcLiftItem->m_idProp);
		pArcLiftItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//��֫������Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Leg Normal Info";
		lpInfo->m_strPropHelp = "Angle's legs normal information";
#else
		lpInfo->m_strPropName = "��֫������Ϣ";
		lpInfo->m_strPropHelp = "�Ǹ���֫������Ϣ";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLineAngle::GetPropID("wingNormInfo");
		pGroupItem->m_bHideChildren = FALSE;
		if(CLDSLineAngle::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
			//��֫�н�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Angle between two leg";
		lpInfo->m_strPropHelp = "Angle between two leg";
#else
		lpInfo->m_strPropName = "��֫�н�";
		lpInfo->m_strPropHelp = "�Ǹ���֫�н�";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("decWingAngle");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
		//��Ҫ�����ض�֫��ȵ�����Ͻǲ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="Internal Width|External Width";
		lpInfo->m_strPropName = "Leg Close Reserved Width Type";
		lpInfo->m_strPropHelp = "Internal Width:means the width from leg close inflexion on leg's internal wall to another leg's internal wall;External Width:means the width from leg close inflexion on leg's external wall to another leg's external wall";
#else
		lpInfo->m_cmbItems="�ڲ�|���";
		lpInfo->m_strPropName = "�ϽǱ����������";
		lpInfo->m_strPropHelp = "�ڲࣺ��ʾ֫�ڱںϽǹյ㵽��һ֫�ڱڵĿ�ȣ���ࣺ��ʾ֫��Ƥ�Ͻǹյ㵽��һ֫��Ƥ�Ŀ��";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("m_cHeJiaoReservedWidthType");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->SetReadOnly(pFirstLineAngle->GetDecWingAngle()>=90||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);

		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Leg Close Reserved Width";
		lpInfo->m_strPropHelp = "When leg close bigger,ridge line needs ensuring extra orthogonal width angle because of process";
#else
		lpInfo->m_strPropName = "�ϽǱ������";
		lpInfo->m_strPropHelp = "�ϴ�Ͻ�ʱ�����ڹ���ԭ�����߲���Ҫ��֤һ����ֱ�ǿ�ȽǸ�";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("m_fHeJiaoReservedWidth");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->SetReadOnly(pFirstLineAngle->GetDecWingAngle()>=90||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		//X/Y���������ԳƷ�ʽ
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		//lpInfo->m_cmbType=CBS_DROPDOWNLIST;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Unchanged|1.Same direction|2.Invert direction";
		lpInfo->m_strPropName = "Leg Layout Style On X/Y Mirror";
		lpInfo->m_strPropHelp = "Leg Layout Style On X/Y Mirror";
#else
		lpInfo->m_cmbItems="0.����ԭ����|1.����ͬ��|2.���ⷴ��";
		lpInfo->m_strPropName = "X/Y��ԳƷ�ʽ";
		lpInfo->m_strPropHelp = "�ڽ���X/Y��Գ�ʱ�Ǹ���֫�Ĳ��÷�ʽ";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("XorYMirLayStyle");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
		UpdateLineAngleWingNormProperty(pPropList,pGroupItem);
		//������Ϣ
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Process Info";
		lpInfo->m_strPropHelp = "The information relates to manu-Process";
#else
		lpInfo->m_strPropName = "������Ϣ";
		lpInfo->m_strPropHelp = "��ӹ������йص���Ϣ";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLineAngle::GetPropID("manuProcess");
		if(CLDSLineAngle::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//blScatterBoltLocaTolerance
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Hole location export mode";
		lpInfo->m_strPropHelp = "Hole location export mode";
		lpInfo->m_cmbItems="0.Space prefer|1.Location prefer";
#else
		lpInfo->m_strPropName = "��λ����";
		lpInfo->m_strPropHelp = "������տ���NC����ʱ��λ���Ĵ���ʽ��һ����ڷ������׼��϶�ʱѡ��'��֤��λ'";
		lpInfo->m_cmbItems="0.��֤���|1.��֤��λ";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("blScatterBoltLocaTolerance");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		pPropItem->m_bHideChildren = FALSE;
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			//ʼ���н���֫
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Cut Leg";
		lpInfo->m_strPropHelp = "Start cuts leg";
#else
		lpInfo->m_strPropName = "ʼ���н���֫";
		lpInfo->m_strPropHelp = "ʼ���н���֫";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("startCutWing");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		pPropItem->m_bHideChildren = FALSE;
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Leg";
		lpInfo->m_strPropHelp = "Cutting leg information of start X leg";
#else
		lpInfo->m_strPropName = "X֫";
		lpInfo->m_strPropHelp = "ʼ��X֫�н���֫��Ϣ";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("startCutWingX");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bUserSpecStartCutAngle;
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		//pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Leg";
		lpInfo->m_strPropHelp = "Cutting leg information of start Y leg";
#else
		lpInfo->m_strPropName = "Y֫";
		lpInfo->m_strPropHelp = "ʼ��Y֫�н���֫��Ϣ";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("startCutWingY");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bUserSpecStartCutAngle;
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		//pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle);
		/*	�����н�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��֫�н�����";
		lpInfo->m_strPropHelp = "��֫�н�����";
		lpInfo->m_cmbItems="0.X֫45���н�|1.Y֫45���н�|2.��ͨ�н�|3.X֫��֫|4.Y֫��֫|5.������֫�н�|6.����֫�н�";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("start_cut_angle_type");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);

		//��֫����
		if(pFirstLineAngle->cut_wing[0]==1||sText[0]=='3'
			||pFirstLineAngle->cut_wing[0]==2||sText[0]=='4')
		{	
			//��һ֫��ȡ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "��֫���";
			if(pFirstLineAngle->cut_wing[0]==1)
				lpInfo->m_strPropHelp = "X֫��֫,��һ֫��ȡ���";
			else 
				lpInfo->m_strPropHelp = "Y֫��֫,��һ֫��ȡ���";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[0][1]");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//�Ǹ�������ȡ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "������֫����";
			if(pFirstLineAngle->cut_wing[0]==1)
				lpInfo->m_strPropHelp = "X֫��֫,�Ǹ�������ȡ����";
			else 
				lpInfo->m_strPropHelp = "Y֫��֫,�Ǹ�������ȡ����";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[0][0]");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//֫������ȡ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "֫������ȡ����";
			if(pFirstLineAngle->cut_wing[0]==1)
				lpInfo->m_strPropHelp = "X֫��֫,֫������ȡ����";
			else 
				lpInfo->m_strPropHelp = "Y֫��֫,֫������ȡ����";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[0][2]");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		}
		if(pFirstLineAngle->cut_angle[0][0]>0||sText[0]=='2')
		{	//֫���нǿ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "X֫�нǿ��";
			lpInfo->m_strPropHelp = "X֫֫���нǿ��";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[0][1]");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//�����нǳ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "X֫�нǳ���";
			lpInfo->m_strPropHelp = "X֫�����нǳ���";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[0][0]");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		}
		if(pFirstLineAngle->cut_angle[1][0]>0||sText[0]=='2')
		{	//Y֫���нǿ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Y֫�нǿ��";
			lpInfo->m_strPropHelp = "Y֫֫���нǿ��";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[1][1]");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//Y�����нǳ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Y֫�нǳ���";
			lpInfo->m_strPropHelp = "Y֫�����нǳ���";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[1][0]");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecStartCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		}
		*/
			//�ն��н���֫
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Cut Leg";
		lpInfo->m_strPropHelp = "End cuts leg";
#else
		lpInfo->m_strPropName = "�ն��н���֫";
		lpInfo->m_strPropHelp = "�ն��н���֫";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("endCutWing");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		pPropItem->m_bHideChildren = FALSE;
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Leg";
		lpInfo->m_strPropHelp = "Cutting leg information of end X leg";
#else
		lpInfo->m_strPropName = "X֫";
		lpInfo->m_strPropHelp = "�ն�X֫�н���֫��Ϣ";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("endCutWingX");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bUserSpecEndCutAngle;
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		//pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle);
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Leg";
		lpInfo->m_strPropHelp = "Cutting leg information of end Y leg";
#else
		lpInfo->m_strPropName = "Y֫";
		lpInfo->m_strPropHelp = "�ն�Y֫�н���֫��Ϣ";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("endCutWingY");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bUserSpecEndCutAngle;
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		//pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle);
		/*	�����н�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "��֫�н�����";
		lpInfo->m_strPropHelp = "��֫�н�����";
		lpInfo->m_cmbItems="0.X֫45���н�|1.Y֫45���н�|2.��ͨ�н�|3.X֫��֫|4.Y֫��֫|5.������֫�н�|6.����֫�н�";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("end_cut_angle_type");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		if(pFirstLineAngle->cut_wing[1]==1||sText[0]=='3'
			||pFirstLineAngle->cut_wing[1]==2||sText[0]=='4')
		{	
			//��һ֫��ȡ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "��֫���";
			if(pFirstLineAngle->cut_wing[1]==1)
				lpInfo->m_strPropHelp = "X֫��֫,��һ֫��ȡ���";
			else 
				lpInfo->m_strPropHelp = "Y֫��֫,��һ֫��ȡ���";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[1][1]");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//�Ǹ�������ȡ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "������֫����";
			if(pFirstLineAngle->cut_wing[1]==1)
				lpInfo->m_strPropHelp = "X֫��֫,�Ǹ�������ȡ����";
			else 
				lpInfo->m_strPropHelp = "Y֫��֫,�Ǹ�������ȡ����";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[1][0]");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//֫������ȡ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "֫������ȡ����";
			if(pFirstLineAngle->cut_wing[1]==1)
				lpInfo->m_strPropHelp = "X֫��֫,֫������ȡ����";
			else 
				lpInfo->m_strPropHelp = "Y֫��֫,֫������ȡ����";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_wing_para[1][2]");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		}
		if(pFirstLineAngle->cut_angle[2][0]>0||sText[0]=='2')
		{	//X֫���нǿ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "X֫�нǿ��";
			lpInfo->m_strPropHelp = "X֫֫���нǿ��";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[2][1]");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//X�����нǳ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "X֫�нǳ���";
			lpInfo->m_strPropHelp = "X֫�����нǳ���";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[2][0]");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		}
		if(pFirstLineAngle->cut_angle[3][0]>0||sText[0]=='2')
		{	//Y֫���нǿ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Y֫�нǿ��";
			lpInfo->m_strPropHelp = "Y֫֫���нǿ��";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[3][1]");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//Y�����нǳ���
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Y֫�нǳ���";
			lpInfo->m_strPropHelp = "Y֫�����нǳ���";
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("cut_angle[3][0]");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->SetReadOnly(!pFirstLineAngle->m_bUserSpecEndCutAngle||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		}
		*/
		//�Ǹ�֫��Բ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Fillet Arc";
		lpInfo->m_strPropHelp = "Start Fillet Arc";
#else
		lpInfo->m_strPropName = "ʼ�˵�Բ��";
		lpInfo->m_strPropHelp = "ʼ�˵�Բ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("FilletArcS");
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
		//ʼ��X֫
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Leg R";
		lpInfo->m_strPropHelp = "FilletArc Ridus of start X leg";
#else
		lpInfo->m_strPropName = "X֫�뾶R";
		lpInfo->m_strPropHelp = "ʼ��X֫��Բ���뾶";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("FilletROfSX");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bUserSpecStartCutAngle;
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		//ʼ��Y֫
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Leg R";
		lpInfo->m_strPropHelp = "FilletArc Ridus of start Y leg";
#else
		lpInfo->m_strPropName = "Y֫�뾶R";
		lpInfo->m_strPropHelp = "ʼ��Y֫��Բ���뾶";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("FilletROfSY");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bUserSpecStartCutAngle;
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Fillet Arc";
		lpInfo->m_strPropHelp = "End Fillet Arc";
#else
		lpInfo->m_strPropName = "�ն˵�Բ��";
		lpInfo->m_strPropHelp = "�ն˵�Բ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("FilletArcE");
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
		//�ն�X֫
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "X Leg R";
		lpInfo->m_strPropHelp = "FilletArc Ridus of end X leg";
#else
		lpInfo->m_strPropName = "X֫�뾶R";
		lpInfo->m_strPropHelp = "�ն�X֫��Բ���뾶";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("FilletROfEX");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bUserSpecStartCutAngle;
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		//�ն�Y֫
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Y Leg R";
		lpInfo->m_strPropHelp = "FilletArc Ridus of end Y leg";
#else
		lpInfo->m_strPropName = "Y֫�뾶R";
		lpInfo->m_strPropHelp = "�ն�Y֫��Բ���뾶";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("FilletROfEY");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bUserSpecStartCutAngle;
		pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//�н���Բ����ֵϵ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_cmbType = CDT_EDIT;
		lpInfo->m_cmbItems = "0|R*1/3|R*2/3|R";
		#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Cut Angle Coef R";
		lpInfo->m_strPropHelp = "When ray angle into the certain region of current angle's inner arc,needs cutting,the value is allowed to express the cutting leg space of ray angle's end as R*1/3+2,when it's 0 means follow the system default value.";
		#else
		lpInfo->m_strPropName = "�ڲ��нǼ�϶";
		lpInfo->m_strPropHelp = "���߽Ǹֽ��뵱ǰ�Ǹ���Բ��һ������֮��ʱ,������и��, ��ֵ������R*1/3+2��ʽ������߽Ǹֶ�ͷ�нǼ�϶ֵ��Ϊ0ʱ��ʾ���ϵͳĬ��ϵ��ֵ";
		#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_fCutAngleCoefR");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//����֫
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_POPMENU;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Open Or Close Leg";
		lpInfo->m_strPropHelp = "The base leg for angle's opening or closing";
		lpInfo->m_cmbItems = "Default|-|X Leg|Y Leg|None";
#else
		lpInfo->m_strPropName = "����֫";
		lpInfo->m_strPropHelp = "�Ǹֿ��Ͻǻ�׼֫";
		lpInfo->m_cmbItems = "�Զ�|-|X֫|Y֫|������";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("kaihe_base_wing_x0_y1");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = (!CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp))
													||(pFirstLineAngle->kaihe_base_wing_x0_y1==2);
		if(pFirstLineAngle->kaihe_base_wing_x0_y1==2)
			pPropItem->m_lpNodeInfo->SetCheck(1);
		else
			pPropItem->m_lpNodeInfo->SetCheck(-1);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Cut Root";
		lpInfo->m_strPropHelp = "Cut root or not";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "���";
		lpInfo->m_strPropHelp = "�Ƿ����";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_bCutRoot");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Cut Back";
		lpInfo->m_strPropHelp = "Cut back or not";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "����";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_bCutBer");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Bend";
		lpInfo->m_strPropHelp = "Bend";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "����";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_bBendPart");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//���Ӽ�
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Weld";
		lpInfo->m_strPropHelp = "Weld part";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "���Ӽ�";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_bWeldPart");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp,pFirstLineAngle->IsShadowObject()));
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		
		//ѹ�⴦��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Push Flat";
		lpInfo->m_strPropHelp = "Start push flat";
		lpInfo->m_cmbItems = "None|X Leg|Y Leg";
#else
		lpInfo->m_strPropName = "ʼ��ѹ��";
		lpInfo->m_strPropHelp = "ʼ��ѹ��";
		lpInfo->m_cmbItems = "��|X֫|Y֫";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("start_push_flat_x1_y2");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		if(pFirstLineAngle->start_push_flat_x1_y2==1 || pFirstLineAngle->start_push_flat_x1_y2==2)
		{
			//ѹ�ⳤ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Push Flat Length";
			lpInfo->m_strPropHelp = "Push flat length";
#else
			lpInfo->m_strPropName = "ѹ�ⳤ��";
			lpInfo->m_strPropHelp = "ѹ�ⳤ��";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("start_push_flat_length");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		}
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Push Flat";
		lpInfo->m_strPropHelp = "End push flat";
		lpInfo->m_cmbItems = "None|X Leg|Y Leg";
#else
		lpInfo->m_strPropName = "�ն�ѹ��";
		lpInfo->m_strPropHelp = "�ն�ѹ��";
		lpInfo->m_cmbItems = "��|X֫|Y֫";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("end_push_flat_x1_y2");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		if(pFirstLineAngle->end_push_flat_x1_y2==1 || pFirstLineAngle->end_push_flat_x1_y2==2)
		{
			//ѹ�ⳤ��
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Push Flat Length";
			lpInfo->m_strPropHelp = "Push flat length";
#else
			lpInfo->m_strPropName = "ѹ�ⳤ��";
			lpInfo->m_strPropHelp = "ѹ�ⳤ��";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("end_push_flat_length");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		}
		
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Middle Push Flat";
		lpInfo->m_strPropHelp = "Middle push flat";
		lpInfo->m_cmbItems = "None|X Leg|Y Leg";
#else
		lpInfo->m_strPropName = "�м�ѹ��";
		lpInfo->m_strPropHelp = "�м�ѹ��";
		lpInfo->m_cmbItems = "��|X֫|Y֫";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("mid_push_flat_x1_y2");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		if(pFirstLineAngle->mid_push_flat_x1_y2==1 || pFirstLineAngle->mid_push_flat_x1_y2==2)
		{
			//ʼ��ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Start Offset";
			lpInfo->m_strPropHelp = "Start Offset";
#else
			lpInfo->m_strPropName = "ʼ��ѹ������";
			lpInfo->m_strPropHelp = "ʼ��ѹ������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("mid_push_flat_start_offset");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//�ն�ƫ����
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "End Offset";
			lpInfo->m_strPropHelp = "End Offset";
#else
			lpInfo->m_strPropName = "�ն�ƫ����";
			lpInfo->m_strPropHelp = "�ն�ƫ����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("mid_push_flat_end_offset");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
			//ѹ��λ��ƫ��ʼ�˾��� hxr 2012-10-09
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Push Flat Node";
			lpInfo->m_strPropHelp = "The node handle of pushing flat position";
#else
			lpInfo->m_strPropName = "ѹ��λ�ýڵ�";
			lpInfo->m_strPropHelp = "ѹ��λ�ýڵ�";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("mid_push_flat_node_handle");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		}
		//����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_POPMENU;
		lpInfo->m_cmbItems="���|���";
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Open Slot";
		lpInfo->m_strPropHelp = "Open Slot";
#else
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "����";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("OpenSlotGroup");
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
		pPropItem->m_bHideChildren = FALSE;
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
		UpdateLineAngle_OpenSlot_TreeItem(pPropList,pPropItem);
#if defined(__TSA_)||defined(__LDS_)
		if(!theApp.m_bDrawingWorkEnvOnly)
		{
			lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Rod Check Info";
			lpInfo->m_strPropHelp = "Rod check information";
#else
			lpInfo->m_strPropName = "�˼�������Ϣ";
			lpInfo->m_strPropHelp = "�˼�������Ϣ";
#endif
			pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
			pGroupItem->m_idProp = CLDSLineAngle::GetPropID("designInfo");
			if(theApp.m_bAnalysisWorkEnv)
				pGroupItem->m_bHideChildren = FALSE;
			else
				pGroupItem->m_bHideChildren = TRUE;
			if(CLDSLineAngle::GetPropStatus(pGroupItem->m_idProp)!=-1)
				pGroupItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pGroupItem->m_idProp);
			pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_DESIGN);
		}
#endif
		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Info";
		lpInfo->m_strPropHelp = "Angle's start information";
#else
		lpInfo->m_strPropName = "ʼ����Ϣ";
		lpInfo->m_strPropHelp = "�Ǹ�ʼ����Ϣ";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLineAngle::GetPropID("startInfo");
		pGroupItem->m_bHideChildren = FALSE;
		if(CLDSLineAngle::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_START)|GetSingleWord(GROUP_DESIGN);
			//�����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Start Handle";
		lpInfo->m_strPropHelp = "Anlge's start handle";
#else
		lpInfo->m_strPropName = "�����";
		lpInfo->m_strPropHelp = "�Ǹ������";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("hStart");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
								pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_START);

		lpInfo = new CItemInfo();
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Info";
		lpInfo->m_strPropHelp = "Angle's end position information";
#else
		lpInfo->m_strPropName = "�ն���Ϣ";
		lpInfo->m_strPropHelp = "�Ǹ��ն�λ����Ϣ";
#endif
		pGroupItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pGroupItem->m_idProp = CLDSLineAngle::GetPropID("endInfo");
		pGroupItem->m_bHideChildren = FALSE;
		if(CLDSLineAngle::GetPropStatus(pGroupItem->m_idProp)!=-1)
			pGroupItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pGroupItem->m_idProp);
		pGroupItem->m_dwPropGroup = GetSingleWord(GROUP_END)|GetSingleWord(GROUP_DESIGN);
			//�յ���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "End Handle";
		lpInfo->m_strPropHelp = "Angle's end handle";
#else
		lpInfo->m_strPropName = "�յ���";
		lpInfo->m_strPropHelp = "�Ǹ��յ���";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("hEnd");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
								pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_END);
		//�Ǹ�λ����Ϣ
		DisplayLineAnglePositionSubSectionProperty(GROUP_START,GROUP_END);
		//�Ǹ�֫ƽ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Leg Plane";
		lpInfo->m_strPropHelp = "Datum point of leg plane is angle's start position.";
#else
		lpInfo->m_strPropName = "�Ǹ�֫ƽ��";
		lpInfo->m_strPropHelp = "֫ƽ���׼��Ϊ�Ǹ�ʼ��λ�á�";
#endif
		pPropItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("start_wing_plane");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_START);
		//�Ǹ�֫ƽ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Leg Plane";
		lpInfo->m_strPropHelp = "Datum point of leg plane is angle's end position.";
#else
		lpInfo->m_strPropName = "�Ǹ�֫ƽ��";
		lpInfo->m_strPropHelp = "֫ƽ���׼��Ϊ�Ǹ��ն�λ�á�";
#endif
		pPropItem = pPropList->InsertItem(pRoot,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("end_wing_plane");
		pPropItem->m_bHideChildren = TRUE;
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(GROUP_END);
		//�Ǹ���ѧ���������Ϣ
		DisplayLineAngleDesignSubSectionProperty(GROUP_DESIGN);
	}
	else
	{
		CPropTreeItem *pFindItem, *pParentItem;
		SetPartMutiObjsProp(pFirstLineAngle,"handle");
		if(pFirstLineAngle->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			SetPartMutiObjsProp(pFirstLineAngle,"group_style");
			SetPartMutiObjsProp(pFirstLineAngle,"jg_space");
			SetPartMutiObjsProp(pFirstLineAngle,"m_bUpdateSonAngleOddment");
			SetPartMutiObjsProp(pFirstLineAngle,"ciMasterIndex");
			SetPartMutiObjsProp(pFirstLineAngle,"ciMasterDirectIndex");
		}
		else if(pFirstLineAngle->group_father_jg_h>0x20)
		{
			SetPartMutiObjsProp(pFirstLineAngle,"group_father_jg_h");
			if(!SetPartMutiObjsProp(pFirstLineAngle,"group_father_jg_h"))
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("basicInfo"),&pParentItem);
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Group Father Angle";
				lpInfo->m_strPropHelp = "The handle of group father angle";
#else
				lpInfo->m_strPropName = "��ϸ��Ǹ�";
				lpInfo->m_strPropHelp = "��ϸ��Ǹ־��";
#endif
				pPropItem = pPropList->InsertItem(pFindItem,lpInfo, 8);
				pPropItem->m_idProp = CLDSLineAngle::GetPropID("group_father_jg_h");
				if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->SetReadOnly();
				pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
			}
		}
		else 
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("group_father_jg_h"));
		
		SetPartMutiObjsProp(pFirstLineAngle,"layer");
		SetPartMutiObjsProp(pFirstLineAngle,"cMaterial");
		SetPartMutiObjsProp(pFirstLineAngle,"m_cQualityLevel");
		SetPartMutiObjsProp(pFirstLineAngle,"specification");
		SetPartMutiObjsProp(pFirstLineAngle,"iSeg");
		SetPartMutiObjsProp(pFirstLineAngle,"sPartNo");
		SetPartMutiObjsProp(pFirstLineAngle,"m_uStatMatCoef");
		SetPartMutiObjsProp(pFirstLineAngle,"m_uStatMatNo");
		SetPartMutiObjsProp(pFirstLineAngle,"m_fWeightRaiseCoef");
		SetPartMutiObjsProp(pFirstLineAngle,"m_hPartWeldParent");
		SetPartMutiObjsProp(pFirstLineAngle,"cfgword");
		pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_hBlock"),&pParentItem);
		if (pFindItem)
		{
			if (pFirstLineAngle->IsShadowObject())
			{	//����ͨ�����л���Ӱ�乹��
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_hBlock"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("relativeObjs"));
				//Ӱ����Ϣ
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Shadow Info";
				lpInfo->m_strPropHelp = "Shadow information";
#else
				lpInfo->m_strPropName = "Ӱ����Ϣ";
				lpInfo->m_strPropHelp = "Ӱ����Ϣ";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,10,true);
				pPropItem->m_idProp = CLDSLineAngle::GetPropID("shadowInfo");
				if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
					pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
				pPropItem->SetReadOnly(FALSE);
				//Ӱ��ĸ����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
				lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Shadow Mother Object";
				lpInfo->m_strPropHelp = "The shadow mother object's handle of currently part,when it's 0x0 means that the currently object is original object";
#else
				lpInfo->m_strPropName = "Ӱ��ĸ����";
				lpInfo->m_strPropHelp = "��ǰ������Ӧ��Ӱ��ĸ����������þ��Ϊ0x0ʱ��ʾ��ǰ����Ϊԭʼ������";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("m_hMotherObj");
				if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				//Ӱ��ĸ����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Synchornize part label";
				lpInfo->m_strPropHelp = "Synchornize part label and segment number with mother part.";
				lpInfo->m_cmbItems = "Yes|No";
#else
				lpInfo->m_strPropName = "�������±��";
				lpInfo->m_strPropHelp = "��ǰӰ�乹������Ӱ��ĸ����ͬ�����¹�����š��κ���Ϣ��";
				lpInfo->m_cmbItems = "��|��";
#endif
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
				pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("m_bSyncShadowPartNo");
				if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				if (pFirstLineAngle->IsBlockRefShadowObject())
				{
					//������������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Belong Block Reference";
					lpInfo->m_strPropHelp = "The block reference's handle currently part belong to.";
#else
					lpInfo->m_strPropName = "������������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
					pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
					pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("m_hBlockRef");
					if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstLineAngle,"m_hBlock");
				SetPartMutiObjsProp(pFirstLineAngle,"m_hMotherObj");
				SetPartMutiObjsProp(pFirstLineAngle,"m_bSyncShadowPartNo");
				SetPartMutiObjsProp(pFirstLineAngle,"relativeObjs");
			}
		}
		pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("shadowInfo"),&pParentItem);
		if (pFindItem)
		{
			if (!pFirstLineAngle->IsShadowObject())
			{	//��Ӱ�乹���л�����ͨ����
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("shadowInfo"));
				pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_hMotherObj"));
				pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_hBlockRef"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_hBlockRef"));
				pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_hBlock"),NULL);
				if (pFindItem)
					pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_hBlock"));
				if(pFindItem!=NULL)
					SetPartMutiObjsProp(pFirstLineAngle,"m_hBlock");
				else
				{	//��������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Vest In Block Handle";
					lpInfo->m_strPropHelp = "The handle of the vest in block��";
#else 
					lpInfo->m_strPropName = "��������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ��������";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_hBlock");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
					pPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
				}
				pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("relativeObjs"),NULL);
				if(pFindItem!=NULL)
					SetPartMutiObjsProp(pFirstLineAngle,"relativeObjs");
				else
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Relative Object";
					lpInfo->m_strPropHelp = "The object relate to currently object and mirror style";
#else
					lpInfo->m_strPropName = "��������";
					lpInfo->m_strPropHelp = "�뵱ǰ����������Ĺ����Լ��ԳƷ�ʽ";
#endif
					pPropItem = pPropList->InsertItem(pParentItem,lpInfo,8,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("relativeObjs");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				}
			}
			else
			{
				SetPartMutiObjsProp(pFirstLineAngle,"m_hMotherObj");
				pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_hBlockRef"),&pParentItem);
				if (pFindItem&&pFirstLineAngle->IsBlockRefShadowObject())
				{
					SetPartMutiObjsProp(pFirstLineAngle,"m_hBlockRef");
					SetPartMutiObjsProp(pFirstLineAngle,"m_hBlock");
				}
				else if (pFindItem&&!pFirstLineAngle->IsBlockRefShadowObject())
				{
					pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_hBlockRef"));
					pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_hBlock"));
				}
				else if(pFindItem==NULL&&pFirstLineAngle->IsBlockRefShadowObject())
				{
					pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_hMotherObj"),&pParentItem);
					//������������
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_BUTTON;
					lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Belong Block Reference";
					lpInfo->m_strPropHelp = "The block reference's handle currently part belong to.";
#else
					lpInfo->m_strPropName = "������������";
					lpInfo->m_strPropHelp = "��ǰ���������Ĳ������þ����";
#endif
					pSonPropItem = pPropList->InsertItem(pParentItem,lpInfo, -1);
					pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("m_hBlockRef");
					if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
						pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
					pSonPropItem->SetReadOnly(pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
				}
			}
		}
		SetPartMutiObjsProp(pFirstLineAngle,"m_bDisplayHole");
		SetPartMutiObjsProp(pFirstLineAngle,"m_bVirtualPart");
		SetPartMutiObjsProp(pFirstLineAngle,"m_bFootNail");
		SetPartMutiObjsProp(pFirstLineAngle,"m_bCutRoot");
		SetPartMutiObjsProp(pFirstLineAngle,"m_bCutBer");
		SetPartMutiObjsProp(pFirstLineAngle,"m_bWeldPart");
		SetPartMutiObjsProp(pFirstLineAngle,"m_bUserSpecColor");
		if(pFirstLineAngle->m_bUserSpecColor)
		{	
			if(!SetPartMutiObjsProp(pFirstLineAngle,"crMaterial"))
			{
				CPropTreeItem *pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_bUserSpecColor"),NULL);
				if(pFindItem)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;	//PIT_BUTTON;
					lpInfo->m_cmbType = CDT_COLOR;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Color";
					lpInfo->m_strPropHelp = "Part color";
#else
					lpInfo->m_strPropName = "������ɫ";
					lpInfo->m_strPropHelp = "������ɫ";
#endif
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("crMaterial");
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_dwPropGroup=GetSingleWord(GROUP_GENERAL);
				}
			}
		}
		else 
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("crMaterial"));
		SetPartMutiObjsProp(pFirstLineAngle,"length");
		SetPartMutiObjsProp(pFirstLineAngle,"sNotes");
		SetPartMutiObjsProp(pFirstLineAngle,"wingXG");
		SetPartMutiObjsProp(pFirstLineAngle,"wingYG");
		SetPartMutiObjsProp(pFirstLineAngle,"hStart");
		SetPartMutiObjsProp(pFirstLineAngle,"startPos.x");
		SetPartMutiObjsProp(pFirstLineAngle,"startPos.y");
		SetPartMutiObjsProp(pFirstLineAngle,"startPos.z");
		SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.jgber_cal_style");
		pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desStartPos.jgber_cal_style"),&pParentItem);
		if(pFindItem)
			UpdateLineAngle_DesPos_JgCalStyle_TreeItem(pPropList,pFindItem,TRUE);
		SetPartMutiObjsProp(pFirstLineAngle,"hEnd");
		SetPartMutiObjsProp(pFirstLineAngle,"endPos.x");
		SetPartMutiObjsProp(pFirstLineAngle,"endPos.y");
		SetPartMutiObjsProp(pFirstLineAngle,"endPos.z");
		SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.jgber_cal_style");
		pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desEndPos.jgber_cal_style"),&pParentItem);
		if(pFindItem)
			UpdateLineAngle_DesPos_JgCalStyle_TreeItem(pPropList,pFindItem,FALSE);
		SetPartMutiObjsProp(pFirstLineAngle,"oddStart");
		SetPartMutiObjsProp(pFirstLineAngle,"oddStart.m_iOddCalStyle");
		SetPartMutiObjsProp(pFirstLineAngle,"oddStart.m_fCollideDist");
		SetPartMutiObjsProp(pFirstLineAngle,"oddStart.m_hRefPart1");
		SetPartMutiObjsProp(pFirstLineAngle,"oddStart.m_hRefPart2");
		//��˨�������ͷʱ����һ����ײ���ո˼��������нǻ�׼�Ǹֵ����ã��ʲ�����Ϊֻ�� wjh-2015.5.6
		//pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("oddStart.m_fCollideDist"),pFirstLineAngle->desStartOdd.m_iOddCalStyle==1);
		//pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("oddStart.m_hRefPart1"),pFirstLineAngle->desStartOdd.m_iOddCalStyle==1);
		//pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("oddStart.m_hRefPart2"),pFirstLineAngle->desStartOdd.m_iOddCalStyle==1);
		SetPartMutiObjsProp(pFirstLineAngle,"oddStartExtra");
		SetPartMutiObjsProp(pFirstLineAngle,"StartOddCanAcrossMiddleSect");
		SetPartMutiObjsProp(pFirstLineAngle,"oddEnd");
		SetPartMutiObjsProp(pFirstLineAngle,"oddEnd.m_iOddCalStyle");
		SetPartMutiObjsProp(pFirstLineAngle,"oddEnd.m_fCollideDist");
		SetPartMutiObjsProp(pFirstLineAngle,"oddEnd.m_hRefPart1");
		SetPartMutiObjsProp(pFirstLineAngle,"oddEnd.m_hRefPart2");
		//pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("oddEnd.m_fCollideDist"),pFirstLineAngle->desEndOdd.m_iOddCalStyle==1);
		//pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("oddEnd.m_hRefPart1"),pFirstLineAngle->desEndOdd.m_iOddCalStyle==1);
		//pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("oddEnd.m_hRefPart2"),pFirstLineAngle->desEndOdd.m_iOddCalStyle==1);
		SetPartMutiObjsProp(pFirstLineAngle,"oddEndExtra");
		SetPartMutiObjsProp(pFirstLineAngle,"EndOddCanAcrossMiddleSect");
		SetPartMutiObjsProp(pFirstLineAngle,"blScatterBoltLocaTolerance");
		SetPartMutiObjsProp(pFirstLineAngle,"connectStart.d");
		SetPartMutiObjsProp(pFirstLineAngle,"connectEnd.d");
		SetPartMutiObjsProp(pFirstLineAngle,"connectStart.m_iConnectWing");
		SetPartMutiObjsProp(pFirstLineAngle,"connectEnd.m_iConnectWing");
		SetPartMutiObjsProp(pFirstLineAngle,"connectStart.N");
		SetPartMutiObjsProp(pFirstLineAngle,"connectEnd.N");
		SetPartMutiObjsProp(pFirstLineAngle,"connectStart.summBoltN");
		SetPartMutiObjsProp(pFirstLineAngle,"connectEnd.summBoltN");
		SetPartMutiObjsProp(pFirstLineAngle,"connectStart.rows");
		SetPartMutiObjsProp(pFirstLineAngle,"connectEnd.rows");
		SetPartMutiObjsProp(pFirstLineAngle,"decWingAngle");
		SetPartMutiObjsProp(pFirstLineAngle,"XorYMirLayStyle");

		pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("wingNormInfo"),&pParentItem);
		if(pFindItem)
			UpdateLineAngleWingNormProperty(pPropList,pFindItem);
#ifndef __TSA_
		SetPartMutiObjsProp(pFirstLineAngle,"m_fHeJiaoReservedWidth");
		SetPartMutiObjsProp(pFirstLineAngle,"m_cHeJiaoReservedWidthType");
		pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("m_fHeJiaoReservedWidth"),pFirstLineAngle->GetDecWingAngle()>=90||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("m_fHeJiaoReservedWidth")));
		pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("m_cHeJiaoReservedWidthType"),pFirstLineAngle->GetDecWingAngle()>=90||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("m_cHeJiaoReservedWidthType")));
#endif
		SetPartMutiObjsProp(pFirstLineAngle,"kaihe_base_wing_x0_y1");
		if ((pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("kaihe_base_wing_x0_y1"), NULL)) != NULL)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp |= (pFirstLineAngle->kaihe_base_wing_x0_y1==2);
			if (pFirstLineAngle->kaihe_base_wing_x0_y1 == 2)
				pFindItem->m_lpNodeInfo->SetCheck(1);
			else
				pFindItem->m_lpNodeInfo->SetCheck(-1);
		}
		SetPartMutiObjsProp(pFirstLineAngle,"m_bEnableTeG");
		SetPartMutiObjsProp(pFirstLineAngle,"wingXG");
		SetPartMutiObjsProp(pFirstLineAngle,"wingYG");
		if((pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("wingXG"),&pParentItem))!=NULL)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pFindItem->m_idProp);
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bEnableTeG;
		}
		if((pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("wingYG"),&pParentItem))!=NULL)
		{
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pFindItem->m_idProp);
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp |= !pFirstLineAngle->m_bEnableTeG;
		}
		SetPartMutiObjsProp(pFirstLineAngle,"m_cPosCtrlDatumLineType");
		if(pFirstLineAngle->m_cPosCtrlDatumLineType=='X'||pFirstLineAngle->m_cPosCtrlDatumLineType=='Y')
		{
			if(!SetPartMutiObjsProp(pFirstLineAngle,"m_cPosCtrlType"))
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("m_cPosCtrlDatumLineType"),&pParentItem);
				if(pFindItem)
				{
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Position Control Style";
					lpInfo->m_strPropHelp = "Position control style";
					lpInfo->m_cmbItems = "0.Project|1.X-Y Plane|2.Y-Z Plane|3.X-Z Plane";
#else
					lpInfo->m_strPropName = "��ӵ㶨λ��ʽ";
					lpInfo->m_strPropHelp = "��ӵ㶨λ��ʽ";
					lpInfo->m_cmbItems = "0.����ͶӰ|1.X-Yƽ��ƽ��|2.Y-Zƽ��ƽ��|3.X-Zƽ��ƽ��";
#endif
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1,true);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_cPosCtrlType");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = GetSingleWord(GROUP_GENERAL);
				}
			}
		}
		else
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("m_cPosCtrlType"));
#if defined(__TSA_)||defined(__LDS_)
		SetPartMutiObjsProp(pFirstLineAngle,"m_uStatMatNo");
		SetPartMutiObjsProp(pFirstLineAngle,"m_iElemType");
		SetPartMutiObjsProp(pFirstLineAngle,"m_fWeightRaiseCoef");
		//SetPartMutiObjsProp(pFirstLineAngle,"m_bNeedSelMat");
		SetPartMutiObjsProp(pFirstLineAngle,"m_bNeedSelSize");
		SetPartMutiObjsProp(pFirstLineAngle,"sel_mat_method");
		//SetPartMutiObjsProp(pFirstLineAngle,"bear_load_coef");
		//���¸���������ʾ��Ŀ��Ϣ
		if(theApp.m_bAnalysisWorkEnv)
			UpdateAuxLineAnglePropItem(pFirstLineAngle,pPropList,this,GROUP_DESIGN);
		pPropItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("hole_out"),&pParentItem);
		if(pPropItem)
			pPropItem->SetReadOnly(pFirstLineAngle->m_bAutoCalHoleOutCoef||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		SetPartMutiObjsProp(pFirstLineAngle,"hole_out");
		SetPartMutiObjsProp(pFirstLineAngle,"m_bAutoCalHoleOutCoef");
		SetPartMutiObjsProp(pFirstLineAngle,"CalLenCoef.iTypeNo");
		//���³�ϸ�����������ʾ��Ŀ��Ϣ
		if(theApp.m_bAnalysisWorkEnv)	//!theApp.m_bDrawingWorkEnvOnly
			UpdateLineAngleCalLenCoefPropItem(pFirstLineAngle,pPropList,this,GROUP_DESIGN);
#endif
		/*SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.datum_pos_style");
		SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.datum_pos_style");
		if(pFirstLineAngle->des_start_pos.jgber_cal_style==1||pFirstLineAngle->des_start_pos.jgber_cal_style==2)
		{
		SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.start_jg_h");
		SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.iStartJgBerStyle");
		SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.fStartJgLengOffsetX");
		SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.fStartJgLengOffsetY");
		SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.end_jg_h");
		SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.iEndJgBerStyle");
		SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.fEndJgLengOffsetX");
		SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.fEndJgLengOffsetY");
		}
		if(pFirstLineAngle->des_end_pos.jgber_cal_style==1||pFirstLineAngle->des_end_pos.jgber_cal_style==2)
		{
		SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.start_jg_h");
		SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.iStartJgBerStyle");
		SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.fStartJgLengOffsetX");
		SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.fStartJgLengOffsetY");
		SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.end_jg_h");
		SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.iEndJgBerStyle");
		SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.fEndJgLengOffsetX");
		SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.fEndJgLengOffsetY");
		}
		if(pFirstLineAngle->desStartPos.datum_to_ber_style==1)
		{
			SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.bByOtherEndOffset2");
			SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.face_offset_norm2");
			SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.face_offset_norm2.x");
			SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.face_offset_norm2.y");
			SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.face_offset_norm2.z");
		}
		SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.datum_to_ber_style");
		SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.wing_x_offset.gStyle");
		SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.wing_x_offset.offsetDist");
		SetPropItemReadOnly(pFirstLineAngle,"desStartPos.wing_x_offset.offsetDist",pFirstLineAngle->desStartPos.wing_x_offset.gStyle!=4||
		pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("desStartPos.wing_x_offset.offsetDist")));
		
		SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.wing_y_offset.gStyle");
		SetPartMutiObjsProp(pFirstLineAngle,"desStartPos.wing_y_offset.offsetDist");
		SetPropItemReadOnly(pFirstLineAngle,"desStartPos.wing_y_offset.offsetDist",pFirstLineAngle->desStartPos.wing_y_offset.gStyle!=4||
		pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("desStartPos.wing_y_offset.offsetDist")));
		
		/*SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.datum_to_ber_style");
		if(pFirstLineAngle->desEndPos.datum_to_ber_style==1)
		{
			SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.bByOtherEndOffset2");
			SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.face_offset_norm2");
			SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.face_offset_norm2.x");
			SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.face_offset_norm2.y");
			SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.face_offset_norm2.z");
		}
		SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.wing_x_offset.gStyle");
		SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.wing_x_offset.offsetDist");
		SetPropItemReadOnly(pFirstLineAngle,"desEndPos.wing_x_offset.offsetDist",pFirstLineAngle->desEndPos.wing_x_offset.gStyle!=4||
		pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("desEndPos.wing_x_offset.offsetDist")));
		
		SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.wing_y_offset.gStyle");
		SetPartMutiObjsProp(pFirstLineAngle,"desEndPos.wing_y_offset.offsetDist");
		SetPropItemReadOnly(pFirstLineAngle,"desEndPos.wing_y_offset.offsetDist",pFirstLineAngle->desEndPos.wing_y_offset.gStyle!=4||
		pFirstLineAngle->GetObjPropReadOnlyDefaultState(CLDSLineAngle::GetPropID("desEndPos.wing_y_offset.offsetDist")));*/
		//Ԥ����
		SetPartMutiObjsProp(pFirstLineAngle,"m_hArcLift");
		pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desStartPos.jgber_cal_style"),NULL);
		if(pFindItem)
			UpdateLineAngle_ArcLift_TreeItem(pPropList,pFindItem,TRUE);
		
		pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("desEndPos.jgber_cal_style"),NULL);
		if(pFindItem)
			UpdateLineAngle_ArcLift_TreeItem(pPropList,pFindItem,FALSE);	

		//ʼ��ѹ��
		SetPartMutiObjsProp(pFirstLineAngle,"start_push_flat_x1_y2");
		if(pFirstLineAngle->start_push_flat_x1_y2==1 || pFirstLineAngle->start_push_flat_x1_y2==2)
		{	
			if(!SetPartMutiObjsProp(pFirstLineAngle,"start_push_flat_length"))
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("start_push_flat_x1_y2"),&pParentItem);
				if(pFindItem)
				{	//ѹ�ⳤ��
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Push Flat Length";
					lpInfo->m_strPropHelp = "Push flat length";
#else
					lpInfo->m_strPropName = "ѹ�ⳤ��";
					lpInfo->m_strPropHelp = "ѹ�ⳤ��";
#endif
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("start_push_flat_length");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
				}
			}
		}
		else 
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("start_push_flat_length"));
		//�ն�ѹ��
		SetPartMutiObjsProp(pFirstLineAngle,"end_push_flat_x1_y2");
		if(pFirstLineAngle->end_push_flat_x1_y2==1 || pFirstLineAngle->end_push_flat_x1_y2==2)
		{	
			if(!SetPartMutiObjsProp(pFirstLineAngle,"end_push_flat_length"))
			{
				pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("end_push_flat_x1_y2"),&pParentItem);
				if(pFindItem)	
				{	//ѹ�ⳤ��
					lpInfo = new CItemInfo();
					lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
					lpInfo->m_strPropName = "Push Flat Length";
					lpInfo->m_strPropHelp = "Push flat length";
#else
					lpInfo->m_strPropName = "ѹ�ⳤ��";
					lpInfo->m_strPropHelp = "ѹ�ⳤ��";
#endif
					pPropItem = pPropList->InsertItem(pFindItem,lpInfo, -1);
					pPropItem->m_idProp = CLDSLineAngle::GetPropID("end_push_flat_length");
					if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
						pPropItem->m_lpNodeInfo->m_strPropValue = sText;
					pPropItem->m_lpNodeInfo->m_bMutiObjsProp = CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
					pPropItem->m_dwPropGroup = GetSingleWord(GROUP_STATMAT);
				}
			}
		}
		else 
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("end_push_flat_length"));
		//�м�ѹ��
		SetPartMutiObjsProp(pFirstLineAngle,"mid_push_flat_x1_y2");
		if(pFirstLineAngle->mid_push_flat_x1_y2==1 || pFirstLineAngle->mid_push_flat_x1_y2==2)
		{	
			pFindItem = pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("mid_push_flat_start_offset"),&pParentItem);
			if(pFindItem==NULL)	
			{	//ʼ��ƫ����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Start Offset";
				lpInfo->m_strPropHelp = "Start Offset";
#else
				lpInfo->m_strPropName = "ʼ��ƫ����";
				lpInfo->m_strPropHelp = "ʼ��ƫ����";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,12,true);
				pPropItem->m_idProp = CLDSLineAngle::GetPropID("mid_push_flat_start_offset");
				if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
				pPropItem->m_dwPropGroup=GetSingleWord(GROUP_STATMAT);
				//�ն�ƫ����
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "End Offset";
				lpInfo->m_strPropHelp = "End Offset";
#else
				lpInfo->m_strPropName = "�ն�ƫ����";
				lpInfo->m_strPropHelp = "�ն�ƫ����";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,12,true);
				pPropItem->m_idProp = CLDSLineAngle::GetPropID("mid_push_flat_end_offset");
				if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
				pPropItem->m_dwPropGroup=GetSingleWord(GROUP_STATMAT);
				//ѹ��λ��ƫ��ʼ�˾��� hxr 2012-10-09
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
				lpInfo->m_strPropName = "Push Flat Node";
				lpInfo->m_strPropHelp = "The node handle of pushing flat position";
#else
				lpInfo->m_strPropName = "ѹ��λ�ýڵ�";
				lpInfo->m_strPropHelp = "ѹ��λ�ýڵ�";
#endif
				pPropItem = pPropList->InsertItem(pParentItem,lpInfo,12,true);
				pPropItem->m_idProp = CLDSLineAngle::GetPropID("mid_push_flat_node_handle");
				if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
					pPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
				pPropItem->m_dwPropGroup=GetSingleWord(GROUP_STATMAT);
			}
			else
			{
				SetPartMutiObjsProp(pFirstLineAngle,"mid_push_flat_start_offset");
				SetPartMutiObjsProp(pFirstLineAngle,"mid_push_flat_end_offset");
				SetPartMutiObjsProp(pFirstLineAngle,"mid_push_flat_node_handle");
			}
		}
		else 
		{
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("mid_push_flat_start_offset"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("mid_push_flat_end_offset"));
			pPropList->DeleteItemByPropId(CLDSLineAngle::GetPropID("mid_push_flat_node_handle"));
		}
		
		SetPartMutiObjsProp(pFirstLineAngle,"m_fCutAngleCoefR");
		//ʼ�ն��н���Ϣ
		SetPartMutiObjsProp(pFirstLineAngle,"startCutWing");
		SetPartMutiObjsProp(pFirstLineAngle,"endCutWing");
		SetPartMutiObjsProp(pFirstLineAngle,"startCutWingX");
		SetPartMutiObjsProp(pFirstLineAngle,"startCutWingY");
		SetPartMutiObjsProp(pFirstLineAngle,"endCutWingX");
		SetPartMutiObjsProp(pFirstLineAngle,"endCutWingY");
		if((pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("startCutWingX"),NULL))!=NULL)
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp|=!pFirstLineAngle->m_bUserSpecStartCutAngle;
		if((pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("startCutWingY"),NULL))!=NULL)
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp|=!pFirstLineAngle->m_bUserSpecStartCutAngle;
		if((pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("endCutWingX"),NULL))!=NULL)
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp|=!pFirstLineAngle->m_bUserSpecEndCutAngle;
		if((pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("endCutWingY"),NULL))!=NULL)
			pFindItem->m_lpNodeInfo->m_bMutiObjsProp|=!pFirstLineAngle->m_bUserSpecEndCutAngle;
		//pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("startCutWingX"),!pFirstLineAngle->m_bUserSpecStartCutAngle);
		//pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("startCutWingY"),!pFirstLineAngle->m_bUserSpecStartCutAngle);
		//pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("endCutWingX"),!pFirstLineAngle->m_bUserSpecEndCutAngle);
		//pPropList->SetItemReadOnly(CLDSLineAngle::GetPropID("endCutWingY"),!pFirstLineAngle->m_bUserSpecEndCutAngle);
		/*
		//ʼ���н���Ϣ
		SetPartMutiObjsProp(pFirstLineAngle,"m_bUserSpecStartCutAngle");
		SetPartMutiObjsProp(pFirstLineAngle,"start_cut_angle_type");
		CString sStartCutType="";
		pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("start_cut_angle_type"),sStartCutType);
		UpdateLineAngleCutAngleInfoPropItem(pFirstLineAngle,pPropList,TRUE,this,sStartCutType,TRUE);
		//
		if(pFirstLineAngle->m_bUserSpecStartCutAngle)
		{
			SetPartMutiObjsProp(pFirstLineAngle,"cut_angle[0][0]");
			SetPartMutiObjsProp(pFirstLineAngle,"cut_angle[0][1]");
			SetPartMutiObjsProp(pFirstLineAngle,"cut_angle[1][0]");
			SetPartMutiObjsProp(pFirstLineAngle,"cut_angle[1][1]");
			SetPartMutiObjsProp(pFirstLineAngle,"cut_wing_para[0][0]");
			SetPartMutiObjsProp(pFirstLineAngle,"cut_wing_para[0][1]");
			SetPartMutiObjsProp(pFirstLineAngle,"cut_wing_para[0][2]");
		}
		pFindItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("cut_angle[1][0]"),NULL);
		//�ն��н���Ϣ
		SetPartMutiObjsProp(pFirstLineAngle,"m_bUserSpecEndCutAngle");
		SetPartMutiObjsProp(pFirstLineAngle,"end_cut_angle_type");
		CString sEndCutType="";
		pPropList->GetItemPropValue(CLDSLineAngle::GetPropID("end_cut_angle_type"),sEndCutType);
		UpdateLineAngleCutAngleInfoPropItem(pFirstLineAngle,pPropList,FALSE,this,sEndCutType,TRUE);
		//
		if(pFirstLineAngle->m_bUserSpecEndCutAngle)
		{
			SetPartMutiObjsProp(pFirstLineAngle,"cut_angle[2][0]");
			SetPartMutiObjsProp(pFirstLineAngle,"cut_angle[2][1]");
			SetPartMutiObjsProp(pFirstLineAngle,"cut_angle[3][0]");
			SetPartMutiObjsProp(pFirstLineAngle,"cut_angle[3][1]");
			SetPartMutiObjsProp(pFirstLineAngle,"cut_wing_para[1][0]");
			SetPartMutiObjsProp(pFirstLineAngle,"cut_wing_para[1][1]");
			SetPartMutiObjsProp(pFirstLineAngle,"cut_wing_para[1][2]");
		}
		*/
		//ʼ�ն˵�Բ����Ϣ
		SetPartMutiObjsProp(pFirstLineAngle,"FilletArcS");
		SetPartMutiObjsProp(pFirstLineAngle,"FilletArcE");
		SetPartMutiObjsProp(pFirstLineAngle,"FilletROfSX");
		SetPartMutiObjsProp(pFirstLineAngle,"FilletROfSY");
		SetPartMutiObjsProp(pFirstLineAngle,"FilletROfEX");
		SetPartMutiObjsProp(pFirstLineAngle,"FilletROfEY");
	}
	
	pPropList->Redraw();
}
void CLDSView::DisplayLineAnglePositionSubSectionProperty(int iStartSectionType,int iEndSectionType)
{
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem* pGroupItem,*pPropItem,*pSonPropItem,*pParentItem;
	CItemInfo* lpInfo;
	CXhChar500 sText;

	try{	//����GetExtraOdd->GetEndDatumPosEx->CSmartPtr->throw ""
		pGroupItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("startInfo"),&pParentItem);
		if(pGroupItem)
		{
			//ʼ������ͷ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Oddment";
			lpInfo->m_strPropHelp = "Oddment of angle start";
#else
			lpInfo->m_strPropName = "����ͷ";
			lpInfo->m_strPropHelp = "�Ǹ�ʼ������ͷ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("oddStart");
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pPropItem->m_dwPropGroup = GetSingleWord(iStartSectionType);
			//ʼ������ͷ�ļ��㷽ʽ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Collide|1.Bolt Check|2.Specify";
			lpInfo->m_strPropName = "Calculate Principle";
			lpInfo->m_strPropHelp = "The calculating principle of angle's start oddment.";
#else
			lpInfo->m_cmbItems="0.��ײ����|1.��˨���|2.ָ����ֵ";
			lpInfo->m_strPropName = "����ԭ��";
			lpInfo->m_strPropHelp = "�Ǹ�ʼ������ͷ�ļ���ԭ��";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("oddStart.m_iOddCalStyle");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(CLDSLineAngle::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(iStartSectionType);
			//
			UpdateLineAngleOddProperty(pPropList,pSonPropItem,TRUE);
			//�Ǹ�ʼ�˹̶�����ͷ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Extra Oddment";
			lpInfo->m_strPropHelp = "Extra oddment refers to the distance from position end datum point of angle's leg center line to center line position datum point in extending view on map.Generally it's zero,when there is angle end intersecting with bending line etc other special situation,it's not zero.Generally the sum of extra oddment and angle's oddment should equal the oddment of center given on map.";
#else
			lpInfo->m_strPropName = "�̶�����ͷ";
			lpInfo->m_strPropHelp = "�̶�����ͷ��ָ�Ǹ�ĳ֫���ߵĶ�λ�˻������ͼֽ��չ��ͼ�е����߶�λ�����ľ��롣һ�����Ϊ�㣬���нǸֶ˽��ڻ����ߵ���������²�Ϊ�㡣һ��̶�����ͷ��Ǹ�����ͷ֮��ӦΪͼֽ�ϸ�������������ͷֵ��";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("oddStartExtra");
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(iStartSectionType);
			//������˨λ�ü���Ǹ�����ͷʱ���Ƿ�����ͷ����˼��е� wjh-2019.8.15
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Can shorten across middle";
			lpInfo->m_strPropHelp = "Extra oddment refers to the distance from position end datum point of angle's leg center line to center line position datum point in extending view on map.Generally it's zero,when there is angle end intersecting with bending line etc other special situation,it's not zero.Generally the sum of extra oddment and angle's oddment should equal the oddment of center given on map.";
			lpInfo->m_cmbItems="0.No|1.Yes";
#else
			lpInfo->m_strPropName = "��ͷ�ɿ���е�";
			lpInfo->m_strPropHelp = "���ݶ�ͷ��˨λ�ü���Ǹָ�ͷʱ���Ƿ�����ͷ��Խ�˼��м���档";
			lpInfo->m_cmbItems="0.������|1.����";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("StartOddCanAcrossMiddleSect");
			if (pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(iStartSectionType);
			//��������������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Ridge Start Position";
			lpInfo->m_strPropHelp = "The start coordinate of angle's ridge line without considering oddment";
#else
			lpInfo->m_strPropName = "��������������";
			lpInfo->m_strPropHelp = "�Ǹ�����δ��������ͷʱ���������";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("startPos");
			pPropItem->m_bHideChildren = TRUE;
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(iStartSectionType);
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "Start point's X coordinate of angle's ridge line";
#else
			lpInfo->m_strPropHelp = "�Ǹ��������X�᷽�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("startPos.x");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup = GetSingleWord(iStartSectionType);
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "Start point's Y coordinate of angle's ridge line";
#else
			lpInfo->m_strPropHelp = "�Ǹ��������Y�᷽�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("startPos.y");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup = GetSingleWord(iStartSectionType);
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "Start point's Z coordinate of angle's ridge line";
#else
			lpInfo->m_strPropHelp = "�Ǹ��������Z�᷽�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("startPos.z");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup = GetSingleWord(iStartSectionType);
			//�������ʵ������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Actual Ridge Start Position";
			lpInfo->m_strPropHelp = "The start coordinate of angle's ridge line after considering oddment";
#else
			lpInfo->m_strPropName = "�������ʵ������";
			lpInfo->m_strPropHelp = "�Ǹ����߿�������ͷ����������";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("actualStartPos");
			pPropItem->m_dwPropGroup = GetSingleWord(iStartSectionType);
			//ʼ�˵㶨λ��ʽ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Ridge Point Position";
			lpInfo->m_strPropHelp = "The position calculate style of angle's start ridge point";
			lpInfo->m_cmbItems = "Parametric|Specify";
#else
			lpInfo->m_strPropName = "��㶨λ��ʽ";
			lpInfo->m_strPropHelp = "�Ǹ�ʼ�����Ķ�λ���㷽ʽ";
			lpInfo->m_cmbItems = "��������λ|ָ������|�򵥶�λ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("desStartPos.jgber_cal_style");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			UpdateLineAngle_DesPos_JgCalStyle_TreeItem(pPropList,pPropItem,TRUE);
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstLineAngle->IsStartPosLocked()||pFirstLineAngle->bHuoQuStart||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pPropItem->m_dwPropGroup = GetSingleWord(iStartSectionType);
			// TODO:�д���������
			UpdateLineAngle_ArcLift_TreeItem(pPropList,pPropItem,TRUE);
		}
	
		pGroupItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("endInfo"),&pParentItem);
		if(pGroupItem)
		{
			//�ն�����ͷ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
			lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Oddment";
			lpInfo->m_strPropHelp = "Oddment of angle end";
#else
			lpInfo->m_strPropName = "����ͷ";
			lpInfo->m_strPropHelp = "�Ǹ��ն�����ͷ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("oddEnd");
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pPropItem->m_dwPropGroup = GetSingleWord(iEndSectionType);
			//�ն�����ͷ�ļ��㷽ʽ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType=PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_cmbItems="0.Collide|1.Bolt Check|2.Specify";
			lpInfo->m_strPropName = "Calculate Principle";
			lpInfo->m_strPropHelp = "The calculating principle of angle's end oddment.";
#else
			lpInfo->m_cmbItems="0.��ײ����|1.��˨���|2.ָ����ֵ";
			lpInfo->m_strPropName = "����ԭ��";
			lpInfo->m_strPropHelp = "�Ǹ��ն�����ͷ�ļ���ԭ��";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("oddEnd.m_iOddCalStyle");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			if(CLDSLineAngle::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(iEndSectionType);
			//
			UpdateLineAngleOddProperty(pPropList,pSonPropItem,FALSE);
			//�Ǹ��ն˹̶�����ͷ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Extra Oddment";
			lpInfo->m_strPropHelp = "Extra oddment refers to the distance from position end datum point of angle's leg center line to center line position datum point in extending view on map.Generally it's zero,when there is angle end intersecting with bending line etc other special situation,it's not zero.Generally the sum of extra oddment and angle's oddment should equal the oddment of center given on map.";
#else
			lpInfo->m_strPropName = "�̶�����ͷ";
			lpInfo->m_strPropHelp = "�̶�����ͷ��ָ�Ǹ�ĳ֫���ߵĶ�λ�˻������ͼֽ��չ��ͼ�е����߶�λ�����ľ��롣һ�����Ϊ�㣬���нǸֶ˽��ڻ����ߵ���������²�Ϊ�㡣һ��̶�����ͷ��Ǹ�����ͷ֮��ӦΪͼֽ�ϸ�������������ͷֵ��";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("oddEndExtra");
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(iEndSectionType);
			//������˨λ�ü���Ǹ�����ͷʱ���Ƿ�����ͷ����˼��е� wjh-2019.8.15
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_cmbType=CDT_LIST;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Can shorten across middle";
			lpInfo->m_strPropHelp = "Extra oddment refers to the distance from position end datum point of angle's leg center line to center line position datum point in extending view on map.Generally it's zero,when there is angle end intersecting with bending line etc other special situation,it's not zero.Generally the sum of extra oddment and angle's oddment should equal the oddment of center given on map.";
			lpInfo->m_cmbItems="0.No|1.Yes";
#else
			lpInfo->m_strPropName = "��ͷ�ɿ���е�";
			lpInfo->m_strPropHelp = "���ݶ�ͷ��˨λ�ü���Ǹָ�ͷʱ���Ƿ�����ͷ��Խ�˼��м���档";
			lpInfo->m_cmbItems="0.������|1.����";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("EndOddCanAcrossMiddleSect");
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(iEndSectionType);
			//�����յ��������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Ridge End Position";
			lpInfo->m_strPropHelp = "The end coordinate of angle's ridge line without considering oddment";
#else
			lpInfo->m_strPropName = "�����յ��������";
			lpInfo->m_strPropHelp = "�Ǹ������յ�δ��������ͷʱ������λ��";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("endPos");
			pPropItem->m_bHideChildren = TRUE;
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(iEndSectionType);
			//X
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "X";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "End point's X coordinate of angle's ridge line";
#else
			lpInfo->m_strPropHelp = "�Ǹ������յ�X�᷽�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("endPos.x");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup = GetSingleWord(iEndSectionType);
			//Y
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Y";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "End point's Y coordinate of angle's ridge line"; 
#else
			lpInfo->m_strPropHelp = "�Ǹ������յ�Y�᷽�������"; 
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("endPos.y");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup = GetSingleWord(iEndSectionType);
			//Z
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
			lpInfo->m_strPropName = "Z";
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropHelp = "End point's Z coordinate of angle's ridge line";
#else
			lpInfo->m_strPropHelp = "�Ǹ������յ�Z�᷽�������";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo, -1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("endPos.z");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
										pFirstLineAngle->GetObjPropReadOnlyDefaultState(pSonPropItem->m_idProp));
			pSonPropItem->m_dwPropGroup = GetSingleWord(iEndSectionType);
			//�����յ�ʵ������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Actual Ridge End Position";
			lpInfo->m_strPropHelp = "The end coordinate of angle's ridge line after considering oddment";
#else
			lpInfo->m_strPropName = "�����յ�ʵ������";
			lpInfo->m_strPropHelp = "�Ǹ������յ㿼������ͷ�������λ��";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("actualEndPos");
			pPropItem->m_dwPropGroup = GetSingleWord(iEndSectionType);
			//ʼ�˵㶨λ��ʽ
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Ridge Point Position";
			lpInfo->m_strPropHelp = "The position calculate style of angle's end ridge point";
			lpInfo->m_cmbItems = "Parametric|Specify";
#else
			lpInfo->m_strPropName = "��㶨λ��ʽ";
			lpInfo->m_strPropHelp = "�Ǹ��ն����Ķ�λ���㷽ʽ";
			lpInfo->m_cmbItems = "��������λ|ָ������|�򵥶�λ";
#endif
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("desEndPos.jgber_cal_style");
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			UpdateLineAngle_DesPos_JgCalStyle_TreeItem(pPropList,pPropItem,FALSE);
			if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
				pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
			pPropItem->SetReadOnly(pFirstLineAngle->IsEndPosLocked()||pFirstLineAngle->bHuoQuEnd||
									pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
			pPropItem->m_dwPropGroup = GetSingleWord(iEndSectionType);
			//TODO: �д���������
			UpdateLineAngle_ArcLift_TreeItem(pPropList,pPropItem,FALSE);
		}
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
	}
}
void CLDSView::DisplayLineAngleDesignSubSectionProperty(int iSectionType)
{
	CLDSLineAngle *pFirstLineAngle = (CLDSLineAngle*)selectObjs.GetFirst();
	CTowerPropertyDlg *pPropDlg=((CMainFrame*)AfxGetMainWnd())->GetTowerPropertyPage();
	CPropertyList *pPropList = pPropDlg->GetPropertyList();
	CPropTreeItem* pGroupItem,*pPropItem,*pSonPropItem,*pParentItem;
	CItemInfo *lpInfo;
	CXhChar500 sText;

#ifdef __LDS_
	pGroupItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("basicInfo"),&pParentItem);
	if(pGroupItem&&!theApp.m_bDrawingWorkEnvOnly)
	{
		//ͳ�ı��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
		lpInfo->m_buttonType = BDT_COMMONEDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Stat. Material No";
		lpInfo->m_strPropHelp = "Number of statistic Material";
#else
		lpInfo->m_strPropName = "ͳ�ı��";
		lpInfo->m_strPropHelp = "ͳ�ı��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_uStatMatNo");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		//�����Ŵ�ϵ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Weight Raise Coef";
		lpInfo->m_strPropHelp = "Weight raise coefficient";
#else
		lpInfo->m_strPropName = "�����Ŵ�ϵ��";
		lpInfo->m_strPropHelp = "�����Ŵ�ϵ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_fWeightRaiseCoef");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		//
		/*lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�Ƿ���в�����ѡ";
		lpInfo->m_strPropHelp = "�Ƿ���в�����ѡ";
		lpInfo->m_cmbItems = "��|��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1,TRUE);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_bNeedSelMat");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
		lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup=GetSingleWord(iSectionType);*/
		//�Ƿ��Ԥѡ��
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Need Select Size";
		lpInfo->m_strPropHelp = "Need select size or not?";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "�Ƿ���й����ѡ";
		lpInfo->m_strPropHelp = "�Ƿ���й����ѡ";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_bNeedSelSize");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
	}
	pGroupItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("designInfo"),&pParentItem);
	if(pGroupItem&&theApp.m_bAnalysisWorkEnv)//!theApp.m_bDrawingWorkEnvOnly)
	{
		//��Ԫ����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Elem Type";
		lpInfo->m_strPropHelp = "Elem type";
		lpInfo->m_cmbItems = "0.�Զ��ж�|1.������|2.������";
#else
		lpInfo->m_strPropName = "��Ԫ����";
		lpInfo->m_strPropHelp = "��Ԫ����";
		lpInfo->m_cmbItems = "0.�Զ��ж�|1.������|2.������";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_iElemType");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		if(toupper(pFirstLineAngle->layer(1))=='F')
		{
			//�������ϼ�������
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_COMBO;
			lpInfo->m_strPropName = "�������ϼ�������";
			lpInfo->m_strPropHelp = "�������ϼ�������";
			lpInfo->m_cmbItems = "�Զ����|����ʼ��|�����ն�|����б��";
			pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
			pPropItem->m_idProp = CLDSLineAngle::GetPropID("sel_mat_method");
			if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
				pPropItem->m_lpNodeInfo->m_strPropValue = sText;
			pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
			pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
			/*if(pFirstLineAngle->sel_mat_method!=AUX_AUTODESIGN)
			{	//������֧����ϵ��
				lpInfo = new CItemInfo();
				lpInfo->m_controlType = PIT_EDIT;
				lpInfo->m_strPropName = "������֧����ϵ��";
				lpInfo->m_strPropHelp = "������֧����ϵ��";
				pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
				pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("bear_load_coef");
				if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
					pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
				pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
				pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
			}*/
		}
		//�Զ�������˨������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Automatic Calculate Hole Out Num";
		lpInfo->m_strPropHelp = "When optimizing select material or checking computation,automatic calculate the number of bolt hole out or not";
		lpInfo->m_cmbItems = "Yes|No";
#else
		lpInfo->m_strPropName = "�Զ�������˨������";
		lpInfo->m_strPropHelp = "�Ƿ����Ż�ѡ�Ļ�����ʱ�Զ����¼�����˨������";
		lpInfo->m_cmbItems = "��|��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("m_bAutoCalHoleOutCoef");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		//��˨������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Hole Out Count";
		lpInfo->m_strPropHelp = "The count of Bolt hole out";
#else
		lpInfo->m_strPropName = "��˨������";
		lpInfo->m_strPropHelp = "��˨������";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("hole_out");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->SetReadOnly(pFirstLineAngle->m_bAutoCalHoleOutCoef||
								pFirstLineAngle->GetObjPropReadOnlyDefaultState(pPropItem->m_idProp));
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		//���㳤������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "���㳤������";
		lpInfo->m_strPropHelp = "���㳤������";
		lpInfo->m_cmbItems = "0.�����ж�|1.Vб��(��Ų�����)|2.Vб��(V������)|3.Vб��3�ڼ��������|4.Vб��4�ڼ��������|5.ָ���˼����㳤��";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.iTypeNo");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			pPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		if(pFirstLineAngle->CalLenCoef.iTypeNo==0)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Reference Pole,Press At The Same Time";
			lpInfo->m_strPropHelp = "Reference pole,press at the same time";
#else
			lpInfo->m_strPropName = "ͬʱ��ѹ���ո˼�";
			lpInfo->m_strPropHelp = "ͬʱ��ѹ���ո˼�";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.pRefPole");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		}
		//
		if(pFirstLineAngle->CalLenCoef.iTypeNo==3||pFirstLineAngle->CalLenCoef.iTypeNo==4)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Reference Horizontal Join Pole";
			lpInfo->m_strPropHelp = "Reference horizontal join pole to calculate length";
#else
			lpInfo->m_strPropName = "���պ�����";
			lpInfo->m_strPropHelp = "���㳤�Ȳ��պ�����";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.pHorizPole");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		}
		//
		if(pFirstLineAngle->CalLenCoef.iTypeNo==5)
		{
			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Minimum Axis Length";
			lpInfo->m_strPropHelp = "Minimum axis length,unit(mm)";
#else
			lpInfo->m_strPropName = "��С�᳤��";
			lpInfo->m_strPropHelp = "��С�᳤��,��λ(mm)";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.spec_callenMinR");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_bHideChildren=FALSE;
			if(CLDSLineAngle::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren=CLDSLineAngle::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Start";
			lpInfo->m_strPropHelp = "The start node handle of specified length";
#else
			lpInfo->m_strPropName = "��ʼ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ʼ�ڵ���";
#endif
			CPropTreeItem* pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.minR.hStartNode");
			if(pFirstLineAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "End";
			lpInfo->m_strPropHelp = "The end node handle of specified length";
#else
			lpInfo->m_strPropName = "��ֹ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ֹ�ڵ���";
#endif
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.minR.hEndNode");
			if(pFirstLineAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Convert Coef";
			lpInfo->m_strPropHelp = "Convert coefficient to calculate length";
#else
			lpInfo->m_strPropName = "����ϵ��";
			lpInfo->m_strPropHelp = "���㳤�ȵ�����ϵ��";
#endif
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.minR.coef");
			if(pFirstLineAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Parallel Axis Length";
			lpInfo->m_strPropHelp = "Parallel axis length,unit(mm)";
#else
			lpInfo->m_strPropName = "ƽ���᳤��";
			lpInfo->m_strPropHelp = "ƽ���᳤��,��λ(mm)";
#endif
			pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
			pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.spec_callenParaR");
			if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
			pSonPropItem->m_bHideChildren=FALSE;
			if(CLDSLineAngle::GetPropStatus(pSonPropItem->m_idProp)!=-1)
				pSonPropItem->m_bHideChildren=CLDSLineAngle::GetPropStatus(pSonPropItem->m_idProp);
			pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Start";
			lpInfo->m_strPropHelp = "The start node handle of specified length";
#else
			lpInfo->m_strPropName = "��ʼ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ʼ�ڵ���";
#endif
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.paraR.hStartNode");
			if(pFirstLineAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "End";
			lpInfo->m_strPropHelp = "The end node handle of specified length";
#else
			lpInfo->m_strPropName = "��ֹ�ڵ�";
			lpInfo->m_strPropHelp = "ָ�����ȵ���ֹ�ڵ���";
#endif
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.paraR.hEndNode");
			if(pFirstLineAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);

			lpInfo = new CItemInfo();
			lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
			lpInfo->m_strPropName = "Convert Coef";
			lpInfo->m_strPropHelp = "Convert coefficient to calculate length";
#else
			lpInfo->m_strPropName = "����ϵ��";
			lpInfo->m_strPropHelp = "���㳤�ȵ�����ϵ��";
#endif
			pLeafItem = pPropList->InsertItem(pSonPropItem,lpInfo,-1,TRUE);
			pLeafItem->m_idProp = CLDSLineAngle::GetPropID("CalLenCoef.paraR.coef");
			if(pFirstLineAngle->GetPropValueStr(pLeafItem->m_idProp,sText)>0)
				lpInfo->m_strPropValue = sText;
			pLeafItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pLeafItem->m_idProp);
			pLeafItem->m_dwPropGroup=GetSingleWord(iSectionType);
		}
	}
#endif

	pGroupItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("startInfo"),&pParentItem);
	if(pGroupItem)
	{
#ifdef __LDS_
		//ʼ�������Ϣ
		if(!theApp.m_bDrawingWorkEnvOnly)
		{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "ʼ��Լ������";
		lpInfo->m_strPropHelp = "ʼ��Լ������";
		lpInfo->m_cmbItems = "�½�|�ս�|�Զ��ж�";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("start_joint_type");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "ʼ����������";
		lpInfo->m_strPropHelp = "ʼ����������";
		lpInfo->m_cmbItems = "��������|ƫ������";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("start_force_type");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		}
#endif
		//ʼ����˨����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Preset Bolts Information";
		lpInfo->m_strPropHelp = "";
#else 
		lpInfo->m_strPropName = "Ԥ����˨��Ϣ";
		lpInfo->m_strPropHelp = "��˨������Ϣ��ָ�û���ʵ�ʲ�����˨֮ǰ��Ԥ�������˨������Ϣ������Ϣ�ڽ���ʵ�ʲ�����˨������ֻ����Ĭ��ֵ�����ã����������յ���˨������Ϣ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("startBoltInfo");
#ifdef __LDS_
		if(theApp.m_bAnalysisWorkEnv)
			pPropItem->m_bHideChildren = TRUE;
		else
			pPropItem->m_bHideChildren = FALSE;
#endif
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		//ʼ����˨����		
		/*�淶��˨�����ڽǸ�Ԥ����˨��Ϣ���ݲ���ʾ��˨�������� wxc-2016.8.8
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Grade";
		lpInfo->m_strPropHelp = "Bolt grade";
#else
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "��˨����";
#endif
		lpInfo->m_cmbItems = "4.8|5.8|6.8|8.8";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("connectStart.grade");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);*/
					//ʼ����˨���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Size";
		lpInfo->m_strPropHelp = "Start bolt size";
#else
		lpInfo->m_strPropName = "��˨���";
		lpInfo->m_strPropHelp = "ʼ����˨���";
#endif
		lpInfo->m_cmbItems = "M12|M16|M20|M24";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("connectStart.d");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
					//ʼ����˨����֫
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_cmbItems="0.Two Leg Connect|1.One Leg Connect";
		lpInfo->m_strPropName = "Connect Leg";
		lpInfo->m_strPropHelp = "Connect Leg";
#else
		lpInfo->m_cmbItems="0.˫֫����|1.��֫����";
		lpInfo->m_strPropName = "����֫";
		lpInfo->m_strPropHelp = "��������֫";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("connectStart.m_iConnectWing");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
					//ʼ����˨����
#if defined(__PART_DESIGN_INC_)||defined(__SMART_DRAW_INC_)
		//ʼ����˨��������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Num";
		lpInfo->m_strPropHelp = "Start Bolt number";
#else
		lpInfo->m_strPropName = "������˨��";
		lpInfo->m_strPropHelp = "����ǿ��Ҫ���������˨����";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("connectStart.summBoltN");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
#endif
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Num";
		lpInfo->m_strPropHelp = "Start Bolt number";
#else
		lpInfo->m_strPropName = "��֫������";
		lpInfo->m_strPropHelp = "ʼ�˵�֫������˨����";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("connectStart.N");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
					//ʼ����˨����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Rows";
		lpInfo->m_strPropHelp = "Start Bolt rows";
#else
		lpInfo->m_strPropName = "��˨����";
		lpInfo->m_strPropHelp = "ʼ����˨����";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("connectStart.rows");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
			//�Ǹ�ʼ�˹�����˨
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Attached Bolts";
		lpInfo->m_strPropHelp = "Start attached Bolts";
#else
		lpInfo->m_strPropName = "�Ѳ���˨";
		lpInfo->m_strPropHelp = "ʼ���Ѳ�����˨";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("startAttachBolts");
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
	}
	
	pGroupItem=pPropList->FindItemByPropId(CLDSLineAngle::GetPropID("endInfo"),&pParentItem);
	if(pGroupItem)
	{
#ifdef __LDS_
		//�ն������Ϣ
		if(!theApp.m_bDrawingWorkEnvOnly)
		{
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�ն�Լ������";
		lpInfo->m_strPropHelp = "�ն�Լ������";
		lpInfo->m_cmbItems = "�½�|�ս�|�Զ��ж�";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("end_joint_type");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		//
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
		lpInfo->m_strPropName = "�ն���������";
		lpInfo->m_strPropHelp = "�ն���������";
		lpInfo->m_cmbItems = "��������|ƫ������";
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("end_force_type");
		if(pFirstLineAngle->GetPropValueStr(pPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		}
#endif
		//�ն���˨����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Preset Bolts Information";
		lpInfo->m_strPropHelp = "";
#else 
		lpInfo->m_strPropName = "Ԥ����˨��Ϣ";
		lpInfo->m_strPropHelp = "��˨������Ϣ��ָ�û���ʵ�ʲ�����˨֮ǰ��Ԥ�������˨������Ϣ������Ϣ�ڽ���ʵ�ʲ�����˨������ֻ����Ĭ��ֵ�����ã����������յ���˨������Ϣ��";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo,-1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("endBoltInfo");
#ifdef __LDS_
		if(theApp.m_bAnalysisWorkEnv)
			pPropItem->m_bHideChildren = TRUE;
		else
			pPropItem->m_bHideChildren = FALSE;
#endif
		if(CLDSLineAngle::GetPropStatus(pPropItem->m_idProp)!=-1)
			pPropItem->m_bHideChildren = CLDSLineAngle::GetPropStatus(pPropItem->m_idProp);
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		//�ն���˨����
		/*�淶��˨�����ڽǸ�Ԥ����˨��Ϣ���ݲ���ʾ��˨�������� wxc-2016.8.8
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Grade";
		lpInfo->m_strPropHelp = "Bolt grade";
#else
		lpInfo->m_strPropName = "����";
		lpInfo->m_strPropHelp = "��˨����";
#endif
		lpInfo->m_cmbItems = "4.8|5.8|6.8|8.8";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("connectEnd.grade");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			lpInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);*/
		//�ն���˨���
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Size";
		lpInfo->m_strPropHelp = "End Bolt Size";
#else
		lpInfo->m_strPropName = "��˨���";
		lpInfo->m_strPropHelp = "�ն���˨���";
#endif
		lpInfo->m_cmbItems = "M12|M16|M20|M24";
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("connectEnd.d");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		//�ն���˨����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_COMBO;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Connect Leg";
		lpInfo->m_strPropHelp = "End Bolt number";
		lpInfo->m_cmbItems="0.Two Leg Connect|1.One Leg Connect";
#else
		lpInfo->m_strPropName = "����֫";
		lpInfo->m_strPropHelp = "��������֫";
		lpInfo->m_cmbItems="0.˫֫����|1.��֫����";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("connectEnd.m_iConnectWing");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		//�ն���˨����
#if defined(__PART_DESIGN_INC_)||defined(__SMART_DRAW_INC_)
		//�ն���˨��������
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Num";
		lpInfo->m_strPropHelp = "Start Bolt number";
#else
		lpInfo->m_strPropName = "������˨��";
		lpInfo->m_strPropHelp = "����ǿ��Ҫ���������˨����";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem, lpInfo, -1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("connectEnd.summBoltN");
		if (pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp, sText) > 0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs, pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
#endif
		//�ն˵�֫������˨����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Num";
		lpInfo->m_strPropHelp = "End Bolt number";
#else
		lpInfo->m_strPropName = "��֫������";
		lpInfo->m_strPropHelp = "�ն˵�֫������˨����";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("connectEnd.N");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
		//�ն���˨����
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_EDIT;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Rows";
		lpInfo->m_strPropHelp = "End Bolt rows";
#else
		lpInfo->m_strPropName = "��˨����";
		lpInfo->m_strPropHelp = "�ն���˨����";
#endif
		pSonPropItem = pPropList->InsertItem(pPropItem,lpInfo,-1);
		pSonPropItem->m_idProp = CLDSLineAngle::GetPropID("connectEnd.rows");
		if(pFirstLineAngle->GetPropValueStr(pSonPropItem->m_idProp,sText)>0)
			pSonPropItem->m_lpNodeInfo->m_strPropValue = sText;
		pSonPropItem->m_lpNodeInfo->m_bMutiObjsProp = !CLDSLineAngle::IsPropEqual(selectObjs,pSonPropItem->m_idProp);
		pSonPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
			//�Ǹ��ն˹�����˨
		lpInfo = new CItemInfo();
		lpInfo->m_controlType = PIT_BUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
		lpInfo->m_strPropName = "Attached Bolts";
		lpInfo->m_strPropHelp = "End attached Bolts";
#else
		lpInfo->m_strPropName = "�Ѳ���˨";
		lpInfo->m_strPropHelp = "�ն��Ѳ�����˨";
#endif
		pPropItem = pPropList->InsertItem(pGroupItem,lpInfo, -1);
		pPropItem->m_idProp = CLDSLineAngle::GetPropID("endAttachBolts");
		pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
	}
		//�Ǹ��м������˨
	lpInfo = new CItemInfo();
	lpInfo->m_controlType = PIT_GRAYBUTTON;
#ifdef AFX_TARG_ENU_ENGLISH
	lpInfo->m_strPropName = "Middle Attached Bolts";
	lpInfo->m_strPropHelp = "Middle attached Bolts";
#else
	lpInfo->m_strPropName = "�Ѳ��м���˨��";
	lpInfo->m_strPropHelp = "�Ѳ��м���˨";
#endif
	pPropItem = pPropList->InsertItem(pPropList->GetRootItem(),lpInfo, -1);
	pPropItem->m_idProp = CLDSLineAngle::GetPropID("midAttachBolts");
	pPropItem->m_dwPropGroup = GetSingleWord(iSectionType);
}
#endif

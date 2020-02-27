//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "MainFrm.h"
#include "LDSView.h"
#include "Tower.h"
#include "env_def.h"
#include "ElectricTower.h"
#include "XhObjHandler.h"

int ObjectDeleteFunc(int ID, int ent_type)
{
	HINSTANCE hOldResHandle=AfxGetResourceHandle();
	//-----vvvvvvv-��ʶ��������״̬Ϊ�棬��ͬһʱ��ֻ����һ����������������---------
	CLDSView* pView=((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
	CCmdLockObject cmdlock(pView);
	if(!cmdlock.LockSuccessed())
		return FALSE;
	
	CLDSNode *pNode=NULL;
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
		pNode->Unlock();
	if(ent_type<=0)
	{
		if(-ent_type == AtomType::prPoint)
		{
			pNode = Ta.Node.FromHandle(ID);
			if(pNode==NULL)
				return FALSE;
			else
			{	//δ���ͨ��ϵͳ WJH--01.06.11
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pNode->dwPermission))
				{
					AfxSetResourceHandle(theApp.m_hResHandle);
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("No corresponding deletion permission!");
#else
					AfxMessageBox("û����Ӧ��ɾ��Ȩ�ޣ�");
#endif
					AfxSetResourceHandle(hOldResHandle);
					return FALSE;
				}
				Ta.RemoveRelativeObjs(pNode);
				if(console.DeleteNode(ID))	//�˽ڵ�λ��Node��
					return TRUE;//console.DispNodeSet.DeleteNode(pNode->handle);
				else
					return FALSE;
			}
		}
		/*else if(-ent_type == AtomType::prLine)
		{
		}*/
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(-ent_type == AtomType::prCircle)
		{	
			CLDSBolt *pBolt=(CLDSBolt*)Ta.Parts.FromHandle(ID,CLS_BOLT);
			if(pBolt==NULL)
				return FALSE;
			CSuperSmartPtr<CLDSPart>pWorkPart=console.m_pWorkPart;
			if(pWorkPart->GetClassTypeId()==CLS_LINEANGLE)
				pWorkPart.LineAnglePointer()->DeleteLsRef(pBolt->handle);
			else if(pWorkPart->GetClassTypeId()==CLS_PLATE)
				pWorkPart.PlatePointer()->DeleteLsRef(pBolt->handle);
			else if(pWorkPart->GetClassTypeId()==CLS_PARAMPLATE)
				pWorkPart.ParamPlatePointer()->DeleteLsRef(pBolt->handle);
			else if(pWorkPart->GetClassTypeId()==CLS_LINEFLAT)
				pWorkPart.LineFlatPointer()->DeleteLsRef(pBolt->handle);
			else if(pWorkPart->GetClassTypeId()==CLS_LINESLOT)
				pWorkPart.LineSlotPointer()->DeleteLsRef(pBolt->handle);
			//g_pSolidDraw->DelEnt(ID);	//�������Ԫ��ɾ�������еĻص�����,������ͼ��ƽ̨������Զ�ɾ��ͼ��Ԫ��,�˴������������ظ�ɾ������ wjh-2013.10.11
			//�����˨Ϊ�鹹����������ף�ɾ����˨���ú������˨����
			if(pBolt->m_bVirtualPart)
			{	//������˨һ�������һ�������ϣ���ɾ����ʱӦͬʱɾ��������˨ wjh 2020.01.03
				Ta.DelBoltAndRelativeBoltRef(pBolt->handle);	//Ta.RemoveRelativeObjs(pBolt);
				return console.DeletePart(pBolt->handle);
			}
		}
#endif
	}
	else
	{
		//ɾ������ʾ�ѽ����˴����ֹ��CLDSView::OnKeyDown�д�δ��ʾ����ɾ����wjh 2013-1-19
		CLDSView* pView=((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
		pView->NotifyObjStateModified((long*)&ID);	//NotifyObjStateModified����ͬʱɾ��CLDSView::selectObjs��m_setSnapObjects wjh-2016.6.15
		if(ent_type==CLS_NODE)
		{
			pNode = Ta.Node.FromHandle(ID);
			if(pNode==NULL)
				return FALSE;
			else
			{
				if(pNode->IsBlockRefPart())
				{
					AfxSetResourceHandle(theApp.m_hResHandle);
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("The current block assembled node cannot be deleted individually!");
#else
					AfxMessageBox("��ǰ�ڵ����ڲ������ýڵ㲻�ܵ���ɾ����");
#endif
					AfxSetResourceHandle(hOldResHandle);
					return FALSE;
				}
				if(pNode->IsMotherObject())
				{
					AfxSetResourceHandle(theApp.m_hResHandle);
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox(CXhChar50("Existing the mapping node that made current node 0X%X as female parent node,the current node cannot be deleted!",pNode->handle));
#else
					AfxMessageBox(CXhChar50("�����Ե�ǰ�ڵ�0X%XΪĸ���ڵ��Ӱ��ڵ㣬����ɾ����ǰ������",pNode->handle));
#endif
					AfxSetResourceHandle(hOldResHandle);
					return FALSE;
				}
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pNode->dwPermission))
				{
					AfxSetResourceHandle(theApp.m_hResHandle);
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("No corresponding deletion permission!");
#else
					AfxMessageBox("û����Ӧ��ɾ��Ȩ�ޣ�");
#endif
					AfxSetResourceHandle(hOldResHandle);
					return FALSE;
				}
				Ta.RemoveRelativeObjs(pNode);
				if(console.DeleteNode(ID))
					return TRUE;//console.DispNodeSet.DeleteNode(ID);
				else
					return FALSE;
			}
		}
		else if(ID<0)
		{
			XHOBJ_HANDLER handler;
			g_pSolidDraw->GetSolidObject(ID,&handler);
			if(handler.ciXhObjType==PROP_OBJ::TYPE_HOLEWALL)
			{	//ciObjType=PROP_OBJ::TYPE_HOLEWALL;
				long hBolt = handler.hiberid.masterId;
				long hResidePart=handler.hBoltResidePart;
				CLDSPart* pResidePart=console.FromPartHandle(hResidePart);
				if (pResidePart)
				{
					pResidePart->DeleteLsRef(hBolt);
					console.hashHoles.DeleteNode(DUALKEY(hBolt, hResidePart));
					pResidePart->Create3dSolidModel();
					g_pSolidDraw->NewSolidPart(pResidePart->GetSolidPartObject());
					CLDSBolt* pBolt=(CLDSBolt*)console.FromPartHandle(hBolt,CLS_BOLT);
					if(pBolt&&pBolt->m_bVirtualPart)	//������˨һ�������һ�������ϣ���ɾ����ʱӦͬʱɾ��������˨ wjh 2020.01.03
						Ta.DelBoltAndRelativeBoltRef(pBolt->handle);	//Ta.RemoveRelativeObjs(pBolt);
					return TRUE;
				}
			}
			else if(handler.ciXhObjType==PROP_OBJ::TYPE_WIREPOINTARROW)
			{	//�ҵ��ͷ����
				return gxWirePointModel.m_hashWirePoints.DeleteNode(handler.hiberid.masterId);
			}
		}
		else
		{
			CLDSPart *pPart=Ta.Parts.FromHandle(ID);
			g_pSolidDraw->DelEnt(ID);
			if(pPart!=NULL)
			{
				if(pPart->IsBlockRefPart())
				{
					AfxSetResourceHandle(theApp.m_hResHandle);
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("The current block assembled part cannot be deleted individually!");
#else
					AfxMessageBox("��ǰ�������ڲ������ù������ܵ���ɾ����");
#endif
					AfxSetResourceHandle(hOldResHandle);
					return FALSE;
				}
				if(pPart->IsMotherObject())
				{
					AfxSetResourceHandle(theApp.m_hResHandle);
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox(CXhChar50("Existing the mapping part that made current node 0X%X as female parent part ��the current part cannot be deleted!",pPart->handle));
#else
					AfxMessageBox(CXhChar50("�����Ե�ǰ����0X%XΪĸ��������Ӱ�乹��������ɾ����ǰ������",pPart->handle));
#endif
					AfxSetResourceHandle(hOldResHandle);
					return FALSE;
				}
				DWORD dwVerifyPermission=pPart->dwPermission;
				if(pPart->IsLinePart())
				{
					dwVerifyPermission&=((CLDSLinePart*)pPart)->dwStartPermission;
					dwVerifyPermission&=((CLDSLinePart*)pPart)->dwEndPermission;
				}
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(dwVerifyPermission))
				{
					AfxSetResourceHandle(theApp.m_hResHandle);
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("No corresponding deletion permission!");
#else
					AfxMessageBox("û����Ӧ��ɾ��Ȩ�ޣ�");
#endif
					AfxSetResourceHandle(hOldResHandle);
					return FALSE;
				}
				Ta.RemoveRelativeObjs(pPart);
				if(console.DeletePart(ID))
					return TRUE;	//console.DispPartSet.DeleteNode(ID);
				else
					return FALSE;
			}
			CLDSFoundation *pFoundation=Ta.Foundations.FromHandle(ID);
			if(pFoundation!=NULL)
			{
				for(WORD wi=0;wi<pFoundation->anchors.Count;wi++)
				{
					console.DeletePart(pFoundation->anchors[wi].hAnchorBolt);
					g_pSolidDraw->DelEnt(pFoundation->anchors[wi].hAnchorBolt);
					Ta.Parts.DeleteNode(pFoundation->anchors[wi].hAnchorBolt);
				}
				console.DispFoundationSet.DeleteNode(ID);
				Ta.Foundations.DeleteNode(ID);
			}
		}
	}
	return 1;
}
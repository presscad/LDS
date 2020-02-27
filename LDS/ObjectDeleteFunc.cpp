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
	//-----vvvvvvv-标识函数运行状态为真，即同一时刻只能有一个塔创建函数运行---------
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
			{	//未完成通告系统 WJH--01.06.11
				if(theApp.m_bCooperativeWork&&!theApp.IsHasModifyPerm(pNode->dwPermission))
				{
					AfxSetResourceHandle(theApp.m_hResHandle);
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("No corresponding deletion permission!");
#else
					AfxMessageBox("没有相应的删除权限！");
#endif
					AfxSetResourceHandle(hOldResHandle);
					return FALSE;
				}
				Ta.RemoveRelativeObjs(pNode);
				if(console.DeleteNode(ID))	//此节点位于Node中
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
			//g_pSolidDraw->DelEnt(ID);	//这里仅是元素删除过程中的回调函数,结束后图形平台引擎会自动删除图形元素,此处多余易引起重复删除问题 wjh-2013.10.11
			//如果螺栓为虚构件仅用来打孔，删除螺栓引用后清除螺栓数据
			if(pBolt->m_bVirtualPart)
			{	//虚拟螺栓一般仅存在一个构件上，故删除孔时应同时删除虚拟螺栓 wjh 2020.01.03
				Ta.DelBoltAndRelativeBoltRef(pBolt->handle);	//Ta.RemoveRelativeObjs(pBolt);
				return console.DeletePart(pBolt->handle);
			}
		}
#endif
	}
	else
	{
		//删除掉表示已进行了处理防止在CLDSView::OnKeyDown中错当未显示构件删除　wjh 2013-1-19
		CLDSView* pView=((CMainFrame*)theApp.m_pMainWnd)->GetLDSView();
		pView->NotifyObjStateModified((long*)&ID);	//NotifyObjStateModified可以同时删除CLDSView::selectObjs及m_setSnapObjects wjh-2016.6.15
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
					AfxMessageBox("当前节点属于部件引用节点不能单独删除！");
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
					AfxMessageBox(CXhChar50("存在以当前节点0X%X为母本节点的影射节点，不能删除当前构件！",pNode->handle));
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
					AfxMessageBox("没有相应的删除权限！");
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
					if(pBolt&&pBolt->m_bVirtualPart)	//虚拟螺栓一般仅存在一个构件上，故删除孔时应同时删除虚拟螺栓 wjh 2020.01.03
						Ta.DelBoltAndRelativeBoltRef(pBolt->handle);	//Ta.RemoveRelativeObjs(pBolt);
					return TRUE;
				}
			}
			else if(handler.ciXhObjType==PROP_OBJ::TYPE_WIREPOINTARROW)
			{	//挂点箭头对象
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
					AfxMessageBox("当前构件属于部件引用构件不能单独删除！");
#endif
					AfxSetResourceHandle(hOldResHandle);
					return FALSE;
				}
				if(pPart->IsMotherObject())
				{
					AfxSetResourceHandle(theApp.m_hResHandle);
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox(CXhChar50("Existing the mapping part that made current node 0X%X as female parent part ，the current part cannot be deleted!",pPart->handle));
#else
					AfxMessageBox(CXhChar50("存在以当前构件0X%X为母本构件的影射构件，不能删除当前构件！",pPart->handle));
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
					AfxMessageBox("没有相应的删除权限！");
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
//<LOCALE_TRANSLATE Confirm by hxr/>
#include "stdafx.h"
#include "LDS.h"
#include "LDSDoc.h"
#include "LDSView.h"
#include "Tower.h"
#include "UserDefOper.h"
#include "env_def.h"
#include "PromptDlg.h"
#include "Query.h"
#include "AngleOddDiffCalDlg.h"
#include "PlateLsRefDlg.h"
#include "CmdLineDlg.h"
#include "MainFrm.h"
#include "SearchPartDlg.h"
#include "ElectricTower.h"
#include "XhObjHandler.h"

//ID： 最高位为0表示普通实体对象（对象句柄与图形引擎内部Id相同）；最高为为1,则表示由图形引擎自动分配Id
//ent_type：类型为SUBID16
LRESULT CLDSView::ObjectSnappedProcess(WPARAM ID, LPARAM ent_type)
{
	fAtom *pFeatAtom=(m_curAppType==TA_PART_EDITOR)?g_pSolidSnap->GetCurSnapedObject():NULL;
	//在构建编辑器环境中,捕捉到辅助点(pAtom->atom_type==AtomType::prPoint)时满足ID==0&&ent_type==0
	if(ID==0&&ent_type==0&&pFeatAtom==NULL)
	{	//空点鼠标左键
		if(GetKeyState(VK_SHIFT)&0x8000)
		{
			this->UpdatePropertyPage();	//按鼠标左键同时按shift键
			UpdatePlateProfileDisplayStates();
		}
		return 0;
	}
	CRect rect;
	UINT nRetCode=0;	//现已不需要检测加密锁状态了 wjh-2017.9.18
	CLDSDbObject *pObj = NULL;
	CLDSPart* pPart=NULL;
	CString sCmdPrompt;
	if(m_curAppType==TA_ASSEMBLE_ENV)
	{
		DISPLAY_TYPE eDispType=DISP_LINE;
		g_pSolidSet->GetDisplayType(&eDispType);
		if(m_curTask==TASK_SPEC_WCS_ORIGIN||m_curTask==TASK_SPEC_WCS_AXIS_X)
		{
			GEPOINT location;
			SUBID16 subtypeid=ent_type;
			IDrawing* pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
			if(eDispType==DISP_SOLID&&subtypeid.Group()==1&&pBuddyDrawing)
			{
				IDbEntity* pSelEnt=pBuddyDrawing->GetDbEntity(ID);
				HIBERID hiberid=pSelEnt?pSelEnt->GetHiberId():0;
				pObj=pPart=console.FromPartHandle(hiberid.masterId);
				if(pPart&&pPart->IsPlate())
				{
					if(console.m_pWorkPart!=pPart)
						console.m_curWCS=pPart->ucs;
					console.m_pWorkPart=pPart;
					CLDSGeneralPlate* pPlate=(CLDSGeneralPlate*)pPart;
					if(subtypeid.Id()==IDbEntity::DbPoint||subtypeid.Id()==IDbEntity::DbText)
					{	//拾取图元为钢板轮廓顶点
						PROFILE_VERTEX* pVertex=NULL;
						for(pVertex=pPlate->vertex_list.GetFirst();pVertex;pVertex=pPlate->vertex_list.GetNext())
						{
							if(hiberid.HiberDownId(1)==pVertex->vertex.ID)
							{
								location=pVertex->vertex;
								break;
							}
						}
						if(pVertex==NULL&&pPlate->IsPeakPointPlate())
							location=((CLDSPlate*)pPlate)->top_point;
					}
					else if(subtypeid.Id()==IDbEntity::DbCircle)
					{
						CLDSBolt* pBolt=(CLDSBolt*)Ta.Parts.FromHandle(hiberid.HiberDownId(1),CLS_BOLT);
						if(!pPlate->SpreadLsPos(pBolt,location))
							return 0;
					}
					else
						return 0;
					location.z+=pPlate->GetNormOffset();
					coord_trans(location,pPlate->ucs,TRUE,TRUE);
				}
				else
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Currently does not support the specified feature points on the non-plate parts for the WCS!");
#else
					AfxMessageBox("目前不支持在非钢板构件上指定特征点为坐标系原点或轴向标定点!");
#endif
					return 0;
				}
			}
			else
			{
				pObj = pPart=console.FromPartHandle(ID);
				if(pPart&&pPart->GetClassTypeId()==CLS_BOLT)
				{
					CLDSBolt* pBolt=(CLDSBolt*)pPart;
					if(console.m_pWorkPart==NULL||console.m_pWorkPart->handle!=pBolt->des_base_pos.hPart)
						console.m_pWorkPart=pBolt->BelongModel()->FromPartHandle(pBolt->des_base_pos.hPart);
					if(console.m_pWorkPart==NULL || !console.m_pWorkPart->GetBoltIntersPos(pBolt,location))
						return 0;
					console.m_curWCS.axis_z=pBolt->get_norm();
				}
			}
			if(console.m_pWorkPart&&console.m_pWorkPart->IsPlate())
				console.m_curWCS.axis_z=console.m_pWorkPart->ucs.axis_z;
			if(m_curTask==TASK_SPEC_WCS_ORIGIN)
				console.m_curWCS.origin=location;
			else if(m_curTask==TASK_SPEC_WCS_AXIS_X)
			{
				console.m_curWCS.axis_x=location-console.m_curWCS.origin;
				console.m_curWCS.axis_x-=(console.m_curWCS.axis_x*console.m_curWCS.axis_z)*console.m_curWCS.axis_z;
				normalize(console.m_curWCS.axis_x);
				console.m_curWCS.axis_y=console.m_curWCS.axis_z^console.m_curWCS.axis_x;
				normalize(console.m_curWCS.axis_y);
			}

			long axis_len=200;
			if(console.m_pWorkPart&&console.m_pWorkPart->GetWidth())
				axis_len=min(200,(long)console.m_pWorkPart->GetWidth());
			g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,console.m_curWCS,"Xw","Yw",NULL,NULL,axis_len);
			g_pSolidDraw->Draw();
			return 1;
		}
		else if((long)ID>0)
		{
			m_nObjectSnapedCounts=m_setSnapObjects.GetNodeNum();
			SUBID16 subtypeid=ent_type;
			if(subtypeid.Group()==1)
			{//if(subtypeid.Group()==1)	//TODO:源头来自于图纸空间
				IDrawing* pBuddyDrawing=NULL;
				if(eDispType==DISP_SOLID)
					pBuddyDrawing=g_pSolidSet->GetSolidBuddyDrawing();
				else if(eDispType==DISP_DRAWING)
				{
					I2dDrawing* pDraw=m_pDrawSolid->Drawing2d();
					pBuddyDrawing=pDraw->GetActiveDrawing();
				}
				else
					return 0;
				IDbEntity* pEnt=pBuddyDrawing->GetDbEntity(ID);
				HIBERID hiberid=pEnt!=NULL?pEnt->GetHiberId():0;
				//TODO:存在hiber.masterId=0或pEnd=NULL情况，未具体分析处理 wjh-2017.12.3
				ID=hiberid.masterId;	//主对象（节点或构件）的模型标识
				if(pEnt&&pEnt->IsDimension())
				{	//尺寸标注
					//TODO:左侧属性栏显示尺寸标注　wjh-2017.8.1
					if(pEnt->GetDbEntType()==IDbEntity::DbAlignedDimension)
					{

					}
					if(pEnt->GetDbEntType()==IDbEntity::DbRotatedDimension)
					{

					}
				}
				else
				{
					DWORD dwClassType=hiberid.HiberUpId(1);
					DWORD dwObjectId=hiberid.HiberDownId(1);
					pObj=(CLDSDbObject*)console.FromHandle(ID,TRUE);
					CLDSPart* pPart=pObj&&pObj->IsPart()?(CLDSPart*)pObj:NULL;
					if(pPart==NULL)
						return 0;	//必须置前判定是否为空，否则可能会死机 wjh-2016.6.23
					Ta.GeEnts.Empty();
					selectSubObjs.Empty();
					CLDSGeneralPlate* pWorkPlate=pPart->IsPlate()?(CLDSGeneralPlate*)pPart:NULL;
					if(pWorkPlate&&(dwClassType==1||dwClassType==4))
					{	//钢板顶点
						SUB_OBJ* pSubObj=Ta.GeEnts.Add(0);//Ta.GeEnts.NewUniqueId());NewUniqueId已变为自动分配Id
						pSubObj->hObj=pPart->handle;
						pSubObj->iSubId=dwObjectId;
						pSubObj->ciSubType=IDbEntity::DbPoint;
						g_pSolidDraw->SetEntSnapStatus(pWorkPlate->handle);	//必须设否则可能失去轮廓点显示状态 wjh-2017.12.3
						selectObjs.SetValue(pWorkPlate->handle,pWorkPlate);
						selectSubObjs.append(pSubObj);
						for(PROFILE_VERTEX* pVertex=pWorkPlate->vertex_list.GetFirst();pVertex;pVertex=pWorkPlate->vertex_list.GetNext())
						{
							if(pVertex->vertex.ID==dwObjectId)
							{
								pSubObj->pSubObj=pVertex;
								break;
							}
						}
						UpdatePlateProfileDisplayStates();
						DisplayProfileVertexProperty(TRUE,pWorkPlate);
						/*HIBERID peerHiberId(pWorkPlate->handle,HIBERARCHY(0,0,0,0));
						if(dwClassType==1)
						hiberid.SetHiberarchy(4,0,dwObjectId,0);
						else //if(dwClassType==4)
						hiberid.SetHiberarchy(1,0,dwObjectId,0);
						//根据HiberId查找对应的实体，选中文字(轮廓点)时选中轮廓点(文字)
						IDbEntity *pPeerEnt=pBuddyDrawing->GetDbEntity(peerHiberId);
						if(pPeerEnt)
						pPeerEnt->SetStateFlag(IDbEntity::SELECTED);*/
						return 0;
					}
					else if(pWorkPlate&&dwClassType==2)
					{	//钢板轮廓边
						SUB_OBJ* pSubObj=Ta.GeEnts.Add(0);//Ta.GeEnts.NewUniqueId());NewUniqueId已变为自动分配Id
						pSubObj->hObj=pPart->handle;
						pSubObj->iSubId=dwObjectId;
						pSubObj->ciSubType=IDbEntity::DbLine;	//轮廓边
						g_pSolidDraw->SetEntSnapStatus(pWorkPlate->handle);	//必须设否则可能失去轮廓点显示状态 wjh-2017.12.3
						selectObjs.SetValue(pWorkPlate->handle,pWorkPlate);
						selectSubObjs.append(pSubObj);
						for(PROFILE_VERTEX* pVertex=pWorkPlate->vertex_list.GetFirst();pVertex;pVertex=pWorkPlate->vertex_list.GetNext())
						{
							if(pVertex->vertex.ID==dwObjectId)
							{
								pSubObj->pSubObj=pVertex;
								break;
							}
						}
						UpdatePlateProfileDisplayStates();
						DisplayProfileVertexProperty(TRUE,pWorkPlate);
						return 0;
					}
					else if(pWorkPlate&&dwClassType==3)
					{	//钢板火曲线
						SUB_OBJ* pSubObj=Ta.GeEnts.Add(0);//Ta.GeEnts.NewUniqueId());NewUniqueId已变为自动分配Id
						pSubObj->hObj=pPart->handle;
						pSubObj->iSubId=dwObjectId;
						pSubObj->ciSubType=-IDbEntity::DbLine;	//负号表示火曲线
						selectSubObjs.append(pSubObj);
						for(PROFILE_VERTEX* pVertex=pWorkPlate->vertex_list.GetFirst();pVertex;pVertex=pWorkPlate->vertex_list.GetNext())
						{
							if(pVertex->vertex.ID==dwObjectId)
							{
								pSubObj->pSubObj=pVertex;
								break;
							}
						}
					}
					else if(pWorkPlate&&dwClassType==5)
					{	//钢板螺栓孔
						pObj = pPart=console.FromPartHandle(dwObjectId,CLS_BOLT);
						if(pObj)
							selectObjs.SetValue(pObj->handle,pObj);
						DisplayBoltProperty(TRUE);
						return 0;
					}
				}
			}
			else if(subtypeid.Group()==2)	//自分配Id的实体构件
			{
				XHOBJ_HANDLER handler;
				if(g_pSolidDraw->GetSolidObject(ID,&handler)&&handler.ciXhObjType==PROP_OBJ::TYPE_HOLEWALL)	//通过(虚拟)螺栓穿出的宿主构件孔壁
				{
					pObj=console.FromPartHandle(handler.hiberid.masterId);
				}
			}
			else if(subtypeid.Id()==AtomType::prPoint)
				pObj = console.FromNodeHandle(ID);
			else
			{
				if((pObj = console.FromPartHandle(ID,ent_type))!=NULL)
				{
					if(pObj->GetClassTypeId()==CLS_PLATE||pObj->GetClassTypeId()==CLS_PARAMPLATE)
					{
						CLDSGeneralPlate::m_hashDisplayPlateStates.Add(pObj->handle);
						RedrawPlateProfileDrawing((CLDSGeneralPlate*)pObj);
					}
				}
				else if(pObj==NULL)
					pObj = Ta.BlockRef.FromHandle(ID);
				else if(pObj==NULL)
					pObj = Ta.Foundations.FromHandle(ID);
			}
			if(pObj!=NULL)
			{
				m_setSnapObjects.append(pObj);
				m_nObjectSnapedCounts++;
			}
			else	//经过以上检查后，pObj一定有值并指向一塔构件
			{
				m_nObjectSnapedCounts = 0;
				m_setSnapObjects.Empty();
				return 0;
			}
		}
		else //if((long)ID<0)
		{	//自分配Id的实体构件
			XHOBJ_HANDLER handler;
			g_pSolidDraw->GetSolidObject(ID,&handler);
			if(handler.ciXhObjType==PROP_OBJ::TYPE_HOLEWALL)	//通过(虚拟)螺栓穿出的宿主构件孔壁
			{
				PROP_OBJ* pPropObj=hashSelectObjs.Add(ID);
				//pPropObj->ciObjType=PROP_OBJ::TYPE_HOLEWALL;
				CLDSPart* pSelPart=console.FromPartHandle(handler.hiberid.masterId);
				if(pSelPart!=NULL&&pSelPart->IsBolt())
				{
					pPropObj->pBolt=(CLDSBolt*)pSelPart;
					pPropObj->hResidePart=handler.hiberid.HiberUpId(2);
				}
				else
				{
					pObj=pSelPart;
					if(pObj!=NULL)
					{
						m_setSnapObjects.append(pObj);
						m_nObjectSnapedCounts++;
					}
					else	//经过以上检查后，pObj一定有值并指向一塔构件
					{
						m_nObjectSnapedCounts = 0;
						m_setSnapObjects.Empty();
						return 0;
					}
				}
			}
			else if(handler.ciXhObjType==PROP_OBJ::TYPE_WIREPOINTARROW)
			{	//挂点箭头对象
				PROP_OBJ* pPropObj=hashSelectObjs.Add(ID);
				pPropObj->ciObjType=PROP_OBJ::TYPE_WIREPOINTARROW;
				CXhWirePoint* pWirePoint=gxWirePointModel.m_hashWirePoints.GetValue(handler.hiberid.masterId);
				if(pWirePoint!=NULL)
				{
					pPropObj->obj.subobj.hObj=pWirePoint->Id;
					//pPropObj->hResidePart=handler.hiberid.HiberUpId(2);
					//TODO:继续处理
				}
				else	//经过以上检查后，pObj一定有值并指向一塔构件
				{
					m_nObjectSnapedCounts = 0;
					m_setSnapObjects.Empty();
					return 0;
				}
			}
		}
	}
	else if(m_curAppType==TA_PART_EDITOR)
	{
		pFeatAtom = g_pSolidSnap->GetCurSnapedObject();
		if(pFeatAtom==NULL)
			return 0;
		else
		{
			m_nObjectSnapedCounts++;
			m_snap_feat_atom_arr.Add(pFeatAtom);
		}
	}
	else
		return 0;
#ifdef DOG_CHECK
	if(nRetCode!=0)
		return 0;
#endif
	long n,*id_arr;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	//更新查看构件螺栓引用对话框
	n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n==1)
	{
		CLDSPart *pPart=console.FromPartHandle(id_arr[0]);
		if(pPart&&g_pLsRefDlg->GetSafeHwnd()!=0&&pPart->GetClassTypeId()!=CLS_BOLT)
		{
			if(pPart->GetClassTypeId()==CLS_LINEANGLE)
				g_pLsRefDlg->InitLsRefDlg(pPart,3);
			else
				g_pLsRefDlg->InitLsRefDlg(pPart);
		}
		if(pPart&&g_pSearchPartDlg->GetSafeHwnd()!=0&&pPart->GetClassTypeId()!=CLS_BOLT)
			g_pSearchPartDlg->InitSearchPartDlg();
		int cls_id=0;
		if(pPart)
			cls_id=pPart->GetClassTypeId(); 
		if(pPart&&cls_id==CLS_LINEANGLE&&m_curAppType==TA_ASSEMBLE_ENV&&g_sysPara.display.bPartSketch)
		{	//在建模环境下，当前仅选中一个构件并且需要显示构件轮廓边以及轮廓顶点 wht 10-09-02
		#ifdef __LDS_
			if(!theApp.m_bAnalysisWorkEnv)
		#endif
			{
				/**	以下工作坐标系计算代码是错误的，未来可能也会被显示钢板轮廓边等功能代码取代，wjh-2016.5.04
				console.m_curACS=pPart->ucs;
				console.m_curWCS=pPart->ucs;
				console.m_pWorkPart=pPart;	//记录当前显示的构件
				UCS_STRU object_ucs;
				g_pSolidSet->GetObjectUcs(object_ucs);
				coord_trans(console.m_curWCS.origin,object_ucs,TRUE);
				vector_trans(console.m_curWCS.axis_x,object_ucs,TRUE);
				vector_trans(console.m_curWCS.axis_y,object_ucs,TRUE);
				vector_trans(console.m_curWCS.axis_z,object_ucs,TRUE);
				//g_pSolidSet->SetDispWorkUcs(TRUE);
				g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,console.m_curWCS);
				Invalidate(FALSE);*/
				//绘制构件轮廓边以及轮廓点
				DrawPartLineMap(pPart);
			}
		}
	}
#endif
	//更新属性对话框
	UpdatePropertyPage(); //更新属性栏操作应放在设置装配坐标系之后，否则钢板轮廓点坐标显示不正确 wht 10-09-02
	UpdatePlateProfileDisplayStates();
	//过滤显示普通钢板、参数化钢板及部件对象的装配坐标系
	if (g_sysPara.m_bDispPartACS&&m_curAppType==TA_ASSEMBLE_ENV)
	{
		n=g_pSolidSnap->GetLastSelectEnts(id_arr);
		for (int i=0;i<n;i++)
		{
			CLDSObject *pObj=console.FromHandle(id_arr[i]);
			if(pObj==NULL)
				continue;
			if (pObj->GetClassTypeId()==CLS_PLATE||pObj->GetClassTypeId()==CLS_PARAMPLATE)
			{
				CLDSPart *pPart=(CLDSPart*)pObj;
				g_pSolidDraw->AddCS(pPart->handle,pPart->ucs);
				//g_pSolidSet->SetDispWorkUcs();
			}
			else if (pObj->GetClassTypeId()==CLS_BLOCK)
			{
				CBlockModel *pBlock=(CBlockModel*)pObj;
				g_pSolidDraw->AddCS(id_arr[i],pBlock->GetLCS());
				//g_pSolidSet->SetDispWorkUcs();
			}
			else if (pObj->GetClassTypeId()==CLS_BLOCKREF)
			{
				CBlockReference *pBlockRef=(CBlockReference*)pObj;
				g_pSolidDraw->AddCS(id_arr[i],pBlockRef->GetACS());
				//g_pSolidSet->SetDispWorkUcs();
			}
		}
	}
	//
	//CTower ta;
#ifdef __SMART_DRAW_INC_
	n=g_pSolidSnap->GetLastSelectEnts(id_arr);
	if(n==1)
	{	//选中单个构件的同时更新构件编号列表 wht 11-04-22
		//CLDSPart *pPart=console.FromPartHandle(id_arr[0]);
		CPartNoListDlg *pPartNoListPage = ((CMainFrame*)AfxGetMainWnd())->GetPartNoListPage();
		//if(pPartNoListPage&&pPartNoListPage->IsWindowVisible()&&pPart&&pPart->GetClassTypeId()!=CLS_BOLT)
		//	pPartNoListPage->SelectPartNo((char*)pPart->GetPartNo());
		//由于工作区中选中单个构件如果在编号栏中进行同编号多选的话,会导致工作区反向错误联动 故改为单选 wjh-2015.5.27
		if(pPartNoListPage&&pPartNoListPage->IsWindowVisible())
			pPartNoListPage->SelectPart(id_arr[0]);
	}
#endif
	CCmdLineDlg *pCmdLine = ((CMainFrame*)AfxGetMainWnd())->GetCmdLinePage();
	switch(m_curTask)
	{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	case TASK_CAL_PART_PLANK_HUOQU_POS:
#ifdef DOG_CHECK
		if(nRetCode!=0)
			break;
#endif
		if(FinishReCalHuoQuPos(pFeatAtom))
			g_pSolidDraw->BuildDisplayList(this);
		m_nObjectSnapedCounts=0;
		m_snap_feat_atom_arr.RemoveAll();
		g_pSolidDraw->ReleaseSnapStatus();
		break;
	case TASK_VIEW_PART_FEATURE:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		FinishViewPartFeature(pFeatAtom);
		m_nObjectSnapedCounts=0;
		m_snap_feat_atom_arr.RemoveAll();
		g_pSolidDraw->ReleaseSnapStatus();
		break;
	case TASK_ROLL_PLANK_EDGE:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		FinishRollPlankEdge(pFeatAtom);
		m_nObjectSnapedCounts=0;
		m_snap_feat_atom_arr.RemoveAll();
		g_pSolidDraw->ReleaseSnapStatus();
		break;
	case TASK_SPEC_WCS_ORIGIN:
	case TASK_SPEC_WCS_AXIS_X:
	case TASK_SPEC_WCS_AXIS_Y:
#ifdef DOG_CHECK
		if(nRetCode!=0)
			break;
#endif
		FinishSpecWCS(pFeatAtom);
		m_nObjectSnapedCounts=0;
		m_snap_feat_atom_arr.RemoveAll();
		g_pSolidDraw->ReleaseSnapStatus();
		break;
	case TASK_MEASURE_VERTEX_DIST:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		if(m_nObjectSnapedCounts==2)
		{
			FinishMeasureVertexDist(m_snap_feat_atom_arr[0],m_snap_feat_atom_arr[1]);
			m_nObjectSnapedCounts=0;
			m_snap_feat_atom_arr.RemoveAll();
		}
		break;
	case TASK_BEND_PLANK:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		if(m_nObjectSnapedCounts==2)
		{
			FinishBendPlank(m_snap_feat_atom_arr[0],m_snap_feat_atom_arr[1]);
			g_pSolidDraw->ReleaseSnapStatus();
			Invalidate(FALSE);
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
			m_snap_feat_atom_arr.RemoveAll();
		}
		break;
	case TASK_INSERT_PLANK_VERTEX:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		FinishInsertPlankVertex(pFeatAtom);
		m_nObjectSnapedCounts=0;
		m_snap_feat_atom_arr.RemoveAll();
		g_pSolidDraw->ReleaseSnapStatus();
		break;
	case TASK_DEL_PART_FEATURE:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		if(FinishDelPartFeature(pFeatAtom))
			g_pSolidDraw->BuildDisplayList(this);
		m_nObjectSnapedCounts=0;
		m_snap_feat_atom_arr.RemoveAll();
		g_pSolidDraw->ReleaseSnapStatus();
		break;
	case TASK_CAL_CUT_JG:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		if(m_nObjectSnapedCounts==2)
		{
			if(!FinishCalCutJg(m_setSnapObjects[0],m_setSnapObjects[1]))
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("angle cut and leg cut calculation failure");
			else
				MessageBox("angle cut and leg cut calculation succeed");
#else
				AfxMessageBox("切角切肢计算失败");
			else
				MessageBox("切角切肢计算成功");
#endif

			g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[0]->handle,FALSE);
			g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[1]->handle,FALSE);
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("Select other cut ray angle!");
		}
		else if(m_nObjectSnapedCounts==1)
			g_pPromptMsg->SetMsg("select one datum angle to be used to cut ray angle!");
#else
			g_pPromptMsg->SetMsg("选择其它被切割的射线角钢！");
		}
		else if(m_nObjectSnapedCounts==1)
			g_pPromptMsg->SetMsg("选择一根基准角钢用以切割射线角钢！");
#endif
		break;
	/*case TASK_LAY_JG_END_LS:
		if(nRetCode!=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("error in checking hardware lock, error in program!");
#else
			AfxMessageBox("检测加密狗出错,程序出错!");
#endif
			break;
		}
		if(m_nObjectSnapedCounts==2)
		{
			if(!FinishLayJgEndLs(m_setSnapObjects[0],
									m_setSnapObjects[1]))
				AfxMessageBox("角钢端螺栓布置失败");

			m_nObjectSnapedCounts=0;
			g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[0]->handle,FALSE);
			g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[1]->handle,FALSE);
			m_setSnapObjects.Empty();
		}
		break;*/
	case TASK_DESIGN_JDB:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("error in checking hardware lock, error in programme!");
#else
			AfxMessageBox("检测加密狗出错,程序出错!");
#endif
			break;
		}
#endif
		if(pObj->GetClassTypeId()==CLS_NODE)
		{
			CCmdLockObject cmdlock(this);
			if(!cmdlock.LockSuccessed())
				return FALSE;
			FinishDesignJdb((CLDSNode*)pObj);
		}
		m_nObjectSnapedCounts=0;
		m_setSnapObjects.Empty();
		if(g_pPromptMsg->GetSafeHwnd()!=0)
			g_pPromptMsg->Destroy();
		g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_POINT);
		m_curTask = TASK_DESIGN_JDB;
		break;
	case TASK_ADD_FILL_PLANK:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("error in checking hardware lock, error in program!");
#else
			AfxMessageBox("检测加密狗出错,程序出错!");
#endif
			break;
		}
#endif
		if(m_nObjectSnapedCounts==1&&pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			FinishAddFillPlank(pObj);
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
			if(g_pPromptMsg->GetSafeHwnd()!=0)
				g_pPromptMsg->Destroy();
		}
		else if(m_nObjectSnapedCounts==1&&pObj->GetClassTypeId()==CLS_NODE)
		{
			g_pSolidSnap->SetSnapType(SNAP_LINE);
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("please select combined angels which need to add filler plate");
#else
			g_pPromptMsg->SetMsg("请选择需要添加填板的组合角钢。");
#endif
		}
		else if(m_nObjectSnapedCounts==2)
		{
			g_pSolidSnap->SetSnapType(SNAP_POINT);
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("please select the other end nodes in the filler plate area of combined angles");
#else
			g_pPromptMsg->SetMsg("请选择组合角钢上填板区间的另一端节点。");
#endif
		}
		else if(m_nObjectSnapedCounts==3)
		{
			FinishAddFillPlank(m_setSnapObjects[0],m_setSnapObjects[1],m_setSnapObjects[2]);
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
			if(g_pPromptMsg->GetSafeHwnd()!=0)
				g_pPromptMsg->Destroy();
			g_pSolidSnap->SetSnapType(SNAP_ALL);
		}
		break;
	case TASK_CAL_ANGLE_FACEOFFSET_ODD_DIFF:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("error in checking hardware lock, error in programme!");
#else
			AfxMessageBox("检测加密狗出错,程序出错!");
#endif
			break;
		}
#endif
		FinishCalAngleFaceOffsetOddDiff(pObj);
		g_pSolidDraw->SetEntSnapStatus(pObj->handle);
		m_nObjectSnapedCounts=0;
		m_setSnapObjects.Empty();
		break;
	case TASK_DESIGN_SHANK_JOINT:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("error in checking hardware lock, error in programme!");
#else
			AfxMessageBox("检测加密狗出错,程序出错!");
#endif
			break;
		}
#endif
		if(m_nObjectSnapedCounts==1)
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("please select two datum angles in the nodes");
#else
			g_pPromptMsg->SetMsg("请选择该节点处的两根基准角钢");
#endif
		if(m_nObjectSnapedCounts==3)
		{
			FinishDesignJoint(m_setSnapObjects[0],m_setSnapObjects[1],m_setSnapObjects[2]);
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
			if(g_pPromptMsg->GetSafeHwnd())
				g_pPromptMsg->Destroy();
			g_pSolidSet->SetOperType(OPER_OSNAP);
			g_pSolidSnap->SetSnapType(SNAP_POINT);
			m_curTask = TASK_DESIGN_SHANK_JOINT;
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("please select the nodes to be designed joints ");
#else
			g_pPromptMsg->SetMsg("请选择要设计接头的节点");
#endif
			break;
		}
		g_pSolidSnap->SetSnapType(SNAP_LINE);
		break;
	/*case TASK_DESIGN_SINGLE_INTERS:
		if(nRetCode!=0)
		{
			AfxMessageBox("检测加密狗出错,程序出错!");
			break;
		}
		FinishDesign1IntersNode(pObj);
		m_nObjectSnapedCounts=0;
		m_setSnapObjects.Empty();
		if(g_pPromptMsg->GetSafeHwnd()!=0)
			g_pPromptMsg->Destroy();
		g_pSolidSnap->SetSnapType(SNAP_POINT);
		break;
	case TASK_DESIGN_SINGLE_XIE_INTERS:
		if(nRetCode!=0)
		{
			AfxMessageBox("检测加密狗出错,程序出错!");
			break;
		}
		if(m_nObjectSnapedCounts==1)
			g_pSolidSnap->SetSnapType(SNAP_LINE);
		else if(m_nObjectSnapedCounts==2)
		{
			if(	m_setSnapObjects[0]->GetClassTypeId()==CLS_NODE
				&&m_setSnapObjects[1]->GetClassTypeId()==CLS_LINEANGLE)
				DesignBoltOnlyConnect(m_setSnapObjects[0],m_setSnapObjects[1]);
			else
				AfxMessageBox("选中了不适宜的塔构件,请重新选择");
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
			g_pSolidSet->SetOperType(OPER_OSNAP);
			g_pSolidSnap->SetSnapType(SNAP_POINT);
		}
		break;
	case TASK_DEF_AXIS_VAL_FIX_NODE:	//定义轴坐标值不变点
		if(nRetCode!=0)
		{
			AfxMessageBox("检测加密狗出错,程序出错!");
			break;;
		}
		if(!FinishDefAxisValFixNode(pObj))
		{
			g_pSolidDraw->SetEntSnapStatus(ID,FALSE);
			Invalidate(FALSE);
		}
		m_nObjectSnapedCounts=0;
		m_setSnapObjects.Empty();
		
		break;*/
	case TASK_DEF_INTERS_NODE:	//定义普通交叉点
		nRetCode=DefIntersNode();
		if(nRetCode==-1)
			break;		//还不具备执行命令的条件
		if(nRetCode==0)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Data input is not the data needed by the current command, command failure！");
		else if(nRetCode==-2)
			AfxMessageBox(" rod's starting node and ending node cannot be the same one！");
#else
			AfxMessageBox("输入数据不是当前命令需要的数据,命令执行失败！");
		else if(nRetCode==-2)
			AfxMessageBox("杆件始末端节点不能是同一节点！");
#endif
		//else	交叉点定义成功
		m_setSnapObjects.Empty();
		m_nObjectSnapedCounts=0;
		break;
	case TASK_CAL_1JG_NORM:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		if(m_nObjectSnapedCounts==2)
		{
			if(!FinishCal1JgNorm(m_setSnapObjects[0],
									m_setSnapObjects[1]))
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("failure in measuring normal direction");
#else
				AfxMessageBox("法线方向计算失败");
#endif

			m_nObjectSnapedCounts=0;
			g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[0]->handle,FALSE);
			g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[1]->handle,FALSE);
			m_setSnapObjects.Empty();
		}
		break;
	case TASK_CAL_2POLE_INTERS_PT:
#ifdef DOG_CHECK
		if(nRetCode!=0)
			break;
#endif
		if(m_nObjectSnapedCounts==2)
		{
			FinishCal2PoleIntersPt(m_setSnapObjects[0],m_setSnapObjects[1]);
			m_nObjectSnapedCounts=0;
			g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[0]->handle,FALSE);
			g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[1]->handle,FALSE);
			m_setSnapObjects.Empty();
		}
		break;
	case TASK_DESIGN_FOOT_PLANK:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		if(m_nObjectSnapedCounts==2)
		{
			FinishFootPlank(m_setSnapObjects[0],m_setSnapObjects[1]);
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
		}
		break;
	case TASK_DESIGN_HOOF_PLANK:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		if(m_nObjectSnapedCounts==3)
		{
			FinishHoofPlank(m_setSnapObjects[0],m_setSnapObjects[1],m_setSnapObjects[2]);
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
		}
		break;
	case TASK_VIEW_KEY_POINT:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		if(m_nObjectSnapedCounts==1)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("Please select node for key points");
#else
			g_pPromptMsg->SetMsg("请选择关键点所对应的节点");
#endif
			g_pSolidSnap->SetSnapType(SNAP_POINT);
		}
		else if(m_nObjectSnapedCounts==2)
		{
			FinishViewKeyPoint(m_setSnapObjects[0],m_setSnapObjects[1]);
			g_pSolidDraw->ReleaseSnapStatus(); ;
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
			g_pSolidSnap->SetSnapType(SNAP_LINE);
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("please select the angle and nodes for the key points to be checked");
#else
			g_pPromptMsg->SetMsg("请依次选择待查看关键点所在的角钢及对应节点");
#endif
		}
		break;
	case TASK_TURN_ANGLE_CLOCKWISE:
	case TASK_TURN_ANGLE_ANTICLOCKWISE:
#ifdef DOG_CHECK
		if (nRetCode != 0)
			break;
#endif
		if (m_nObjectSnapedCounts == 1 && pObj->GetClassTypeId() == CLS_LINEANGLE || pObj->GetClassTypeId() == CLS_GROUPLINEANGLE)
		{
			if (m_curTask == TASK_TURN_ANGLE_CLOCKWISE)
				FinishTurnAngleClockwise((CLDSLineAngle*)pObj);
			else if (m_curTask == TASK_TURN_ANGLE_ANTICLOCKWISE)
				FinishTurnAngleAnticlockwise((CLDSLineAngle*)pObj);
		}
		m_nObjectSnapedCounts = 0;
		m_setSnapObjects.Empty();
		m_nObjectSnapedCounts = 0;
		m_setSnapObjects.Empty();
		if (g_pPromptMsg->GetSafeHwnd())
			g_pPromptMsg->Destroy();
		g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_LINE);
		g_pSolidDraw->ReleaseSnapStatus();
		//m_curTask = TASK_TURN_ANGLE_CLOCKWISE/TASK_TURN_ANGLE_ANTICLOCKWISE;
		pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
		if (m_curTask == TASK_TURN_ANGLE_CLOCKWISE)
			sCmdPrompt = "please select clockwise-turn angle:";
		else //if(m_curTask == TASK_TURN_ANGLE_ANTICLOCKWISE)
			sCmdPrompt = "please select anticlockwise-turn angle:";
#else
		if (m_curTask == TASK_TURN_ANGLE_CLOCKWISE)
			sCmdPrompt = "请选择需要进行顺时针翻转的角钢:";
		else //if(m_curTask == TASK_TURN_ANGLE_ANTICLOCKWISE)
			sCmdPrompt = "请选择需要进行逆时针翻转的角钢:";
#endif
		pCmdLine->FillCmdLine(sCmdPrompt, "");
		break;
	case TASK_TURN_ANGLE_INOUT_SIDE:
#ifdef DOG_CHECK
		if(nRetCode!=0)
			break;
#endif
		if(m_nObjectSnapedCounts==1&&pObj->GetClassTypeId()==CLS_LINEANGLE||pObj->GetClassTypeId()==CLS_GROUPLINEANGLE)
			FinishTurnAngle((CLDSLineAngle*)pObj,0);
		m_nObjectSnapedCounts=0;
		m_setSnapObjects.Empty();
		m_nObjectSnapedCounts=0;
		m_setSnapObjects.Empty();
		if(g_pPromptMsg->GetSafeHwnd())
			g_pPromptMsg->Destroy();
		g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_LINE);
		g_pSolidDraw->ReleaseSnapStatus();
		m_curTask = TASK_TURN_ANGLE_INOUT_SIDE;
		pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("please select overturn angle:","");
#else
		pCmdLine->FillCmdLine("请选择需要进行里外铁翻转的角钢:","");
#endif
		break;
	case TASK_TURN_ANGLE_WING:
#ifdef DOG_CHECK
		if(nRetCode!=0)
			break;
#endif
		if(m_nObjectSnapedCounts==1&&(pObj->GetClassTypeId()==CLS_LINEANGLE||pObj->GetClassTypeId()==CLS_GROUPLINEANGLE))
			FinishTurnAngle((CLDSLineAngle*)pObj,1);
		m_nObjectSnapedCounts=0;
		m_setSnapObjects.Empty();
		m_nObjectSnapedCounts=0;
		m_setSnapObjects.Empty();
		if(g_pPromptMsg->GetSafeHwnd())
			g_pPromptMsg->Destroy();
		g_pSolidSet->SetOperType(OPER_OSNAP);
		g_pSolidSnap->SetSnapType(SNAP_LINE);
		g_pSolidDraw->ReleaseSnapStatus();
		m_curTask = TASK_TURN_ANGLE_WING;
		pCmdLine->FinishCmdLine();
#ifdef AFX_TARG_ENU_ENGLISH
		pCmdLine->FillCmdLine("please select left and right wing overturn angle:","");
#else
		pCmdLine->FillCmdLine("请选择需要进行里左右肢翻转的角钢:","");
#endif
#endif	
		break;
	case TASK_LINK_LINEPART:	//连接塔材
		if(ID==0)	//防止第二次点击选空，第三次再选第一点时，提示重复选中同一节点提示框 wjh-2015.8.26
			m_setSnapObjects.Empty();
		nRetCode=FinishLinkPart();
		//if(nRetCode==0)	
		//	AfxMessageBox("命令撤销！");
		if(nRetCode==-1)
			break;		//还不具备执行命令的条件
		else if(nRetCode==-2)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("rod's starting node and ending node cannot be the same one!");
		else if(int(nRetCode)<-2)
			AfxMessageBox("Data input is not the data needed by the current command, command failure!");
#else
			AfxMessageBox("杆件始末端节点不能是同一节点！");
		else if(int(nRetCode)<-2)
			AfxMessageBox("输入数据不是当前命令需要的数据,命令执行失败！");
#endif
		m_setSnapObjects.Empty();
		break;
	case TASK_MERGE_NODES:	//合并节点
		nRetCode=MergeNodes();
		//if(nRetCode==0)	
		//	AfxMessageBox("命令撤销！");
		if(nRetCode==-1)
			break;		//还不具备执行命令的条件
		else if(nRetCode==-2)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("two nodes to be combined cannot be the same node！");
		else if(int(nRetCode)<-2)
			AfxMessageBox("Data input is not the data needed by the current command, command failure！");
#else
			AfxMessageBox("待合节的两个节点不能是同一节点！");
		else if(int(nRetCode)<-2)
			AfxMessageBox("输入数据不是当前命令需要的数据,命令执行失败！");
#endif
		m_setSnapObjects.Empty();
		break;
	case TASK_DEF_OFFSET_NODE:	//定义偏移点
		/*
		if(nRetCode!=0)
			break;
		
		if(m_nObjectSnapedCounts==2)
		{
			g_pPromptMsg->SetMsg("红色标记表示偏移点沿基准角钢上偏移方向(起始端方向)");
			if(!FinishDefOffsetNode(m_setSnapObjects[0],m_setSnapObjects[1]))
			{
				g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[0]->handle,FALSE);
				g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[1]->handle,FALSE);
				Invalidate(FALSE);
			}
			g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[0]->handle,FALSE);
			g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[1]->handle,FALSE);
			g_pSolidDraw->DestroyAllMark();
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
		}
		
		break;
	case TASK_DEF_HORIZON_NODE:	//定义等高点
		
		if(nRetCode!=0)
			break;
		if(m_nObjectSnapedCounts==2)
		{
			if(!FinishDefHorizonNode(m_setSnapObjects[0],m_setSnapObjects[1]))
			{
				g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[0]->handle,FALSE);
				g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[1]->handle,FALSE);
				Invalidate(FALSE);
			}
			g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[0]->handle,FALSE);
			g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[1]->handle,FALSE);
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
		}
		*/
		break;
	case TASK_MEASURE_NODE_DIST:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		if(m_nObjectSnapedCounts==1&&pObj->GetClassTypeId()!=CLS_NODE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("please select the nodes for key points on angle");
#else
			g_pPromptMsg->SetMsg("请选择角钢上关键点所在的节点");
#endif
			g_pSolidSnap->SetSnapType(SNAP_POINT);
		}
		else if(m_nObjectSnapedCounts==1&&pObj->GetClassTypeId()==CLS_NODE)
		{
			g_pSolidSnap->SetSnapType(SNAP_ALL);
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("please select angle and nodes for the other key point");
#else
			g_pPromptMsg->SetMsg("请选择另一关键点所在的角钢或相应节点");
#endif
		}
		else if(m_nObjectSnapedCounts==2&&pObj->GetClassTypeId()==CLS_NODE)
		{
			if(m_setSnapObjects[0]->GetClassTypeId()==CLS_NODE)
			{
				FinishMeasure(m_setSnapObjects[0],m_setSnapObjects[1]);
				g_pSolidDraw->ReleaseSnapStatus();
				Invalidate(FALSE);
				m_nObjectSnapedCounts=0;
				m_setSnapObjects.Empty();
				g_pSolidSnap->SetSnapType(SNAP_ALL);
			}
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				g_pPromptMsg->SetMsg("please select angle and nodes for the other key point");
#else
				g_pPromptMsg->SetMsg("请选择另一关键点所在的角钢或相应节点");
#endif
				g_pSolidSnap->SetSnapType(SNAP_ALL);
			}
		}
		else if(m_nObjectSnapedCounts==2&&pObj->GetClassTypeId()==CLS_LINEANGLE)
		{

#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("please select nodes for the key points on angle");
#else
			g_pPromptMsg->SetMsg("请选择角钢上关键点所在的节点");
#endif
			g_pSolidSnap->SetSnapType(SNAP_POINT);
		}
		else if(m_nObjectSnapedCounts==3&&pObj->GetClassTypeId()==CLS_LINEANGLE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("please select nodes for the key points on angle");
#else
			g_pPromptMsg->SetMsg("请选择角钢上关键点所在的节点");
#endif
			g_pSolidSnap->SetSnapType(SNAP_POINT);
		}
		else if(m_nObjectSnapedCounts==3&&pObj->GetClassTypeId()==CLS_NODE)
		{
			FinishMeasure(m_setSnapObjects[0],m_setSnapObjects[1],m_setSnapObjects[2]);
			g_pSolidDraw->ReleaseSnapStatus();
			Invalidate(FALSE);
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
			g_pSolidSnap->SetSnapType(SNAP_ALL);
		}
		else if(m_nObjectSnapedCounts==4)
		{
			FinishMeasure(m_setSnapObjects[0],m_setSnapObjects[1],m_setSnapObjects[2],m_setSnapObjects[3]);
			g_pSolidDraw->ReleaseSnapStatus();
			Invalidate(FALSE);
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
			g_pSolidSnap->SetSnapType(SNAP_ALL);
		}
		break;
	case TASK_SPREAD_FACE:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		nRetCode=FinishSpreadFace();
		if(nRetCode==-1)
			break;		//还不具备执行命令的条件
		else if((int)nRetCode<-1)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Data input is not the data needed by the current command, command failure!");
#else
			AfxMessageBox("输入数据不是当前命令需要的数据,命令执行失败！");
#endif
		m_nObjectSnapedCounts=0;
		m_setSnapObjects.Empty();
		break;
	case TASK_SPREAD_CONTINUOUS_FACE:
#ifdef DOG_CHECK
		if(nRetCode!=0)
			break;
#endif
		/*if(pObj!=NULL&&ent_type==AtomType::prLine)
			FinishSpreadContinuousFace(pObj);
		else if(pObj!=NULL)
			g_pSolidDraw->ReleaseSnapStatus();*/
		m_nObjectSnapedCounts=0;
		m_setSnapObjects.Empty();
		break;
	case TASK_CAL_JG_EXTEND_VEC:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		if(pObj!=NULL)
			FinishCalLinePartExtendVec(pObj);
		m_nObjectSnapedCounts=0;
		m_setSnapObjects.Empty();
		break;
	case TASK_CAL_WORK_FACE_NORM:
#ifdef DOG_CHECK	
		if(nRetCode!=0)
			break;
#endif
		if(m_nObjectSnapedCounts==2)
		{
			FinishCalWorkFaceNorm(m_setSnapObjects[0],m_setSnapObjects[1]);
			g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[0]->handle,FALSE);
			g_pSolidDraw->SetEntSnapStatus(m_setSnapObjects[1]->handle,FALSE);
			Invalidate(FALSE);
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
		}
		break;
	case TASK_DEF_DIVIDE_NODE:
		nRetCode=FinishDefDivideScaleNode();
		if(nRetCode==0)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("command failure!");
#else
			AfxMessageBox("命令执行失败！");
#endif
		else if(nRetCode==-1)
			break;		//还不具备执行命令的条件
		else if(int(nRetCode)<-1)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Data input is not the data needed by the current command, command failure！");
#else
			AfxMessageBox("输入数据不是当前命令需要的数据,命令执行失败！");
#endif
		m_setSnapObjects.Empty();
		break;
	case TASK_OPEN_WND_SEL:
		OnOperOther();
		break;
	default:
		break;
	}
	return 0;
}
LRESULT CLDSView::ObjectSelectProcess(WPARAM nSelect, LPARAM other)
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	union SCRPOINT_FLAG{
		DWORD dwFlag;
		struct{ short x,y;};
	};
	f3dPoint face_vertex[4];
	SCRPOINT_FLAG startPoint,endPoint;
	UCS_STRU object_ucs;
	g_pSolidSet->GetObjectUcs(object_ucs);
	switch(m_curTask)
	{
	case TASK_DEF_POLY_STYLE:
		startPoint.dwFlag=nSelect;
		endPoint.dwFlag=other;
		face_vertex[0].Set(startPoint.x,startPoint.y,0);
		face_vertex[1].Set(startPoint.x,startPoint.y,1);
		face_vertex[2].Set(endPoint.x,endPoint.y,1);
		face_vertex[3].Set(endPoint.x,endPoint.y,0);
		for(int i=0;i<4;i++)
		{
			g_pSolidOper->ScreenToUser(&face_vertex[i].x,&face_vertex[i].y,&face_vertex[i].z,
									    face_vertex[i].x,face_vertex[i].y,face_vertex[i].z);
			coord_trans(face_vertex[i],object_ucs,FALSE);
		}
		FinishDefPoly(face_vertex,4);
		g_pSolidDraw->ReleaseSnapStatus();
		m_pDoc->SetModifiedFlag();
		break;
	case TASK_OPEN_WND_SEL:
		UpdatePropertyPage();	//更新属性对话框
		//温州泰昌反馈说从TMA V4.0转换过来，使用习惯不适应，应保持原有开窗操作状态 wjh-2015.11.26
		//OnOperOther();
		break;
	default:
		UpdatePropertyPage();	//更新属性对话框
		break;
	}
#else
	UpdatePropertyPage();	//更新属性对话框
#endif
	return 0;
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
void CLDSView::FinishCalAngleFaceOffsetOddDiff(CLDSDbObject *pTowerAtom)
{
	if(pTowerAtom->GetClassTypeId()!=CLS_LINEANGLE)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("error in selecting parts!");
#else
		AfxMessageBox("选择构件错误！");
#endif
		return;
	}
	CLDSLineAngle *pJg = (CLDSLineAngle*)pTowerAtom;
	CAngleOddDiffCalDlg odddiffdlg;
	odddiffdlg.m_pBaseJg = pJg;
	JGZJ jgzj;
	getjgzj(jgzj,pJg->GetWidth());
	odddiffdlg.m_fWingXG = odddiffdlg.m_fWingYG = jgzj.g;
	odddiffdlg.DoModal();
}
#endif
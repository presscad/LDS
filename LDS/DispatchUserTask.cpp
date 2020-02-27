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

//ID�� ���λΪ0��ʾ��ͨʵ����󣨶�������ͼ�������ڲ�Id��ͬ�������ΪΪ1,���ʾ��ͼ�������Զ�����Id
//ent_type������ΪSUBID16
LRESULT CLDSView::ObjectSnappedProcess(WPARAM ID, LPARAM ent_type)
{
	fAtom *pFeatAtom=(m_curAppType==TA_PART_EDITOR)?g_pSolidSnap->GetCurSnapedObject():NULL;
	//�ڹ����༭��������,��׽��������(pAtom->atom_type==AtomType::prPoint)ʱ����ID==0&&ent_type==0
	if(ID==0&&ent_type==0&&pFeatAtom==NULL)
	{	//�յ�������
		if(GetKeyState(VK_SHIFT)&0x8000)
		{
			this->UpdatePropertyPage();	//��������ͬʱ��shift��
			UpdatePlateProfileDisplayStates();
		}
		return 0;
	}
	CRect rect;
	UINT nRetCode=0;	//���Ѳ���Ҫ��������״̬�� wjh-2017.9.18
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
					{	//ʰȡͼԪΪ�ְ���������
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
					AfxMessageBox("Ŀǰ��֧���ڷǸְ幹����ָ��������Ϊ����ϵԭ�������궨��!");
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
			{//if(subtypeid.Group()==1)	//TODO:Դͷ������ͼֽ�ռ�
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
				//TODO:����hiber.masterId=0��pEnd=NULL�����δ����������� wjh-2017.12.3
				ID=hiberid.masterId;	//�����󣨽ڵ�򹹼�����ģ�ͱ�ʶ
				if(pEnt&&pEnt->IsDimension())
				{	//�ߴ��ע
					//TODO:�����������ʾ�ߴ��ע��wjh-2017.8.1
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
						return 0;	//������ǰ�ж��Ƿ�Ϊ�գ�������ܻ����� wjh-2016.6.23
					Ta.GeEnts.Empty();
					selectSubObjs.Empty();
					CLDSGeneralPlate* pWorkPlate=pPart->IsPlate()?(CLDSGeneralPlate*)pPart:NULL;
					if(pWorkPlate&&(dwClassType==1||dwClassType==4))
					{	//�ְ嶥��
						SUB_OBJ* pSubObj=Ta.GeEnts.Add(0);//Ta.GeEnts.NewUniqueId());NewUniqueId�ѱ�Ϊ�Զ�����Id
						pSubObj->hObj=pPart->handle;
						pSubObj->iSubId=dwObjectId;
						pSubObj->ciSubType=IDbEntity::DbPoint;
						g_pSolidDraw->SetEntSnapStatus(pWorkPlate->handle);	//������������ʧȥ��������ʾ״̬ wjh-2017.12.3
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
						//����HiberId���Ҷ�Ӧ��ʵ�壬ѡ������(������)ʱѡ��������(����)
						IDbEntity *pPeerEnt=pBuddyDrawing->GetDbEntity(peerHiberId);
						if(pPeerEnt)
						pPeerEnt->SetStateFlag(IDbEntity::SELECTED);*/
						return 0;
					}
					else if(pWorkPlate&&dwClassType==2)
					{	//�ְ�������
						SUB_OBJ* pSubObj=Ta.GeEnts.Add(0);//Ta.GeEnts.NewUniqueId());NewUniqueId�ѱ�Ϊ�Զ�����Id
						pSubObj->hObj=pPart->handle;
						pSubObj->iSubId=dwObjectId;
						pSubObj->ciSubType=IDbEntity::DbLine;	//������
						g_pSolidDraw->SetEntSnapStatus(pWorkPlate->handle);	//������������ʧȥ��������ʾ״̬ wjh-2017.12.3
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
					{	//�ְ������
						SUB_OBJ* pSubObj=Ta.GeEnts.Add(0);//Ta.GeEnts.NewUniqueId());NewUniqueId�ѱ�Ϊ�Զ�����Id
						pSubObj->hObj=pPart->handle;
						pSubObj->iSubId=dwObjectId;
						pSubObj->ciSubType=-IDbEntity::DbLine;	//���ű�ʾ������
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
					{	//�ְ���˨��
						pObj = pPart=console.FromPartHandle(dwObjectId,CLS_BOLT);
						if(pObj)
							selectObjs.SetValue(pObj->handle,pObj);
						DisplayBoltProperty(TRUE);
						return 0;
					}
				}
			}
			else if(subtypeid.Group()==2)	//�Է���Id��ʵ�幹��
			{
				XHOBJ_HANDLER handler;
				if(g_pSolidDraw->GetSolidObject(ID,&handler)&&handler.ciXhObjType==PROP_OBJ::TYPE_HOLEWALL)	//ͨ��(����)��˨���������������ױ�
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
			else	//�������ϼ���pObjһ����ֵ��ָ��һ������
			{
				m_nObjectSnapedCounts = 0;
				m_setSnapObjects.Empty();
				return 0;
			}
		}
		else //if((long)ID<0)
		{	//�Է���Id��ʵ�幹��
			XHOBJ_HANDLER handler;
			g_pSolidDraw->GetSolidObject(ID,&handler);
			if(handler.ciXhObjType==PROP_OBJ::TYPE_HOLEWALL)	//ͨ��(����)��˨���������������ױ�
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
					else	//�������ϼ���pObjһ����ֵ��ָ��һ������
					{
						m_nObjectSnapedCounts = 0;
						m_setSnapObjects.Empty();
						return 0;
					}
				}
			}
			else if(handler.ciXhObjType==PROP_OBJ::TYPE_WIREPOINTARROW)
			{	//�ҵ��ͷ����
				PROP_OBJ* pPropObj=hashSelectObjs.Add(ID);
				pPropObj->ciObjType=PROP_OBJ::TYPE_WIREPOINTARROW;
				CXhWirePoint* pWirePoint=gxWirePointModel.m_hashWirePoints.GetValue(handler.hiberid.masterId);
				if(pWirePoint!=NULL)
				{
					pPropObj->obj.subobj.hObj=pWirePoint->Id;
					//pPropObj->hResidePart=handler.hiberid.HiberUpId(2);
					//TODO:��������
				}
				else	//�������ϼ���pObjһ����ֵ��ָ��һ������
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
	//���²鿴������˨���öԻ���
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
		{	//�ڽ�ģ�����£���ǰ��ѡ��һ������������Ҫ��ʾ�����������Լ��������� wht 10-09-02
		#ifdef __LDS_
			if(!theApp.m_bAnalysisWorkEnv)
		#endif
			{
				/**	���¹�������ϵ��������Ǵ���ģ�δ������Ҳ�ᱻ��ʾ�ְ������ߵȹ��ܴ���ȡ����wjh-2016.5.04
				console.m_curACS=pPart->ucs;
				console.m_curWCS=pPart->ucs;
				console.m_pWorkPart=pPart;	//��¼��ǰ��ʾ�Ĺ���
				UCS_STRU object_ucs;
				g_pSolidSet->GetObjectUcs(object_ucs);
				coord_trans(console.m_curWCS.origin,object_ucs,TRUE);
				vector_trans(console.m_curWCS.axis_x,object_ucs,TRUE);
				vector_trans(console.m_curWCS.axis_y,object_ucs,TRUE);
				vector_trans(console.m_curWCS.axis_z,object_ucs,TRUE);
				//g_pSolidSet->SetDispWorkUcs(TRUE);
				g_pSolidDraw->AddCS(ISolidDraw::WORK_CS,console.m_curWCS);
				Invalidate(FALSE);*/
				//���ƹ����������Լ�������
				DrawPartLineMap(pPart);
			}
		}
	}
#endif
	//�������ԶԻ���
	UpdatePropertyPage(); //��������������Ӧ��������װ������ϵ֮�󣬷���ְ�������������ʾ����ȷ wht 10-09-02
	UpdatePlateProfileDisplayStates();
	//������ʾ��ͨ�ְ塢�������ְ弰���������װ������ϵ
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
	{	//ѡ�е���������ͬʱ���¹�������б� wht 11-04-22
		//CLDSPart *pPart=console.FromPartHandle(id_arr[0]);
		CPartNoListDlg *pPartNoListPage = ((CMainFrame*)AfxGetMainWnd())->GetPartNoListPage();
		//if(pPartNoListPage&&pPartNoListPage->IsWindowVisible()&&pPart&&pPart->GetClassTypeId()!=CLS_BOLT)
		//	pPartNoListPage->SelectPartNo((char*)pPart->GetPartNo());
		//���ڹ�������ѡ�е�����������ڱ�����н���ͬ��Ŷ�ѡ�Ļ�,�ᵼ�¹���������������� �ʸ�Ϊ��ѡ wjh-2015.5.27
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
				AfxMessageBox("�н���֫����ʧ��");
			else
				MessageBox("�н���֫����ɹ�");
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
			g_pPromptMsg->SetMsg("ѡ���������и�����߽Ǹ֣�");
		}
		else if(m_nObjectSnapedCounts==1)
			g_pPromptMsg->SetMsg("ѡ��һ����׼�Ǹ������и����߽Ǹ֣�");
#endif
		break;
	/*case TASK_LAY_JG_END_LS:
		if(nRetCode!=0)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("error in checking hardware lock, error in program!");
#else
			AfxMessageBox("�����ܹ�����,�������!");
#endif
			break;
		}
		if(m_nObjectSnapedCounts==2)
		{
			if(!FinishLayJgEndLs(m_setSnapObjects[0],
									m_setSnapObjects[1]))
				AfxMessageBox("�Ǹֶ���˨����ʧ��");

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
			AfxMessageBox("�����ܹ�����,�������!");
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
			AfxMessageBox("�����ܹ�����,�������!");
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
			g_pPromptMsg->SetMsg("��ѡ����Ҫ���������ϽǸ֡�");
#endif
		}
		else if(m_nObjectSnapedCounts==2)
		{
			g_pSolidSnap->SetSnapType(SNAP_POINT);
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("please select the other end nodes in the filler plate area of combined angles");
#else
			g_pPromptMsg->SetMsg("��ѡ����ϽǸ�������������һ�˽ڵ㡣");
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
			AfxMessageBox("�����ܹ�����,�������!");
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
			AfxMessageBox("�����ܹ�����,�������!");
#endif
			break;
		}
#endif
		if(m_nObjectSnapedCounts==1)
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("please select two datum angles in the nodes");
#else
			g_pPromptMsg->SetMsg("��ѡ��ýڵ㴦��������׼�Ǹ�");
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
			g_pPromptMsg->SetMsg("��ѡ��Ҫ��ƽ�ͷ�Ľڵ�");
#endif
			break;
		}
		g_pSolidSnap->SetSnapType(SNAP_LINE);
		break;
	/*case TASK_DESIGN_SINGLE_INTERS:
		if(nRetCode!=0)
		{
			AfxMessageBox("�����ܹ�����,�������!");
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
			AfxMessageBox("�����ܹ�����,�������!");
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
				AfxMessageBox("ѡ���˲����˵�������,������ѡ��");
			m_nObjectSnapedCounts=0;
			m_setSnapObjects.Empty();
			g_pSolidSet->SetOperType(OPER_OSNAP);
			g_pSolidSnap->SetSnapType(SNAP_POINT);
		}
		break;
	case TASK_DEF_AXIS_VAL_FIX_NODE:	//����������ֵ�����
		if(nRetCode!=0)
		{
			AfxMessageBox("�����ܹ�����,�������!");
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
	case TASK_DEF_INTERS_NODE:	//������ͨ�����
		nRetCode=DefIntersNode();
		if(nRetCode==-1)
			break;		//�����߱�ִ�����������
		if(nRetCode==0)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Data input is not the data needed by the current command, command failure��");
		else if(nRetCode==-2)
			AfxMessageBox(" rod's starting node and ending node cannot be the same one��");
#else
			AfxMessageBox("�������ݲ��ǵ�ǰ������Ҫ������,����ִ��ʧ�ܣ�");
		else if(nRetCode==-2)
			AfxMessageBox("�˼�ʼĩ�˽ڵ㲻����ͬһ�ڵ㣡");
#endif
		//else	����㶨��ɹ�
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
				AfxMessageBox("���߷������ʧ��");
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
			g_pPromptMsg->SetMsg("��ѡ��ؼ�������Ӧ�Ľڵ�");
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
			g_pPromptMsg->SetMsg("������ѡ����鿴�ؼ������ڵĽǸּ���Ӧ�ڵ�");
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
			sCmdPrompt = "��ѡ����Ҫ����˳ʱ�뷭ת�ĽǸ�:";
		else //if(m_curTask == TASK_TURN_ANGLE_ANTICLOCKWISE)
			sCmdPrompt = "��ѡ����Ҫ������ʱ�뷭ת�ĽǸ�:";
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
		pCmdLine->FillCmdLine("��ѡ����Ҫ������������ת�ĽǸ�:","");
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
		pCmdLine->FillCmdLine("��ѡ����Ҫ����������֫��ת�ĽǸ�:","");
#endif
#endif	
		break;
	case TASK_LINK_LINEPART:	//��������
		if(ID==0)	//��ֹ�ڶ��ε��ѡ�գ���������ѡ��һ��ʱ����ʾ�ظ�ѡ��ͬһ�ڵ���ʾ�� wjh-2015.8.26
			m_setSnapObjects.Empty();
		nRetCode=FinishLinkPart();
		//if(nRetCode==0)	
		//	AfxMessageBox("�������");
		if(nRetCode==-1)
			break;		//�����߱�ִ�����������
		else if(nRetCode==-2)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("rod's starting node and ending node cannot be the same one!");
		else if(int(nRetCode)<-2)
			AfxMessageBox("Data input is not the data needed by the current command, command failure!");
#else
			AfxMessageBox("�˼�ʼĩ�˽ڵ㲻����ͬһ�ڵ㣡");
		else if(int(nRetCode)<-2)
			AfxMessageBox("�������ݲ��ǵ�ǰ������Ҫ������,����ִ��ʧ�ܣ�");
#endif
		m_setSnapObjects.Empty();
		break;
	case TASK_MERGE_NODES:	//�ϲ��ڵ�
		nRetCode=MergeNodes();
		//if(nRetCode==0)	
		//	AfxMessageBox("�������");
		if(nRetCode==-1)
			break;		//�����߱�ִ�����������
		else if(nRetCode==-2)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("two nodes to be combined cannot be the same node��");
		else if(int(nRetCode)<-2)
			AfxMessageBox("Data input is not the data needed by the current command, command failure��");
#else
			AfxMessageBox("���Ͻڵ������ڵ㲻����ͬһ�ڵ㣡");
		else if(int(nRetCode)<-2)
			AfxMessageBox("�������ݲ��ǵ�ǰ������Ҫ������,����ִ��ʧ�ܣ�");
#endif
		m_setSnapObjects.Empty();
		break;
	case TASK_DEF_OFFSET_NODE:	//����ƫ�Ƶ�
		/*
		if(nRetCode!=0)
			break;
		
		if(m_nObjectSnapedCounts==2)
		{
			g_pPromptMsg->SetMsg("��ɫ��Ǳ�ʾƫ�Ƶ��ػ�׼�Ǹ���ƫ�Ʒ���(��ʼ�˷���)");
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
	case TASK_DEF_HORIZON_NODE:	//����ȸߵ�
		
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
			g_pPromptMsg->SetMsg("��ѡ��Ǹ��Ϲؼ������ڵĽڵ�");
#endif
			g_pSolidSnap->SetSnapType(SNAP_POINT);
		}
		else if(m_nObjectSnapedCounts==1&&pObj->GetClassTypeId()==CLS_NODE)
		{
			g_pSolidSnap->SetSnapType(SNAP_ALL);
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("please select angle and nodes for the other key point");
#else
			g_pPromptMsg->SetMsg("��ѡ����һ�ؼ������ڵĽǸֻ���Ӧ�ڵ�");
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
				g_pPromptMsg->SetMsg("��ѡ����һ�ؼ������ڵĽǸֻ���Ӧ�ڵ�");
#endif
				g_pSolidSnap->SetSnapType(SNAP_ALL);
			}
		}
		else if(m_nObjectSnapedCounts==2&&pObj->GetClassTypeId()==CLS_LINEANGLE)
		{

#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("please select nodes for the key points on angle");
#else
			g_pPromptMsg->SetMsg("��ѡ��Ǹ��Ϲؼ������ڵĽڵ�");
#endif
			g_pSolidSnap->SetSnapType(SNAP_POINT);
		}
		else if(m_nObjectSnapedCounts==3&&pObj->GetClassTypeId()==CLS_LINEANGLE)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			g_pPromptMsg->SetMsg("please select nodes for the key points on angle");
#else
			g_pPromptMsg->SetMsg("��ѡ��Ǹ��Ϲؼ������ڵĽڵ�");
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
			break;		//�����߱�ִ�����������
		else if((int)nRetCode<-1)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Data input is not the data needed by the current command, command failure!");
#else
			AfxMessageBox("�������ݲ��ǵ�ǰ������Ҫ������,����ִ��ʧ�ܣ�");
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
			AfxMessageBox("����ִ��ʧ�ܣ�");
#endif
		else if(nRetCode==-1)
			break;		//�����߱�ִ�����������
		else if(int(nRetCode)<-1)
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("Data input is not the data needed by the current command, command failure��");
#else
			AfxMessageBox("�������ݲ��ǵ�ǰ������Ҫ������,����ִ��ʧ�ܣ�");
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
		UpdatePropertyPage();	//�������ԶԻ���
		//����̩������˵��TMA V4.0ת��������ʹ��ϰ�߲���Ӧ��Ӧ����ԭ�п�������״̬ wjh-2015.11.26
		//OnOperOther();
		break;
	default:
		UpdatePropertyPage();	//�������ԶԻ���
		break;
	}
#else
	UpdatePropertyPage();	//�������ԶԻ���
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
		AfxMessageBox("ѡ�񹹼�����");
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
//<LOCALE_TRANSLATE/>
//������ͼ
#include "stdafx.h"
#include "stdarx.h"
#include "Resource.h"
#include "MenuFunc.h"
#include "SelMapSegDlg.h"
#include "Tower.h"
#include "GlobalFunc.h"
#include "sysPara.h"
#include "Draw3dFunc.h"
#include "adsTools.h"
#include "DrawTaMap.h"
#include "FormatTable.h"
#include "DragEntSet.h"
#include "InputAnValDlg.h"
#include "MatStatRecord.h"
#include "SortFunc.h"
#include "..\Query.h"
#include "ComparePartNoString.h"
#include "SetActiveModuleDlg.h"
#include "HashTable.h"
#include "PlankDrawing.h"
#include "Drawing.h"
#include "SumTaMatTblCfgDlg.h"
#include "LibraryQuery.h"
#include "segmatsum.h"
#include "material.h"
#include "HashTable.h"
#include "BomCfgDlg.h"
#include "TowerBom.h"
#include "scrTools.h"
//#include "TimerCount.h"
#include "DrawUnit.h"
#include "AcExtensionModule.h"  // Utility class for extension dlls
#include "ExportSetDlg.h"
#include "folder_dialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void GetTaUCS(UCS_STRU &ucs,BOOL is_frontface)
{
	if(is_frontface)
	{
		ucs.axis_x.Set(1,0,0);
		ucs.axis_y.Set(0,0,-1);
		ucs.axis_z.Set(0,1,0);
	}
	else
	{
		ucs.axis_x.Set(0,-1,0);
		ucs.axis_y.Set(0,0,-1);
		ucs.axis_z.Set(1,0,0);
	}
}
void SelModuleNo()
{
	CAcModuleResourceOverride resOverride;
	CSetActiveModuleDlg active_module_dlg;
	active_module_dlg.DoModal();
}

void SumFrontFace()
{
	//�������ø�ֵ
	sys.m_iCurMapType = 1;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	SCOPE_STRU scope;
	ads_point basepnt;
	LINEPARTSET sum_partset;
	PARTSET partset;
	NODESET sum_nodeset,nodeset;
	CLDSModule *pModule=Ta.GetActiveModule();
	if(pModule==NULL)
		return;
	pModule->RefreshModule(sum_nodeset,sum_partset,TRUE);
	for(CLDSNode *pNode=sum_nodeset.GetFirst();pNode;pNode=sum_nodeset.GetNext())
	{
		if(pNode->layer(2)=='Q'||pNode->layer(2)=='1'||pNode->layer(2)=='2')
		{
			pNode->feature=-1;
			nodeset.append(pNode);
		}
	}
	for(CLDSLinePart *pLinePart=sum_partset.GetFirst();pLinePart;pLinePart=sum_partset.GetNext())
	{
		if(pLinePart->layer(2)=='Q'||pLinePart->layer(2)=='1'||pLinePart->layer(2)=='2')
		{
			pLinePart->feature = 0;
			partset.append(pLinePart);
		}
	}
	GetNodeSetSizeScope(nodeset,scope);
	basepnt[X]=0.0; 
	basepnt[Y]=-((scope.fMaxZ-scope.fMinZ)/2.0+scope.fMinZ)/sys.general_set.LineMapScale;
	basepnt[Z]=0.0;

	UCS_STRU ucs;
	GetTaUCS(ucs,TRUE);
	DRAGSET.ClearEntSet();
	DrawSingleLineZhunMap(partset,nodeset,ucs,sys.general_set.LineMapScale,TRUE);
#ifdef _ARX_2007
	acedCommand(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
#else
	acedCommand(RTSTR,"zoom",RTSTR,"e",RTNONE);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	DragEntSet(basepnt,"please input insertion point\n");
#else
	DragEntSet(basepnt,"����������\n");
#endif
}
void SumSideFace()
{
	//�������ø�ֵ
	sys.m_iCurMapType = 1;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	SCOPE_STRU scope;
	ads_point basepnt;
	LINEPARTSET sum_partset;
	PARTSET partset;
	NODESET sum_nodeset,nodeset;
	CLDSModule *pModule=Ta.GetActiveModule();
	if(pModule==NULL)
		return;
	pModule->RefreshModule(sum_nodeset,sum_partset);
	for(CLDSNode *pNode=sum_nodeset.GetFirst();pNode;pNode=sum_nodeset.GetNext())
	{
		if(pNode->layer(2)=='Y'||pNode->layer(2)=='1'||pNode->layer(2)=='3')
		{
			pNode->feature=-1;
			nodeset.append(pNode);
		}
	}
	for(CLDSLinePart *pLinePart=sum_partset.GetFirst();pLinePart;pLinePart=sum_partset.GetNext())
	{
		if(pLinePart->layer(2)=='Y'||pLinePart->layer(2)=='1'||pLinePart->layer(2)=='3')
		{
			pLinePart->feature = 0;
			partset.append(pLinePart);
		}
	}
	GetNodeSetSizeScope(nodeset,scope);
	basepnt[X]=0.0; 
	basepnt[Y]=-((scope.fMaxZ-scope.fMinZ)/2.0+scope.fMinZ)/sys.general_set.LineMapScale;
	basepnt[Z]=0.0;

	UCS_STRU ucs;
	GetTaUCS(ucs,FALSE);
	DRAGSET.ClearEntSet();
	DrawSingleLineZhunMap(partset,nodeset,ucs, sys.general_set.LineMapScale,TRUE);
#ifdef _ARX_2007
	acedCommand(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
#else
	acedCommand(RTSTR,"zoom",RTSTR,"e",RTNONE);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	DragEntSet(basepnt,"please input insertion point\n");
#else
	DragEntSet(basepnt,"����������\n");
#endif
}
void FrontMutiLeg()
{
	//�������ø�ֵ
	sys.m_iCurMapType = 1;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	SCOPE_STRU scope;
	ads_point basepnt;
	LINEPARTSET sum_partset;
	PARTSET partset;
	NODESET sum_nodeset,nodeset;
	CLDSModule *pModule=Ta.GetActiveModule();
	if(pModule==NULL)
		return;
	pModule->RefreshModule(sum_nodeset,sum_partset);
	for(CLDSNode *pNode=sum_nodeset.GetFirst();pNode;pNode=sum_nodeset.GetNext())
	{
		if(pNode->layer(0)!='L')
			continue;
		if(pNode->layer(2)=='Q'||pNode->layer(2)=='1'||pNode->layer(2)=='2')
		{
			pNode->feature=-1;
			nodeset.append(pNode);
		}
	}
	for(CLDSLinePart *pLinePart=sum_partset.GetFirst();pLinePart;pLinePart=sum_partset.GetNext())
	{
		if(pLinePart->layer(0)!='L')
			continue;
		if(pLinePart->layer(2)=='Q'||pLinePart->layer(2)=='1'||pLinePart->layer(2)=='2')
		{
			pLinePart->feature = 0;
			partset.append(pLinePart);
		}
	}
	GetNodeSetSizeScope(nodeset,scope);
	basepnt[X]=0.0; 
	basepnt[Y]=-((scope.fMaxZ-scope.fMinZ)/2.0+scope.fMinZ)/sys.general_set.LineMapScale;
	basepnt[Z]=0.0;

	UCS_STRU ucs;
	GetTaUCS(ucs,TRUE);
	DRAGSET.ClearEntSet();
	DrawSingleLineZhunMap(partset, nodeset,ucs,sys.general_set.LineMapScale,TRUE);
#ifdef _ARX_2007
	acedCommand(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
#else
	acedCommand(RTSTR,"zoom",RTSTR,"e",RTNONE);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	DragEntSet(basepnt,"please input insertion point\n");
#else
	DragEntSet(basepnt,"����������\n");
#endif
}
void SideMutiLeg()
{
	//�������ø�ֵ
	sys.m_iCurMapType = 1;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	SCOPE_STRU scope;
	ads_point basepnt;
	LINEPARTSET sum_partset;
	PARTSET partset;
	NODESET sum_nodeset,nodeset;
	CLDSModule *pModule=Ta.GetActiveModule();
	if(pModule==NULL)
		return;
	pModule->RefreshModule(sum_nodeset,sum_partset);
	for(CLDSNode *pNode=sum_nodeset.GetFirst();pNode;pNode=sum_nodeset.GetNext())
	{
		if(pNode->layer(0)!='L')
			continue;
		if(pNode->layer(2)=='Y'||pNode->layer(2)=='1'||pNode->layer(2)=='3')
		{
			pNode->feature=-1;
			nodeset.append(pNode);
		}
	}
	for(CLDSLinePart *pLinePart=sum_partset.GetFirst();pLinePart;pLinePart=sum_partset.GetNext())
	{
		if(pLinePart->layer(0)!='L')
			continue;
		if(pLinePart->layer(2)=='Y'||pLinePart->layer(2)=='1'||pLinePart->layer(2)=='3')
		{
			pLinePart->feature = 0;
			partset.append(pLinePart);
		}
	}
	GetNodeSetSizeScope(nodeset,scope);
	basepnt[X]=0.0; 
	basepnt[Y]=-((scope.fMaxZ-scope.fMinZ)/2.0+scope.fMinZ)/sys.general_set.LineMapScale;
	basepnt[Z]=0.0;

	UCS_STRU ucs;
	GetTaUCS(ucs,FALSE);
	DRAGSET.ClearEntSet();
	DrawSingleLineZhunMap(partset, nodeset,ucs,sys.general_set.LineMapScale,TRUE);
#ifdef _ARX_2007
	acedCommand(RTSTR,L"zoom",RTSTR,L"e",RTNONE);
#else
	acedCommand(RTSTR,"zoom",RTSTR,"e",RTNONE);
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	DragEntSet(basepnt,"please input insertion point\n");
#else
	DragEntSet(basepnt,"����������\n");
#endif
}
#ifdef __SMART_DRAW_INC_
void DimSegI(CDrawingCommand *pCommand)
{	
	if(pCommand==NULL||pCommand->m_pDrawingCom==NULL)
		return;
	if(pCommand->m_eCmdType!=CMD_DIM_SEGI)
		return;
	//�������ø�ֵ
	sys.m_iCurMapType = 1;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	ads_name ent_name;
	ATOM_LIST<AcDbObjectId> entIdList;
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	CDrawingCommand *pTempCmd=NULL;
	SCOPE_STRU scope;
	f3dPoint basepnt;
	BOOL bHasEnt=TRUE;
	if(pCommand->m_paraDimSegI.bSelectPos==1)
	{	//��Ҫѡ���עλ��ʱ�����ҵ�ǰ�ζ�Ӧ�Ĺ������� wht 12-03-27
		int nPush=pCommand->m_pDrawingCom->cmdList.push_stack();
		for(pTempCmd=pCommand->m_pDrawingCom->cmdList.GetFirst();pTempCmd;pTempCmd=pCommand->m_pDrawingCom->cmdList.GetNext())
		{
			if(pTempCmd->m_eCmdType!=CMD_DRAW_LINEPART)
				continue;	//���Ƶ���
			AcDbObjectId *pEntId=NULL;
			for(pEntId=pTempCmd->entIdList.GetFirst();pEntId;pEntId=pTempCmd->entIdList.GetNext())
			{
				AcDbEntity *pEnt=NULL;
				acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
				if(pEnt==NULL)
					continue;
				long handle = GetTaAtomHandle(pEnt);
				pEnt->close();
				CLDSPart* pPart = Ta.FromPartHandle(handle);
				if(pPart&&pPart->iSeg.iSeg==pCommand->m_paraDimSegI.nSegI.iSeg)
				{
					if(pEnt->isKindOf(AcDbLine::desc()))
					{
						AcDbLine *pLine=(AcDbLine*)pEnt;
						scope.VerifyVertex(f3dPoint(pLine->startPoint().x,pLine->startPoint().y,0));
						scope.VerifyVertex(f3dPoint(pLine->endPoint().x,pLine->endPoint().y,0));
					}
					else if(pEnt->isKindOf(AcDbPoint::desc()))
					{
						AcDbPoint *pPoint=(AcDbPoint*)pEnt;
						scope.VerifyVertex(f3dPoint(pPoint->position().x,pPoint->position().y,0));
					}
					entIdList.append(*pEntId);
					acdbGetAdsName(ent_name,*pEntId);
					ads_redraw(ent_name,3);//������ʾ
					//���½���
					actrTransactionManager->flushGraphics();
					acedUpdateDisplay();
				}
			}
		}
		pCommand->m_pDrawingCom->cmdList.pop_stack(nPush);
		basepnt.x=0.5*(scope.fMinX+scope.fMaxX);
		basepnt.y=0.5*(scope.fMinY+scope.fMaxY);
		if(entIdList.GetNodeNum()<=0)
			bHasEnt=FALSE;
	}
	else if(pCommand->m_paraDimSegI.bSelectPos==2)
	{
		int nPush=pCommand->m_pDrawingCom->cmdList.push_stack();
		for(pTempCmd=pCommand->m_pDrawingCom->cmdList.GetFirst();pTempCmd;pTempCmd=pCommand->m_pDrawingCom->cmdList.GetNext())
		{
			if(pTempCmd->m_eCmdType!=CMD_DRAW_LINEPART)
				continue;	//���Ƶ���
			AcDbObjectId *pEntId=NULL;
			for(pEntId=pTempCmd->entIdList.GetFirst();pEntId;pEntId=pTempCmd->entIdList.GetNext())
			{
				AcDbEntity *pEnt=NULL;
				acdbOpenObject(pEnt,*pEntId,AcDb::kForRead);
				if(pEnt==NULL)
					continue;
				long handle = GetTaAtomHandle(pEnt);
				pEnt->close();
				CLDSPart* pPart = Ta.FromPartHandle(handle);
				if(pPart&&pPart->iSeg.iSeg==pCommand->m_paraDimSegI.nSegI.iSeg)
				{
					if(pEnt->isKindOf(AcDbLine::desc()))
					{
						AcDbLine *pLine=(AcDbLine*)pEnt;
						scope.VerifyVertex(f3dPoint(pLine->startPoint().x,pLine->startPoint().y,0));
						scope.VerifyVertex(f3dPoint(pLine->endPoint().x,pLine->endPoint().y,0));
					}
					else if(pEnt->isKindOf(AcDbPoint::desc()))
					{
						AcDbPoint *pPoint=(AcDbPoint*)pEnt;
						scope.VerifyVertex(f3dPoint(pPoint->position().x,pPoint->position().y,0));
					}
					entIdList.append(*pEntId);
				}
			}
		}
		pCommand->m_pDrawingCom->cmdList.pop_stack(nPush);
		f3dPoint center(0.5*(scope.fMinX+scope.fMaxX),0.5*(scope.fMinY+scope.fMaxY));
		basepnt=pCommand->m_pDrawingCom->GetMapFrmInsideTopLeft();
		double fDimPosX=pCommand->m_paraDimSegI.fDimPosX+center.x-basepnt.x;
		double fDimPosY=pCommand->m_paraDimSegI.fDimPosY+center.y-basepnt.y;
		basepnt.x=center.x+pCommand->m_paraDimSegI.fDimPosX;
		basepnt.y=center.y+pCommand->m_paraDimSegI.fDimPosY;
		pCommand->m_paraDimSegI.fDimPosX=fDimPosX;
		pCommand->m_paraDimSegI.fDimPosY=fDimPosY;
		pCommand->m_paraDimSegI.bSelectPos=FALSE;	//תΪ����ͼ��ԭ������λ������
		if(entIdList.GetNodeNum()<=0)
			bHasEnt=FALSE;
	}
	else 
	{
		basepnt=pCommand->m_pDrawingCom->GetMapFrmInsideTopLeft();
		basepnt.x+=pCommand->m_paraDimSegI.fDimPosX;
		basepnt.y+=pCommand->m_paraDimSegI.fDimPosY;
	}
	if(bHasEnt)
	{	//��ǰ��������һ����������
		DRAGSET.ClearEntSet();
		GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		double fTextSize=sys.dim_map.fSegITextSize;	//�������û��ƶκ� wht 11-06-28
		double fCirD=sys.dim_map.fSegICirD;
		if(fabs(fCirD)<EPS)
			fCirD=2*fTextSize;
		CreateAcadCircle(pBlockTableRecord,f3dPoint(basepnt.x,basepnt.y,0),0.5*fCirD);
		char sSegI[20]="";
		sprintf(sSegI,"%s",(char*)pCommand->m_paraDimSegI.nSegI.ToString());
		f3dPoint text_pos=basepnt;
		if(pCommand->m_paraDimSegI.nSegI>=10)
			text_pos.x -= 3;
		else
			text_pos.x -= 1.5;
		text_pos.y -= 3;
		DimText(pBlockTableRecord,text_pos,sSegI,TextStyleTable::hzfs.textStyleId,fTextSize,0);
		if(pCommand->m_paraDimSegI.bSelectPos)
		{
			//acedCommand(RTSTR,"zoom",RTPOINT,L_T,RTPOINT,R_B,0);
			//ads_command(RTSTR,"RE",RTNONE);
			ZoomAcadView(scope,100);
			actrTransactionManager->flushGraphics();
			acedUpdateDisplay();
#ifdef AFX_TARG_ENU_ENGLISH
			DragEntSet(basepnt,"please input insertion point\n");
#else
			DragEntSet(basepnt,"����������\n");
#endif
			f3dPoint datum_pos=pCommand->m_pDrawingCom->GetMapFrmInsideTopLeft();
			pCommand->m_paraDimSegI.fDimPosX=basepnt.x-datum_pos.x;
			pCommand->m_paraDimSegI.fDimPosY=basepnt.y-datum_pos.y;
			pCommand->m_paraDimSegI.bSelectPos=FALSE;
			//��Ҫѡ���עλ��ʱ������Ҫ������ʾ��ǰ�ι��� wht 12-03-27
			AcDbObjectId *pEntId=NULL;
			for(pEntId=entIdList.GetFirst();pEntId;pEntId=entIdList.GetNext())
			{
				acdbGetAdsName(ent_name,*pEntId);
				ads_redraw(ent_name,4);		//�ָ�ͼԪ������ʾ
			}
		}
		//���½���
		actrTransactionManager->flushGraphics();
		acedUpdateDisplay();
	}
	pBlockTableRecord->close();
}
#endif

#ifndef __TSA_FILE_
struct ADSNAME{	//�����Ŀ����ȫ����Ϊads_nameΪ���鲻�㸳ֵ wjh-2017.10.13
	ads_name _name;
	ADSNAME(){_name[0]=_name[1]=0;}
	ADSNAME(ads_name name){_name[0]=name[0];_name[1]=name[1];}
	void CopyTo(ads_name& name){name[0]=_name[0];name[1]=_name[1];}
};
void DimSegI()
{
	//�������ø�ֵ
	sys.m_iCurMapType = 1;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	long j,ent_N_OfSet;
	ads_name ent_set,ent_name;
	CArray<ADSNAME>seg_name_arr;
	THANDLE handle;
	AcGePoint3d LeftBottomPos,TopRightPos;
	f3dPoint basepnt;
	char sSegI[8];
	GreateSelectionSet(ent_set,"*");	//ѡ��ǰ��Ļ�ϵ�����ͼԪ
	acedSSLength(ent_set, &ent_N_OfSet);
	LeftBottomPos = GetCurDwg()->extmin();	
	TopRightPos = GetCurDwg()->extmax();
	if(fabs(LeftBottomPos.x)>1e5)
		LeftBottomPos.x = LeftBottomPos.y = 0;
	if(fabs(TopRightPos.x)>1e5)
		TopRightPos.x = TopRightPos.y = 0;
	basepnt.x = (LeftBottomPos.x+TopRightPos.x)/2;
	basepnt.y = (LeftBottomPos.y+TopRightPos.y)/2;

	//TOWER_TXT�㲻����ͼԪ��ʱ����,����ͬʱ�������������׳���WJH--01.10.21
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	ATOM_LIST<SEGI> segNoList;
	int nSegNum=Ta.GetSegNoList(segNoList);
	if(nSegNum<=0)
		return;
	for(SEGI *pSegI=segNoList.GetFirst();pSegI;pSegI=segNoList.GetNext())
	{
		AcDbEntity * pEnt;
		AcDbObjectId eId;
		seg_name_arr.RemoveAll();

		for(j=0;j<ent_N_OfSet;j++)
		{
			acedSSName(ent_set,j,ent_name);
			acdbGetObjectId(eId, ent_name);
			acdbOpenObject(pEnt, eId, AcDb::kForRead);
			if(pEnt==NULL)
				continue;
			handle = GetTaAtomHandle(pEnt);
			pEnt->close();
			if(handle>0x20)
			{
				CLDSNode *pNode = Ta.FromNodeHandle(handle);
				if(pNode!=NULL)
				{
					/*
					CLDSNode::ATTACH_PART *pAttach;
					for(pAttach=&pNode->head_attach_part;pAttach;pAttach=pAttach->next)
					{
						if(pAttach->iSeg==i+1)
						{
							seg_name1_arr.Add(ent_name[0]);
							seg_name2_arr.Add(ent_name[1]);
							ads_redraw(ent_name,3);//������ʾ
						}
					}*/
				}
				else
				{
					CLDSPart* pPart = Ta.FromPartHandle(handle);
					if(pPart!=NULL)
					{
						if(pPart->iSeg.iSeg==(*pSegI).iSeg)
						{
							seg_name_arr.Add(ent_name);
							ads_redraw(ent_name,3);//������ʾ
						}
					}
				}
			}
		}
		if(seg_name_arr.GetSize()>0)
		{	//��ǰ��������һ����������
			DRAGSET.ClearEntSet();
			double fTextSize=sys.dim_map.fSegITextSize;	//�������û��ƶκ� wht 11-06-28
			double fCirD=sys.dim_map.fSegICirD;
			if(fabs(fCirD)<EPS)
				fCirD=2*fTextSize;
			CreateAcadCircle(pBlockTableRecord,f3dPoint(basepnt.x,basepnt.y,0),0.5*fCirD);
			sprintf(sSegI,"%s",(char*)pSegI->ToString());
			if(*pSegI>=10)
				basepnt.x -= 3;
			else
				basepnt.x -= 1.5;
			basepnt.y -= 3;
			DimText(pBlockTableRecord,basepnt,sSegI,TextStyleTable::hzfs.textStyleId,fTextSize,0);
#ifdef AFX_TARG_ENU_ENGLISH
			DragEntSet(basepnt,"please input insertion point\n");
#else
			DragEntSet(basepnt,"����������\n");
#endif
			for(j=0;j<seg_name_arr.GetSize();j++)
			{
				seg_name_arr[j].CopyTo(ent_name);
				ads_redraw(ent_name,4);		//�ָ�ͼԪ������ʾ
			}
		}
	}
	seg_name_arr.RemoveAll();
	pBlockTableRecord->close();
}

//��Ű���������
typedef CMatStatRecord* CMatStatRecordPtr;
static char GetMatBriefMarkForSorting(const char cOrgMat)
{
	char cNewMat=cOrgMat;
	char cQ235BriefMark=CSteelMatLibrary::Q235BriefMark();
	if(cOrgMat==CSteelMatLibrary::Q345BriefMark())
		cNewMat=cQ235BriefMark+1;
	if(cOrgMat==CSteelMatLibrary::Q390BriefMark())
		cNewMat=cQ235BriefMark+2;
	if(cOrgMat==CSteelMatLibrary::Q420BriefMark())
		cNewMat=cQ235BriefMark+3;
	if(cOrgMat==CSteelMatLibrary::Q460BriefMark())
		cNewMat=cQ235BriefMark+4;
	return cNewMat;
}
//����˳��Q235(S)��Q345(H)��Q390(G)��Q420(T)��Q460(U);
//��˨����4.8����6.8����8.8����
//    ��񣺴�С����
int compareMatStatRecord(const CMatStatRecordPtr &rec1,const CMatStatRecordPtr &rec2)
{
	char cMat1=GetMatBriefMarkForSorting(rec1->cMaterial);
	char cMat2=GetMatBriefMarkForSorting(rec2->cMaterial);
	if(rec1->idClassTypeOrder>rec2->idClassTypeOrder)
		return 1;
	else if(rec1->idClassTypeOrder<rec2->idClassTypeOrder)
		return -1;
	else if(rec1->idClassTypeOrder==4)
	{	//��˨���Ŷ�����Ȧ��
		int types[2]={0};
		if(rec1->cMaterial>='A'&&rec1->cMaterial<='G')
			types[0]=0;	//��ͨ��˨
		else if(rec1->cMaterial>='a'&&rec1->cMaterial<='g')
			types[0]=1;	//˫ñ
		else if(rec1->cMaterial>='H'&&rec1->cMaterial<='L')
			types[0]=2;	//�Ŷ�
		else if(rec1->cMaterial=='S')
			types[0]=3;	//��Ȧ
		if(rec2->cMaterial>='A'&&rec2->cMaterial<='G')
			types[1]=0;	//��ͨ��˨
		else if(rec2->cMaterial>='a'&&rec2->cMaterial<='g')
			types[1]=1;	//˫ñ
		else if(rec2->cMaterial>='H'&&rec2->cMaterial<='L')
			types[1]=2;	//�Ŷ�
		else if(rec2->cMaterial=='S')
			types[1]=3;	//��Ȧ
		if(types[0]>types[1])
			return 1;
		else if(types[0]<types[1])
			return -1;
		int cmpspec_code=stricmp(rec1->spec,rec2->spec);
		if(cmpspec_code>0)
			return 1;
		else if(cmpspec_code<0)
			return -1;
		else
			return 0;
	}
	else if(cMat1>cMat2)
		return 1;	//���ʴӴ�С����
	else if(cMat1<cMat2)
		return -1;
	else if(strlen(rec1->spec)>strlen(rec2->spec))
		return 1;
	else if(strlen(rec1->spec)<strlen(rec2->spec))
		return -1;
	else 
		return CompareMultiSectionString(rec1->spec,rec2->spec);	//stricmp(rec1->spec,rec2->spec);
}
//��������ͼ�͹���涨����ҵ��׼�����壩090303���й涨 wht 11-05-06
//����˳��Q460(U) Q420(T)��Q390(G)��Q345(H)��Q235(S);
//��˨����4.8����6.8����8.8����
//    ��񣺽Ǹֲ��ôӴ�С���������ϴ�С����
int compareMatStatRecord1(const CMatStatRecordPtr &rec1,const CMatStatRecordPtr &rec2)
{	//���������ַ�˳����ڰ��������� wht 11-05-06
	char cMat1=GetMatBriefMarkForSorting(rec1->cMaterial);
	char cMat2=GetMatBriefMarkForSorting(rec2->cMaterial);
	if(rec1->idClassTypeOrder>rec2->idClassTypeOrder)
		return 1;
	else if(rec1->idClassTypeOrder<rec2->idClassTypeOrder)
		return -1;
	else if(rec1->idClassTypeOrder==4)
	{	//��˨���Ŷ�����Ȧ��
		int types[2]={0};
		if(rec1->cMaterial>='A'&&rec1->cMaterial<='G')
			types[0]=0;	//��ͨ��˨
		else if(rec1->cMaterial>='a'&&rec1->cMaterial<='g')
			types[0]=1;	//˫ñ
		else if(rec1->cMaterial>='H'&&rec1->cMaterial<='L')
			types[0]=2;	//�Ŷ�
		else if(rec1->cMaterial=='S')
			types[0]=3;	//��Ȧ
		if(rec2->cMaterial>='A'&&rec2->cMaterial<='G')
			types[1]=0;	//��ͨ��˨
		else if(rec2->cMaterial>='a'&&rec2->cMaterial<='g')
			types[1]=1;	//˫ñ
		else if(rec2->cMaterial>='H'&&rec2->cMaterial<='L')
			types[1]=2;	//�Ŷ�
		else if(rec2->cMaterial=='S')
			types[1]=3;	//��Ȧ
		if(types[0]>types[1])
			return 1;
		else if(types[0]<types[1])
			return -1;
		int cmpspec_code=stricmp(rec1->spec,rec2->spec);
		if(cmpspec_code>0)
			return -1;
		else if(cmpspec_code<0)
			return 1;
		else
			return 0;
	}
	else if(cMat1>cMat2)
	{
		if(rec1->idClassTypeOrder==4)	
			return 1;	//��˨�����С��������
		else 
			return -1;	//���๹�����ʴӴ�С����
	}
	else if(cMat1<cMat2)
	{
		if(rec1->idClassTypeOrder==4)	
			return -1;	//��˨�����С��������
		else 
			return 1;	//���๹�����ʴӴ�С����
	}
	else if(strlen(rec1->spec)>strlen(rec2->spec))
	{
		if(rec1->idClassTypeOrder==0)	
			return -1;	//�Ǹֹ��Ӵ�С����
		else 
			return 1;	//���๹������С��������
	}
	else if(strlen(rec1->spec)<strlen(rec2->spec))
	{
		if(rec1->idClassTypeOrder==0)	
			return 1;	//�Ǹֹ��Ӵ�С����
		else 
			return -1;	//���๹������С��������
	}
	else
	{
		if(rec1->idClassTypeOrder==0)	
			return -1*CompareMultiSectionString(rec1->spec,rec2->spec);	//�Ǹֹ��Ӵ�С����
		else 
			return CompareMultiSectionString(rec1->spec,rec2->spec);	//���๹������С��������
	}
}

//ͳ�ƹ���
void SummarizeParts(ATOM_LIST<CMatStatRecord> &matStat,IModel *pModel,
					CHashTable<int>&segTable,int nUseLegDesign,
					CBlockModel *pEmbededBlock=NULL)
{
	char limit_str[3001]="";
	int nSegN=segTable.m_nHashSize;
	int nBodyN=Ta.Module.GetNodeNum();
	if(nUseLegDesign>0)
		nBodyN=nUseLegDesign;
	for(CLDSPart *pPart=pModel->EnumPartFirst();pPart;pPart=pModel->EnumPartNext())
	{
		if(pEmbededBlock&&pEmbededBlock->handle!=pPart->m_hBlock&&!pPart->IsBlockRefPart())
			continue;	//���˳�����ָ�������Ĺ���
		if(pPart->m_bVirtualPart)
			continue;	//���⸨������
		if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE||pPart->GetClassTypeId()==CLS_POLYJOINT)
			continue;	//��ͳ����ϽǸ֣��������ε� wht 10-06-17
		double piece_weight=pPart->GetWeight();
		if(pPart->GetClassTypeId()==CLS_BOLT)
		{	//����˨�ļ�������ʱ�ʶ��Ӧ�����Ա������ڹ�������
			if(stricmp(((CLDSBolt*)pPart)->Grade(),"4.8")==0)
				pPart->cMaterial='A';
			else if(stricmp(((CLDSBolt*)pPart)->Grade(),"5.8")==0)
				pPart->cMaterial='B';
			else if(stricmp(((CLDSBolt*)pPart)->Grade(),"6.8")==0)
				pPart->cMaterial='C';
			else if(stricmp(((CLDSBolt*)pPart)->Grade(),"8.8")==0)
				pPart->cMaterial='D';
			else //if(stricmp(((CLDSBolt*)pPart)->grade,"10.9")==0)
				pPart->cMaterial='E';
		}
		else
			piece_weight=TruncateDouble(piece_weight,2);	//����˨����ͨ������ȷ��С�����2λ
		//���ɰ�����������������Ϣ���ַ���
		limit_str[0]='\0';
		int iNo=1;
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pModule->IsSonPart(pPart))
			{
				char ss[4];
				if(strlen(limit_str)>0)	//��������
					sprintf(ss,",%d",iNo);
				else					//��������
					sprintf(ss,"%d",iNo);
				strcat(limit_str,ss);
			}
			iNo++;
		}
		//����ʵ�ʹ���ͳ������
		char spec[50]="";
		int iSegIndex=-1;
		int part_seg_n=1;
		SEGI part_seg_arr[50]={SEGI()};
		part_seg_arr[0]=pPart->iSeg;
		if(pPart->GetClassTypeId()==CLS_BOLT)
			part_seg_n=((CLDSBolt*)pPart)->GetStatSegArr(part_seg_arr,50);
		for(int iIndex=0;iIndex<part_seg_n;iIndex++)
		{
			if(part_seg_arr[iIndex].iSeg<=0)
				iSegIndex=0;
			else
				segTable.GetValueAt(part_seg_arr[iIndex],iSegIndex);
			CMatStatRecord* pRec=NULL;
			for(pRec=matStat.GetFirst();pRec;pRec=matStat.GetNext())
			{
				if(GetClassTypeOrder(pPart->GetClassTypeId())!=pRec->idClassTypeOrder)
					continue;
				if(pPart->cMaterial!=pRec->cMaterial)
					continue;
				pPart->GetSpecification(spec,FALSE);
				if(stricmp(spec,pRec->spec)!=0)
					continue;
				if(iSegIndex>=0)
					pRec->AddSegWeight(iSegIndex,piece_weight*pPart->m_uStatMatCoef);
				if (nUseLegDesign==0)//�����Զ�����ϻ��ܱ�ʱ��ʹ��BodyWeight
					pRec->AddBodyWeight(limit_str,piece_weight*pPart->m_uStatMatCoef);
				break;
			}
			if(pRec==NULL)
			{
				pRec=matStat.append();
				pRec->idClassTypeOrder=GetClassTypeOrder(pPart->GetClassTypeId());
				pRec->cMaterial=pPart->cMaterial;
				pPart->GetSpecification(pRec->spec,FALSE);
				pRec->Create(nSegN,nBodyN);
				if(iSegIndex>=0)
					pRec->AddSegWeight(iSegIndex,piece_weight*pPart->m_uStatMatCoef);
				if (nUseLegDesign==0)	//Ĭ�Ϻ�������ͳ�����
					pRec->AddBodyWeight(limit_str,piece_weight*pPart->m_uStatMatCoef);
			}
			//ͳ����˨��Ȧ
			if(pPart->GetClassTypeId()==CLS_BOLT&&((CLDSBolt*)pPart)->DianQuan.N>0
				&&((CLDSBolt*)pPart)->DianQuan.thick>0)
			{	
				CLDSBolt *pBolt=(CLDSBolt*)pPart;
				double unit_weight=FindPadWei(pBolt->DianQuan.thick,pBolt->get_d());//������Ȧ����
				for(pRec=matStat.GetFirst();pRec;pRec=matStat.GetNext())
				{
					if(6!=pRec->idClassTypeOrder)
						continue;	//��Ȧ
					sprintf(spec,"M%dX%d",pBolt->get_d(),pBolt->DianQuan.thick);
					if(stricmp(spec,pRec->spec)!=0)
						continue;
					if(iSegIndex>=0)
						pRec->AddSegWeight(iSegIndex,unit_weight*pBolt->DianQuan.N*pPart->m_uStatMatCoef);
					if (nUseLegDesign==0)
						pRec->AddBodyWeight(limit_str,unit_weight*pBolt->DianQuan.N*pPart->m_uStatMatCoef);
					break;
				}
				if(pRec==NULL)
				{
					pRec=matStat.append();
					pRec->cMaterial='S';	//Q235
					pRec->idClassTypeOrder=6;	//��Ȧ
					sprintf(pRec->spec,"M%dX%d",pBolt->get_d(),pBolt->DianQuan.thick);	//��Ȧ���
					pRec->Create(nSegN,nBodyN);
					if(iSegIndex>=0)
						pRec->AddSegWeight(iSegIndex,unit_weight*pBolt->DianQuan.N*pPart->m_uStatMatCoef);
					if (nUseLegDesign==0)	//Ĭ�Ϻ�������ͳ�����
						pRec->AddBodyWeight(limit_str,unit_weight*pBolt->DianQuan.N*pPart->m_uStatMatCoef);
				}
			}
		}
	}
}

//ͳ�Ƹ��ӹ���
void SummarizeSupplementParts(ATOM_LIST<CMatStatRecord> &matStat,
							  CHashTable<int>&segTable,int nUseLegDesign)
{	
	char limit_str[3001]="";
	int nSegN=segTable.m_nHashSize;
	int nBodyN=Ta.Module.GetNodeNum();
	if(nUseLegDesign>0)
		nBodyN=nUseLegDesign;
	for (CSupplementPart *pSuppPart=Ta.SuppPart.GetFirst();pSuppPart;pSuppPart=Ta.SuppPart.GetNext())
	{
		limit_str[0]='\0';
		int iNo=1;
		for(CLDSModule *pModule=Ta.Module.GetFirst();pModule;pModule=Ta.Module.GetNext())
		{
			if(pModule->IsSonPart(pSuppPart))
			{
				char ss[4];
				if(strlen(limit_str)>0)	//��������
					sprintf(ss,",%d",iNo);
				else					//��������
					sprintf(ss,"%d",iNo);
				strcat(limit_str,ss);
			}
			iNo++;
		}
		//����ʵ�ʹ���ͳ������
		char spec[50]="";
		int iSegIndex=0;
		CMatStatRecord* pRec=NULL;
		int iType = 0;
		if (pSuppPart->part_type==STEELTYPE_ANGLE)
			iType=CLS_LINEANGLE;
		else if(pSuppPart->part_type==STEELTYPE_PLATE)
			iType=CLS_PLATE;
		else if(pSuppPart->part_type==STEELTYPE_TUBE)
			iType=CLS_LINETUBE;
		else if(pSuppPart->part_type==STEELTYPE_SLOT)
			iType=CLS_LINESLOT;
		if(pSuppPart->iSeg.iSeg<=0)
			iSegIndex=0;
		else
			segTable.GetValueAt(pSuppPart->iSeg,iSegIndex);
		double piece_weight=TruncateDouble(pSuppPart->piece_weight,2);
		for(pRec=matStat.GetFirst();pRec;pRec=matStat.GetNext())
		{
			if(GetClassTypeOrder(iType)!=pRec->idClassTypeOrder)
				continue;
			if(pSuppPart->cMaterial!=pRec->cMaterial)
				continue;
			if(stricmp(pSuppPart->guige,pRec->spec)!=0)
				continue;
			pRec->AddSegWeight(iSegIndex,piece_weight*pSuppPart->nQuantity);
			if (nUseLegDesign==0)
				pRec->AddBodyWeight(limit_str,piece_weight*pSuppPart->nQuantity);
			break;
		}
		if(pRec==NULL)
		{
			pRec=matStat.append();
			pRec->idClassTypeOrder=GetClassTypeOrder(iType);
			pRec->cMaterial=pSuppPart->cMaterial;
			sprintf(pRec->spec,"%s",(char*)pSuppPart->guige);
			pRec->Create(nSegN,nBodyN);
			pRec->AddSegWeight(iSegIndex,piece_weight*pSuppPart->nQuantity);
			if (nUseLegDesign==0)
				pRec->AddBodyWeight(limit_str,piece_weight*pSuppPart->nQuantity);
		}
	}
}

//ͳ�Ʋ�������
void SummarizeBlockRefs(ATOM_LIST<CMatStatRecord> &matStat,
						CHashTable<int>&segTable,int nUseLegDesign)
{
	for(CBlockReference *pBlockRef=Ta.BlockRef.GetFirst();pBlockRef;pBlockRef=Ta.BlockRef.GetNext())
	{
		if(pBlockRef->IsExploded())
			continue;
		CBlockModel *pBlock=Ta.Block.FromHandle(pBlockRef->m_hBlock);
		if(pBlock==NULL)
			continue;
		if(pBlock->IsEmbeded())	//Ƕ��ʽ����
			SummarizeParts(matStat,&Ta,segTable,nUseLegDesign,pBlock);
		else							//����ʽ����
			SummarizeParts(matStat,pBlock,segTable,nUseLegDesign);
	}
}


//Ԥ��׼�����ϻ��ܱ�����
void PrepareSumMatTable(CFormatTbl &xSumTbl)
{
	LEG_DESIGN useLegDesign[500]; 
	ATOM_LIST<CMatStatRecord>matStat;
	int i,j,k,nUseLegDesign=0;//�Ƿ����Զ����ͼ�ļ�
#ifdef AFX_TARG_ENU_ENGLISH
	BOOL EXPORT_BY_ENGLISH=TRUE;
#else
	BOOL EXPORT_BY_ENGLISH=sys.stru_map.bExportByEnglish;
#endif
#if defined(__TMA_FILE_)||defined(__LMA_FILE_)
	int n = Ta.AttachBuffer[CTower::BOM_CONFIG].GetLength();
	if (n>0)
	{
		char sBufferType[50]="";
		Ta.AttachBuffer[CTower::BOM_CONFIG].SeekToBegin();
		Ta.AttachBuffer[CTower::BOM_CONFIG].ReadString(sBufferType);
		//���Զ�ȡ���ݣ���鱣��������Ƿ�Ϊ���ϻ��������ļ� wht 11-01-10
		if(strcmp(sBufferType,"���ϻ��������ļ�")==0)
		{
			char *pStr = new char[n];
			Ta.AttachBuffer[CTower::BOM_CONFIG].ReadString(pStr);
			CMarkup xml;
			xml.Load("",pStr);
			delete [] pStr;
			xml.ResetMainPos();
			if(xml.FindElem("TOWERSET"))
			{
				while(xml.FindChildElem("TOWER"))   
				{ 
					xml.IntoElem();
					char segstr[MAX_TEMP_CHAR_200+1];
					sprintf(useLegDesign[nUseLegDesign].sHeight,"%s",xml.GetAttrib("high"));
					sprintf(segstr,"%s",xml.GetAttrib("seg"));
					useLegDesign[nUseLegDesign].bodySegStr.Copy(segstr);
					i=0;
					while(xml.FindChildElem("LEG"))
					{
						xml.IntoElem();
						useLegDesign[nUseLegDesign].legStrArr[i].Copy(xml.GetAttrib("seg"));
						useLegDesign[nUseLegDesign].coef[i]=atoi(xml.GetAttrib("coef"));
						i++;
						if(i==4)	//ÿ��������������ĸ���ͬ�߶ȵ���
							break;
						xml.OutOfElem();
					}
					xml.OutOfElem();
					nUseLegDesign++;
				}
			}
		}
		else 
#ifdef AFX_TARG_ENU_ENGLISH
			AfxMessageBox("unexpected materials Summary configuration file, generate material summary table by default!");
#else
			AfxMessageBox("��Ԥ�ڵĲ��ϻ��������ļ���ʽ,��Ĭ�Ϸ�ʽ���ɲ��ϻ��ܱ�!");
#endif
	}
#endif
	int nBodyN=0,nSegN=0;
	CHashTable<int>segTable;
	ATOM_LIST<SEGI> segNoList;
	if(bom.Parts.GetNodeNum()==0)
	{	//����ȫ�������Զ����»���
		if (nUseLegDesign>0)//��������û��Զ����ͼ
			nBodyN=nUseLegDesign;
		else
			nBodyN=Ta.Module.GetNodeNum();
		nSegN=Ta.GetSegNoList(segNoList);
		segTable.CreateHashTable(nSegN);
		for(i=0;i<nSegN;i++)
		{
			SEGI *pSegI=segNoList.GetByIndex(i);
			if(*pSegI<=0)	//����κŴ���
				continue;
			else
				segTable.SetValueAt(*pSegI,i);
		}
		SummarizeParts(matStat,&Ta,segTable,nUseLegDesign);
		SummarizeSupplementParts(matStat,segTable,nUseLegDesign);
		SummarizeBlockRefs(matStat,segTable,nUseLegDesign);
	}
	else
	{
		bom.StatTower(&matStat);
		nBodyN=bom.bodyList.GetNodeNum();
		nSegN=bom.hashSegI.GetNodeNum();
		segTable.CreateHashTable(nSegN);
		i=0;
		for(int* pSegI=bom.hashSegI.GetFirst();pSegI;pSegI=bom.hashSegI.GetNext(),i++)
		{
			int segI=bom.hashSegI.GetCursorKey()-1;	//��������κŴ���
			if(segI>0)
			{
				segTable.SetValueAt(segI,i);
				segNoList.append(SEGI(segI));
			}
		}
	}
	CMatStatRecordPtr* recPtrArr=new CMatStatRecordPtr[matStat.GetNodeNum()];
	CMatStatRecord *pPrevRec=NULL;
	i=0;
	for(CMatStatRecord *pTmRec=matStat.GetFirst();pTmRec;pTmRec=matStat.GetNext())
	{
		recPtrArr[i]=pTmRec;
		i++;
	}
	if(sys.general_set.iSumMatBOMStyle==0)	//��ʽһ ���ʴӵ�С �Ǹֹ��Ӵ�С wht 11-05-06
		CHeapSort<CMatStatRecordPtr>::HeapSort(recPtrArr,matStat.GetNodeNum(),compareMatStatRecord1);
	else									//��ʽ��
		CHeapSort<CMatStatRecordPtr>::HeapSort(recPtrArr,matStat.GetNodeNum(),compareMatStatRecord);
	int nGroupN=1;	//С�Ʒ�������
	for(i=0;i<matStat.GetNodeNum();i++)
	{
		if(i==0)
			continue;
		else if(recPtrArr[i-1]->idClassTypeOrder!=recPtrArr[i]->idClassTypeOrder || recPtrArr[i-1]->cMaterial!=recPtrArr[i]->cMaterial)
			nGroupN++;
	}
	//��ȫ�����ϻ��ܽ����������ܱ���
	double weight;
	char sGridText[100];
	int nColumnsN = 3+nSegN+nBodyN;	//��������
	int nRowsN=4+matStat.GetNodeNum()+nGroupN;
	CGrid::PREFER_TEXT_SIZE=sys.fonts.summary.fNumericTextSize;
	xSumTbl.Create(nRowsN,nColumnsN);
	for(i=0;i<nColumnsN;i++)	//����Ϊ������
	{
		xSumTbl.GetGridAt(0,i)->textsize = sys.fonts.summary.fHeaderTextSize;//dim_map.fHeaderTextSize;
		xSumTbl.GetGridAt(1,i)->textsize = sys.fonts.summary.fCommonTextSize;//.dim_map.fCommonTextSize;
		xSumTbl.GetGridAt(2,i)->textsize = sys.fonts.summary.fCommonTextSize;//.dim_map.fCommonTextSize;
	}
	for(i=3;i<nRowsN;i++)
	{
		xSumTbl.GetGridAt(i,0)->textsize = sys.fonts.summary.fCommonTextSize;//.dim_map.fCommonTextSize;
		xSumTbl.GetGridAt(i,1)->textsize = sys.fonts.summary.fCommonTextSize;//.dim_map.fCommonTextSize;
		for(int j=2;j<nColumnsN;j++)
			xSumTbl.GetGridAt(i,j)->textsize = sys.fonts.summary.fNumericTextSize;//.dim_map.fCommonTextSize;
	}
	//���ɴ����-------���ϻ��ܱ�-----------
	xSumTbl.SetRowHigh(0,16);		//������
	xSumTbl.SetRowHigh(1,8);		//�α�--���Ƹ���
	xSumTbl.SetRowHigh(2,8);		//�κ�--�����ߺ���
	for(i=3;i<nRowsN-1;i++)			//������
		xSumTbl.SetRowHigh(i,6);
	xSumTbl.SetRowHigh(nRowsN-1,10);//�ϼ���

	xSumTbl.SetColumnWide(0,10);	//����������
	xSumTbl.SetColumnWide(1,12);	//������
	xSumTbl.SetColumnWide(2,20);	//�����
	for(j=3;j<nColumnsN;j++)		//������
		xSumTbl.SetColumnWide(j,16);
	//����е�����ȱʡ�����½Ƕ��뼴feature = 0
	//feature��λ����ʾ�����ֵĶ��뷽ʽ0:������ 1��ʾ������ 2��ʾ���½�
	//ʮλ��ʾ������д����0:������1:���ϵ���

	//���������
	for(j=1;j<nColumnsN;j++)//ˮƽ�ϲ���һ���е����б��Ԫ
		xSumTbl.GetGridAt(0,j)->m_bHGridVirtual = TRUE;
	if(EXPORT_BY_ENGLISH)
		xSumTbl.GetGridAt(0,0)->data.SetGridString("SUMMARY BILL OF MATERIAL");
	else
		xSumTbl.GetGridAt(0,0)->data.SetGridString("���ϻ��ܱ�");
	xSumTbl.GetGridAt(0,0)->feature = 1;	//����Գ�MC

	//����--�������Ʊ��Ԫ
	xSumTbl.GetGridAt(2,0)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	if(EXPORT_BY_ENGLISH)
		xSumTbl.GetGridAt(1,0)->data.SetGridString("Type");
	else
		xSumTbl.GetGridAt(1,0)->data.SetGridString("����\\P����");
	xSumTbl.GetGridAt(1,0)->feature = 1;	//����Գ�MC
	//����--���ʱ��Ԫ
	xSumTbl.GetGridAt(2,1)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	xSumTbl.GetGridAt(1,1)->feature = 1;
	if(EXPORT_BY_ENGLISH)
		xSumTbl.GetGridAt(1,1)->data.SetGridString("Material\\PGrade");
	else
		xSumTbl.GetGridAt(1,1)->data.SetGridString("����");
	//����--�����Ԫ
	xSumTbl.GetGridAt(2,2)->m_bVGridVirtual = TRUE;//��ֱ�ϲ���Ԫ��
	xSumTbl.GetGridAt(1,2)->feature = 1;
	if(EXPORT_BY_ENGLISH)
		xSumTbl.GetGridAt(1,2)->data.SetGridString("Spec.");
	else
		xSumTbl.GetGridAt(1,2)->data.SetGridString("��  ��");
	//����--�α���Ԫ
	for(j=4;j<3+nSegN;j++)
		xSumTbl.GetGridAt(1,j)->m_bHGridVirtual=TRUE;//ˮƽ�ϲ���Ԫ��
	xSumTbl.GetGridAt(1,3)->feature = 1;
	if(EXPORT_BY_ENGLISH)
		xSumTbl.GetGridAt(1,3)->data.SetGridString("Segment");
	else
		xSumTbl.GetGridAt(1,3)->data.SetGridString("��  ��");
	for(j=3;j<3+nSegN;j++)
	{
		char tem_str[100];
		sprintf(tem_str,"%s",(char*)segNoList[j-3].ToString());
		xSumTbl.GetGridAt(2,j)->data.SetGridString(tem_str);
		xSumTbl.GetGridAt(2,j)->feature=1;
	}
	//����--���Ƹ߱��Ԫ
	for(j=4+nSegN;j<3+nSegN+nBodyN;j++)
		xSumTbl.GetGridAt(1,j)->m_bHGridVirtual=TRUE;//ˮƽ�ϲ���Ԫ��
	xSumTbl.GetGridAt(1,3+nSegN)->feature = 1;
	if(EXPORT_BY_ENGLISH)
		xSumTbl.GetGridAt(1,3+nSegN)->data.SetGridString("Body");
	else
		xSumTbl.GetGridAt(1,3+nSegN)->data.SetGridString("�� �� ��");
	int leg_i=0;
	
	j=3+nSegN;
#if defined(__TMA_FILE_)||defined(__LMA_FILE_)
	if (nUseLegDesign>0)//��������û��Զ����ͼ
	{
		for (i=0;i<nUseLegDesign;i++,j++)
		{
			xSumTbl.GetGridAt(2,j)->feature = 1;
			xSumTbl.GetGridAt(2,j)->data.SetGridString(useLegDesign[i].sHeight);
		}
	}
#else
	if(bom.bodyList.GetNodeNum()>0)
	{
		for(LEG_DESIGN *pBody=bom.bodyList.GetFirst();pBody&&j<3+nSegN+nBodyN;pBody=bom.bodyList.GetNext(),j++)
		{
			xSumTbl.GetGridAt(2,j)->feature = 1;
			xSumTbl.GetGridAt(2,j)->data.SetGridString(pBody->sHeight);
		}
	}
#endif
	else
	{
		CLDSModule *pModule;
		for(pModule=Ta.Module.GetFirst();pModule&&j<3+nSegN+nBodyN;pModule=Ta.Module.GetNext(),j++)
		{
			xSumTbl.GetGridAt(2,j)->feature = 1;
			xSumTbl.GetGridAt(2,j)->data.SetGridString(pModule->description);
		}
	}
	
	//���������
	int nMatGroupRowsN=0,iMatLineStartRow=3,iMatGroupStartRow=0,nTypeGroupRowsN=0,iTypeLineStartRow=3,iTypeGroupStartRow=0,nAddRows=3;
	for(i=0;i<=matStat.GetNodeNum();i++)
	{
		if(i>0)
		{
			if(i==matStat.GetNodeNum()||recPtrArr[i-1]->idClassTypeOrder!=recPtrArr[i]->idClassTypeOrder)
			{	//�����͹���
				for(k=iTypeLineStartRow+1;k<i+nAddRows+1;k++)
					xSumTbl.GetGridAt(k,0)->m_bVGridVirtual=TRUE;//��ֱ�ϲ���Ԫ��
				xSumTbl.GetGridAt(iTypeLineStartRow,0)->feature = 1;
				if(EXPORT_BY_ENGLISH)
				{
					if(recPtrArr[iTypeGroupStartRow]->idClassTypeOrder==0)	//�Ǹ�
					{
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->data.SetGridString("Angle");
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->text_angle=270*RADTODEG_COEF;
					}
					else if(recPtrArr[iTypeGroupStartRow]->idClassTypeOrder==1)	//tube
					{
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->data.SetGridString("Tube");
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->text_angle=270*RADTODEG_COEF;
					}
					else if(recPtrArr[iTypeGroupStartRow]->idClassTypeOrder==2)	//U-steel
					{
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->data.SetGridString("U-steel");
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->text_angle=270*RADTODEG_COEF;
					}
					else if(recPtrArr[iTypeGroupStartRow]->idClassTypeOrder==3)	//plate
					{
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->data.SetGridString("Plate");
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->text_angle=270*RADTODEG_COEF;
					}
					else if(recPtrArr[iTypeGroupStartRow]->idClassTypeOrder==4)	//bolt
					{
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->data.SetGridString("Bolt");
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->text_angle=270*RADTODEG_COEF;
					}
					else if(recPtrArr[iTypeGroupStartRow]->idClassTypeOrder==6)	//washer
					{
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->data.SetGridString("Washer");
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->text_angle=270*RADTODEG_COEF;
					}
				}
				else
				{
					if(recPtrArr[iTypeGroupStartRow]->idClassTypeOrder==0)	//�Ǹ�
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->data.SetGridString("��\\P��");
					else if(recPtrArr[iTypeGroupStartRow]->idClassTypeOrder==1)	//�ֹ�
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->data.SetGridString("��\\P��");
					else if(recPtrArr[iTypeGroupStartRow]->idClassTypeOrder==2)	//�۸�
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->data.SetGridString("��\\P��");
					else if(recPtrArr[iTypeGroupStartRow]->idClassTypeOrder==3)	//�ְ�
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->data.SetGridString("��\\P��");
					else if(recPtrArr[iTypeGroupStartRow]->idClassTypeOrder==4)	//��˨
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->data.SetGridString("��\\P˨");
					else if(recPtrArr[iTypeGroupStartRow]->idClassTypeOrder==6)	//��Ȧ
						xSumTbl.GetGridAt(iTypeLineStartRow,0)->data.SetGridString("��\\PȦ");
				}
				nTypeGroupRowsN=0;
				iTypeGroupStartRow=i;
				iTypeLineStartRow=i+nAddRows+1;
			}
			if(i==matStat.GetNodeNum()||recPtrArr[i-1]->cMaterial!=recPtrArr[i]->cMaterial||
				recPtrArr[i-1]->idClassTypeOrder!=recPtrArr[i]->idClassTypeOrder)
			{	//�µ�С�Ʒ��鿪ʼ
				//������һ������Ϣ
				//if(i==matStat.GetNodeNum()-1) //wht 10-06-17 ���һ���¼����һ��ʱ������ ����
				//	nMatGroupRowsN++;
				for(j=3;j<3+nSegN+nBodyN;j++)
				{	//��дÿ�εĽǸ�����С��
					weight = 0;
					for(k=iMatGroupStartRow;k<iMatGroupStartRow+nMatGroupRowsN;k++)
					{
						if(j<3+nSegN)	//������ε�����С��
							weight += TruncateDouble(recPtrArr[k]->GetSegWeight(j-3),1);
						else if (nUseLegDesign==0)
							weight += TruncateDouble(recPtrArr[k]->GetBodyWeight(j-2-nSegN),1);
					}
					if(weight>0)
					{
						sprintf(sGridText,"%.1f",weight);
						xSumTbl.GetGridAt(iMatLineStartRow+nMatGroupRowsN,j)->data.SetGridString(sGridText);
					}
				}//������û��Զ����ͼ
				if(nUseLegDesign>0)
				{
					for (j=0;j<nBodyN;j++)
					{
						weight=0;
						int segArr[100]={0};
						//��������ͳ��
						int segN = GetNoArr(useLegDesign[j].bodySegStr,segArr,100);
						for(k=0;k<segN;k++)
						{
							char sWeight[50]="";
							int index = -4;
							segTable.GetValueAt(segArr[k],index);
							if(xSumTbl.GetGridString(iMatLineStartRow+nMatGroupRowsN,index+3,sWeight))
								weight += atof(sWeight);
						}
						//��������ͳ��
						for(int iLeg=0;iLeg<4;iLeg++)
						{
							if(strlen(useLegDesign[j].legStrArr[iLeg])==0||useLegDesign[j].coef[iLeg]==0)
								break;
							segN = GetNoArr(useLegDesign[j].legStrArr[iLeg],segArr,100);
							for(k=0;k<segN;k++)
							{
								char sWeight[50]="";
								int index = -4;
								segTable.GetValueAt(segArr[k],index);
								if(xSumTbl.GetGridString(iMatLineStartRow+nMatGroupRowsN,index+3,sWeight))
									weight += atof(sWeight)*0.25*useLegDesign[j].coef[iLeg];
							}
						}
						if(weight>0)
						{
							sprintf(sGridText,"%.1f",weight);
							xSumTbl.GetGridAt(iMatLineStartRow+nMatGroupRowsN,j+3+nSegN)->data.SetGridString(sGridText);
						}
					}
				}
				for(k=iMatLineStartRow+1;k<=iMatLineStartRow+nMatGroupRowsN;k++)
					xSumTbl.GetGridAt(k,1)->m_bVGridVirtual=TRUE;//��ֱ�ϲ���Ԫ��
				char steelmark[50];
				if(recPtrArr[iMatGroupStartRow]->idClassTypeOrder<4
					||recPtrArr[iMatGroupStartRow]->idClassTypeOrder==6)	//��˨��Ȧ	
					QuerySteelMatMark(recPtrArr[iMatGroupStartRow]->cMaterial,steelmark);
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='A')
					strcpy(steelmark,"4.8");
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='B')
					strcpy(steelmark,"5.8");
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='C')
					strcpy(steelmark,"6.8");
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='D')
					strcpy(steelmark,"8.8");
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='E')
					strcpy(steelmark,"10.8");
				//˫ñ
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='a')
					strcpy(steelmark,"4.8��\n˫ñ");
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='b')
					strcpy(steelmark,"5.8��\n˫ñ");
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='c')
					strcpy(steelmark,"6.8��\n˫ñ");
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='d')
					strcpy(steelmark,"8.8��\n˫ñ");
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='e')
					strcpy(steelmark,"10.8��\n˫ñ");
				//�Ŷ�
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='H')
					strcpy(steelmark,"4.8��\n�Ŷ�");
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='I')
					strcpy(steelmark,"5.8��\n�Ŷ�");
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='J')
					strcpy(steelmark,"6.8��\n�Ŷ�");
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='K')
					strcpy(steelmark,"8.8��\n�Ŷ�");
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='L')
					strcpy(steelmark,"10.8��\n�Ŷ�");
				else if(recPtrArr[iMatGroupStartRow]->cMaterial=='S')
					strcpy(steelmark,"Q235");
				xSumTbl.GetGridAt(iMatLineStartRow,1)->feature = 1;
				xSumTbl.GetGridAt(iMatLineStartRow,1)->data.SetGridString(steelmark);
				xSumTbl.GetGridAt(iMatLineStartRow+nMatGroupRowsN,2)->feature = 1;
				if(EXPORT_BY_ENGLISH)
					xSumTbl.GetGridAt(iMatLineStartRow+nMatGroupRowsN,2)->data.SetGridString("Subtotal");
				else
					xSumTbl.GetGridAt(iMatLineStartRow+nMatGroupRowsN,2)->data.SetGridString("С  ��");
				nAddRows++;
				iMatGroupStartRow=i;
				iMatLineStartRow=i+nAddRows;
				nMatGroupRowsN=0;		//С���¼������
			}
		}
		nTypeGroupRowsN++;
		nMatGroupRowsN++;
		//��д�������
		if(i<matStat.GetNodeNum())
		{
			xSumTbl.GetGridAt(i+nAddRows,2)->feature = 1;
			if(recPtrArr[i]->idClassTypeOrder==6)	//��Ȧ
			{
				CLDSBolt ls;
				restore_Ls_guige(recPtrArr[i]->spec,ls);	//��������Ȧ����е���˨ֱ�����������
				int thick = (int)ls.get_L();
				int d = ls.get_d();
				CXhChar16 tem_str;
				if(EXPORT_BY_ENGLISH)
				{
					if(d<=16)	//�����¹����ʽ
						tem_str.Printf("-%d(��%.1f)",thick,d+1.5);
					else
						tem_str.Printf("-%d(��%.0f)",thick,d+2.0);
				}
				else
				{
					if(d<=16)	//�����¹����ʽ
						tem_str.Printf("-%d(��%.1f)",thick,d+1.5);
					else
						tem_str.Printf("-%d(��%.0f)",thick,d+2.0);
				}
				xSumTbl.GetGridAt(i+nAddRows,2)->data.SetGridString(tem_str);
			}
			else
				xSumTbl.GetGridAt(i+nAddRows,2)->data.SetGridString(recPtrArr[i]->spec);
			for(j=3;j<3+nSegN;j++)
			{	//��д��ǰ��¼��ÿ������
				weight = recPtrArr[i]->GetSegWeight(j-3);
				if(weight>0)
				{
					sprintf(sGridText,"%.1f",weight);
					xSumTbl.GetGridAt(i+nAddRows,j)->data.SetGridString(sGridText);
				}
			}
			if(nUseLegDesign>0)//��������û��Զ����ͼ
			{
				for(j=0;j<nBodyN;j++)
				{
					weight=0;
					int segArr[100]={0};
					//��������ͳ��
					int segN = GetNoArr(useLegDesign[j].bodySegStr,segArr,100);
					for(k=0;k<segN;k++)
					{
						int index = -1;
						char sWeight[50]="";
						segTable.GetValueAt(segArr[k],index);//�õ�������
						if(xSumTbl.GetGridString(i+nAddRows,index+3,sWeight))
							weight += atof(sWeight);
							//weight += recPtrArr[i]->GetSegWeight(index);
					}
					//��������ͳ��
					for(int iLeg=0;iLeg<4;iLeg++)
					{
						if(strlen(useLegDesign[j].legStrArr[iLeg])==0||useLegDesign[j].coef[iLeg]==0)
							break;
						segN = GetNoArr(useLegDesign[j].legStrArr[iLeg],segArr,100);
						for(k=0;k<segN;k++)
						{
							int index = -1;
							char sWeight[50]="";
							segTable.GetValueAt(segArr[k],index);
							if(xSumTbl.GetGridString(i+nAddRows,index+3,sWeight))
								weight += atof(sWeight)*0.25*useLegDesign[j].coef[iLeg];
							//weight += recPtrArr[i]->GetSegWeight(index)*0.25*useLegDesign[j].coef[iLeg];
						}
					}
					if(weight>0)
					{
						sprintf(sGridText,"%.1f",weight);
						xSumTbl.GetGridAt(i+nAddRows,3+nSegN+j)->data.SetGridString(sGridText);
					}
				}
			}
			else
			{
				for(j=3+nSegN;j<3+nSegN+nBodyN;j++)
				{	//��д�����߼�¼����
					weight = recPtrArr[i]->GetBodyWeight(j-2-nSegN);
					if(weight>0)
					{
						sprintf(sGridText,"%.1f",weight);
						xSumTbl.GetGridAt(i+nAddRows,j)->data.SetGridString(sGridText);
					}
				}
			}
		}
	}
	//����--�ϼ���
	xSumTbl.GetGridAt(nRowsN-1,1)->m_bHGridVirtual=TRUE;//ˮƽ�ϲ���Ԫ��
	xSumTbl.GetGridAt(nRowsN-1,2)->m_bHGridVirtual=TRUE;//ˮƽ�ϲ���Ԫ��
	if(EXPORT_BY_ENGLISH)
		xSumTbl.GetGridAt(nRowsN-1,0)->data.SetGridString("Total(kg)");
	else
		xSumTbl.GetGridAt(nRowsN-1,0)->data.SetGridString("��     ��(kg)");
	xSumTbl.GetGridAt(nRowsN-1,0)->feature = 1;
	char tem_str[100];
	for(j=0;j<nSegN;j++)
	{
		weight = 0 ;
		for(i=0;i<matStat.GetNodeNum();i++)
			weight+=TruncateDouble(recPtrArr[i]->GetSegWeight(j),1);
		sprintf(tem_str,"%.1f",weight);
		xSumTbl.GetGridAt(nRowsN-1,j+3)->data.SetGridString(tem_str);
	}
	if(nUseLegDesign>0)//��������û��Զ����ͼ�ļ�
	{
		for (j=0;j<nBodyN;j++)
		{
			weight=0;
			int segArr[100]={0};
			//��������ͳ��
			int segN = GetNoArr(useLegDesign[j].bodySegStr,segArr,100);
			for(k=0;k<segN;k++)
			{
				char sWeight[50] = "";
				int index = -4;//��֤index+3<0
				segTable.GetValueAt(segArr[k],index);
				if(xSumTbl.GetGridString(nRowsN-1,index+3,sWeight))
					weight += atof(sWeight);
			}
					//��������ͳ��
			for(int iLeg=0;iLeg<4;iLeg++)
			{
				if(strlen(useLegDesign[j].legStrArr[iLeg])==0||useLegDesign[j].coef[iLeg]==0)
					break;
				segN = GetNoArr(useLegDesign[j].legStrArr[iLeg],segArr,100);
				for(k=0;k<segN;k++)
				{
					char sWeight[50] = "";
					int index = -4;
					segTable.GetValueAt(segArr[k],index);
					if(xSumTbl.GetGridString(nRowsN-1,index+3,sWeight))
						weight += atof(sWeight)*0.25*useLegDesign[j].coef[iLeg];
				}
			}
			if(weight>0)
			{
				sprintf(sGridText,"%.1f",weight);
				xSumTbl.GetGridAt(nRowsN-1,j+3+nSegN)->data.SetGridString(sGridText);
			}
		}
	}
	else
	{
		for(j=0;j<nBodyN;j++)
		{
			weight = 0 ;
			for(i=0;i<matStat.GetNodeNum();i++)
				weight+=TruncateDouble(recPtrArr[i]->GetBodyWeight(j+1),1);
			sprintf(tem_str,"%.1f",weight);
			xSumTbl.GetGridAt(nRowsN-1,j+3+nSegN)->data.SetGridString(tem_str);
		}
	}
	delete []recPtrArr;
}
void SumTaMat()
{
	//�������ø�ֵ
	//CTowerBOM tower;
	//tower.ReadTextBOMFile("F:\\SHJ461-140.bom");
	//tower.ReadExcelBOMFile("F:\\Book1.xls");

	CFormatTbl xSumTbl;
	sys.m_iCurMapType = 1;
	//sys.fonts.InitSpecialProp�ѷ���ϸ��������Ĺ��ܴ����� �Ժ���Ҫ�ٵ��� wjh-2017.8.27
	//sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	PrepareSumMatTable(xSumTbl);
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	DrawTable(pBlockTableRecord,&xSumTbl);
	pBlockTableRecord->close();
}

//���ò��ϻ��ܱ�
void SumTaMatCfg()
{
#ifndef __TSA_FILE_
	CAcModuleResourceOverride resOverride;
#if defined(__TMA_FILE_)||defined(__LMA_FILE_)
	CSumTaMatTblCfgDlg cfgDlg;
	cfgDlg.DoModal();
#elif defined(__TDA_FILE_)||defined(__LDS_FILE_)
	CBomCfgDlg bomDlg;
	bomDlg.m_xBomObj.CopyFrom(bom);
	if(bomDlg.DoModal()==IDOK)
		bom.CopyFrom(bomDlg.m_xBomObj);
#endif
#endif
}

typedef CLDSParamPlate* PLATE_PTR;
int compare_plate(const PLATE_PTR &pPart1,const PLATE_PTR &pPart2)
{
	//return stricmp(pPart1->sPartNo,pPart2->sPartNo);
	return ComparePartNoString(pPart1->GetPartNo(),pPart2->GetPartNo());
}
#ifdef __PART_DESIGN_INC_//defined(__TSA_FILE_)||defined(__LDS_FILE_)
void CreateVirtualFL(AcDbBlockTableRecord *pBlockTableRecord,f3dPoint origin,CConnectFL* pConnectFL)
{
	if(pBlockTableRecord==NULL||pConnectFL==NULL)
		return;
	f3dLine line;
	double W=64,T=6,tube_r=20,tube_thick=2,tube_len=40;
	double A=20,B=10,C=5,D=5,thick=1;
	//��˨�ڷ����ϵı���(��˨���ڲ�뾶�ľ����뷨����ȵı�ֵ)
	double bolt_r_scale=(pConnectFL->m_fBoltRadius-pConnectFL->m_fInnerRadius)/pConnectFL->m_nWidthFL;
	double fBoltCirR=tube_r+12*bolt_r_scale;	
	f3dPoint down_datum_pos(origin.x,origin.y-W);	//�²��׼�����ڻ��Ʒ���ƽ��ͼ
	f3dPoint up_datum_pos(origin.x,origin.y);	//�ϲ��׼�����ڻ��Ʒ�������ͼ
	//1. ����ƽ��ͼ
	f3dPoint center=down_datum_pos;
	//1.1���Ʒ���ƽ��ͼ���ֹܽ���ͼ����˨����Բ��XY��������
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	CreateAcadCircle(pBlockTableRecord,center,0.5*W);		
	CreateAcadCircle(pBlockTableRecord,center,tube_r);
	CreateAcadCircle(pBlockTableRecord,center,tube_r-tube_thick);
	GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
	CreateAcadCircle(pBlockTableRecord,center,fBoltCirR);
	line.startPt.Set(center.x-0.5*(W+tube_r),center.y);
	line.endPt.Set(center.x+0.5*(W+tube_r),center.y);
	CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
	line.startPt.Set(center.x,center.y-0.5*(W+tube_r));
	line.endPt.Set(center.x,center.y+0.5*(W+tube_r));
	CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
	//1.2���Ʒ����ϵ���弰��˨��
	int i=0; 
	int nQuarterNum=1;	//�ķ�֮һ��˨��
	if(pConnectFL->bolt_n%4>0)
		nQuarterNum=pConnectFL->bolt_n/4+1;
	else 
		nQuarterNum=pConnectFL->bolt_n/4;
	int nDimIndex=1;	//�����ڵڼ�����˨�ϱ�ע��˨��ϸ
	if(nQuarterNum%2==0)
		nDimIndex=nQuarterNum/2;
	else 
		nDimIndex=nQuarterNum/2+1;
	f3dPoint bolt_dim_pos,fl_dim_pos,plate_dim_pos;
	double sector_angle=(2*Pi)/pConnectFL->bolt_n;
	f3dPoint first_bolt_pos(fBoltCirR,0);
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	for(i=0;i<pConnectFL->bolt_n;i++)
	{
		f3dPoint bolt_pos=first_bolt_pos;
		rotate_point_around_axis(bolt_pos,(i+0.5)*sector_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
		bolt_pos+=center;
		CreateAcadCircle(pBlockTableRecord,bolt_pos,1);
		if(i==nDimIndex-1)	//��¼��˨��Ϣ��עλ��
		{
			bolt_dim_pos=bolt_pos;
			bolt_dim_pos.x+=1;	//������˨�뾶
		}
		else if(i==pConnectFL->bolt_n-nDimIndex)
		{	//��¼������Ϣ��עλ��	
			fl_dim_pos=bolt_pos;
			fl_dim_pos.y+=2;	//Y�����ʵ���������ָ����˨	
		}
	}
	f3dPoint first_start(tube_r,0),first_end(0.5*W,0),sec_start(tube_r,0),sec_end(0.5*W,0);
	double offset_angle=asin(0.5*thick/tube_r);
	rotate_point_around_axis(first_start,offset_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
	rotate_point_around_axis(sec_start,-1.0*offset_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
	offset_angle=asin(thick/W);
	rotate_point_around_axis(first_end,offset_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
	rotate_point_around_axis(sec_end,-1.0*offset_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
	GetCurDwg()->setClayer(LayerTable::UnvisibleProfileLayer.layerId);
	for(i=0;i<pConnectFL->bolt_n;i++)
	{
		f3dPoint start=first_start,end=first_end;
		rotate_point_around_axis(start,i*sector_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
		rotate_point_around_axis(end,i*sector_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
		start+=center;
		end+=center;
		CreateAcadLine(pBlockTableRecord,start,end);
		if(i==nQuarterNum+nDimIndex)	
			plate_dim_pos=start+end;
		//
		start=sec_start;
		end=sec_end;
		rotate_point_around_axis(start,i*sector_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
		rotate_point_around_axis(end,i*sector_angle,f3dPoint(0,0,0),f3dPoint(0,0,1));
		start+=center;
		end+=center;
		CreateAcadLine(pBlockTableRecord,start,end);
		if(i==nQuarterNum+nDimIndex)	
		{	//��¼�߰��עλ��
			plate_dim_pos+=(start+end);
			plate_dim_pos*=0.25;
		}
	}
	//1.3��ע��˨��ֱ������������Բ�뾶����Բ�뾶���߰��źͷ������
	GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	DimStyleTable::dimRadiusStyle.InitDimStyle(sys.dim_map.fDimTextSize);
	// ��ע��Բ�뾶
	CXhChar100 sText("R%f",pConnectFL->m_fInnerRadius);
	SimplifiedNumString(sText);
	f3dPoint dim_pos(tube_r,0);
	rotate_point_around_axis(dim_pos,0.25*Pi,f3dPoint(0,0,0),f3dPoint(0,0,1));
	dim_pos+=center;
	DimRadial(pBlockTableRecord,center,dim_pos,0,sText,DimStyleTable::dimRadiusStyle.dimStyleId);
	// ��ע��Բ�뾶
	sText.Printf("R%f",pConnectFL->m_fInnerRadius+pConnectFL->m_nWidthFL);
	SimplifiedNumString(sText);
	dim_pos.Set(0.5*W,0);
	rotate_point_around_axis(dim_pos,-0.3*Pi,f3dPoint(0,0,0),f3dPoint(0,0,1));
	dim_pos+=center;
	DimRadial(pBlockTableRecord,center,dim_pos,0,sText,DimStyleTable::dimRadiusStyle.dimStyleId);
	// ��ע��˨�׾�������
	double fLineLen=0.5*W-tube_r;
#ifdef AFX_TARG_ENU_ENGLISH
	sText.Printf("%d%%%%C%d hole",pConnectFL->bolt_n,pConnectFL->bolt_d);
#else
	sText.Printf("%d%%%%C%d��",pConnectFL->bolt_n,pConnectFL->bolt_d);
#endif
	double fTextLen=sys.dim_map.fDimTextSize*(sText.GetLength()-1)*0.7;
	f3dPoint ptArr[3];
	ptArr[0]=bolt_dim_pos;
	ptArr[1].Set(bolt_dim_pos.x+0.866*fLineLen,bolt_dim_pos.y+0.5*fLineLen);	//30��
	ptArr[2].Set(ptArr[1].x+fTextLen,ptArr[1].y);
	dim_pos.Set(ptArr[1].x+0.5*fTextLen,ptArr[1].y+1);
	CreateAcadLine(pBlockTableRecord,ptArr[0],ptArr[1]);
	CreateAcadLine(pBlockTableRecord,ptArr[1],ptArr[2]);
	DimText(pBlockTableRecord,dim_pos,sText,TextStyleTable::hzfs.textStyleId,
		sys.dim_map.fDimTextSize,0,AcDb::kTextCenter);
	// ��ע�߰���
	sText.Copy("2");
	double radius=sys.dim_map.PartNo.fPartNoCirD*0.5;
	if(radius<=0)
		radius=sys.dim_map.PartNo.fPartNoTextSize*0.7*sText.GetLength()*0.5;
	line.startPt=plate_dim_pos;
	line.endPt.Set(plate_dim_pos.x-0.5*fLineLen,plate_dim_pos.y+0.866*fLineLen);	//60��
	dim_pos.Set(plate_dim_pos.x-0.5*(radius+fLineLen),plate_dim_pos.y+0.866*(radius+fLineLen));
	CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
	CreateAcadCircle(pBlockTableRecord,dim_pos,radius);
	DimText(pBlockTableRecord,dim_pos,sText,TextStyleTable::hzfs.textStyleId,
		sys.dim_map.PartNo.fPartNoTextSize,0,AcDb::kTextMid);
	// ��ע�������
	sText.Copy("1");
	line.startPt=fl_dim_pos;
	line.endPt.Set(fl_dim_pos.x+0.866*fLineLen,fl_dim_pos.y-0.5*fLineLen);
	dim_pos.Set(fl_dim_pos.x+0.866*(radius+fLineLen),fl_dim_pos.y-0.5*(radius+fLineLen));
	CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
	CreateAcadCircle(pBlockTableRecord,dim_pos,radius);
	DimText(pBlockTableRecord,dim_pos,sText,TextStyleTable::hzfs.textStyleId,
		sys.dim_map.PartNo.fPartNoTextSize,0,AcDb::kTextMid);
	//1.4��ע�������
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	double txt_size=7;
	sText.Printf("%s",pConnectFL->m_sPartNo);
	dim_pos.Set(center.x,center.y-0.5*(W+tube_r)-1.5*txt_size);
	DimText(pBlockTableRecord,dim_pos,sText,TextStyleTable::hzfs.textStyleId,txt_size,0,AcDb::kTextCenter);
	fTextLen=txt_size*sText.GetLength()*0.7;
	line.startPt.Set(dim_pos.x-0.5*fTextLen,dim_pos.y-1);
	line.endPt.Set(dim_pos.x+0.5*fTextLen,dim_pos.y-1);
	GetCurDwg()->setClayer(LayerTable::DimTextLayer.layerId);
	CreateAcadPolyLine(pBlockTableRecord,line.startPt,line.endPt,0.5);
	line.startPt.y-=1;
	line.endPt.y-=1;
	CreateAcadLine(pBlockTableRecord,line.startPt,line.endPt);
	//2.���Ʒ�������ͼ
	f3dPoint datum_pos=up_datum_pos;
	//2.1�ֹ�����ͼ����������ͼ���߰�ƽ��ͼ����˨�ס�������(��˨���ֹ�)���ֹ��ƶ���
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	f3dPoint startPt,endPt;
	// ��������ͼ
	startPt.Set(-0.5*W+datum_pos.x,datum_pos.y);
	endPt.Set(0.5*W+datum_pos.x,datum_pos.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt=endPt;
	endPt.y+=T;
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt=endPt;
	endPt.x-=B;
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt.Set(endPt.x-tube_thick,endPt.y);
	endPt.Set(startPt.x-2*tube_r,startPt.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt.Set(endPt.x-tube_thick,endPt.y);
	endPt.Set(startPt.x-B,startPt.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt=endPt;
	endPt.y-=T;
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	// �Ҳ�ֹ�����ͼ
	startPt.Set(tube_r+tube_thick+datum_pos.x,datum_pos.y);
	endPt.Set(tube_r+tube_thick+datum_pos.x,tube_len+datum_pos.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt.Set(tube_r+datum_pos.x,0.5*T+datum_pos.y);
	endPt.Set(tube_r+datum_pos.x,tube_len+datum_pos.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt.Set(tube_r+datum_pos.x,0.5*T+datum_pos.y);
	endPt.Set(tube_r+tube_thick+datum_pos.x,datum_pos.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt.Set(tube_r+datum_pos.x,0.5*T+datum_pos.y);
	endPt.Set(tube_r+tube_thick+datum_pos.x,0.5*T+datum_pos.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	// ���ֹ�����ͼ
	startPt.Set(-(tube_r+tube_thick)+datum_pos.x,datum_pos.y);
	endPt.Set(-(tube_r+tube_thick)+datum_pos.x,tube_len+datum_pos.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt.Set(-tube_r+datum_pos.x,0.5*T+datum_pos.y);
	endPt.Set(-tube_r+datum_pos.x,tube_len+datum_pos.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt.Set(-tube_r+datum_pos.x,0.5*T+datum_pos.y);
	endPt.Set(-tube_r-tube_thick+datum_pos.x,datum_pos.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt.Set(-tube_r+datum_pos.x,0.5*T+datum_pos.y);
	endPt.Set(-tube_r-tube_thick+datum_pos.x,0.5*T+datum_pos.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	// �Ҳ��߰�ƽ��ͼ
	f3dPoint platePtArr[8];
	platePtArr[0].Set(tube_r+tube_thick+datum_pos.x,A+T+datum_pos.y);
	platePtArr[1].Set(platePtArr[0].x+D,platePtArr[0].y);
	platePtArr[3].Set(0.5*W+datum_pos.x,T+datum_pos.y);
	platePtArr[2].Set(platePtArr[3].x,platePtArr[3].y+C);
	CreateAcadLine(pBlockTableRecord,platePtArr[0],platePtArr[1]);
	CreateAcadLine(pBlockTableRecord,platePtArr[1],platePtArr[2]);
	CreateAcadLine(pBlockTableRecord,platePtArr[2],platePtArr[3]);
	// ����߰�ƽ��ͼ
	platePtArr[4].Set(-(tube_r+tube_thick)+datum_pos.x,A+T+datum_pos.y);
	platePtArr[5].Set(platePtArr[4].x-D,platePtArr[4].y);
	platePtArr[7].Set(-0.5*W+datum_pos.x,T+datum_pos.y);
	platePtArr[6].Set(platePtArr[7].x,platePtArr[7].y+C);
	CreateAcadLine(pBlockTableRecord,platePtArr[4],platePtArr[5]);
	CreateAcadLine(pBlockTableRecord,platePtArr[5],platePtArr[6]);
	CreateAcadLine(pBlockTableRecord,platePtArr[6],platePtArr[7]);
	// ��˨��
	GetCurDwg()->setClayer(LayerTable::BoltSymbolLayer.layerId);
	startPt.Set(fBoltCirR-1+datum_pos.x,datum_pos.y);
	endPt.Set(startPt.x,T+datum_pos.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt.Set(fBoltCirR+1+datum_pos.x,datum_pos.y);
	endPt.Set(startPt.x,T+datum_pos.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt.Set(-(fBoltCirR-1)+datum_pos.x,datum_pos.y);
	endPt.Set(startPt.x,T+datum_pos.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt.Set(-(fBoltCirR+1)+datum_pos.x,datum_pos.y);
	endPt.Set(startPt.x,T+datum_pos.y);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	// ����
	GetCurDwg()->setClayer(LayerTable::AxisLineLayer.layerId);
	startPt.Set(datum_pos.x,datum_pos.y-3);
	endPt.Set(datum_pos.x,tube_len+datum_pos.y+6);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt.Set(fBoltCirR+datum_pos.x,datum_pos.y-3);
	endPt.Set(fBoltCirR+datum_pos.x,T+datum_pos.y+3);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	startPt.Set(-fBoltCirR+datum_pos.x,datum_pos.y-3);
	endPt.Set(-fBoltCirR+datum_pos.x,T+datum_pos.y+3);
	CreateAcadLine(pBlockTableRecord,startPt,endPt);
	// �����ƶ���
	startPt.Set(0.5*W+5+datum_pos.x,tube_len+datum_pos.y);
	endPt.Set(-(0.5*W+5)+datum_pos.x,tube_len+datum_pos.y);
	double len=0.0;
	f3dPoint line_vec,line_norm,midPt;
	line_vec=endPt-startPt;
	len=line_vec.mod();
	normalize(line_vec);
	line_norm.Set(-line_vec.y,line_vec.x);
	midPt=startPt+0.5*len*line_vec;
	GetCurDwg()->setClayer(LayerTable::BreakLineLayer.layerId);
	double symbol_width=3;		//�ƶϷ��Ŵ�С
	CreateAcadLine(pBlockTableRecord,startPt,midPt-symbol_width*line_vec);
	CreateAcadLine(pBlockTableRecord,midPt-symbol_width*line_vec,midPt+symbol_width*line_norm);
	CreateAcadLine(pBlockTableRecord,midPt+symbol_width*line_norm,midPt-symbol_width*line_norm);
	CreateAcadLine(pBlockTableRecord,midPt-symbol_width*line_norm,midPt+symbol_width*line_vec);
	CreateAcadLine(pBlockTableRecord,midPt+symbol_width*line_vec,endPt);
	//2.2��ע������Բֱ������Բֱ������˨�׵������ڲ��ȡ���˨�׵���������ȡ���������Լ��߰峤��߲���
	GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
	// ��ע������Բֱ��
	startPt.Set(-(tube_thick+tube_r)+datum_pos.x,datum_pos.y-1);
	endPt.Set(tube_thick+tube_r+datum_pos.x,datum_pos.y-1);
	dim_pos=0.5*(startPt+endPt);
	dim_pos.y-=(2*sys.dim_map.fDimTextSize+5);
	sText.Printf("%f",pConnectFL->m_fInnerRadius*2);
	SimplifiedNumString(sText);
	DimSize(pBlockTableRecord,startPt,endPt,dim_pos,sText,DimStyleTable::dimStyle.dimStyleId,2,sys.fonts.chief.fDimTextSize);
	// ��ע��˨����Բ�ľ���
	startPt=endPt;
	endPt.x=fBoltCirR+datum_pos.x;
	dim_pos=0.5*(startPt+endPt);
	dim_pos.y-=(2*sys.dim_map.fDimTextSize+5);
	sText.Printf("%f",pConnectFL->m_fBoltRadius-pConnectFL->m_fInnerRadius);
	SimplifiedNumString(sText);
	DimSize(pBlockTableRecord,startPt,endPt,dim_pos,sText,DimStyleTable::dimStyle.dimStyleId,2,sys.fonts.chief.fDimTextSize);
	// ��ע��˨����Բ�ľ���
	startPt=endPt;
	endPt.x=0.5*W+datum_pos.x;
	dim_pos=0.5*(startPt+endPt);
	dim_pos.y-=(2*sys.dim_map.fDimTextSize+5);
	sText.Printf("%f",pConnectFL->m_fInnerRadius+pConnectFL->m_nWidthFL-pConnectFL->m_fBoltRadius);
	SimplifiedNumString(sText);
	DimSize(pBlockTableRecord,startPt,endPt,dim_pos,sText,DimStyleTable::dimStyle.dimStyleId,2,sys.fonts.chief.fDimTextSize);
	// ��ע��Բֱ��
	startPt.Set(-0.5*W+datum_pos.x,datum_pos.y-1);
	endPt.Set(0.5*W+datum_pos.x,datum_pos.y-1);
	dim_pos=0.5*(startPt+endPt);
	dim_pos.y-=(4*sys.dim_map.fDimTextSize+5);
	sText.Printf("%f",(pConnectFL->m_fInnerRadius+pConnectFL->m_nWidthFL)*2);
	SimplifiedNumString(sText);
	DimSize(pBlockTableRecord,startPt,endPt,dim_pos,sText,DimStyleTable::dimStyle.dimStyleId,2,sys.fonts.chief.fDimTextSize);
	// ��ע�߰�ߴ�D
	startPt.Set(platePtArr[0].x,platePtArr[0].y+1);
	endPt.Set(platePtArr[1].x,platePtArr[1].y+1);
	dim_pos=0.5*(startPt+endPt);
	dim_pos.y+=sys.dim_map.fDimTextSize;
	sText.Printf("30");
	DimSize(pBlockTableRecord,startPt,endPt,dim_pos,sText,DimStyleTable::dimStyle.dimStyleId,2,sys.fonts.chief.fDimTextSize);
	// ��ע�߰�ߴ�C
	startPt.Set(platePtArr[2].x+1,platePtArr[2].y);
	endPt.Set(platePtArr[3].x+1,platePtArr[3].y);
	dim_pos=0.5*(startPt+endPt);
	dim_pos.x+=2*sys.dim_map.fDimTextSize;
	sText.Printf("30");
	DimSize(pBlockTableRecord,startPt,endPt,dim_pos,sText,DimStyleTable::dimStyle.dimStyleId,2,sys.fonts.chief.fDimTextSize);
	// �߰�߶�
	startPt.Set(platePtArr[1].x+1,platePtArr[1].y);
	endPt.Set(platePtArr[3].x+1,platePtArr[3].y);
	dim_pos.y=0.5*(startPt.y+endPt.y);
	dim_pos.x=platePtArr[3].x+4*sys.dim_map.fDimTextSize+1;
	sText.Printf("%d",pConnectFL->m_nHeightRib);
	DimSize(pBlockTableRecord,startPt,endPt,dim_pos,sText,DimStyleTable::dimStyle.dimStyleId,1,sys.fonts.chief.fDimTextSize);
	// �������
	startPt.Set(platePtArr[3].x+1,platePtArr[3].y);
	endPt.Set(platePtArr[3].x+1,datum_pos.y);
	dim_pos=0.5*(startPt+endPt);
	dim_pos.x+=4*sys.dim_map.fDimTextSize;
	sText.Printf("%d",pConnectFL->m_nThickFL);
	DimSize(pBlockTableRecord,startPt,endPt,dim_pos,sText,DimStyleTable::dimStyle.dimStyleId,1,sys.fonts.chief.fDimTextSize);
	// �ֹܶ�ͷ����������ĳߴ�
	startPt.Set(-(tube_r+tube_thick)+datum_pos.x+1,datum_pos.y);
	endPt.Set(-tube_r+datum_pos.x+1,datum_pos.y+0.5*T);
	dim_pos=0.5*(startPt+endPt);
	dim_pos.x+=2*sys.dim_map.fDimTextSize;
	sText.Printf("%f",pConnectFL->m_nThickFL*0.5);
	SimplifiedNumString(sText);
	DimSize(pBlockTableRecord,startPt,endPt,dim_pos,sText,DimStyleTable::dimStyle.dimStyleId,1,sys.fonts.chief.fDimTextSize);
	// ͼ�����
	fPtList vertexList;
	// ����Ҳ�ֹ�����ͼ
	vertexList.append(datum_pos.x+tube_r,datum_pos.y+0.5*T,0);
	vertexList.append(datum_pos.x+tube_r,datum_pos.y+tube_len,0);
	vertexList.append(datum_pos.x+tube_r+tube_thick,datum_pos.y+tube_len,0);
	vertexList.append(datum_pos.x+tube_r+tube_thick,datum_pos.y+0.5*T,0);
	CreateAcadHatch(pBlockTableRecord,vertexList,"ANSI31",0.3);
	// ������ֹ�����ͼ
	vertexList[0].Set(datum_pos.x-tube_r,datum_pos.y+0.5*T,0);
	vertexList[1].Set(datum_pos.x-tube_r,datum_pos.y+tube_len,0);
	vertexList[2].Set(datum_pos.x-tube_r-tube_thick,datum_pos.y+tube_len,0);
	vertexList[3].Set(datum_pos.x-tube_r-tube_thick,datum_pos.y+0.5*T,0);
	CreateAcadHatch(pBlockTableRecord,vertexList,"ANSI31",0.3);
	// �����෨������ͼ
	vertexList[0].Set(datum_pos.x-tube_r-tube_thick,datum_pos.y,0);
	vertexList[1].Set(datum_pos.x-fBoltCirR+1,datum_pos.y,0);
	vertexList[2].Set(datum_pos.x-fBoltCirR+1,datum_pos.y+T,0);
	vertexList[3].Set(datum_pos.x-tube_r-tube_thick,datum_pos.y+T,0);
	CreateAcadHatch(pBlockTableRecord,vertexList,"ANSI31",0.5);
	//
	vertexList[0].Set(datum_pos.x-fBoltCirR-1,datum_pos.y,0);
	vertexList[1].Set(datum_pos.x-0.5*W,datum_pos.y,0);
	vertexList[2].Set(datum_pos.x-0.5*W,datum_pos.y+T,0);
	vertexList[3].Set(datum_pos.x-fBoltCirR-1,datum_pos.y+T,0);
	CreateAcadHatch(pBlockTableRecord,vertexList,"ANSI31",0.5);
	// ����Ҳ෨������ͼ
	vertexList[0].Set(datum_pos.x+fBoltCirR+1,datum_pos.y,0);
	vertexList[1].Set(datum_pos.x+fBoltCirR+1,datum_pos.y+T,0);
	vertexList[2].Set(datum_pos.x+0.5*W,datum_pos.y+T,0);
	vertexList[3].Set(datum_pos.x+0.5*W,datum_pos.y,0);
	CreateAcadHatch(pBlockTableRecord,vertexList,"ANSI31",0.5);
	//
	vertexList[0].Set(datum_pos.x+tube_r+tube_thick,datum_pos.y,0);
	vertexList[1].Set(datum_pos.x+fBoltCirR-1,datum_pos.y,0);
	vertexList[2].Set(datum_pos.x+fBoltCirR-1,datum_pos.y+T,0);
	vertexList[3].Set(datum_pos.x+tube_r+tube_thick,datum_pos.y+T,0);
	CreateAcadHatch(pBlockTableRecord,vertexList,"ANSI31",0.5);
	// ����Ҳຸ��
	vertexList.Empty();
	vertexList.append(tube_r+tube_thick+datum_pos.x,datum_pos.y,0);
	vertexList.append(tube_r+datum_pos.x,0.5*T+datum_pos.y,0);
	vertexList.append(tube_r+tube_thick+datum_pos.x,0.5*T+datum_pos.y,0);
	CreateAcadHatch(pBlockTableRecord,vertexList,"SOLID",1);
	// �����ຸ��
	vertexList[0].Set(-tube_r-tube_thick+datum_pos.x,datum_pos.y,0);
	vertexList[1].Set(-tube_r-tube_thick+datum_pos.x,0.5*T+datum_pos.y,0);
	vertexList[2].Set(-tube_r+datum_pos.x,0.5*T+datum_pos.y,0);
	CreateAcadHatch(pBlockTableRecord,vertexList,"SOLID",1);
}

#endif
void FLPartBOM()
{
	//�������ø�ֵ
	sys.m_iCurMapType = 1;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	BOOL bDimByAnalysisMode = FALSE; 
#ifdef __LDS_FILE_
#ifdef _ARX_2007
	ACHAR result[20]={0};
#ifdef AFX_TARG_ENU_ENGLISH
	acedGetString(FALSE,L"draw structure map mode or [Mechanical diagram mode(S)]:",result);
#else
	acedGetString(FALSE,L"���ƽṹͼģʽ�� [��ѧ��ͼģʽ(S)]:",(ACHAR*)_bstr_t(result));
#endif
	if(wcsicmp(result,L"S")==0)
#else
	char result[20]="";
#ifdef AFX_TARG_ENU_ENGLISH
	acedGetString(FALSE,"draw structure map mode or [Mechanical diagram mode(S)]:",result);
#else
	acedGetString(FALSE,"���ƽṹͼģʽ�� [��ѧ��ͼģʽ(S)]:",result);
#endif
	if(stricmp(result,"S")==0)
#endif
		bDimByAnalysisMode=TRUE;
	else
		bDimByAnalysisMode=FALSE;
#elif defined(__TSA_FILE)||defined(__TSA_)
		bDimByAnalysisMode=TRUE;
#endif
	CFormatTbl xFLTbl;
	CSuperSmartPtr<CLDSPart>pPart;
	PARTSET flset;
	int i,j,nRows=2;
	if (!bDimByAnalysisMode)
	{
		for(pPart=Ta.Parts.GetFirst();pPart.IsHasPtr();pPart=Ta.Parts.GetNext())
		{
			if(pPart->GetClassTypeId()!=CLS_PARAMPLATE)
				continue;
			if(pPart.ParamPlatePointer()->IsFL()==FALSE)
				continue;
			CLDSParamPlate *pFL=pPart.ParamPlatePointer();
			CLDSPart *pLegacyFl=NULL;
			for(pLegacyFl=flset.GetFirst();pLegacyFl;pLegacyFl=flset.GetNext())
			{
				if(stricmp(pLegacyFl->GetPartNo(),pFL->GetPartNo())==0)
				{
					pLegacyFl->feature+=pFL->m_uStatMatCoef;
					break;
				}
			}
			if(pLegacyFl==NULL)
			{
				pFL->feature=pFL->m_uStatMatCoef;
				flset.append(pFL);
				nRows+=2;
				//��������¼��
				for(ATTACHPART_ITEM *pItem=pFL->AttachPart.GetFirst();pItem;pItem=pFL->AttachPart.GetNext())
				{
					pFL->AttachPart.push_stack();
					ATTACHPART_ITEM *pTmItem=NULL;
					for(pTmItem=pFL->AttachPart.GetNext();pTmItem;pTmItem=pFL->AttachPart.GetNext())
					{
						if(stricmp(pItem->sPartNo,pTmItem->sPartNo)==0&&pItem->idClsType==pTmItem->idClsType)
							break;
					}
					if(pTmItem==NULL)
						nRows++;	//�������Ӱ���û��ͬ��ţ��˱�Ų�Ԥͳ��
					pFL->AttachPart.pop_stack();
				}
				//������˨��¼��
				nRows+=pFL->GetLsRefList()->GetBoltRecordArr();
			}
		}
	}
#ifdef __PART_DESIGN_INC_//defined(__TSA_FILE_)||defined(__LDS_FILE_)
	else
	{
		

		f3dPoint origin(0,0),table_pos(0,50);
		int iOrder=0,nRowHigh=170,nColumnWidth=100;
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		for(CConnectFL *pHashFL = Ta.hashConnectFl.GetFirst();pHashFL;pHashFL=Ta.hashConnectFl.GetNext())
		{
			nRows+=2;
			//if (pHashFL->m_nThickRib>0)
				nRows++;	//�߰�
			if(sys.general_set.bCardLayoutBaseRow)
			{	//����Ϊ��׼��������
				origin.x = (iOrder%sys.general_set.nPartRowsN)*nColumnWidth;
				origin.y = -(iOrder/sys.general_set.nPartRowsN)*nRowHigh;
			}
			else
			{	//����Ϊ��׼��������
				origin.x = (iOrder/sys.general_set.nPartRowsN)*nColumnWidth;
				origin.y = -(iOrder%sys.general_set.nPartRowsN)*nRowHigh;
			}
			table_pos.x=max(table_pos.x,origin.x);
			//���Ʒ���
			CreateVirtualFL(pBlockTableRecord,origin,pHashFL);
			iOrder++;
		}
		table_pos.x+=70;
		pBlockTableRecord->close();
		xFLTbl.SetBasePos(f2dPoint(table_pos.x,table_pos.y));	//���ñ��λ��
	}
#endif
	xFLTbl.Create(nRows,10);
	//������ϸ�����
	for(j=0;j<7;j++)
		xFLTbl.GetGridAt(1,j)->m_bVGridVirtual=TRUE;
	xFLTbl.GetGridAt(1,9)->m_bVGridVirtual=TRUE;
	xFLTbl.GetGridAt(0,8)->m_bHGridVirtual=TRUE;
	xFLTbl.SetColumnWide(0,10);
	xFLTbl.SetColumnWide(1,8);
	xFLTbl.SetColumnWide(2,8);
	xFLTbl.SetColumnWide(3,16);
	xFLTbl.SetColumnWide(4,20);
	xFLTbl.SetColumnWide(5,8);
	xFLTbl.SetColumnWide(6,8);
	xFLTbl.SetColumnWide(7,16);
	xFLTbl.SetColumnWide(8,16);
	xFLTbl.SetColumnWide(9,16);
	xFLTbl.SetRowHigh(0,7.2);
	xFLTbl.SetRowHigh(1,7.2);
	for(i=2;i<nRows;i++)
		xFLTbl.SetRowHigh(i,4.8);

#ifdef AFX_TARG_ENU_ENGLISH
	xFLTbl.GetGridAt(0,0)->data.SetGridString("flange");
	xFLTbl.GetGridAt(0,0)->feature=1;
	xFLTbl.GetGridAt(0,1)->data.SetGridString("Number");
	xFLTbl.GetGridAt(0,1)->feature=1;
	xFLTbl.GetGridAt(0,2)->data.SetGridString("Name");
	xFLTbl.GetGridAt(0,2)->feature=1;
	xFLTbl.GetGridAt(0,3)->data.SetGridString("Spec");
	xFLTbl.GetGridAt(0,3)->feature=1;
	xFLTbl.GetGridAt(0,4)->data.SetGridString("length P(mm)");
	xFLTbl.GetGridAt(0,4)->feature=1;
	xFLTbl.GetGridAt(0,5)->data.SetGridString("unit");
	xFLTbl.GetGridAt(0,5)->feature=1;
	xFLTbl.GetGridAt(0,6)->data.SetGridString("piece of unit");
	xFLTbl.GetGridAt(0,6)->feature=1;
	xFLTbl.GetGridAt(0,7)->data.SetGridString("weight(kg)");
	xFLTbl.GetGridAt(0,7)->feature=1;
	xFLTbl.GetGridAt(1,7)->data.SetGridString("one piece");
	xFLTbl.GetGridAt(1,7)->feature=1;
	xFLTbl.GetGridAt(1,8)->data.SetGridString("subtotal");
	xFLTbl.GetGridAt(1,8)->feature=1;
	xFLTbl.GetGridAt(0,9)->data.SetGridString("notes");
#else
	xFLTbl.GetGridAt(0,0)->data.SetGridString("��\\P��");
	xFLTbl.GetGridAt(0,0)->feature=1;
	xFLTbl.GetGridAt(0,1)->data.SetGridString("��\\P��");
	xFLTbl.GetGridAt(0,1)->feature=1;
	xFLTbl.GetGridAt(0,2)->data.SetGridString("��\\P��");
	xFLTbl.GetGridAt(0,2)->feature=1;
	xFLTbl.GetGridAt(0,3)->data.SetGridString("��\\P��");
	xFLTbl.GetGridAt(0,3)->feature=1;
	xFLTbl.GetGridAt(0,4)->data.SetGridString("����\\P(mm)");
	xFLTbl.GetGridAt(0,4)->feature=1;
	xFLTbl.GetGridAt(0,5)->data.SetGridString("��\\Pλ");
	xFLTbl.GetGridAt(0,5)->feature=1;
	xFLTbl.GetGridAt(0,6)->data.SetGridString("����\\P����");
	xFLTbl.GetGridAt(0,6)->feature=1;
	xFLTbl.GetGridAt(0,7)->data.SetGridString("����(kg)");
	xFLTbl.GetGridAt(0,7)->feature=1;
	xFLTbl.GetGridAt(1,7)->data.SetGridString("һ��");
	xFLTbl.GetGridAt(1,7)->feature=1;
	xFLTbl.GetGridAt(1,8)->data.SetGridString("С��");
	xFLTbl.GetGridAt(1,8)->feature=1;
	xFLTbl.GetGridAt(0,9)->data.SetGridString("��ע");
#endif
	xFLTbl.GetGridAt(0,9)->feature=1;
	int iRow=2;
	for(i=0;i<nRows;i++)
	{
		for(j=0;j<10;j++)
		{
			if(i<2)
				xFLTbl.GetGridAt(i,j)->textsize=sys.dim_map.fNumericTextSize;
			else
			{
				xFLTbl.GetGridAt(i,j)->textsize=sys.dim_map.fNumericTextSize;
				xFLTbl.GetGridAt(i,j)->feature=1;
			}
		}
	}
#ifdef __PART_DESIGN_INC_//defined(__TSA_FILE_)||defined(__LDS_FILE_)
	if (bDimByAnalysisMode)
	{
		for(CConnectFL *pHashFL = Ta.hashConnectFl.GetFirst();pHashFL;pHashFL=Ta.hashConnectFl.GetNext())
		{
			xFLTbl.GetGridAt(iRow,0)->data.SetGridString(GetPartNoIncMat(pHashFL->m_sPartNo,pHashFL->cMaterial));
			xFLTbl.GetGridAt(iRow,1)->data.SetGridString("1");
#ifdef AFX_TARG_ENU_ENGLISH
			xFLTbl.GetGridAt(iRow,2)->data.SetGridString("flange");
#else
			xFLTbl.GetGridAt(iRow,2)->data.SetGridString("����");
#endif
			char spec[50]="",size[50]="";
			long nThickFL = pHashFL->m_nThickFL;
			if(pHashFL->cMaterial!='S')
				QuerySteelMatMark(pHashFL->cMaterial,spec);
			sprintf(spec,"%s-%d",spec,nThickFL);
			xFLTbl.GetGridAt(iRow,3)->data.SetGridString(spec);
			xFLTbl.GetGridAt(iRow,3)->feature=4;
			double R,W;
			R = pHashFL->m_fInnerRadius;
			W = (R+pHashFL->m_nWidthFL)*2;
			sprintf(spec,"%%%%C%f",R*2);
			SimplifiedNumString(spec);
			sprintf(size,"~%%%%C%f",W);	//ԭ���Ĳ���W��ʾFL��� ���ڵ�W��ʾ�⻷ֱ�� W==��0.5*W-R
			SimplifiedNumString(size);
			strcat(spec,size);
			xFLTbl.GetGridAt(iRow,4)->data.SetGridString(spec);
			xFLTbl.GetGridAt(iRow,4)->feature=4;
#ifdef AFX_TARG_ENU_ENGLISH
			xFLTbl.GetGridAt(iRow,5)->data.SetGridString("block");
#else
			xFLTbl.GetGridAt(iRow,5)->data.SetGridString("��");
#endif
			xFLTbl.GetGridAt(iRow,6)->data.SetGridString("1");
			double weight=CalSteelPlankWei(Pi*((0.5*W*0.5*W)-R*R)*nThickFL);
			sprintf(spec,"%.2f",weight);
			xFLTbl.GetGridAt(iRow,7)->data.SetGridString(spec);
			xFLTbl.GetGridAt(iRow,7)->feature=4;
			//С������
			sprintf(spec,"%.1f",weight);
			xFLTbl.GetGridAt(iRow,8)->data.SetGridString(spec);
			xFLTbl.GetGridAt(iRow,8)->feature=4;
#ifdef AFX_TARG_ENU_ENGLISH
			xFLTbl.GetGridAt(iRow,9)->data.SetGridString("electrical weld");
#else
			xFLTbl.GetGridAt(iRow,9)->data.SetGridString("�纸");
#endif
			xFLTbl.GetGridAt(iRow,9)->feature=4;
			iRow++;
			//�߰�
			//if (pHashFL->m_nThickRib>0)
			{
				xFLTbl.GetGridAt(iRow,1)->data.SetGridString("2");
#ifdef AFX_TARG_ENU_ENGLISH
				xFLTbl.GetGridAt(iRow,2)->data.SetGridString("rib plate");
#else
				xFLTbl.GetGridAt(iRow,2)->data.SetGridString("�߰�");
#endif
				strcpy(spec,"");
				if(pHashFL->cMaterial!='S')
					QuerySteelMatMark(pHashFL->cMaterial,spec);
				long nThickRib = pHashFL->m_nThickRib;
				sprintf(spec,"%s-%d",spec,nThickRib);
				xFLTbl.GetGridAt(iRow,3)->data.SetGridString(spec);
				xFLTbl.GetGridAt(iRow,3)->feature=4;
				double fHigh = pHashFL->m_nHeightRib;
				sprintf(spec,"%.2f",fHigh);
				SimplifiedNumString(spec);
				sprintf(size,"X%d",pHashFL->m_nWidthFL);
				strcat(spec,size);
				xFLTbl.GetGridAt(iRow,4)->data.SetGridString(spec);
				xFLTbl.GetGridAt(iRow,4)->feature=4;
#ifdef AFX_TARG_ENU_ENGLISH
				xFLTbl.GetGridAt(iRow,5)->data.SetGridString("block");
#else
				xFLTbl.GetGridAt(iRow,5)->data.SetGridString("��");
#endif
				long boltNum = pHashFL->bolt_n;
				sprintf(spec,"%d",boltNum);
				xFLTbl.GetGridAt(iRow,6)->data.SetGridString(spec);
				double weight=nThickRib*pHashFL->m_nWidthFL*fHigh*7.85e-6;
				sprintf(spec,"%.2f",weight);
				xFLTbl.GetGridAt(iRow,7)->data.SetGridString(spec);
				xFLTbl.GetGridAt(iRow,7)->feature=4;
				//С������
				sprintf(spec,"%.1f",weight*boltNum);
				xFLTbl.GetGridAt(iRow,8)->data.SetGridString(spec);
				xFLTbl.GetGridAt(iRow,8)->feature=4;
#ifdef AFX_TARG_ENU_ENGLISH
				xFLTbl.GetGridAt(iRow,9)->data.SetGridString("slope electric weld");
#else
				xFLTbl.GetGridAt(iRow,9)->data.SetGridString("�¿ڵ纸");
#endif
				xFLTbl.GetGridAt(iRow,9)->feature=4;
				xFLTbl.GetGridAt(iRow,0)->m_bVGridVirtual=TRUE;
				iRow++;
			}
			if (pHashFL->m_nThickRib>0)
				xFLTbl.GetGridAt(iRow,1)->data.SetGridString("3");
			else
				xFLTbl.GetGridAt(iRow,1)->data.SetGridString("2");
			xFLTbl.GetGridAt(iRow,0)->m_bVGridVirtual=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
			xFLTbl.GetGridAt(iRow,2)->data.SetGridString("bolt");
#else
			xFLTbl.GetGridAt(iRow,2)->data.SetGridString("��˨");
#endif
			xFLTbl.GetGridAt(iRow,2)->feature=4;
			long boltD = pHashFL->bolt_d;
			LS_XING_HAO *pLs = CLsLibrary::FindLsGuiGe(boltD,nThickFL+pHashFL->m_nCoupleThickFL,4);
			double lsLongth = 0;
			if (pLs)
			{
				lsLongth = pLs->valid_len;
				sprintf(spec,"%.1fM%d",pLs->grade,boltD);
			}
			else
				strcpy(spec,"?M?");
			xFLTbl.GetGridAt(iRow,3)->data.SetGridString(spec);
			xFLTbl.GetGridAt(iRow,3)->feature=4;
			sprintf(spec,"%.0f",lsLongth);
			xFLTbl.GetGridAt(iRow,4)->data.SetGridString(spec);
			xFLTbl.GetGridAt(iRow,4)->feature=4;
#ifdef AFX_TARG_ENU_ENGLISH
			xFLTbl.GetGridAt(iRow,5)->data.SetGridString("piece");
#else
			xFLTbl.GetGridAt(iRow,5)->data.SetGridString("ֻ");
#endif
			long boltNum = pHashFL->bolt_n;
			sprintf(spec,"%d",boltNum);
			xFLTbl.GetGridAt(iRow,6)->data.SetGridString(spec);
			//Ĭ��Ϊ��ð��˨
			weight=globalLibrary.GetLsWeight(boltD,ftol(lsLongth),4);
			sprintf(spec,"%f",weight);
			SimplifiedNumString(spec);
			xFLTbl.GetGridAt(iRow,7)->data.SetGridString(spec);
			xFLTbl.GetGridAt(iRow,7)->feature=4;
			sprintf(spec,"%f",weight*boltNum);
			SimplifiedNumString(spec);
			xFLTbl.GetGridAt(iRow,8)->data.SetGridString(spec);
			xFLTbl.GetGridAt(iRow,8)->feature=4;
			iRow++;
		}
	}
	else
#endif
	{
		PLATE_PTR *fl_arr = new PLATE_PTR[flset.GetNodeNum()];
		i=0;
		for(pPart=flset.GetFirst();pPart.IsHasPtr();pPart=flset.GetNext())
		{
			fl_arr[i]=pPart.ParamPlatePointer();
			i++;
		}
		CQuickSort<PLATE_PTR>::QuickSort(fl_arr,flset.GetNodeNum(),compare_plate);
		for(j=0;j<flset.GetNodeNum();j++)
		{
			CLDSParamPlate *pFL=fl_arr[j];
			xFLTbl.GetGridAt(iRow,0)->data.SetGridString(GetPartNoIncMat(pFL->GetPartNo(),pFL->cMaterial));
			xFLTbl.GetGridAt(iRow,1)->data.SetGridString("1");
#ifdef AFX_TARG_ENU_ENGLISH
			xFLTbl.GetGridAt(iRow,2)->data.SetGridString("plate");
#else
			xFLTbl.GetGridAt(iRow,2)->data.SetGridString("�ְ�");
#endif
			char spec[50],size[50];
			pFL->GetSpecification(spec,TRUE);
			xFLTbl.GetGridAt(iRow,3)->data.SetGridString(spec);
			xFLTbl.GetGridAt(iRow,3)->feature=4;
			double R,W;
			if(pFL->m_iParamType==TYPE_FL||
				pFL->m_iParamType==TYPE_FLG||
				pFL->m_iParamType==TYPE_FLR)
			{
				pFL->GetDesignItemValue('R',&R);	//�����ڻ��뾶
				pFL->GetDesignItemValue('W',&W);	//�����⻷ֱ��
			}
			else	
			{	//��׼����
				pFL->GetDesignItemValue('B',&R);
				pFL->GetDesignItemValue('D',&W);
				R*=0.5;
			}
			sprintf(spec,"%%%%C%f",R*2);
			SimplifiedNumString(spec);
			sprintf(size,"~%%%%C%f",W);	//ԭ���Ĳ���W��ʾFL��� ���ڵ�W��ʾ�⻷ֱ�� W==��0.5*W-R
			SimplifiedNumString(size);
			strcat(spec,size);
			xFLTbl.GetGridAt(iRow,4)->data.SetGridString(spec);
			xFLTbl.GetGridAt(iRow,4)->feature=4;
#ifdef AFX_TARG_ENU_ENGLISH
			xFLTbl.GetGridAt(iRow,5)->data.SetGridString("block");
#else
			xFLTbl.GetGridAt(iRow,5)->data.SetGridString("��");
#endif
			xFLTbl.GetGridAt(iRow,6)->data.SetGridString("1");
			//��������
			double weight=CalSteelPlankWei(Pi*((0.5*W*0.5*W)-R*R)*pFL->thick);
			sprintf(spec,"%.2f",weight);
			xFLTbl.GetGridAt(iRow,7)->data.SetGridString(spec);
			xFLTbl.GetGridAt(iRow,7)->feature=4;
			//С������
			sprintf(spec,"%.1f",weight);
			xFLTbl.GetGridAt(iRow,8)->data.SetGridString(spec);
			xFLTbl.GetGridAt(iRow,8)->feature=4;
#ifdef AFX_TARG_ENU_ENGLISH
			xFLTbl.GetGridAt(iRow,9)->data.SetGridString("electric weld");
#else
			xFLTbl.GetGridAt(iRow,9)->data.SetGridString("�纸");
#endif
			xFLTbl.GetGridAt(iRow,9)->feature=4;
			iRow++;
			//��������¼��
			int max_ribno=1;
			for(ATTACHPART_ITEM *pItem=pFL->AttachPart.GetFirst();pItem;pItem=pFL->AttachPart.GetNext())
			{
				pFL->AttachPart.push_stack();
				ATTACHPART_ITEM *pTmItem=NULL;
				for(pTmItem=pFL->AttachPart.GetNext();pTmItem;pTmItem=pFL->AttachPart.GetNext())
				{
					if(stricmp(pItem->sPartNo,pTmItem->sPartNo)==0&&pItem->idClsType==pTmItem->idClsType)
						break;
				}
				pFL->AttachPart.pop_stack();
				if(pTmItem==NULL)
				{
					int hits=1;
					pFL->AttachPart.push_stack();
					for(ATTACHPART_ITEM *pStatItem=pFL->AttachPart.GetFirst();pStatItem;pStatItem=pFL->AttachPart.GetNext())
					{
						if(pStatItem==pItem)
							continue;
						else if(pItem->idClsType==pStatItem->idClsType&&stricmp(pItem->sPartNo,pStatItem->sPartNo)==0)
							hits++;	//�˴��ݻ�δ����ͳ��ϵ������
					}
					pFL->AttachPart.pop_stack();
					strcpy(spec,pItem->sPartNo);
					strtok(spec,"-_#");
					char *serialno=strtok(NULL,"-_#");
					if(serialno)
						xFLTbl.GetGridAt(iRow,1)->data.SetGridString(serialno);
					else
						serialno=spec;
					if(atoi(serialno)>max_ribno)
						max_ribno=atoi(serialno);
#ifdef AFX_TARG_ENU_ENGLISH
					xFLTbl.GetGridAt(iRow,2)->data.SetGridString("plate");
#else
					xFLTbl.GetGridAt(iRow,2)->data.SetGridString("�ְ�");
#endif
					CSuperSmartPtr<CLDSPart> pRibPlate=Ta.FromPartHandle(pItem->h);
					if(pRibPlate.IsHasPtr()&&(pRibPlate->GetClassTypeId()==CLS_PLATE||pRibPlate->GetClassTypeId()==CLS_PARAMPLATE))
					{
						pRibPlate->GetSpecification(spec,TRUE);
						xFLTbl.GetGridAt(iRow,3)->data.SetGridString(spec);
						xFLTbl.GetGridAt(iRow,3)->feature=4;
						double length=pRibPlate->GetLength();
						double width=pRibPlate->GetWidth();
						sprintf(spec,"%.0f",length);
						xFLTbl.GetGridAt(iRow,4)->data.SetGridString(spec);
						xFLTbl.GetGridAt(iRow,4)->feature=4;
#ifdef AFX_TARG_ENU_ENGLISH
						xFLTbl.GetGridAt(iRow,5)->data.SetGridString("block");
#else
						xFLTbl.GetGridAt(iRow,5)->data.SetGridString("��");
#endif
						sprintf(spec,"%d",hits);
						xFLTbl.GetGridAt(iRow,6)->data.SetGridString(spec);
						double thick=0;
						if(pRibPlate->GetClassTypeId()==CLS_PLATE)
							thick=pRibPlate.PlatePointer()->GetThick();
						else
							thick=pRibPlate.ParamPlatePointer()->thick;
						double weight=CalSteelPlankWei(length*width*thick);
						sprintf(spec,"%.2f",weight);
						xFLTbl.GetGridAt(iRow,7)->data.SetGridString(spec);
						xFLTbl.GetGridAt(iRow,7)->feature=4;
						sprintf(spec,"%.1f",weight*hits);
						xFLTbl.GetGridAt(iRow,8)->data.SetGridString(spec);
						xFLTbl.GetGridAt(iRow,8)->feature=4;
					}
					xFLTbl.GetGridAt(iRow,0)->m_bVGridVirtual=TRUE;
#ifdef AFX_TARG_ENU_ENGLISH
					xFLTbl.GetGridAt(iRow,9)->data.SetGridString("slope electric weld");
#else
					xFLTbl.GetGridAt(iRow,9)->data.SetGridString("�¿ڵ纸");
#endif
					xFLTbl.GetGridAt(iRow,9)->feature=4;
					iRow++;	//�������Ӱ���û��ͬ��ţ��˱�Ų�Ԥͳ��
				}
			}
			//������˨��¼��
			int nBoltRec=pFL->GetLsRefList()->GetBoltRecordArr();
			BOLT_RECORD *recArr = new BOLT_RECORD[nBoltRec];
			pFL->GetLsRefList()->GetBoltRecordArr(recArr);
			for(i=0;i<nBoltRec;i++)
			{
				xFLTbl.GetGridAt(iRow+i,0)->m_bVGridVirtual=TRUE;
				max_ribno++;
				sprintf(spec,"%d",max_ribno);
				xFLTbl.GetGridAt(iRow+i,1)->data.SetGridString(spec);
#ifdef AFX_TARG_ENU_ENGLISH
				xFLTbl.GetGridAt(iRow+i,2)->data.SetGridString("bolts");
#else
				xFLTbl.GetGridAt(iRow+i,2)->data.SetGridString("��˨");
#endif
				sprintf(spec,"M%d",recArr[i].d);
				xFLTbl.GetGridAt(iRow+i,3)->data.SetGridString(spec);
				xFLTbl.GetGridAt(iRow+i,3)->feature=4;
				sprintf(spec,"%f",recArr[i].L);
				SimplifiedNumString(spec);
				xFLTbl.GetGridAt(iRow+i,4)->data.SetGridString(spec);
				xFLTbl.GetGridAt(iRow+i,4)->feature=4;
#ifdef AFX_TARG_ENU_ENGLISH
				xFLTbl.GetGridAt(iRow+i,5)->data.SetGridString("piece");
#else
				xFLTbl.GetGridAt(iRow+i,5)->data.SetGridString("ֻ");
#endif
				sprintf(spec,"%d",recArr[i].num);
				xFLTbl.GetGridAt(iRow+i,6)->data.SetGridString(spec);
				//weigth=0;
				//if(recArr[i].m_bTwoCapLs)
					weight=globalLibrary.GetLsWeight(recArr[i].d,ftol(recArr[i].L),recArr[i].m_hFamily);
				//else
				//	weight=FindLsWei(recArr[i].d,ftol(recArr[i].L),0);
				sprintf(spec,"%f",weight);
				SimplifiedNumString(spec);
				xFLTbl.GetGridAt(iRow+i,7)->data.SetGridString(spec);
				xFLTbl.GetGridAt(iRow+i,7)->feature=4;
				sprintf(spec,"%f",weight*recArr[i].num);
				SimplifiedNumString(spec);
				xFLTbl.GetGridAt(iRow+i,8)->data.SetGridString(spec);
				xFLTbl.GetGridAt(iRow+i,8)->feature=4;
				strcpy(spec,"");
				if(recArr[i].m_bAntiTheft)
#ifdef AFX_TARG_ENU_ENGLISH
					strcat(spec,"anti-theft");
				if(recArr[i].m_bAntiLoose)
					strcat(spec,"Anti-Loose Washer");
#else
					strcat(spec,"����");
				if(recArr[i].m_bAntiLoose)
					strcat(spec,"���ɵ�");
#endif
				xFLTbl.GetGridAt(iRow+i,0)->m_bVGridVirtual=TRUE;
			}
			xFLTbl.GetGridAt(iRow+nBoltRec,0)->m_bVGridVirtual=TRUE;
			iRow+=nBoltRec+1;
			delete []recArr;
		}
		delete []fl_arr;
	}
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	DrawTable(pBlockTableRecord,&xFLTbl,FALSE);
	pBlockTableRecord->close();
}
#else
void DimSegI(){;}
void SumTaMat(){;}
void FLPartBOM(){;}
#endif
//�Ǹ�׼���
void JgZhunJuTable()
{
	//�������ø�ֵ
	sys.m_iCurMapType = 1;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	int i,j,nRows=zhunju_N+1;
	CFormatTbl xFLTbl;
	CGrid::PREFER_TEXT_SIZE=sys.dim_map.fNumericTextSize;
	xFLTbl.Create(nRows,5);
	//������ϸ�����
	xFLTbl.SetColumnWide(0,16);
	xFLTbl.SetColumnWide(1,12);
	xFLTbl.SetColumnWide(2,12);
	xFLTbl.SetColumnWide(3,12);
	xFLTbl.SetColumnWide(4,12); 
	for(i=0;i<nRows;i++)
		xFLTbl.SetRowHigh(i,6);
#ifdef AFX_TARG_ENU_ENGLISH
	xFLTbl.GetGridAt(0,0)->data.SetGridString("leg width\\criterion");
#else
	xFLTbl.GetGridAt(0,0)->data.SetGridString("֫��\\׼��");
#endif
	xFLTbl.GetGridAt(0,0)->feature=1;
	xFLTbl.GetGridAt(0,1)->data.SetGridString("g");
	xFLTbl.GetGridAt(0,1)->feature=1;
	xFLTbl.GetGridAt(0,2)->data.SetGridString("g1");
	xFLTbl.GetGridAt(0,2)->feature=1;
	xFLTbl.GetGridAt(0,3)->data.SetGridString("g2");
	xFLTbl.GetGridAt(0,3)->feature=1;
	xFLTbl.GetGridAt(0,4)->data.SetGridString("g3");
	xFLTbl.GetGridAt(0,4)->feature=1;
	for(j=1;j<nRows;j++)
	{
		CXhChar100 sText("%d",zhunju_table[j-1].wide);
		xFLTbl.GetGridAt(j,0)->data.SetGridString(sText);
		sText.Printf("%d",zhunju_table[j-1].g);
		xFLTbl.GetGridAt(j,1)->data.SetGridString(sText);
		sText.Printf("%d",zhunju_table[j-1].g1);
		xFLTbl.GetGridAt(j,2)->data.SetGridString(sText);
		sText.Printf("%d",zhunju_table[j-1].g2);
		xFLTbl.GetGridAt(j,3)->data.SetGridString(sText);
		sText.Printf("%d",zhunju_table[j-1].g3);
		xFLTbl.GetGridAt(j,4)->data.SetGridString(sText);
	}
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	DrawTable(pBlockTableRecord,&xFLTbl);
	pBlockTableRecord->close();
}
#ifndef __TSA_FILE_
#include "LicFuncDef.h"
#include "LocalFeatureDef.h"
//������άʵ��
void DrawSolid()
{
	if(  GetLicVersion()<1000003||
		(!VerifyValidFunction(LICFUNC::FUNC_IDENTITY_DWGSOLID_EXPORT)&&ValidateLocalizeFeature(FEATURE::EXPORT_3DSOLID_DWGMODEL)==0))
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("lack of authority to use export autoCAD three-dimension solid model");
#else
		AfxMessageBox("ȱ�����AutoCAD��άʵ��ģ�͵�ʹ����Ȩ");
#endif
		return;
	}
	//�������ģ��
	CAcModuleResourceOverride resOverride;
	CSetActiveModuleDlg active_module_dlg;
	if (active_module_dlg.DoModal() != IDOK)
		return;
	CLDSModule *pModule = Ta.GetActiveModule();
	if (pModule == NULL)
		return;
	//����ʵ��ͼԪ
	InitDrawingEnv();
	double height = 0;
	pModule->GetModuleScopeZ(NULL, &height);
	UCS_STRU ucs;
	ucs.origin.Set(0, 0, height);
	ucs.axis_x.Set(1, 0, 0);
	ucs.axis_y.Set(0, -1, 0);
	ucs.axis_z.Set(0, 0, -1);
	Draw3DSolid(ucs,TRUE);
#ifdef _ARX_2007
	acedCommand(RTSTR, L"-view", RTSTR, L"_front", RTNONE);
	acedCommand(RTSTR, L"zoom", RTSTR, L"e", RTNONE);
	acedCommand(RTSTR, L"_shademode", RTSTR, L"_g", RTNONE);
#else
	acedCommand(RTSTR, "-view", RTSTR, "_front", RTNONE);
	acedCommand(RTSTR, "zoom", RTSTR, "e", RTNONE);
	acedCommand(RTSTR, "_shademode", RTSTR, "_g", RTNONE);
#endif
}
//������ά����
void Draw3DLine()
{
	//�������ģ��
	CAcModuleResourceOverride resOverride;
	CSetActiveModuleDlg active_module_dlg;
	if (active_module_dlg.DoModal() != IDOK)
		return;
	CLDSModule *pModule = Ta.GetActiveModule();
	if (pModule == NULL)
		return;
	//���Ƶ���ͼԪ
	InitDrawingEnv();
	double height = 0;
	pModule->GetModuleScopeZ(NULL,&height);
	UCS_STRU ucs;
	ucs.origin.Set(0, 0, height);
	ucs.axis_x.Set(1, 0, 0);
	ucs.axis_y.Set(0, -1, 0);
	ucs.axis_z.Set(0, 0, -1);
	Draw3DLineMap(ucs);
#ifdef _ARX_2007
	acedCommand(RTSTR, L"-view", RTSTR, L"_front", RTNONE);
	acedCommand(RTSTR, L"zoom", RTSTR, L"e", RTNONE);
#else
	acedCommand(RTSTR, "-view", RTSTR, "_front", RTNONE);
	acedCommand(RTSTR, "zoom", RTSTR, "e", RTNONE);
#endif
}
#endif
//////////////////////////////////////////////////////////////////////////
//���ܵ��������ƽ���άʵ������
//���ݲ������Ҫ����ʾ��ά����ͼ
static BOOL IsPublicPart(CLDSPart* pPart, ATOM_LIST<int>& bodyNoList)
{
	if (pPart->IsLegObj())	//�Ȳ��Ĺ���
		return FALSE;
	for (int i = 0; i < bodyNoList.GetNodeNum(); i++)
	{
		if (!pPart->cfgword.IsHasBodyNo(bodyNoList[i]))
			return FALSE;
	}
	return TRUE;
}
void Draw3DLineBC()
{
	PARTSET partSet, legPartSet, boltSet;
	if (Ta.GetActiveModule() == NULL)
		return;
	InitDrawingEnv();
	double scale = 1000, fDistance = 60000;		//��άʵ�嵥������λ��λ
	//1��ȷ������ϵ������ԭ��λ����ʹ�
	double max_z = 0;
	for (CLDSNode* pNode = Ta.EnumNodeFirst(); pNode; pNode = Ta.EnumNodeNext())
	{
		f3dPoint pos = pNode->Position();
		if (pos.z > max_z)
			max_z = pos.z;
	}
	UCS_STRU ucs;
	ucs.origin.Set(0, 0, max_z);
	ucs.axis_x.Set(1, 0, 0);
	ucs.axis_y.Set(0, -1, 0);
	ucs.axis_z.Set(0, 0, -1);
	//2����ȡ�������к��߱����,
	CLDSModule *pModule = NULL;
	ATOM_LIST<int> bodyNoList;
	for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
		bodyNoList.append(pModule->GetBodyNo());
	//3�����������ж�
	int pushed = Ta.Parts.push_stack();
	for (CLDSPart* pPart = Ta.EnumPartFirst(); pPart; pPart = Ta.EnumPartNext())
	{
		if (pPart->GetClassTypeId() == CLS_BOLT)
		{
			CLDSBolt* pBolt = (CLDSBolt*)pPart;
			if (pBolt->FuncType() == CLDSBolt::FUNC_WIREHOLE)
				boltSet.append(pPart);
		}
		if (!pPart->IsLinePart())
			continue;
		if (IsPublicPart(pPart, bodyNoList))
			partSet.append(pPart);
		else
			legPartSet.append(pPart);
	}
	Ta.Parts.pop_stack(pushed);
	//ˢѡ���߹���
	CHashList<CLDSPart*> partHashList;
	for (CLDSBolt* pBolt = (CLDSBolt*)boltSet.GetFirst(); pBolt; pBolt = (CLDSBolt*)boltSet.GetNext())
	{
		CLDSPart* pGuaPart = NULL;
		int pushed = Ta.Parts.push_stack();
		for (pGuaPart = Ta.EnumPartFirst(); pGuaPart; pGuaPart = Ta.EnumPartNext())
		{
			if (pGuaPart->FindLsByHandle(pBolt->handle))
				break;
		}
		Ta.Parts.pop_stack(pushed);
		if (pGuaPart == NULL)
			continue;
		if (partHashList.GetValue(pGuaPart->handle) == NULL)
			partHashList.SetValue(pGuaPart->handle, pGuaPart);
	}
	for (CLDSPart* pPart = partSet.GetFirst(); pPart; pPart = partSet.GetNext())
	{
		int pushed = partSet.push_stack();
		if (partHashList.GetValue(pPart->handle))
		{
			partSet.DeleteNode(pPart->handle);
			partSet.pop_stack(pushed);
		}
	}

	DrawSingLine3DMap(partSet, ucs, scale);
	//4�����Ƹ��������ж�
	BOOL bOnlyOneLeg = FALSE;
	for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
	{
		partSet.Empty();
		for (CLDSPart* pPart = legPartSet.GetFirst(); pPart; pPart = legPartSet.GetNext())
		{
			if (pPart->IsLegObj() && pModule->m_dwLegCfgWord.And(pPart->cfgword))	//�Ȳ�����
			{
				if (towupper(pPart->Layer(1)) == 'Z' || towupper(pPart->Layer(1)) == 'X')
					partSet.append(pPart);
			}
			else if (pPart->cfgword.IsHasBodyNo(pModule->GetBodyNo()))	//����˼�
				partSet.append(pPart);
		}
		ucs.origin.x -= fDistance;
		DrawSingLine3DMap(partSet, ucs, scale);
	}
	//5�����ƹ��߿׼��Ҽ�
	COLORREF crSolid = RGB(255, 255, 255);
	ucs.origin.x = 0;
	for (CLDSPart** ppPart = partHashList.GetFirst(); ppPart; ppPart = partHashList.GetNext())
	{
		if ((*ppPart)->GetClassTypeId() == CLS_LINEANGLE)
		{
			CLDSLineAngle* pAngle = (CLDSLineAngle*)(*ppPart);
			DrawSolidAngle(pAngle, &ucs, scale, &crSolid);
		}
		else if ((*ppPart)->GetClassTypeId() == CLS_PLATE)
		{
			CLDSPlate* pPlate = (CLDSPlate*)(*ppPart);
			DrawSolidPlate(pPlate, &ucs, scale, &crSolid);
		}
	}
#ifdef _ARX_2007
	acedCommand(RTSTR, L"-view", RTSTR, L"_front", RTNONE);
	acedCommand(RTSTR, L"zoom", RTSTR, L"e", RTNONE);
#else
	acedCommand(RTSTR, "-view", RTSTR, "_front", RTNONE);
	acedCommand(RTSTR, "zoom", RTSTR, "e", RTNONE);
#endif
}
#ifdef __RAPID_ROUGH_3DMODEL_
void ShiftSoildShadeMode(const char* sDwgFolder)
{
	if (strlen(sDwgFolder) <= 0)
		return;
	//��ָ��·���²���DWG�ļ�
	CXhChar200 sFindFile("%s\\*.dwg", (char*)sDwgFolder);
	ATOM_LIST<CString>  file_list;
	CFileFind file_find;
	BOOL bFind = file_find.FindFile(sFindFile);
	while (bFind)
	{
		bFind = file_find.FindNextFile();
		if (file_find.IsDots() || file_find.IsHidden() || file_find.IsReadOnly() ||
			file_find.IsSystem() || file_find.IsTemporary() || file_find.IsDirectory())
			continue;
		CString file_path = file_find.GetFilePath();
		CString str_ext = file_path.Right(4);	//ȡ��׺��
		str_ext.MakeLower();
		if (str_ext.CompareNoCase(".dwg") != 0)
			continue;
		file_list.append(file_path);
	}
	file_find.Close();
	//��DWG�ļ����޸�ʵ����ʾģʽ
	AcApDocument* pOldDoc = acDocManager->mdiActiveDocument();
	for (int i = 0; i < file_list.GetNodeNum(); i++)
	{
		AcApDocument *pDoc = OpenDwgFile(CXhChar500(file_list[i]));
		if (pDoc == NULL)
			continue;
		//acDocManager->activateDocument(pDoc);
		//�л�����ʵ�����ʾģʽ(��������ɫ)
		acDocManager->lockDocument(pDoc, AcAp::kWrite, NULL, NULL, true);
#ifdef _ARX_2007
		acDocManager->sendStringToExecute(pDoc, _bstr_t(CXhChar200("-view _front\n")), false);
		acDocManager->sendStringToExecute(pDoc, _bstr_t(CXhChar200("zoom e\n")), false);
		acDocManager->sendStringToExecute(pDoc, _bstr_t(CXhChar200("_shademode _g\n")), false);
#else
		acDocManager->sendStringToExecute(pDoc,CXhChar200("-view _front\n"),false);
		acDocManager->sendStringToExecute(pDoc, CXhChar200("zoom e\n"), false);
		acDocManager->sendStringToExecute(pDoc, CXhChar200("_shademode _g\n"), false);
#endif
		acDocManager->unlockDocument(pDoc);
		//acDocManager->closeDocument(pDoc);
	}
}
//���ƹ���Ҫ�����ά����
void Draw3dLineGW()
{
	if (Ta.GetActiveModule() == NULL)
		return;
	if (strlen(sys.sFilePath) <= 0)
		return;
	CString file_path(sys.sFilePath);
	int index = file_path.ReverseFind('\\');	//�������'\\'
	file_path = file_path.Left(index);		//�Ƴ��ļ���
	CXhChar100 sWorkDir("%s\\%s", file_path, g_sFileName);
	MakeDirectory(sWorkDir);
	//
	UCS_STRU ucs;
	ucs.origin.Set(0, 0, Ta.NamedHeightZeroZ);
	ucs.axis_x.Set(1, 0, 0);
	ucs.axis_y.Set(0, -1, 0);
	ucs.axis_z.Set(0, 0, -1);
	//
	PARTSET bodyPartSet, legPartSet;
	CLDSModule *pModule = NULL;
	ATOM_LIST<int> bodyNoList;
	for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
		bodyNoList.append(pModule->GetBodyNo());
	int pushed = Ta.Parts.push_stack();
	for (CLDSPart* pPart = Ta.EnumPartFirst(); pPart; pPart = Ta.EnumPartNext())
	{
		if (pPart->m_bVirtualPart)
			continue;
		if (!pPart->IsLinePart())
			continue;
		if (IsPublicPart(pPart, bodyNoList))
			bodyPartSet.append(pPart);
		else
			legPartSet.append(pPart);
	}
	Ta.Parts.pop_stack(pushed);
	//����������
	if (bodyPartSet.GetNodeNum() > 0)
	{
		CNewDatabaseLife newDb;
		Draw3DLineMap(bodyPartSet, ucs);
		CXhChar200 sFullPath("%s\\body-L.dwg", (char*)sWorkDir);
		if (!newDb.SaveAs(sFullPath))
			return;
	}
	//���Ƹ���������������
	PARTSET segPartSet;
	for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
	{
		segPartSet.Empty();
		for (CLDSPart* pPart = legPartSet.GetFirst(); pPart; pPart = legPartSet.GetNext())
		{
			if (pPart->IsLegObj())
				continue;
			if (pModule->IsSonPart(pPart))
				segPartSet.append(pPart);
		}
		if (segPartSet.GetNodeNum())
		{
			CNewDatabaseLife newDb;
			Draw3DLineMap(segPartSet, ucs);
			CXhChar200 sFullPath("%s\\%s-L.dwg", (char*)sWorkDir, pModule->description);
			if (!newDb.SaveAs(sFullPath))
				return;
		}
	}
	//���Ƹ����߽��ȵ���
	PARTSET partSet;
	int nMaxLegs = CFGLEG::MaxLegs();
	for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
	{
		DWORD dwIncLegItemFlag = pModule->m_dwLegCfgWord.GetBodyLegs(pModule->idBodyNo);
		for (int i = 0; i < 32; i++)
		{
			if (dwIncLegItemFlag % 2 == 0)
				continue;
			dwIncLegItemFlag >>= 1;
			double heightdiff = i* pModule->uiLevelHeight*0.001;
			CXhChar16 szLegItem("-%3.1fm",heightdiff);
			pModule->m_arrActiveQuadLegNo[0] = (i + 1) + (pModule->idBodyNo - 1)*nMaxLegs;
			pModule->m_arrActiveQuadLegNo[1] = (i + 1) + (pModule->idBodyNo - 1)*nMaxLegs;
			pModule->m_arrActiveQuadLegNo[2] = (i + 1) + (pModule->idBodyNo - 1)*nMaxLegs;
			pModule->m_arrActiveQuadLegNo[3] = (i + 1) + (pModule->idBodyNo - 1)*nMaxLegs;
			//
			partSet.Empty();
			for (CLDSPart* pPart = legPartSet.GetFirst(); pPart; pPart = legPartSet.GetNext())
			{	//ֻȡA��
				if(!pPart->IsLegObj())
					continue;
				BYTE cLegQuad = pPart->ciLegQuad;
				if(cLegQuad ==2 && pPart->cfgword.IsHasNo(pModule->m_arrActiveQuadLegNo[1]))
					partSet.append(pPart);
			}
			if (partSet.GetNodeNum())
			{
				CNewDatabaseLife newDb;
				Draw3DLineMap(partSet, ucs);
				CXhChar200 sFullPath("%s\\%s%s-L.dwg", (char*)sWorkDir, pModule->description,(char*)szLegItem);
				if (!newDb.SaveAs(sFullPath))
					return;
			}
		}
	}
	MessageBox(adsw_acadMainWnd(),"�ļ��������!","��ʾ",MB_OK);
}
//���ƹ���Ҫ�����άʵ��
void DrawSolidGW()
{
	if (Ta.GetActiveModule() == NULL)
		return;
	if (strlen(sys.sFilePath) <= 0)
		return;
	CString file_path(sys.sFilePath);
	int index = file_path.ReverseFind('\\');	//�������'\\'
	file_path = file_path.Left(index);		//�Ƴ��ļ���
	CXhChar100 sWorkDir("%s\\%s", file_path, g_sFileName);
	MakeDirectory(sWorkDir);
	//��ʼ����ͼ����ϵ
	UCS_STRU ucs;
	ucs.origin.Set(0, 0, Ta.NamedHeightZeroZ);
	ucs.axis_x.Set(1, 0, 0);
	ucs.axis_y.Set(0, -1, 0);
	ucs.axis_z.Set(0, 0, -1);
	//
	PARTSET bodyPartSet, legPartSet;
	CLDSModule *pModule = NULL;
	ATOM_LIST<int> bodyNoList;
	for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
		bodyNoList.append(pModule->GetBodyNo());
	int pushed = Ta.Parts.push_stack();
	for (CLDSPart* pPart = Ta.EnumPartFirst(); pPart; pPart = Ta.EnumPartNext())
	{
		if(pPart->m_bVirtualPart)
			continue;
		if (IsPublicPart(pPart, bodyNoList))
			bodyPartSet.append(pPart);
		else
			legPartSet.append(pPart);
	}
	Ta.Parts.pop_stack(pushed);
	//����������άʵ��
	if (bodyPartSet.GetNodeNum() > 0)
	{
		CNewDatabaseLife newDb;
		Draw3DSolid(bodyPartSet, ucs,TRUE);
		CXhChar200 sFullPath("%s\\body-S.dwg", (char*)sWorkDir);
		if (!newDb.SaveAs(sFullPath))
			return;
	}
	//���Ƹ�������������ʵ��
	PARTSET segPartSet;
	for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
	{
		segPartSet.Empty();
		for (CLDSPart* pPart = legPartSet.GetFirst(); pPart; pPart = legPartSet.GetNext())
		{
			if (pPart->IsLegObj())
				continue;
			if (pModule->IsSonPart(pPart))
				segPartSet.append(pPart);
		}
		if (segPartSet.GetNodeNum())
		{
			CNewDatabaseLife newDb;
			Draw3DSolid(segPartSet, ucs,TRUE);
			CXhChar200 sFullPath("%s\\%s-S.dwg", (char*)sWorkDir, pModule->description);
			if (!newDb.SaveAs(sFullPath))
				return;
		}
	}
	//���Ƹ����߽���ʵ��
	PARTSET partSet;
	int nMaxLegs = CFGLEG::MaxLegs();
	for (pModule = Ta.Module.GetFirst(); pModule; pModule = Ta.Module.GetNext())
	{
		DWORD dwIncLegItemFlag = pModule->m_dwLegCfgWord.GetBodyLegs(pModule->idBodyNo);
		for (int i = 0; i < 32; i++)
		{
			if (dwIncLegItemFlag % 2 == 0)
				continue;
			dwIncLegItemFlag >>= 1;
			double heightdiff = i * pModule->uiLevelHeight*0.001;
			CXhChar16 szLegItem("-%3.1fm", heightdiff);
			pModule->m_arrActiveQuadLegNo[0] = (i + 1) + (pModule->idBodyNo - 1)*nMaxLegs;
			pModule->m_arrActiveQuadLegNo[1] = (i + 1) + (pModule->idBodyNo - 1)*nMaxLegs;
			pModule->m_arrActiveQuadLegNo[2] = (i + 1) + (pModule->idBodyNo - 1)*nMaxLegs;
			pModule->m_arrActiveQuadLegNo[3] = (i + 1) + (pModule->idBodyNo - 1)*nMaxLegs;
			//
			partSet.Empty();
			for (CLDSPart* pPart = legPartSet.GetFirst(); pPart; pPart = legPartSet.GetNext())
			{	//ֻȡA��
				if(!pPart->IsLegObj() || !pPart->cfgword.IsHasNo(pModule->m_arrActiveQuadLegNo[1]))
					continue;
				BYTE cLegQuad = pPart->ciLegQuad;
				if (cLegQuad == 2)
					partSet.append(pPart);
				else if (pPart->GetClassTypeId() == CLS_PLATE)
				{	//����Ȳ�б�ĵİ��ְ�
					CLDSPlate* pPlate = (CLDSPlate*)pPart;
					CLDSNode* pNode = (pPlate->face_N == 1) ? Ta.FromNodeHandle(pPlate->m_hBaseNode) : NULL;
					if(pNode==NULL)
						continue;
					cLegQuad = (pNode->xOriginalPos.x <= 0 && pNode->xOriginalPos.y >= 0) ? 2 : 0;
					if(cLegQuad ==2 && pNode->cfgword.IsHasNo(pModule->m_arrActiveQuadLegNo[1]))
						partSet.append(pPart);
				}
			}
			if (partSet.GetNodeNum())
			{
				CNewDatabaseLife newDb;
				Draw3DSolid(partSet, ucs, TRUE);
				CXhChar200 sFullPath("%s\\%s%s-S.dwg", (char*)sWorkDir, pModule->description, (char*)szLegItem);
				if (!newDb.SaveAs(sFullPath))
					return;
			}
		}
	}
	MessageBox(adsw_acadMainWnd(), "�ļ��������!", "��ʾ", MB_OK);
	//��������ɫ
	//ShiftSoildShadeMode(sWorkDir);
}
//����������λʵ���ļ�
void Export3DFiles()
{
	CLogErrorLife logErrLife(&xMyErrLog);
	CXhChar500 filter("�����������xlsx�ļ�(*.xlsx)|*.xlsx");
	filter.Append("|�����������XLS�ļ�(*.xls)|*.xls");
	filter.Append("|�����������TXT�ļ�(*.txt)|*.txt");
	filter.Append("|�����ļ�(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, sys.sFilePath, "���������ϸ��", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	if (fileDlg.DoModal() != IDOK)
		return;
	CString file_path = fileDlg.GetPathName();
	CXhChar16 extension;
	_splitpath(file_path, NULL, NULL, NULL, extension);
	if (extension.EqualNoCase(".txt"))
		gDrawManager.ReadTxtFile(file_path);
	else if (extension.EqualNoCase(".xls") || extension.EqualNoCase(".xlsx"))
		gDrawManager.ReadExcleFile(file_path);
	else
		return;
	if (gDrawManager.GetTaModelNum()<=0)
	{
		logerr.Log("�������������ļ�����");
		return;
	}
	CExportSetDlg dlg;
	if (dlg.DoModal() != IDOK)
		return;
	BYTE ciModelLev = gDrawManager.m_ciInstanceLevel = dlg.m_iModelLevel;
	double scale = (dlg.m_iLengthUnit == 1) ? 1000 : 1;
	//��ʼ�����·��
	int index = file_path.ReverseFind('\\');//�������'\\'
	file_path = file_path.Left(index);		//�Ƴ��ļ���
	file_path += "\\DWG";
	MakeDirectory(file_path.GetBuffer());
	//����DWG�ļ�
	int i=1,nNum = gDrawManager.GetTaModelNum();
	DisplayProcess(0, "����DWG�ļ�����");
	for (CDrawModel* pDrawModel = gDrawManager.EnumFirstTaModel(); pDrawModel; pDrawModel = gDrawManager.EnumNextTaModel(), i++)
	{
		DisplayProcess(int(100 * i / nNum), "����DWG�ļ�����");
		if (pDrawModel->m_ciErrorType > 0)
			continue;	//
		void* pInstance = pDrawModel->ExtractActiveTaInstance();
		if (pInstance == NULL)
			continue;
		UCS_STRU ucs(pDrawModel->BuildTransCS(pInstance));
		CNewDatabaseLife newDb;
		if (ciModelLev == 0)
		{	//����ģ��
			if (pDrawModel->m_ciModelFlag == CDrawModel::TYPE_LDS)
				Draw3DLineMap(ucs,scale, pDrawModel->m_bTurnLeft);
			else if (pDrawModel->m_ciModelFlag == CDrawModel::TYPE_TID)
				DrawTid3DLine((ITidTowerInstance*)pInstance, ucs, scale, pDrawModel->m_bTurnLeft);
		}
		else
		{	//ʵ��ģ��
			if (pDrawModel->m_ciModelFlag == CDrawModel::TYPE_LDS)
				Draw3DSolid(ucs, FALSE,scale, ciModelLev, pDrawModel->m_bTurnLeft);
			else if (pDrawModel->m_ciModelFlag == CDrawModel::TYPE_TID)
				DrawTid3DSolid((ITidTowerInstance*)pInstance, ucs, FALSE, scale, ciModelLev, pDrawModel->m_bTurnLeft);
		}
		CXhChar200 sFullPath("%s\\%s.dwg", file_path, (char*)pDrawModel->GetFileName(pInstance));
		if (!newDb.SaveAs(sFullPath))
			return;
	}
	CloseTaFile(FALSE, TRUE);
	DisplayProcess(100, "����DWG�ļ�����");
	//��������ɫ
	ShiftSoildShadeMode(file_path);
}
void ShiftShadeMode()
{
	CString sDwgFolder;
	if (!InvokeFolderPickerDlg(sDwgFolder))
		return;
	ShiftSoildShadeMode(sDwgFolder);
}
#endif
//////////////////////////////////////////////////////////////////////////
//����TID�ļ�����DWG�ļ�(����ģ��+ʵ��ģ��)
#ifdef __LDS_FILE_
#include "TidToDwgDlg.h"
#include "direct.h"
static CTidToDwgDlg tidDlg;
void TidToDwg()
{
	CAcModuleResourceOverride resOverride;
	CLogErrorLife logobj;
	InitDrawingEnv();
	if(tidDlg.DoModal()!=IDOK)
		return;
	//��¼�ļ�·��
	CString file_path=tidDlg.m_sFilePath;
	int index=file_path.ReverseFind('\\');	//�������'\\'
	file_path=file_path.Left(index);		//�Ƴ��ļ���
	file_path+="\\instances\\";
	_mkdir(file_path);
	//����Ҫ�����ѡ������
	int nSelNum=tidDlg.xTidModuleList.GetNodeNum();
	if(nSelNum<=0)
		return;
	CXhChar100 sName,sFullPath;
	if (tidDlg.m_bDrawLine)
	{
		for (ITidHeightGroup** ppModule = tidDlg.xTidModuleList.GetFirst(); ppModule; ppModule = tidDlg.xTidModuleList.GetNext())
		{
			
			ITidHeightGroup* pHeightGroup = *ppModule;
			pHeightGroup->GetName(sName, 100);
			ITidTowerInstance* pTowerInstance = pHeightGroup->GetTowerInstance(1, 1, 1, 1);
			if (pTowerInstance)
			{
				CNewDatabaseLife newDb;
				UCS_STRU ucs;
				ucs.origin.Set(0, 0, pTowerInstance->GetInstanceHeight());
				ucs.axis_x.Set(1, 0, 0);
				ucs.axis_y.Set(0, -1, 0);
				ucs.axis_z.Set(0, 0, -1);
				DrawTid3DLine(pTowerInstance,ucs);	//���л���
				//
				sFullPath.Printf("%s%s-L.dwg", file_path, (char*)sName);
				if (!newDb.SaveAs(sFullPath))
					return;
			}
		}
	}
	if (tidDlg.m_bDrawSolie)
	{
		for (ITidHeightGroup** ppModule = tidDlg.xTidModuleList.GetFirst(); ppModule; ppModule = tidDlg.xTidModuleList.GetNext())
		{
			ITidHeightGroup* pHeightGroup = *ppModule;
			pHeightGroup->GetName(sName, 100);
			ITidTowerInstance* pTowerInstance = pHeightGroup->GetTowerInstance(1, 1, 1, 1);
			if (pTowerInstance)
			{
				CNewDatabaseLife newDb;
				UCS_STRU ucs;
				ucs.origin.Set(0, 0, pTowerInstance->GetInstanceHeight());
				ucs.axis_x.Set(1, 0, 0);
				ucs.axis_y.Set(0, -1, 0);
				ucs.axis_z.Set(0, 0, -1);
				DrawTid3DSolid(pTowerInstance,ucs,TRUE);	//���л���
				sFullPath.Printf("%s%s-S.dwg", file_path, (char*)sName);
				if (!newDb.SaveAs(sFullPath))
					return;
			}
		}
	}
	AfxMessageBox("�ļ�������ϣ�");
}
#else
void TidToDwg(){;}
#endif
//////////////////////////////////////////////////////////////////////////
//
void ConfigInfoBOM()
{
#ifdef __PART_DESIGN_INC_//defined(__TSA_FILE_)||defined(__LDS_FILE_)
	//�������ø�ֵ
	/*sys.m_iCurMapType = 1;
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	InitDrawingEnv();
	CString sTaType = Ta.m_sTaType;
	double fMaxIceThick = Ta.weather.m_fMaxIceThick;
	double fMaxWindV = Ta.weather.m_fMaxWindV;
	int nHighestTemperature = Ta.weather.m_nHighestT;
	int nLowestTemperature = Ta.weather.m_nLowestT;
	double fHorizonSpace[10] = {0};
	double fRepresentSpace[10]={0};
	double fVerticalSpace[10]={0};
	double fXianluangle[10]={0};
	int nVoltGrade[10] = {0};
	int nHSNum = 0;
	int nRSNum = 0;
	int nVSNum = 0;
	int nXANum = 0;
	int nVoltGradeNum = 0;
	CArray<CXhChar100,CXhChar100&>sCWireType;
	double fCWireSafeCof[10]={0};
	double fCWireMaxUseTension[10]={0};
	int nCWireNum = 0;
	CArray<CXhChar100,CXhChar100&>sEWireType;
	double fEWireSafeCof[10]={0};
	double fEWireMaxUseTension[10]={0};
	int nEWireNum = 0;
	int i=0;
	for(CWireLayout *pWireLayout=Ta.groupWireLayout.GetFirst();pWireLayout;pWireLayout=Ta.groupWireLayout.GetNext())
	{
		for(CWireLoadModel *pWireLoadModel = pWireLayout->hashWireLoadModel.GetFirst();pWireLoadModel;pWireLoadModel = pWireLayout->hashWireLoadModel.GetNext())
		{
			CXhChar100 wireType;
			if(pWireLoadModel->wireInfo.m_cXianType == 'C')
			{
				wireType.Copy(pWireLoadModel->wireInfo.wire.specWire);
				int num = sCWireType.GetSize();
				BOOL bSame = FALSE;
				if (num>0)
				{
					for (i=0;i<num;i++)
					{
						if (strcmp(wireType,sCWireType[i])==0)
						{
							bSame = TRUE;
							break;
						}
					}
				}
				if (!bSame)
				{
					sCWireType.Add(wireType);
					fCWireSafeCof[nCWireNum++] = pWireLoadModel->wireInfo.wire.m_fSafeCoef;
					fCWireMaxUseTension[nCWireNum-1] = pWireLoadModel->wireInfo.wire.m_fMaxUseTension;
				}
			}
			else if(pWireLoadModel->wireInfo.m_cXianType =='E')
			{
				wireType.Copy(pWireLoadModel->wireInfo.wire.specWire);
				BOOL bSame = FALSE;
				for (i=0;i<sEWireType.GetSize();i++)
				{
					if (strcmp(wireType,sEWireType[i])==0)
					{
						bSame = TRUE;
						break;
					}
				}
				if (!bSame)
				{
					sEWireType.Add(wireType);
					fEWireSafeCof[nEWireNum++] = pWireLoadModel->wireInfo.wire.m_fSafeCoef;
					fEWireMaxUseTension[nEWireNum-1] = pWireLoadModel->wireInfo.wire.m_fMaxUseTension;
				}
			}
			double horizon_space = pWireLoadModel->wireInfo.xianlu.horizon_space;
			double vertical_space = pWireLoadModel->wireInfo.xianlu.vertical_space;
			double represent_space = pWireLoadModel->wireInfo.xianlu.represent_space;
			double xianlu_angle = pWireLoadModel->wireInfo.xianlu.xianlu_angle;
			int voltGrade = pWireLoadModel->wireInfo.m_nVoltGrade;
			for (i=0;i<nHSNum;i++)
			{
				if (fHorizonSpace[i] == horizon_space)
					break;
			}
			if (i==nHSNum&&horizon_space>EPS)
				fHorizonSpace[nHSNum++] = horizon_space;
			for (i=0;i<nVSNum;i++)
			{
				if (fVerticalSpace[i] == vertical_space)
					break;
			}
			if (i==nVSNum&&vertical_space>EPS)
				fVerticalSpace[nVSNum++] = vertical_space;
			for (i=0;i<nRSNum;i++)
			{
				if (fRepresentSpace[i] == represent_space)
					break;
			}
			if (i==nRSNum&&represent_space>EPS)
				fRepresentSpace[nRSNum++] = represent_space;
			for (i=0;i<nXANum;i++)
			{
				if (fXianluangle[i] == xianlu_angle)
					break;
			}
			if (i==nXANum&&xianlu_angle>EPS)
				fXianluangle[nXANum++] = xianlu_angle;
			for (i=0;i<nVoltGradeNum;i++)
			{
				if (nVoltGrade[i] == voltGrade)
					break;
			}
			if (i==nVoltGradeNum&&voltGrade>EPS)
				nVoltGrade[nVoltGradeNum++] = voltGrade;
		}
	}
	CFormatTbl xFLTbl;
	int nRow = 3*(nEWireNum + nCWireNum) + 10;
	xFLTbl.Create(nRow,3);
	xFLTbl.SetColumnWide(0,16);
	xFLTbl.SetColumnWide(1,26);
	xFLTbl.SetColumnWide(2,36);
	xFLTbl.SetRowHigh(0,8);
	for (i=0;i<nRow;i++)
	{
		if (i==0)
		{
			//ˮƽ�ϲ���Ԫ��
			xFLTbl.GetGridAt(i,1)->m_bHGridVirtual=TRUE;
			xFLTbl.GetGridAt(i,2)->m_bHGridVirtual=TRUE;
			xFLTbl.GetGridAt(i,0)->textsize = sys.dim_map.fHeaderTextSize;
			xFLTbl.GetGridAt(i,0)->feature = 1;
			xFLTbl.GetGridAt(i,0)->data.SetGridString(sTaType.GetBuffer(1));
		}
		else if (i<2+nEWireNum+nCWireNum||i>=2+nEWireNum+nCWireNum+4)
		{
			//ˮƽ�ϲ���Ԫ��
			xFLTbl.GetGridAt(i,1)->m_bHGridVirtual=TRUE;
			if (i==1)
			{
				xFLTbl.GetGridAt(i,0)->textsize = sys.dim_map.fNumericTextSize;
				xFLTbl.GetGridAt(i,0)->feature = 1;
				xFLTbl.GetGridAt(i,0)->data.SetGridString("��ѹ�ȼ�");
				CXhChar100 sText;
				
				xFLTbl.GetGridAt(i,2)->textsize = sys.dim_map.fNumericTextSize;
				xFLTbl.GetGridAt(i,2)->feature = 1;
				sprintf(sText,"%dKV",nVoltGrade[0]);
				xFLTbl.GetGridAt(i,2)->data.SetGridString(sText);
				
				if (nVoltGradeNum > 1)
				{
					for (int j=1;j<nVoltGradeNum;j++)
						sprintf(sText,"%s��%dKV",&sText,nVoltGrade[j]);
					xFLTbl.GetGridAt(i,2)->data.SetGridString(sText);
				}
			}
			else if (i<2+nCWireNum)
			{
				if (nCWireNum<2)
				{
					xFLTbl.GetGridAt(i,0)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i,0)->feature = 1;
					xFLTbl.GetGridAt(i,0)->data.SetGridString("�����ͺ�");
					xFLTbl.GetGridAt(i,2)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i,2)->feature = 1;
					xFLTbl.GetGridAt(i,2)->data.SetGridString(sCWireType[0]);
				}
				else if(i==2)
				{
					for (int j=0;j<nCWireNum;j++)
					{
						xFLTbl.GetGridAt(i+j,0)->textsize = sys.dim_map.fNumericTextSize;
						xFLTbl.GetGridAt(i+j,0)->feature = 1;
						CXhChar100 sText;
						sprintf(sText,"%d�ŵ����ͺ�",j);
						xFLTbl.GetGridAt(i+j,0)->data.SetGridString(sText);
						xFLTbl.GetGridAt(i+j,2)->textsize = sys.dim_map.fNumericTextSize;
						xFLTbl.GetGridAt(i+j,2)->feature = 1;
						xFLTbl.GetGridAt(i+j,2)->data.SetGridString(sCWireType[j]);
					}
				}
			}
			else if (i<2+nCWireNum+nEWireNum)
			{
				if (nEWireNum<2)
				{
					xFLTbl.GetGridAt(i,0)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i,0)->feature = 1;
					xFLTbl.GetGridAt(i,0)->data.SetGridString("�����ͺ�");
					xFLTbl.GetGridAt(i,2)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i,2)->feature = 1;
					xFLTbl.GetGridAt(i,2)->data.SetGridString(sEWireType[0]);
				}
				else if(i==2+nCWireNum)
				{
					for (int j=0;j<nEWireNum;j++)
					{
						xFLTbl.GetGridAt(i+j,0)->textsize = sys.dim_map.fNumericTextSize;
						xFLTbl.GetGridAt(i+j,0)->feature = 1;
						CXhChar100 sText;
						sprintf(sText,"%d�ŵ����ͺ�",j);
						xFLTbl.GetGridAt(i+j,0)->data.SetGridString(sText);
						xFLTbl.GetGridAt(i+j,2)->textsize = sys.dim_map.fNumericTextSize;
						xFLTbl.GetGridAt(i+j,2)->feature = 1;
						xFLTbl.GetGridAt(i+j,2)->data.SetGridString(sEWireType[j]);
					}
				}
			}
			else if (i>2+nEWireNum+nCWireNum+3&&i<2+nEWireNum+nCWireNum*2+4)
			{
				if (nCWireNum<2)
				{
					xFLTbl.GetGridAt(i,0)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i,0)->feature = 1;
					xFLTbl.GetGridAt(i,0)->data.SetGridString("���߰�ȫϵ��");
					xFLTbl.GetGridAt(i,2)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i,2)->feature = 1;
					CXhChar100 sText;
					sprintf(sText,"%f",fCWireSafeCof[0]);
					SimplifiedNumString(sText);
					xFLTbl.GetGridAt(i,2)->data.SetGridString(sText);

					xFLTbl.GetGridAt(i+1,0)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i+1,0)->feature = 1;
					xFLTbl.GetGridAt(i+1,0)->data.SetGridString("�������ʹ������(N)");
					xFLTbl.GetGridAt(i+1,2)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i+1,2)->feature = 1;
					sprintf(sText,"%.0f",fCWireMaxUseTension[0]);
					//SimplifiedNumString(sText);
					xFLTbl.GetGridAt(i+1,2)->data.SetGridString(sText);
				}
				else if(i == 2+nEWireNum+nCWireNum+4)
				{
					for (int j=0;j<nCWireNum;j++)
					{
						xFLTbl.GetGridAt(i+j*2,0)->textsize = sys.dim_map.fNumericTextSize;
						xFLTbl.GetGridAt(i+j*2,0)->feature = 1;
						CXhChar100 sText;
						sprintf(sText,"%d�ŵ��߰�ȫϵ��",j);
						SimplifiedNumString(sText);
						xFLTbl.GetGridAt(i+j*2,0)->data.SetGridString(sText);

						xFLTbl.GetGridAt(i+j*2,2)->textsize = sys.dim_map.fNumericTextSize;
						xFLTbl.GetGridAt(i+j*2,2)->feature = 1;
						sprintf(sText,"%f",fCWireSafeCof[j]);
						SimplifiedNumString(sText);
						xFLTbl.GetGridAt(i+j*2,2)->data.SetGridString(sText);

						xFLTbl.GetGridAt(i+j*2+1,0)->textsize = sys.dim_map.fNumericTextSize;
						xFLTbl.GetGridAt(i+j*2+1,0)->feature = 1;
						sprintf(sText,"%d�ŵ������ʹ������(N)",j);
						SimplifiedNumString(sText);
						xFLTbl.GetGridAt(i+j*2+1,0)->data.SetGridString(sText);
						xFLTbl.GetGridAt(i+j*2+1,2)->textsize = sys.dim_map.fNumericTextSize;
						xFLTbl.GetGridAt(i+j*2+1,2)->feature = 1;
						sprintf(sText,"%.0f",fCWireMaxUseTension[j]);
						//SimplifiedNumString(sText);
						xFLTbl.GetGridAt(i+j*2+1,2)->data.SetGridString(sText);
					}
				}
			}
			else if (i>2+nEWireNum+nCWireNum*2+4&&i<=2+(nEWireNum+nCWireNum)*2+4)
			{
				if (nEWireNum<2)
				{
					xFLTbl.GetGridAt(i,0)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i,0)->feature = 1;
					xFLTbl.GetGridAt(i,0)->data.SetGridString("���߰�ȫϵ��");
					xFLTbl.GetGridAt(i,2)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i,2)->feature = 1;
					CXhChar100 sText;
					sprintf(sText,"%f",fEWireSafeCof[0]);
					SimplifiedNumString(sText);
					xFLTbl.GetGridAt(i,2)->data.SetGridString(sText);

					xFLTbl.GetGridAt(i+1,0)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i+1,0)->feature = 1;
					xFLTbl.GetGridAt(i+1,0)->data.SetGridString("�������ʹ������(N)");
					xFLTbl.GetGridAt(i+1,2)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i+1,2)->feature = 1;
					sprintf(sText,"%.0f",fEWireMaxUseTension[0]);
					//SimplifiedNumString(sText);
					xFLTbl.GetGridAt(i+1,2)->data.SetGridString(sText);
				}
				else if(i==3+nEWireNum+nCWireNum*2+4)
				{
					for (int j=0;j<nEWireNum;j++)
					{
						xFLTbl.GetGridAt(i+j*2,0)->textsize = sys.dim_map.fNumericTextSize;
						xFLTbl.GetGridAt(i+j*2,0)->feature = 1;
						CXhChar100 sText;
						sprintf(sText,"%d�ŵ��߰�ȫϵ��",j);
						SimplifiedNumString(sText);
						xFLTbl.GetGridAt(i+j*2,0)->data.SetGridString(sText);
						xFLTbl.GetGridAt(i+j*2,2)->textsize = sys.dim_map.fNumericTextSize;
						xFLTbl.GetGridAt(i+j*2,2)->feature = 1;
						sprintf(sText,"%f",fEWireSafeCof[j]);
						SimplifiedNumString(sText);
						xFLTbl.GetGridAt(i+j*2,2)->data.SetGridString(sText);

						xFLTbl.GetGridAt(i+j*2+1,0)->textsize = sys.dim_map.fNumericTextSize;
						xFLTbl.GetGridAt(i+j*2+1,0)->feature = 1;
						sprintf(sText,"%d�ŵ������ʹ������(N)",j);
						SimplifiedNumString(sText);
						xFLTbl.GetGridAt(i+j*2+1,0)->data.SetGridString(sText);
						xFLTbl.GetGridAt(i+j*2+1,2)->textsize = sys.dim_map.fNumericTextSize;
						xFLTbl.GetGridAt(i+j*2+1,2)->feature = 1;
						sprintf(sText,"%.0f",fEWireMaxUseTension[j]);
						//SimplifiedNumString(sText);
						xFLTbl.GetGridAt(i+j*2+1,2)->data.SetGridString(sText);
					}
				}
			}
			else
			{
				if (i==6+(nEWireNum+nCWireNum)*3)
				{
					xFLTbl.GetGridAt(i,0)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i,0)->feature = 1;
					xFLTbl.GetGridAt(i,0)->data.SetGridString("��ֱ����(m)");
				
					xFLTbl.GetGridAt(i,2)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i,2)->feature = 1;
					CXhChar100 sText;
					sprintf(sText,"%f",fVerticalSpace[0]);
					SimplifiedNumString(sText);
					xFLTbl.GetGridAt(i,2)->data.SetGridString(sText);
					
					if (nVSNum > 1)
					{
						for (int j=1;j<nVSNum;j++)
							sprintf(sText,"%s/%.0f",&sText,fVerticalSpace[j]);
						SimplifiedNumString(sText);
						xFLTbl.GetGridAt(i,2)->data.SetGridString(sText);
					}
					
					xFLTbl.GetGridAt(i+1,0)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i+1,0)->feature = 1;
					xFLTbl.GetGridAt(i+1,0)->data.SetGridString("ˮƽ����(m)");
					
					xFLTbl.GetGridAt(i+1,2)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i+1,2)->feature = 1;
				
					sprintf(sText,"%f",fHorizonSpace[0]);
					SimplifiedNumString(sText);
					xFLTbl.GetGridAt(i+1,2)->data.SetGridString(sText);
					
					if (nHSNum > 1)
					{
						for (int j=1;j<nHSNum;j++)
							sprintf(sText,"%s/%.0f",&sText,fHorizonSpace[j]);
						SimplifiedNumString(sText);
						xFLTbl.GetGridAt(i+1,2)->data.SetGridString(sText);
					}
					
					xFLTbl.GetGridAt(i+2,0)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i+2,0)->feature = 1;
					xFLTbl.GetGridAt(i+2,0)->data.SetGridString("������(m)");
				
					
					xFLTbl.GetGridAt(i+2,2)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i+2,2)->feature = 1;
					
					sprintf(sText,"%.0f",fRepresentSpace[0]);
					SimplifiedNumString(sText);
					xFLTbl.GetGridAt(i+2,2)->data.SetGridString(sText);
					
					if (nRSNum == 1)
					{
						for (int j=1;j<nRSNum;j++)
							sprintf(sText,"%s/%.0f",&sText,fRepresentSpace[j]);
						SimplifiedNumString(sText);
						xFLTbl.GetGridAt(i+2,2)->data.SetGridString(sText);
					}
					
					xFLTbl.GetGridAt(i+3,0)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i+3,0)->feature = 1;
					xFLTbl.GetGridAt(i+3,0)->data.SetGridString("��·ת��(��)");
					
					if (fXianluangle[0]>90)
						sprintf(sText,"%f",fXianluangle[1]+180-fXianluangle[0]);
					else
						sprintf(sText,"%f",fXianluangle[0]+180-fXianluangle[1]);
					xFLTbl.GetGridAt(i+3,2)->textsize = sys.dim_map.fNumericTextSize;
					xFLTbl.GetGridAt(i+3,2)->feature = 1;
					SimplifiedNumString(sText);
					xFLTbl.GetGridAt(i+3,2)->data.SetGridString(sText);
				}
			}
		}
		else  if (i>=2+nEWireNum+nCWireNum&&i<=2+nEWireNum+nCWireNum+3)
		{
			if (i>2+nEWireNum+nCWireNum)
				xFLTbl.GetGridAt(i,0)->m_bVGridVirtual=TRUE;//��ֱ�ϲ���Ԫ��	
			CXhChar100 sText;
			if (i == 2+nEWireNum+nCWireNum)
			{
				xFLTbl.GetGridAt(i,1)->textsize = sys.dim_map.fNumericTextSize;
				xFLTbl.GetGridAt(i,1)->feature = 1;
				xFLTbl.GetGridAt(i,1)->data.SetGridString("�������(mm)");
				sprintf(sText,"%f",fMaxIceThick);	
			}
			else if (i == 3+nEWireNum+nCWireNum)
			{
				xFLTbl.GetGridAt(i,1)->textsize = sys.dim_map.fNumericTextSize;
				xFLTbl.GetGridAt(i,1)->feature = 1;
				xFLTbl.GetGridAt(i,1)->data.SetGridString("������(m/s)");
				sprintf(sText,"%f",fMaxWindV);	
			}
			else if (i == 4+nEWireNum+nCWireNum)
			{
				xFLTbl.GetGridAt(i,1)->textsize = sys.dim_map.fNumericTextSize;
				xFLTbl.GetGridAt(i,1)->feature = 1;
				xFLTbl.GetGridAt(i,1)->data.SetGridString("����¶�(��)");
				sprintf(sText,"%d",nHighestTemperature);	
			}
			else if (i == 5+nEWireNum+nCWireNum)
			{
				xFLTbl.GetGridAt(i,1)->textsize = sys.dim_map.fNumericTextSize;
				xFLTbl.GetGridAt(i,1)->feature = 1;
				xFLTbl.GetGridAt(i,1)->data.SetGridString("����¶�(��)");
				sprintf(sText,"%d",nLowestTemperature);	
			}
			xFLTbl.GetGridAt(i,2)->textsize = sys.dim_map.fNumericTextSize;
			xFLTbl.GetGridAt(i,2)->feature = 1;
			SimplifiedNumString(sText);
			xFLTbl.GetGridAt(i,2)->data.SetGridString(sText);

			xFLTbl.GetGridAt(i,0)->textsize = sys.dim_map.fNumericTextSize;
			xFLTbl.GetGridAt(i,0)->feature = 1;
			xFLTbl.GetGridAt(i,0)->data.SetGridString("��������");
		}
	}
	
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	DrawTable(pBlockTableRecord,&xFLTbl);
	pBlockTableRecord->close();*/
#endif
}

#ifdef __PART_DESIGN_INC_//defined(__TSA_FILE_)||defined(__LDS_FILE_)
//����ָ���˼����ϵ���ͼ
void DrawSingleLineMapByAnalysisMode(f3dPoint base_pos,PARTSET& partset,NODESET& nodeset,UCS_STRU ucs,double scale)
{
	AcDbObjectId LineId,pointId;//�����ʶ��
	AcGePoint3d start,end;

	InitDrawingEnv();
	//ת������ͼ��
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	//��д��ʽ��ģ�Ϳռ䣬��ÿ���¼ָ��
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	//���Ƹ˼����ߣ�����ǰ���Ƚ�ԭ��������ϵת�����û�����ϵucs��
	CLDSPart *pPart=NULL;
	CLDSLinePart *pLinePart = NULL;
	for(pPart=partset.GetFirst();pPart!=NULL;pPart=partset.GetNext())
	{
		if(pPart && pPart->IsLinePart())
			pLinePart = (CLDSLinePart*)pPart;
		else
			continue;
		if(pLinePart->pStart)
			pLinePart->pStart->is_visible=FALSE;
		if(pLinePart->pEnd)
			pLinePart->pEnd->is_visible=FALSE;
	}
	for(pPart = partset.GetFirst();pPart!=NULL;pPart=partset.GetNext())
	{
		f3dPoint org_start,org_end,point;
		if(pPart && pPart->IsLinePart())
			pLinePart = (CLDSLinePart*)pPart;
		else
			continue;
		if(pLinePart->layer(1)=='F')
			GetCurDwg()->setClayer(LayerTable::AuxLineLayer.layerId);
		else
			GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
		if(pLinePart->m_bVirtualPart)	//�����˼���ͳ��
			continue;
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;
		org_start=pLinePart->pStart->Position(false);
		org_end=pLinePart->pEnd->Position(false);
		
		//����û�����ϵ�µĸ˼���ʼ��
		point = org_start;
		coord_trans(point,ucs,FALSE);
		point = point*(1.0/scale);
		point+=base_pos;	//����ƫ�Ƶ�������λ��
		Cpy_Pnt(start,point);
		//����û�����ϵ�µĸ˼���ֹ��
		point = org_end;
		coord_trans(point,ucs,FALSE);
		point = point*(1.0/scale);
		point+=base_pos;	//����ƫ�Ƶ�����յ�λ��
		Cpy_Pnt(end,point);
		start.z = end.z = 0; //����άͼͶӰ��X-Yƽ����

		//����ACADLINE����
		AcDbLine *pLine=new AcDbLine(start,end);//����LINE����
		if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,LineId,pLine))//��ʵ��д�����¼
		{
			pLine->createExtensionDictionary();
			pLine->close();//���ùرն���ĳ�Ա����
		}
		if(pLinePart->pStart&&!pLinePart->pStart->is_visible)	//û���ƵĽڵ�
		{	//����û�е�
			//����ACADPOINT����
			AcDbPoint *pPoint=new AcDbPoint(start);//����LINE����
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))//��ʵ��д�����¼
			{
				pPoint->createExtensionDictionary();
				pPoint->close();//���ùرն���ĳ�Ա����
			}
			ads_name ent_name;
			acdbGetAdsName(ent_name,pointId);
			acdbEntUpd(ent_name);
			pLinePart->pStart->is_visible=TRUE;
		}
		if(pLinePart->pEnd&&!pLinePart->pEnd->is_visible)	//û���ƵĽڵ�
		{	//����û�е�
			//����ACADPOINT����
			AcDbPoint *pPoint=new AcDbPoint(end);//����LINE����
			if(DRAGSET.AppendAcDbEntity(pBlockTableRecord,pointId,pPoint))//��ʵ��д�����¼
			{
				pPoint->createExtensionDictionary();
				pPoint->close();//���ùرն���ĳ�Ա����
			}
			ads_name ent_name;
			acdbGetAdsName(ent_name,pointId);
			acdbEntUpd(ent_name);
			pLinePart->pEnd->is_visible=TRUE;
		}
	}
	//nodeset
	pBlockTableRecord->close();//�رտ���¼
}

//From TsaFunc.cpp
void PartSpecBriefTbl(f3dPoint *pBasePos=NULL,BOOL bPromptPos=TRUE,double scale=1);
//������ѧ������ͼ
void DrawSumMapByAnalysisMode()
{
	//������ͼ����ע�ڵ����Լ��˼����
	//1.���ɼ򻯹���
	f3dPoint base_pos;
	PartSpecBriefTbl(&base_pos,FALSE);
	//2.������ͼ(��ע���δ��ߣ��ᵣ���������ĵľ��룬���¿����Լ����´����ڣ���ע������)
	//2.1����������ͼ
	base_pos.x+=200;	//��ʱ
	LINEPARTSET sum_partset;
	PARTSET partset;
	NODESET sum_nodeset,nodeset;
	CLDSModule *pModule=Ta.GetActiveModule();
	if(pModule==NULL)
		return;
	pModule->RefreshModule(sum_nodeset,sum_partset);
	for(CLDSNode *pNode=sum_nodeset.GetFirst();pNode;pNode=sum_nodeset.GetNext())
	{
		if(pNode->layer(2)=='Q'||pNode->layer(2)=='1'||pNode->layer(2)=='2')
		{
			pNode->feature=-1;
			nodeset.append(pNode);
		}
	}
	for(CLDSLinePart *pLinePart=sum_partset.GetFirst();pLinePart;pLinePart=sum_partset.GetNext())
	{
		if(pLinePart->layer(2)=='Q'||pLinePart->layer(2)=='1'||pLinePart->layer(2)=='2')
		{
			pLinePart->feature = 0;
			partset.append(pLinePart);
		}
	}

	UCS_STRU ucs;
	GetTaUCS(ucs,TRUE);
	DrawSingleLineMapByAnalysisMode(base_pos,partset,nodeset,ucs,sys.general_set.LineMapScale);
	//3.�ᵣ����ͼ��ǰ���Լ����²��浥��ͼ(��ע�ᵣ���)
	//4.����浥��ͼ
	//5.������߷���(��ע���δ����Լ����¿��ڣ���ע������)
	//6.����ͼ(��ע���δ��ߣ����¿����Լ����´����ڣ���ע������)
	//7.��עV��ͼ
}
#else
void DrawSumMapByAnalysisMode(){;}
#endif

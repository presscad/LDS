//<LOCALE_TRANSLATE BY hxr /> 2015-04-27
// ����ߴ��ע
#include "stdafx.h"
#include "stdarx.h"
#include "MenuFunc.h"
#include "DrawTaMap.h"
#include "DimSpaceSizeDlg.h"
#include "MapSizeDimDlg.h"
#include "adstools.h"
#include "RxTools.h"
#include "geassign.h"
#include "sysPara.h"
#include "SortFunc.h"
#include "DragEntSet.h"
//4.1

void DimSpaceSize()//�ռ�ߴ��ע
{
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	int i=0,num=0;
	char dimText[20];						//�ߴ�����
	static CDimSpaceSizeDlg dlg;
	ULONG handle, NodeHandleDim[50];		//HANDLE����
    CAcModuleResourceOverride resOverride;

	//��ӳߴ��ע�㣬��Ȼ
	//��ʱ�������ǳ����Ҳ���ԭ��	WJH--01.10.21
	AcDbObjectId layerId;
#ifdef _ARX_2007
	acedCommand(RTSTR,L"PDMODE",RTSTR,L"34",RTNONE);//��ʾ�� X
	acedCommand(RTSTR,L"REGEN",RTNONE);
#else
	acedCommand(RTSTR,"PDMODE",RTSTR,"34",RTNONE);//��ʾ�� X
	acedCommand(RTSTR,"REGEN",RTNONE);
#endif
	if(dlg.DoModal()!=IDOK)
	{
	#ifdef _ARX_2007
		acedCommand(RTSTR,L"PDMODE",RTSTR,L"0",RTNONE);//��ʾ�� .
		acedCommand(RTSTR,L"REGEN",RTNONE);
		return;
	#else
		acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);//��ʾ�� .
		acedCommand(RTSTR,"REGEN",RTNONE);
		return;
	#endif
	}
	//ͨ��ѡ��ʵ���ȡʵ���ID
	AcDbObjectId entId,dimStyleId;
	AcDbEntity *pEnt;
	ads_point snap_pt,ptres;
	f3dPoint dimpnt[50];				//��ʼ��ֹ����
	f3dPoint sizepnt;				    //�ߴ��עλ�� 
	ads_name entres;

	while(1)
	{
#ifdef _ARX_2007
#ifdef AFX_TARG_ENU_ENGLISH
		if(ads_entsel(L"\n Please pick any node in order,<Enter Exit>:",entres,ptres)==RTNORM)
#else
		if(ads_entsel(L"\n��˳��׽��һ�ڵ�,<Enter�˳�>:",entres,ptres)==RTNORM)
#endif
		{
			if (ads_osnap(ptres,L"nod",snap_pt)==RTNORM)
#else
#ifdef AFX_TARG_ENU_ENGLISH
		if(ads_entsel("\n Please pick any node in order,<Enter Exit>:",entres,ptres)==RTNORM)
#else
		if(ads_entsel("\n��˳��׽��һ�ڵ�,<Enter�˳�>:",entres,ptres)==RTNORM)
#endif
		{
			if (ads_osnap(ptres,"nod",snap_pt)==RTNORM)
#endif
			{
				//ȡ��ԓ�ڵ��HANDLE��
				acdbGetObjectId(entId, entres);
				acdbOpenObject(pEnt,entId,AcDb::kForRead);
				if(!pEnt->isKindOf(AcDbPoint::desc()))
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("\n The selected is not a node");
#else
					AfxMessageBox("\nѡ�еĲ��ǽڵ�");
#endif
					pEnt->close();
					continue;
				}
				handle=GetTaAtomHandle(pEnt);	//��ȡѡ��Ľڵ���
				if(handle<0x20)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("\n The selected is not a system node");
#else
					AfxMessageBox("\nѡ�еĲ���ϵͳ�ڵ�");
#endif
					pEnt->close();
					continue;
				}
				pEnt->highlight();
				pEnt->close();
				NodeHandleDim[i]=handle;
				dimpnt[i].Set(snap_pt[X],snap_pt[Y],snap_pt[Z]);
				i=i+1;
				if(i>=50)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("\n Most support selecting 50 nodes continuous!");
#else
					AfxMessageBox("\n���֧������ѡ��50���ڵ�!");
#endif
					break;
				}
				continue;
			} 
			else
			{
#ifdef AFX_TARG_ENU_ENGLISH
				AfxMessageBox("\n The selected is not node");
#else
				AfxMessageBox("\nѡ�еĲ��ǽڵ�");
#endif
				continue;
			}
		}
		else
		{	
			if (i<2)
			{	
#ifdef AFX_TARG_ENU_ENGLISH
				if (IDNO==AfxMessageBox("\n The selected nodes less than 2,exit or not?",MB_YESNO))
#else
				if (IDNO==AfxMessageBox("\nѡ�нڵ�����2�����˳���?",MB_YESNO))
#endif
					continue;
			}
			break;
		}
	}
	num=i-1;

	//��õ�ǰͼ�εĿ��ָ��
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//��õ�ǰͼ�ο���¼ָ��
	AcDbBlockTableRecord *pBlockTableRecord;//�������¼ָ��
	//��д��ʽ��ģ�Ϳռ䣬��ÿ���¼ָ��
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();//�رտ��
	DimStyleTable::dimStyle.InitDimStyle(sys.dim_map.fDimTextSize);
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
	if (ads_getpoint(NULL,L"\n Please click the place of size line:",snap_pt)==RTNORM)
#else
	if (ads_getpoint(NULL,"\n Please click the place of size line:",snap_pt)==RTNORM)
#endif
#else
#ifdef _ARX_2007
	if (ads_getpoint(NULL,L"\n���ߴ���λ��:",snap_pt)==RTNORM)
#else
	if (ads_getpoint(NULL,"\n���ߴ���λ��:",snap_pt)==RTNORM)
#endif
#endif
	{
		sizepnt.Set(snap_pt[X],snap_pt[Y],snap_pt[Z]);
		for (i=0;i<num;i++)
		{	
			double dist=DistOf2NodeByHandle(NodeHandleDim[i],NodeHandleDim[i+1],dlg.m_iDimDirection);
			sprintf(dimText,"%.0f",dist);
			int direction;
			if(dlg.m_iDimDirection<2)
				direction=0;
			else if(dlg.m_iDimDirection==2)
				direction=2;
			else
				direction=1;
			DimSize(pBlockTableRecord,
					dimpnt[i],					//��ע��ʼ��
					dimpnt[i+1],				//��ע��ֹ��
					sizepnt,					//�ߴ���λ��
					dimText,					//�ߴ�����
					DimStyleTable::dimStyle.dimStyleId,direction,sys.fonts.chief.fDimTextSize);		//dimtype��ע�ߴ緽ʽ*/
		}
	}
	pBlockTableRecord->close();
	//ȡ����ʾ�� X
#ifdef _ARX_2007
	acedCommand(RTSTR,L"PDMODE",RTSTR,L"1",RTNONE);
	acedCommand(RTSTR,L"REGEN",RTNONE);
#else
	acedCommand(RTSTR,"PDMODE",RTSTR,"1",RTNONE);
	acedCommand(RTSTR,"REGEN",RTNONE);
#endif
}
BOOL FireSnapPoint(CDialog *pDlg)
{
	CMapSizeDimDlg *pMapSizeDlg=(CMapSizeDimDlg*)pDlg;
	pMapSizeDlg->resultList.Empty();
	char sPrompt[200]=""; 
	//��׽��һ����ע��
	ads_point temp_pt;
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sPrompt,"\n Please pick the first dimension point<Enter Confirm>:");
#else
	sprintf(sPrompt,"\n�벶׽��һ����ע��<Enterȷ��>:");
#endif
#ifdef _ARX_2007
	if(acedGetPoint(NULL,(ACHAR*)_bstr_t(sPrompt),temp_pt)==RTNORM)
#else	
	if(acedGetPoint(NULL,sPrompt,temp_pt)==RTNORM)
#endif
	{
		CAD_SCREEN_ENT *pCADEnt=pMapSizeDlg->resultList.append();
		pCADEnt->m_ptPick.Set(temp_pt[X],temp_pt[Y],temp_pt[Z]);
	}
	//��׽�ڶ�����ע��
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sPrompt,"\n Please pick the second dimension point<Enter Confirm>:");
#else
	sprintf(sPrompt,"\n�벶׽�ڶ�����ע��<Enterȷ��>:");
#endif
#ifdef _ARX_2007
	if(acedGetPoint(NULL,(ACHAR*)_bstr_t(sPrompt),temp_pt)==RTNORM)
#else	
	if(acedGetPoint(NULL,sPrompt,temp_pt)==RTNORM)
#endif
	{
		CAD_SCREEN_ENT *pCADEnt=pMapSizeDlg->resultList.append();
		pCADEnt->m_ptPick.Set(temp_pt[X],temp_pt[Y],temp_pt[Z]);
	}
	//��������ע���ʵ�ʳߴ�
	
#ifdef AFX_TARG_ENU_ENGLISH
	sprintf(sPrompt,"\n Please input the actual size of the two dimension point<Enter Confirm>:");
#else
	sprintf(sPrompt,"\n����������ע���ʵ�ʳߴ�<Enterȷ��>:");
#endif
#ifdef _ARX_2007
	ACHAR result[20]={0};
	if(acedGetString(NULL,(ACHAR*)_bstr_t(sPrompt),result)==RTNORM)
		pMapSizeDlg->m_fActualDist=atof(_bstr_t(result));
#else	
	char result[20]="";
	if(acedGetString(NULL,sPrompt,result)==RTNORM)
		pMapSizeDlg->m_fActualDist=atof(result);
#endif
	return TRUE;
}
void DimMapSize()
{
	sys.fonts.InitSpecialProp(sys.m_iCurMapType);
	int i=0,num=0;
	char dimText[20];					//�ߴ�����
	static CMapSizeDimDlg map_dlg;
	map_dlg.SetCallBackFunc(FireSnapPoint);
    CAcModuleResourceOverride resOverride;

	if(map_dlg.DoModal()!=IDOK)
		return;
	ads_point snap_pt;
	f3dPoint dimpnt[20];				//��ʼ��ֹ����
	f3dPoint sizepnt;				    //�ߴ��עλ�� 

	while(1)
	{
		if(i==0)
		{	//ѡ���һ����ע��
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
			if(acedGetPoint(NULL,L"\n Please pick any point in order,<Enter Exit>:",snap_pt)==RTNORM)
#else
			if(acedGetPoint(NULL,"\n Please pick any point in order,<Enter Exit>:",snap_pt)==RTNORM)
#endif
#else
#ifdef _ARX_2007
			if(acedGetPoint(NULL,L"\n��˳��׽��һ��,<Enter�˳�>:",snap_pt)==RTNORM)
#else
			if(acedGetPoint(NULL,"\n��˳��׽��һ��,<Enter�˳�>:",snap_pt)==RTNORM)
#endif
#endif
			{
				dimpnt[i].Set(snap_pt[X],snap_pt[Y],snap_pt[Z]);
				i=i+1;
				continue;
			} 
			else
			{		
#ifdef AFX_TARG_ENU_ENGLISH
				if (IDNO==AfxMessageBox("\n The selected nodes less than 2,exit or not?",MB_YESNO))
#else
				if (IDNO==AfxMessageBox("\nѡ�е�����2�����˳���?",MB_YESNO))
#endif
					continue;
				else
					return;
				break;
			}
		}
		else
		{	//ѡ��ڶ����Լ�֮��ı�ע��ʱ��ʾ��Ƥ����
			ads_point prev_pt;
			prev_pt[X]=dimpnt[i-1].x;
			prev_pt[Y]=dimpnt[i-1].y;
			prev_pt[Z]=dimpnt[i-1].z;
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
			if(acedGetPoint(prev_pt,L"\n Please pick any point in order,<Enter Exit>:",snap_pt)==RTNORM)
#else
			if(acedGetPoint(prev_pt,"\n Please pick any point in order,<Enter Exit>:",snap_pt)==RTNORM)
#endif
#else
	#ifdef _ARX_2007
			if(acedGetPoint(prev_pt,L"\n��˳��׽��һ��,<Enter�˳�>:",snap_pt)==RTNORM)
	#else
			if(acedGetPoint(prev_pt,"\n��˳��׽��һ��,<Enter�˳�>:",snap_pt)==RTNORM)
	#endif
#endif
			{
				dimpnt[i].Set(snap_pt[X],snap_pt[Y],snap_pt[Z]);
				i=i+1;
				if(i>=20)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("\n Most support selecting 20 dimension points continuous!");
#else
					AfxMessageBox("\n���֧������ѡ��20����ע��!");
#endif
					break;
				}
				continue;
			} 
			else
			{	
				if (i<2)
				{	
#ifdef AFX_TARG_ENU_ENGLISH
					if (IDNO==AfxMessageBox("\n The selected nodes less than 2,exit or not?",MB_YESNO))
#else
					if (IDNO==AfxMessageBox("\nѡ�е�����2�����˳���?",MB_YESNO))
#endif
						continue;
					else
						return;
				}
				break;
			}
		}
	}
	num=i-1;

	//��õ�ǰͼ�εĿ��ָ��
	AcDbBlockTable *pBlockTable;
	GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
	//��õ�ǰͼ�ο���¼ָ��
	AcDbBlockTableRecord *pBlockTableRecord;//�������¼ָ��
	//��д��ʽ��ģ�Ϳռ䣬��ÿ���¼ָ��
	pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
	pBlockTable->close();//�رտ��
	DimStyleTable::dimStyle.InitDimStyle(sys.dim_map.fDimTextSize);
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
	if (ads_getpoint(NULL,L"\n Please click the place of size line:",snap_pt)==RTNORM)
#else
	if (ads_getpoint(NULL,"\n Please click the place of size line:",snap_pt)==RTNORM)
#endif
#else
#ifdef _ARX_2007
	if (ads_getpoint(NULL,L"\n���ߴ���λ��:",snap_pt)==RTNORM)
#else
	if (ads_getpoint(NULL,"\n���ߴ���λ��:",snap_pt)==RTNORM)
#endif
#endif
	{
		sizepnt.Set(snap_pt[X],snap_pt[Y],snap_pt[Z]);
		for (i=0;i<num;i++)
		{	
			double dist;
			if(map_dlg.m_iDirection==0)
				dist=fabs(dimpnt[i].x-dimpnt[i+1].x)*map_dlg.m_fMapScale;
			else if(map_dlg.m_iDirection==1)
				dist=fabs(dimpnt[i].y-dimpnt[i+1].y)*map_dlg.m_fMapScale;
			else
				dist=DISTANCE(dimpnt[i],dimpnt[i+1])*map_dlg.m_fMapScale;
			if(map_dlg.m_iPrecision==0)
				sprintf(dimText,"%.0f",dist);
			else if(map_dlg.m_iPrecision==1)
				sprintf(dimText,"%.1f",dist);
			else if(map_dlg.m_iPrecision==2)
				sprintf(dimText,"%.2f",dist);
			else
				sprintf(dimText,"%.0f",dist);
			f3dPoint start,end;
			start=dimpnt[i];
			end = dimpnt[i+1];
			if(map_dlg.m_iDirection==2)
			{
				double angle=Cal2dLineAng(start.x,start.y,end.x,end.y);
				double dd = DistOf2dPtLine(sizepnt,start,end);
				if(dd>=0)	//��ֱ�����
				{
					start = GetPtInPolar(start,angle+Pi/2.0,1.5);
					end = GetPtInPolar(end,angle+Pi/2.0,1.5);
				}
				else
				{
					start = GetPtInPolar(start,angle-Pi/2.0,1.5);
					end = GetPtInPolar(end,angle-Pi/2.0,1.5);
				}
			}
			DimSize(pBlockTableRecord,start,end,sizepnt,dimText,DimStyleTable::dimStyle.dimStyleId,map_dlg.m_iDirection,sys.fonts.segment.fDimTextSize);//dimtype��ע�ߴ緽ʽ*/
		}
	}
	pBlockTableRecord->close();
}
#ifndef __TSA_FILE_
//�Ŷ����
typedef struct tagFOOL_NAIL_SPACE
{
	AcDbEntity *pEnt;
	CLDSBolt *pBolt;
	CLDSLineAngle *pLineAngle;
	double z_coord;	//Z����
	tagFOOL_NAIL_SPACE(){memset(this,0,sizeof(tagFOOL_NAIL_SPACE));}
}FOOL_NAIL_SPACE;
//
static int CompareNailSpaceFunc(const FOOL_NAIL_SPACE& item1,const FOOL_NAIL_SPACE& item2)
{
	if(item1.z_coord>item2.z_coord)
		return 1;
	else if(item1.z_coord<item2.z_coord)
		return -1;
	else
		return 0;
}
//��ע�����˼��ϵĽŶ����
void DimSinglePoleFootNailSpace(AcDbBlockTableRecord *pBlockTableRecord,AcDbObjectId entId,
								double fPara1=10,double fPara2=5,BOOL bSelectBasePt=TRUE,
								double draw_scale=0.05,UCS_STRU *pDrawUCS=NULL)
{
	AcDbEntity *pEnt=NULL;
	acdbOpenObject(pEnt,entId,AcDb::kForRead);
	if(pEnt==NULL)
		return;
	AcDbLine *pEntLine=NULL;
	if(pEnt->isKindOf(AcDbLine::desc()))
		pEntLine=(AcDbLine*)pEnt;
	pEnt->close();
	if(pEntLine==NULL)
		return;
	long handle=GetTaAtomHandle(pEnt);
	CLDSLinePart *pLinePart=(CLDSLinePart*)Ta.FromPartHandle(handle,CLS_LINETUBE,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
	if(pLinePart==NULL)
		return;
	CArray<FOOL_NAIL_SPACE,FOOL_NAIL_SPACE&> nailArr;
	FOOL_NAIL_SPACE nailSpace;
	CLDSLineAngle *pLineAngle=NULL;
	CLDSLineTube *pLineTube=NULL;
	if(pLinePart->GetClassTypeId()==CLS_LINEANGLE)
		pLineAngle=(CLDSLineAngle*)pLinePart;
	else if(pLinePart->GetClassTypeId()==CLS_LINETUBE)
		pLineTube=(CLDSLineTube*)pLinePart;
	else 
		return;
	//���Ҹ˼��ϵĽŶ�
	if(pLineAngle)
	{
		CLsRef *pLsRef=NULL;
		pLineAngle->getUCS(pLineAngle->ucs);
		for(pLsRef=pLineAngle->GetFirstLsRef();pLsRef;pLsRef=pLineAngle->GetNextLsRef())
		{
			CLDSBolt *pBolt=pLsRef->GetLsPtr();
			if(!pBolt->IsFootNail())
				continue;
			f3dPoint pos=pBolt->ucs.origin;
			coord_trans(pos,pLineAngle->ucs,FALSE);
			nailSpace.pBolt=pBolt;
			nailSpace.z_coord=pos.z;
			nailArr.Add(nailSpace);
		}
	}
	else if(pLineTube)
	{
		PARTSET partset;
		pLineTube->BuildUCS();
		Ta.GetPartGroupPartSet(pLineTube->handle,partset);
		CLDSPart *pPart=NULL;
		for(pPart=partset.GetFirst();pPart;pPart=partset.GetNext())
		{
			if(pPart->GetClassTypeId()==CLS_LINESLOT)
			{
				CLDSLineSlot *pLineSlot=(CLDSLineSlot*)pPart;
				if(pLineSlot->pStart||pLineSlot->pEnd||pLineSlot->GetLsCount()!=1)
					continue;
				CLDSBolt *pBolt=pLineSlot->GetFirstLsRef()->GetLsPtr();
				f3dPoint pos=pBolt->ucs.origin;
				coord_trans(pos,pLineTube->ucs,FALSE);
				nailSpace.pBolt=pBolt;
				nailSpace.z_coord=pos.z;
				nailSpace.pEnt=NULL;
				nailSpace.pLineAngle=NULL;
				nailArr.Add(nailSpace);
			}
			else if(pPart->GetClassTypeId()==CLS_LINEANGLE)
			{
				CLDSLineAngle *pAngle=(CLDSLineAngle*)pPart;
				if(pLineAngle->pStart||pLineAngle->pEnd||!pAngle->m_bFootNail)
					continue;
				f3dPoint pos=pAngle->ucs.origin;
				coord_trans(pos,pLineTube->ucs,FALSE);
				nailSpace.pBolt=NULL;
				nailSpace.z_coord=pos.z;
				nailSpace.pEnt=NULL;
				nailSpace.pLineAngle=pLineAngle;
				nailArr.Add(nailSpace);
			}
		}
	}
	if(nailArr.GetSize()<=0)
		return;	//�ø˼����޽Ŷ�
	//���Ŷ����鰴�������ϵ�µ�Z�����С��������
	CQuickSort<FOOL_NAIL_SPACE>::QuickSort(nailArr.GetData(),nailArr.GetSize(),CompareNailSpaceFunc);
	//��󽫽Ǹ��ն���ӵ��Ŷ�������
	nailSpace.pLineAngle=NULL;
	nailSpace.pBolt=NULL;
	nailSpace.pEnt=NULL;
	f3dPoint pos=pLinePart->End()+pLinePart->ucs.axis_z*pLinePart->endOdd();
	coord_trans(pos,pLinePart->ucs,FALSE);
	nailSpace.z_coord=pos.z;
	nailArr.Add(nailSpace);
	
	//
	f3dPoint line_vec,line_norm,line_start,line_end;
	Cpy_Pnt(line_start,pEntLine->startPoint());
	Cpy_Pnt(line_end,pEntLine->endPoint());
	Sub_Pnt(line_vec,line_end,line_start);
	normalize(line_vec);
	if(line_start.x>line_end.x)	//˳ʱ����ת90��
		line_norm.Set(line_vec.y,-line_vec.x);
	else						//��ʱ����ת90��
		line_norm.Set(-line_vec.y,line_vec.x);
	if(pLineAngle&&pDrawUCS)
	{	//���Ǹ��������ת������ͼ����ϵ�£���������ʼ�˵�
		//����ʼ�˵ķ�������line_norm,��֤�Ŷ���ע�ڽǸ����߲� wht 11-07-15
		f3dPoint ber_start=pLineAngle->Start();
		coord_trans(ber_start,*pDrawUCS,FALSE);
		ber_start*=draw_scale;
		ber_start.z=0;
		f3dPoint verfiy_vec=ber_start-line_start;
		if(line_norm*verfiy_vec<0)
			line_norm*=-1.0;
	}
	normalize(line_norm);
	//��ȡCADͼֽ�иø˼��ϵ�һ�ŽŶ���Ӧ��ʵ�����������ע��ʼλ��
	f3dPoint rgn_vert[4];
	ads_point ads_vert[4];
	double scope_width=pLinePart->GetWidth();
	rgn_vert[0]=line_start+line_norm*scope_width*draw_scale*0.5;
	rgn_vert[1]=line_start-line_norm*scope_width*draw_scale*0.5;
	rgn_vert[2]=line_end+line_norm*scope_width*draw_scale*0.5;
	rgn_vert[3]=line_end-line_norm*scope_width*draw_scale*0.5;
	for(int i=0;i<4;i++)
	{
		ads_vert[i][X] = rgn_vert[i].x;
		ads_vert[i][Y] = rgn_vert[i].y;
		ads_vert[i][Z] = rgn_vert[i].z;
	}
	FOOL_NAIL_SPACE *pFirstNail=NULL;	//���Ҹ˼��ϵ�һ���Ŷ���������Ŷ���ע��ʼλ��
	ads_name ss_name;
	struct resbuf *plist;
	plist=acutBuildList(RTPOINT,ads_vert[0],RTPOINT,ads_vert[1],RTPOINT,ads_vert[2],RTPOINT,ads_vert[3],RTNONE);
#ifdef _ARX_2007
	if (acedSSGet(L"all",plist,NULL,NULL,ss_name)==RTNORM)
#else
	if (acedSSGet("all",plist,NULL,NULL,ss_name)==RTNORM)
#endif
	{
		long i,ll;
		ads_name entname;
		AcDbObjectId boltEntId;
		acedSSLength(ss_name,&ll);
		for(i=0;i<ll;i++)
		{
			acedSSName(ss_name,i,entname);
			acdbGetObjectId(boltEntId,entname);
			AcDbEntity *pNailEnt=NULL;
			acdbOpenObject(pNailEnt,boltEntId,AcDb::kForRead);
			if(pNailEnt==NULL)
				continue;
			pNailEnt->close();
			long handle=GetTaAtomHandle(pNailEnt);
			for(int j=0;j<nailArr.GetSize();j++)
			{
				long hNail=0;
				if(nailArr[j].pBolt)
					hNail=nailArr[j].pBolt->handle;
				else if(nailArr[j].pLineAngle)
					hNail=nailArr[j].pLineAngle->handle;
				else 
					continue;
				if(hNail==handle)
				{
					nailArr[j].pEnt=pNailEnt;
					pFirstNail=&nailArr[j];
					break;
				}
			}
			if(pFirstNail)
				break;
		}
	}
	acutRelRb(plist);
	acedSSFree(ss_name);
	if(pFirstNail==NULL||pFirstNail->pEnt==NULL)
		return;
	f3dPoint first_nail_pos;
	if(pFirstNail->pEnt->isKindOf(AcDbBlockReference::desc()))
	{
		AcDbBlockReference *pBlockRef=(AcDbBlockReference*)pFirstNail->pEnt;
		Cpy_Pnt(first_nail_pos,pBlockRef->position());
	}
	else if(pFirstNail->pEnt->isKindOf(AcDbCircle::desc()))
		Cpy_Pnt(first_nail_pos,((AcDbCircle*)pFirstNail->pEnt)->center());
	else
		return;
	//�Ŷ�������ͶӰ
	SnapPerp(&first_nail_pos,line_start,line_end,first_nail_pos);
	//��ע�Ŷ����
	FOOL_NAIL_SPACE *pNailSpace=NULL;
	f3dPoint dim_start,dim_end,dim_pos;
	double offset_dist=nailArr[nailArr.GetSize()-1].z_coord-pFirstNail->z_coord;
	dim_start=first_nail_pos+line_vec*offset_dist*draw_scale;
	dim_start+=line_norm*(scope_width*draw_scale*0.5+fPara1);
	DRAGSET.ClearEntSet();
	for(i=nailArr.GetSize()-1;i>0;i--)
	{
		double fSpace=nailArr[i].z_coord-nailArr[i-1].z_coord;
		CXhChar100 dimText("%.f",fSpace);
		dim_end=dim_start-line_vec*fSpace*draw_scale;
		dim_pos=0.5*(dim_start+dim_end)+line_norm*fPara2;	
		DimSize(pBlockTableRecord,dim_start,dim_end,dim_pos,dimText,
			DimStyleTable::dimStyle.dimStyleId,2,sys.fonts.segment.fDimTextSize);
		dim_start=dim_end;
	}
	if(bSelectBasePt)
	{
		f3dPoint insert_pt=0.5*(line_start+line_end)+line_norm*(fPara1+fPara2);
#ifdef AFX_TARG_ENU_ENGLISH
		DragEntSet(insert_pt,"\n Please select a right dimension place:\n");
#else
		DragEntSet(insert_pt,"\n��ѡ��һ�����ʵı�עλ��:\n");
#endif
	}
}
//�����
typedef struct tagFILL_PLATE_SPACE
{
	AcDbEntity *pEnt;
	CLDSPlate *pFillPlate;
	CLDSGroupLineAngle *pGroupLineAngle;
	double z_coord;	//Z����
	tagFILL_PLATE_SPACE(){memset(this,0,sizeof(tagFILL_PLATE_SPACE));}
}FILL_PLATE_SPACE;
//
static int CompareFillPlateSpaceFunc(const FILL_PLATE_SPACE& item1,const FILL_PLATE_SPACE& item2)
{
	if(item1.z_coord>item2.z_coord)
		return 1;
	else if(item1.z_coord<item2.z_coord)
		return -1;
	else
		return 0;
}
//��ע������ϽǸ��ϵ������
void DimSingleGroupLineAngleFillPlateSpace(AcDbBlockTableRecord *pBlockTableRecord,AcDbObjectId entId,
								double fPara1=10,double fPara2=5,BOOL bSelectBasePt=TRUE,
								double draw_scale=0.05,UCS_STRU *pDrawUCS=NULL)
{
	AcDbEntity *pEnt=NULL;
	acdbOpenObject(pEnt,entId,AcDb::kForRead);
	if(pEnt==NULL)
		return;
	AcDbLine *pEntLine=NULL;
	if(pEnt->isKindOf(AcDbLine::desc()))
		pEntLine=(AcDbLine*)pEnt;
	pEnt->close();
	if(pEntLine==NULL)
		return;
	long handle=GetTaAtomHandle(pEnt);
	CLDSLinePart *pLinePart=(CLDSLinePart*)Ta.FromPartHandle(handle,CLS_GROUPLINEANGLE);
	if(pLinePart==NULL)
		return;
	CArray<FILL_PLATE_SPACE,FILL_PLATE_SPACE&> fillPlateArr;
	FILL_PLATE_SPACE fillPlateSpace;
	CLDSGroupLineAngle *pGroupLineAngle=NULL;
	if(pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		pGroupLineAngle=(CLDSGroupLineAngle*)pLinePart;
	//������ϽǸ��ϵ����
	if(pGroupLineAngle)
	{
		pGroupLineAngle->getUCS(pGroupLineAngle->ucs);
		for (CLDSPlate *pFillPlate=(CLDSPlate*)Ta.Parts.GetFirst(CLS_PLATE);pFillPlate;pFillPlate=(CLDSPlate*)Ta.Parts.GetNext(CLS_PLATE))
		{
			if (pFillPlate->designInfo.origin.des_para.SCALE_NODE.hLinePart!=pLinePart->handle)
				continue;
			if (pFillPlate->designInfo.origin.datum_pos_style!=10)	
				continue;
			f3dPoint pos=pFillPlate->ucs.origin;
			coord_trans(pos,pGroupLineAngle->ucs,FALSE);
			fillPlateSpace.pFillPlate=pFillPlate;
			fillPlateSpace.z_coord=pos.z;
			fillPlateArr.Add(fillPlateSpace);
		}
	}
	if(fillPlateArr.GetSize()<=0)
		return;	//����ϽǸ��������
	//��������鰴�������ϵ�µ�Z�����С��������
	CQuickSort<FILL_PLATE_SPACE>::QuickSort(fillPlateArr.GetData(),fillPlateArr.GetSize(),CompareFillPlateSpaceFunc);
	//�����ϽǸ��ն���ӵ����������
	fillPlateSpace.pGroupLineAngle=NULL;
	fillPlateSpace.pFillPlate=NULL;
	fillPlateSpace.pEnt=NULL;
	f3dPoint pos=pLinePart->End()+pLinePart->ucs.axis_z*pLinePart->endOdd();
	coord_trans(pos,pLinePart->ucs,FALSE);
	fillPlateSpace.z_coord=pos.z;
	fillPlateArr.Add(fillPlateSpace);

	//
	f3dPoint line_vec,line_norm,line_start,line_end;
	Cpy_Pnt(line_start,pEntLine->startPoint());
	Cpy_Pnt(line_end,pEntLine->endPoint());
	Sub_Pnt(line_vec,line_end,line_start);
	normalize(line_vec);
	if(line_start.x>line_end.x)	//˳ʱ����ת90��
		line_norm.Set(line_vec.y,-line_vec.x);
	else						//��ʱ����ת90��
		line_norm.Set(-line_vec.y,line_vec.x);
	if(pGroupLineAngle&&pDrawUCS)
	{	//���Ǹ��������ת������ͼ����ϵ�£���������ʼ�˵�
		//����ʼ�˵ķ�������line_norm,��֤����ע�ڽǸ����߲� wht 11-07-15
		f3dPoint ber_start=pGroupLineAngle->Start();
		coord_trans(ber_start,*pDrawUCS,FALSE);
		ber_start*=draw_scale;
		ber_start.z=0;
		f3dPoint verfiy_vec=ber_start-line_start;
		if(line_norm*verfiy_vec<0)
			line_norm*=-1.0;
	}
	normalize(line_norm);
	//��ȡCADͼֽ�и���ϽǸ��ϵ�һ������Ӧ��ʵ�����������ע��ʼλ��
	f3dPoint rgn_vert[4];
	ads_point ads_vert[4];
	double scope_width=pLinePart->GetWidth();
	rgn_vert[0]=line_start+line_norm*scope_width*draw_scale*0.5;
	rgn_vert[1]=line_start-line_norm*scope_width*draw_scale*0.5;
	rgn_vert[2]=line_end+line_norm*scope_width*draw_scale*0.5;
	rgn_vert[3]=line_end-line_norm*scope_width*draw_scale*0.5;
	for(int i=0;i<4;i++)
	{
		ads_vert[i][X] = rgn_vert[i].x;
		ads_vert[i][Y] = rgn_vert[i].y;
		ads_vert[i][Z] = rgn_vert[i].z;
	}
	FILL_PLATE_SPACE *pFirstFillPlate=NULL;	//������ϽǸ��ϵ�һ�����������������ע��ʼλ��
	ads_name ss_name;
	struct resbuf *plist;
	plist=acutBuildList(RTPOINT,ads_vert[0],RTPOINT,ads_vert[1],RTPOINT,ads_vert[2],RTPOINT,ads_vert[3],RTNONE);
#ifdef _ARX_2007
	if (acedSSGet(L"all",plist,NULL,NULL,ss_name)==RTNORM)
#else
	if (acedSSGet("all",plist,NULL,NULL,ss_name)==RTNORM)
#endif
	{
		long i,ll;
		ads_name entname;
		AcDbObjectId plateEntId;
		acedSSLength(ss_name,&ll);
		for(i=0;i<ll;i++)
		{
			acedSSName(ss_name,i,entname);
			acdbGetObjectId(plateEntId,entname);
			AcDbEntity *pFillPlateEnt=NULL;
			acdbOpenObject(pFillPlateEnt,plateEntId,AcDb::kForRead);
			if(pFillPlateEnt==NULL)
				continue;
			pFillPlateEnt->close();
			long handle=GetTaAtomHandle(pFillPlateEnt);
			for(int j=0;j<fillPlateArr.GetSize();j++)
			{
				long hFillPlate=0;
				if(fillPlateArr[j].pFillPlate)
					hFillPlate=fillPlateArr[j].pFillPlate->handle;
				else if(fillPlateArr[j].pGroupLineAngle)
					hFillPlate=fillPlateArr[j].pGroupLineAngle->handle;
				else 
					continue;
				if(hFillPlate==handle)
				{
					fillPlateArr[j].pEnt=pFillPlateEnt;
					pFirstFillPlate=&fillPlateArr[j];
					break;
				}
			}
			if(pFirstFillPlate)
				break;
		}
	}
	acutRelRb(plist);
	acedSSFree(ss_name);
	if(pFirstFillPlate==NULL||pFirstFillPlate->pEnt==NULL)
		return;
	f3dPoint first_fillplate_pos;
	if(pFirstFillPlate->pEnt->isKindOf(AcDbLine::desc()))
	{
		f3dPoint start,end;
		AcGePoint3d acad_pnt;
		acad_pnt=((AcDbLine*)pEnt)->startPoint();
		Cpy_Pnt(start,acad_pnt);
		acad_pnt=((AcDbLine*)pEnt)->endPoint();
		Cpy_Pnt(end,acad_pnt);
		first_fillplate_pos=(start+end)/2.0;
	}
	else if(pFirstFillPlate->pEnt->isKindOf(AcDbPolyline::desc()))
	{
		f3dPoint start,end;
		AcGePoint2d acad_point;
		((AcDbPolyline*)pEnt)->getPointAt(0,acad_point);
		start.Set(acad_point.x,acad_point.y);
		((AcDbPolyline*)pEnt)->getPointAt(1,acad_point);
		end.Set(acad_point.x,acad_point.y);
		first_fillplate_pos=(start+end)/2.0;
	}
	else if(pFirstFillPlate->pEnt->isKindOf(AcDbPoint::desc()))
		Cpy_Pnt(first_fillplate_pos,((AcDbPoint*)pFirstFillPlate->pEnt)->position());
	else
		return;
	//���������ͶӰ
	SnapPerp(&first_fillplate_pos,line_start,line_end,first_fillplate_pos);
	//��ע�����
	FOOL_NAIL_SPACE *pNailSpace=NULL;
	f3dPoint dim_start,dim_end,dim_pos;
	double offset_dist=fillPlateArr[fillPlateArr.GetSize()-1].z_coord-pFirstFillPlate->z_coord;
	dim_start=first_fillplate_pos+line_vec*offset_dist*draw_scale;
	dim_start+=line_norm*(scope_width*draw_scale*0.5+fPara1);
	DRAGSET.ClearEntSet();
	for(i=fillPlateArr.GetSize()-1;i>0;i--)
	{
		double fSpace=fillPlateArr[i].z_coord-fillPlateArr[i-1].z_coord;
		CXhChar100 dimText("%.f",fSpace);
		dim_end=dim_start-line_vec*fSpace*draw_scale;
		dim_pos=0.5*(dim_start+dim_end)+line_norm*fPara2;	
		DimSize(pBlockTableRecord,dim_start,dim_end,dim_pos,dimText,
			DimStyleTable::dimStyle.dimStyleId,2,sys.fonts.segment.fDimTextSize);
		dim_start=dim_end;
	}
	if(bSelectBasePt)
	{
		f3dPoint insert_pt=0.5*(line_start+line_end)+line_norm*(fPara1+fPara2);
#ifdef AFX_TARG_ENU_ENGLISH
		DragEntSet(insert_pt,"\n Please select a right dimension place:\n");
#else
		DragEntSet(insert_pt,"\n��ѡ��һ�����ʵı�עλ��:\n");
#endif
	}
}
//��ע�Ŷ����
void DimFootNailSpace()
{
	AcDbObjectId entId;
	ads_name entname,ent_sel_set;
	GetCurDwg()->setClayer(LayerTable::DimSizeLayer.layerId);
#ifdef _ARX_2007
	struct resbuf *filter = acutBuildList(RTDXF0,L"LINE",0);
#else
	struct resbuf *filter = acutBuildList(RTDXF0,"LINE",0);
#endif
	if(acedSSGet(NULL,NULL,NULL,filter,ent_sel_set)==RTNORM)
	{
		long ll;
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		acedSSLength(ent_sel_set,&ll);
		for(long i=0;i<ll;i++)
		{
			acedSSName(ent_sel_set,i,entname);
			acdbGetObjectId(entId,entname);
			DimSinglePoleFootNailSpace(pBlockTableRecord,entId,10,5,TRUE,1.0/sys.general_set.StruMapScale.fRealToDraw);
		}
		pBlockTableRecord->close();
	}
	acutRelRb(filter);
	acedSSFree(ent_sel_set);
}
#endif
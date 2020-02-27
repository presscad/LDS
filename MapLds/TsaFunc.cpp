#include "stdafx.h"
#include "stdarx.h"
#include "Tower.h"
#include "DrawTaMap.h"
#include "SysPara.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "DimPlankLs.h"
#include "DimJgZhun.h"
#include "AdsTools.h"
#include "RxTools.h"
#include "DragEntSet.h"
#include "DrawUnit.h"
#include "SortFunc.h"
#include "FormatTable.h"
#include "LoadArrowLocateDlg.h"
#include "LoadGraphDlg.h"
#include "SelectSimpleSpecDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int compare_spec(const SPEC_MARK &spec_mark1,const SPEC_MARK &spec_mark2)
{
	if(spec_mark1.N>spec_mark2.N)
		return -1;
	else if(spec_mark1.N<spec_mark2.N)
		return 1;
	else
		return 0;
}
static int compare_spec1(const SPEC_MARK &spec_mark1,const SPEC_MARK &spec_mark2)
{
	if(spec_mark1.wide>spec_mark2.wide)
		return 1;
	else if(spec_mark1.wide<spec_mark2.wide)
		return -1;
	else
	{
		if(spec_mark1.thick>spec_mark2.thick)
			return 1;
		else if(spec_mark1.thick<spec_mark2.thick)
			return -1;
		else
			return 0;
	}
}
void PreparePartSpecList(ATOM_LIST<SPEC_MARK> &specList)
{
	CAcModuleResourceOverride resOverride;
	CSelectSimpleSpecDlg dlg;
	dlg.m_pSpecList=&specList;
	if(dlg.DoModal()!=IDOK)
		return ;
}
void PreparePartSpecBriefTbl(ATOM_LIST<SPEC_MARK> &specList,CFormatTbl &xSpecTbl,int nBriefGuigeN)
{
	int i,curSpecNum= specList.GetNodeNum();
	xSpecTbl.Destroy();	//���ԭ������ wht 11-06-15
	xSpecTbl.Create(curSpecNum+1,2);
	xSpecTbl.SetColumnWide(0,10);
	xSpecTbl.SetColumnWide(1,30);
	//����е�����ȱʡ�����½Ƕ��뼴feature = 0
	//feature��λ����ʾ�����ֵĶ��뷽ʽ0:������ 1��ʾ������ 2��ʾ���½�
	//ʮλ��ʾ������д����0:������1:���ϵ���
	for(i=0;i<curSpecNum+1;i++)
	{
		xSpecTbl.SetRowHigh(i,4);
		xSpecTbl.GetGridAt(i,0)->feature=1;
	}
	xSpecTbl.GetGridAt(0,1)->feature=1;
	xSpecTbl.SetGrid(0,0,"����");
	xSpecTbl.SetGrid(0,1,"���");
	i=0;
	for(SPEC_MARK *pMark=specList.GetFirst();pMark;pMark=specList.GetNext())
	{
		char ss[2];
		sprintf(ss,"%C",pMark->cMark);
		xSpecTbl.SetGrid(i+1,0,ss);
		xSpecTbl.SetGrid(i+1,1,pMark->spec);
		i++;
	}
}
//���ƹ������򻯱�
void PartSpecBriefTbl(f3dPoint *pBasePos=NULL,BOOL bPromptPos=TRUE,double scale=1)
{
	CFormatTbl spec_tbl;
	PreparePartSpecList(dimpartspec.SpecList); 
	PreparePartSpecBriefTbl(dimpartspec.SpecList,spec_tbl,sys.nBriefGuigeN);
	if(pBasePos) //���ñ��λ��
		spec_tbl.SetBasePos(f2dPoint(pBasePos->x,pBasePos->y));
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	DrawTable(pBlockTableRecord,&spec_tbl,bPromptPos,scale);
	pBlockTableRecord->close();
}

void PartSpecTbl()
{
	PartSpecBriefTbl();
}

/*void DimPartSpec()
{
	//ͨ��ѡ��ʵ���ȡʵ���ID
	AcDbObjectId entId;
	ads_name entname,ent_sel_set;
	struct resbuf *filter;

	SetCurLayer("0");
	dimpartspec.EmptyPartSpecDim();
	acedCommand(RTSTR,"PDMODE",RTSTR,"0",RTNONE);
	filter = acutBuildList(RTDXF0,"POINT,LINE",0);
	if(acedSSGet(NULL,NULL,NULL,filter,ent_sel_set)==RTNORM)
	{
		long ll;
		BOOL bDimPoleSpec;
		if(AfxMessageBox("�Ƿ񽫸˼���Ϣ��ڵ���ͬʱ��ע(Y)������ֻ��ע�ڵ���(N)��",MB_YESNO|MB_ICONQUESTION)==IDYES)
			bDimPoleSpec=TRUE;
		else
			bDimPoleSpec=FALSE;
		acedSSLength(ent_sel_set,&ll);

		for(long i=0;i<ll;i++)
		{
			AcDbEntity *pEnt;
			acedSSName(ent_sel_set,i,entname);
			acdbGetObjectId(entId,entname);
			acdbOpenObject(pEnt,entId,AcDb::kForRead);
			long handle=GetTaAtomHandle(pEnt);	//,&atom_type);
			CLDSDbObject *pDbObj=Ta.FromHandle(handle);
			if(pDbObj==NULL)
			{
				pEnt->close();
				continue;
			}
			if(bDimPoleSpec&&pDbObj->IsPart()
				&&((CLDSPart*)pDbObj)->IsLinePart())
			{
				double angle;
				f3dPoint dim_pos;
				CLDSLinePart *pPole=(CLDSLinePart*)pDbObj;
				BOOL bReverseLsInfo=FALSE;
				if(pEnt->isKindOf(AcDbLine::desc()))
				{
					AcGePoint3d acad_pnt;
					f3dPoint start,end;
					acad_pnt=((AcDbLine*)pEnt)->startPoint();
					Cpy_Pnt(start,acad_pnt);
					acad_pnt=((AcDbLine*)pEnt)->endPoint();
					Cpy_Pnt(end,acad_pnt);
					angle=Cal2dLineAng(start.x,start.y,end.x,end.y);
					if(angle>Pi/2.0&&angle<=1.5*Pi)
					{
						angle-=Pi;
						dim_pos=end+(start-end)*0.5;
						bReverseLsInfo=TRUE;
					}
					else
						dim_pos=start+(end-start)*0.5;
				}
				else
				{
					pEnt->close();
					continue;
				}
				CDimPartSpec *pPartSpec=dimpartspec.AppendPartSpec(pPole,dim_pos,angle);
				pPartSpec->bReverseLsInfo=bReverseLsInfo;
			}
			else if(pDbObj->IsNode())
			{
				f3dPoint dim_pos;
				AcGePoint3d acad_pnt;
				CLDSNode *pNode=(CLDSNode*)pDbObj;
				if(pEnt->isKindOf(AcDbPoint::desc()))
				{
					acad_pnt=((AcDbPoint*)pEnt)->position();
					Cpy_Pnt(dim_pos,acad_pnt);
				}
				else
				{
					pEnt->close();
					continue;
				}
				CDimPartSpec *pPartSpec=dimpartspec.AppendPartSpec(pNode,dim_pos);
			}
			pEnt->close();
		}
		AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
		dimpartspec.DimToMap(pBlockTableRecord);
		pBlockTableRecord->close();
	}
	acutRelRb(filter);
	acedSSFree(ent_sel_set);
	acedCommand(RTSTR,"PDMODE",RTSTR,"1",RTNONE);
}*/

#ifdef __PART_DESIGN_INC_//defined(__TSA_FILE_)||defined(__LDS_FILE_)
void CreateLoadArrow(AcDbBlockTableRecord *pBlockTableRecord,char cAxis,double val,f3dPoint pos,int iLocate)
{
	AcDbObjectId LineId;//�����ʶ��
	AcGePoint2d acad_pos;
	if(val==0)
		return;
	AcDbPolyline *pLine=new AcDbPolyline(3);//����LINE����
	f3dPoint vec,norm,base;
	double rot_ang;
	if(cAxis=='X')
	{
		vec.x=1;
		rot_ang=0;
	}
	else if(cAxis=='Y')
	{
		vec.Set(0.7071,-0.7071);
		rot_ang=-Pi/4;
	}
	else if(cAxis=='Z')
	{
		vec.y=-1;
		rot_ang=-Pi/2;
	}
	norm.Set(-vec.y,vec.x);
	if(val<0)	//������ֵ
	{
		vec*=-1;
		iLocate=1-iLocate;	//��ͷ��β��λҲ��λ
	}
	if(iLocate==0)	//��ͷ��λ
	{
		pos.x-=12*vec.x;
		pos.y-=12*vec.y;
		if(val>0)
			base=pos+vec*0.5+norm*0.5;
		else
			base=pos+vec*0.5-norm*0.5;
	}
	else
	{
		if(val>0)
			base=pos+vec*9.5+norm*0.5;
		else
			base=pos+vec*9.5-norm*0.5;
	}
	acad_pos.x=pos.x;
	acad_pos.y=pos.y;
	pLine->addVertexAt(0,acad_pos,0,0,0);
	acad_pos.x+=10*vec.x;
	acad_pos.y+=10*vec.y;
	pLine->addVertexAt(1,acad_pos,0,0,0);
	acad_pos.x+=2*vec.x;
	acad_pos.y+=2*vec.y;
	pLine->addVertexAt(1,acad_pos,0,0,1.3);
	//SetCurLayer("LOADLINE");
	if(pBlockTableRecord->appendAcDbEntity(LineId,pLine)!=Acad::eOk)//��ʵ��д�����¼
		return;
	char dimText[50];
	//SetCurLayer("DIMTEXT");
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	sprintf(dimText,"%.f",fabs(val));
	if(iLocate==0)	//��ͷ��λ
		DimText(pBlockTableRecord,base,dimText,TextStyleTable::hzfs.textStyleId,sys.dim_map.fDimTextSize,rot_ang,AcDb::kTextLeft,AcDb::kTextBottom);
	else
		DimText(pBlockTableRecord,base,dimText,TextStyleTable::hzfs.textStyleId,sys.dim_map.fDimTextSize,rot_ang,AcDb::kTextRight,AcDb::kTextBottom);
	pLine->close();//���ùرն���ĳ�Ա����
}
void DrawLoadGraph()
{
	CLDSNode *pNode;
	CWorkStatus *pStatus;
	if(Ta.GetActiveModule()==NULL)
	{
		AfxMessageBox("û��ѡ��ǰ����ģ�ͣ�");
		return;	//û�е�ǰģ��
	}
	NODESET nodeset;
	for(pNode=Ta.Node.GetFirst();pNode;pNode=Ta.Node.GetNext())
	{
		for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
		{
			
			CExternalNodeLoad *pNodeLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
			if(pNodeLoad&&pNodeLoad->Fxyz.mod2()>0)
			{	//���к��صĽڵ�
				nodeset.append(pNode);
				break;
			}
		}
	}
	if(nodeset.GetNodeNum()==0)
	{
		AfxMessageBox("û�к��ؽڵ㣡");
		return;
	}
	f3dPoint *loadnode_arr=new f3dPoint[nodeset.GetNodeNum()+1];
	f3dPoint pos;
	f3dRect status_region;
	ads_point acad_pnt;
	char sPrompt[MAX_PATH];
	int i=0;
	CLoadArrowLocateDlg locate_dlg;
	CLoadGraphDlg graphdlg;
	AcDbObjectId LineId;//�����ʶ��
	AcDbBlockTableRecord *pBlockTableRecord=GetBlockTableRecord();
	struct resbuf *filter=NULL;
	ads_name entname,ent_sel_set;
	for(pStatus=Ta.WorkStatus.GetFirst();pStatus;pStatus=Ta.WorkStatus.GetNext())
	{
		if(i==0)
		{	//�׹�����Ҫѡ������ؽڵ�����λ��
			int j=0;
			for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
			{
				CExternalNodeLoad *pNodeLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
				//ѡ���ص�
				if(pNodeLoad&&pNodeLoad->Fxyz.mod2()>0)
				{	//�к���ֵ
				
					sprintf(sPrompt,"\n��ѡ��%d�ź��ؽڵ�����λ��,<Enter�˳�>:",pNode->point_i);
				#ifdef _ARX_2007
					if (ads_getpoint(NULL,_bstr_t(sPrompt),acad_pnt)!=RTNORM)
				#else
					if (ads_getpoint(NULL,sPrompt,acad_pnt)!=RTNORM)
				#endif
						goto END;
					loadnode_arr[j].Set(acad_pnt[X],acad_pnt[Y],acad_pnt[Z]);
					//���ؼ�ͷ��β��λѡ��
					if(locate_dlg.DoModal()!=IDOK)
						goto END;
					loadnode_arr[j].feature=0;
					if(locate_dlg.m_iXArrowLocate==1)
						loadnode_arr[j].feature|=1;
					if(locate_dlg.m_iYArrowLocate==1)
						loadnode_arr[j].feature|=2;
					if(locate_dlg.m_iZArrowLocate==1)
						loadnode_arr[j].feature|=4;
					char layer[3]={0};
					if(loadnode_arr[j].feature&1)
						layer[0]='1';
					if(loadnode_arr[j].feature&2)
						layer[1]='1';
					if(loadnode_arr[j].feature&4)
						layer[2]='1';
					CreateLoadArrow(pBlockTableRecord,'X',pNodeLoad->Fxyz.x,loadnode_arr[j],layer[0]-'0');
					CreateLoadArrow(pBlockTableRecord,'Y',pNodeLoad->Fxyz.y,loadnode_arr[j],layer[1]-'0');
					CreateLoadArrow(pBlockTableRecord,'Z',pNodeLoad->Fxyz.z,loadnode_arr[j],layer[2]-'0');
				}
				j++;
			}
			//ѡ�񹤿��������λ�ã������ı��м�㣩
		#ifdef _ARX_2007
			if (ads_getpoint(NULL,L"\n��ѡ�񹤿����Ʊ�עλ�ã��������м�Ϊ��λ��׼����",acad_pnt)!=RTNORM)
		#else
			if (ads_getpoint(NULL,"\n��ѡ�񹤿����Ʊ�עλ�ã��������м�Ϊ��λ��׼����",acad_pnt)!=RTNORM)
		#endif
				goto END;
			//SetCurLayer("FORCELINE");
			GetCurDwg()->setClayer(LayerTable::BriefLineLayer.layerId);
			loadnode_arr[nodeset.GetNodeNum()].Set(acad_pnt[X],acad_pnt[Y],acad_pnt[Z]);
			DimText(pBlockTableRecord,f3dPoint(acad_pnt[X],acad_pnt[Y],acad_pnt[Z]),pStatus->description,
				TextStyleTable::hzfs.textStyleId,sys.dim_map.fDimTextSize,0,AcDb::kTextCenter,AcDb::kTextVertMid);
			//����ѡ����ع������ƶ���
			AcDbObjectId entId;

			filter = acutBuildList(RTDXF0,"LINE",0);
		#ifdef _ARX_2007
			acedPrompt(L"\n��ѡ����Ҫ��ͬ����ͼ���Ƶĸ���ʾ���ߣ�\n");
		#else
			acedPrompt("\n��ѡ����Ҫ��ͬ����ͼ���Ƶĸ���ʾ���ߣ�\n");
		#endif
			if(acedSSGet(NULL,NULL,NULL,filter,ent_sel_set)!=RTNORM)
				goto END;
			//ѡ��ѡ����ع������������ϽǺ����½�
		#ifdef _ARX_2007
			if (ads_getpoint(NULL,L"\n��ѡ����ع�����ͼ���������Ͻ�λ�ã�",acad_pnt)!=RTNORM)
		#else
			if (ads_getpoint(NULL,"\n��ѡ����ع�����ͼ���������Ͻ�λ�ã�",acad_pnt)!=RTNORM)
		#endif
				goto END;
			status_region.topLeft.Set(acad_pnt[X],acad_pnt[Y],0);
		#ifdef _ARX_2007
			if (ads_getpoint(NULL,L"\n��ѡ����ع�����ͼ���������½�λ�ã�\n",acad_pnt)!=RTNORM)
		#else	
			if (ads_getpoint(NULL,"\n��ѡ����ع�����ͼ���������½�λ�ã�\n",acad_pnt)!=RTNORM)
		#endif
				goto END;
			status_region.bottomRight.Set(acad_pnt[X],acad_pnt[Y],0);
			//��ʾ����������������϶ֵ
			if(graphdlg.DoModal()!=IDOK)
				goto END;
		}
		else
		{//��ʼ��������ÿ�������ĺ���ͼ
			long ll;
			acedSSLength(ent_sel_set,&ll);
			//SetCurLayer("FORCELINE");
			GetCurDwg()->setClayer(LayerTable::BriefLineLayer.layerId);
			f3dPoint offset;
			if(graphdlg.m_iRowFirst==0)
			{
				offset.x=(i%graphdlg.m_nRowsOrColumns)*(status_region.Width()+graphdlg.m_nColumnGap);
				offset.y=-(i/graphdlg.m_nRowsOrColumns)*(status_region.Height()+graphdlg.m_nRowGap);
			}
			else
			{
				offset.x=(i/graphdlg.m_nRowsOrColumns)*(status_region.Width()+graphdlg.m_nColumnGap);
				offset.y=-(i%graphdlg.m_nRowsOrColumns)*(status_region.Height()+graphdlg.m_nRowGap);
			}
			for(long j=0;j<ll;j++)
			{
				AcDbEntity *pEnt;
				AcDbLine *pLine,*pNewLine;
				acedSSName(ent_sel_set,j,entname);
				AcDbObjectId entId;
				acdbGetObjectId(entId,entname);
				if(acdbOpenObject(pEnt,entId,AcDb::kForWrite)!=Acad::eOk)
					continue;
				if(!pEnt->isKindOf(AcDbLine::desc()))
				{
					pEnt->close();
					continue;
				}
				pLine=(AcDbLine*)pEnt;
				AcGePoint3d acad_start,acad_end;
				acad_start[X]=pLine->startPoint().x+offset.x;
				acad_start[Y]=pLine->startPoint().y+offset.y;
				acad_end[X]=pLine->endPoint().x+offset.x;
				acad_end[Y]=pLine->endPoint().y+offset.y;
				pLine->close();
				pNewLine=new AcDbLine(acad_start,acad_end);
				AcDbObjectId lineId;
				if(pBlockTableRecord->appendAcDbEntity(LineId,pNewLine)!=Acad::eOk)//��ʵ��д�����¼
					return;
				pNewLine->close();
			}
			//��ʼ��עÿ�����ؽڵ�ĺ��ؼ�ͷ
			j=0;
			f3dPoint pos;
			for(pNode=nodeset.GetFirst();pNode;pNode=nodeset.GetNext())
			{
				CExternalNodeLoad *pNodeLoad=pStatus->hashNodeLoad.GetValue(pNode->handle);
				//ѡ���ص�
				if(pNodeLoad&&pNodeLoad->Fxyz.mod2()>0)
				{	//�к���ֵ
					pos=loadnode_arr[j]+offset;
					char layer[3]={0};
					if(loadnode_arr[j].feature&1)
						layer[0]='1';
					if(loadnode_arr[j].feature&2)
						layer[1]='1';
					if(loadnode_arr[j].feature&4)
						layer[2]='1';
					CreateLoadArrow(pBlockTableRecord,'X',pNodeLoad->Fxyz.x,pos,layer[0]-'0');
					CreateLoadArrow(pBlockTableRecord,'Y',pNodeLoad->Fxyz.y,pos,layer[1]-'0');
					CreateLoadArrow(pBlockTableRecord,'Z',pNodeLoad->Fxyz.z,pos,layer[2]-'0');
				}
				j++;
			}
			pos=loadnode_arr[nodeset.GetNodeNum()]+offset;
			//SetCurLayer("FORCELINE");
			GetCurDwg()->setClayer(LayerTable::BriefLineLayer.layerId);
			DimText(pBlockTableRecord,pos,pStatus->description,
				TextStyleTable::hzfs.textStyleId,sys.dim_map.fDimTextSize,0,AcDb::kTextCenter,AcDb::kTextVertMid);
		}
		i++;
	}
END:
	if(filter)
		acutRelRb(filter);
	acedSSFree(ent_sel_set);
	pBlockTableRecord->close();
	delete []loadnode_arr;
}
#endif

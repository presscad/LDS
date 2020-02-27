//<LOCALE_TRANSLATE BY hxr /> 2015-04-27
//  定义函数 ---- DefTable            定义各种表格
#include "stdafx.h"
#include "stdarx.h"
//#include "TMA.h"
#include "MenuFunc.h"
#include "DrawTaMap.h"
#include "GlobalFunc.h"
#include "Query.h"
#include "adstools.h"
#include "rxtools.h"
#include "rxtools.h"
#include "SysPara.h"
#include "Tower.h"
#include "rxdefs.h"
#include "dbxrecrd.h"
#include "TblDef.h"
#include "TableGridDlg.h"
static void SetGridData(AcDbPoint *pPoint, GRID_DATA_STRU &grid_data)
{
	resbuf *pRb,*pNextRb;
	AcDbXrecord *pXrec;
	AcDbDictionary *pDict;
	AcDbObjectId dictObjId, xrecObjId;
	f2dRect rect;
	
	dictObjId = pPoint->extensionDictionary();
	if(dictObjId != AcDbObjectId::kNull)
	{
		acdbOpenObject(pDict, dictObjId, AcDb::kForRead);
		CAcDbObjLife dictLife(pDict);

#ifdef _ARX_2007
		if(pDict->getAt(L"TOWER_XREC",(AcDbObject* &)pXrec, AcDb::kForWrite)==Acad::eOk)
#else
		if(pDict->getAt("TOWER_XREC",(AcDbObject* &)pXrec, AcDb::kForWrite)==Acad::eOk)
#endif
		{
			pXrec->rbChain(&pRb);
			if(pRb==NULL)
				return;
			pRb->resval.rlong = grid_data.data_type;	//数据类型
			pNextRb=pRb->rbnext;
			if(pNextRb==NULL)
				return;
			pNextRb->resval.rlong = grid_data.type_id;	//内容ID
			pNextRb=pNextRb->rbnext;
			if(pNextRb==NULL)
				return;
			pNextRb->resval.rreal = grid_data.fTextHigh;//文字高度
			pNextRb=pNextRb->rbnext;
			if(pNextRb==NULL)
				return;
			pNextRb->resval.rreal = grid_data.fHighToWide;//文字宽高比
			pNextRb=pNextRb->rbnext;
			if(pNextRb==NULL)
				return;
			pNextRb->resval.rlong = grid_data.align_type+1;	//对齐方式
			//暂时不更新前后缀，需要添加前后缀时需要删除重新添加 wht 18-09-22
			pXrec->setFromRbChain(*pRb);
			pXrec->close();
			ads_relrb(pRb);
		}
	}
}
static void BuildTableGrid(SCOPE_STRU &grid_scope)
{
	ads_name key_pt_set,key_pt;
	struct resbuf *vert_list,*ent_type;
	ads_point vert_pt[4];

	vert_pt[0][X]=grid_scope.fMinX;
	vert_pt[0][Y]=grid_scope.fMinY;
	vert_pt[1][X]=grid_scope.fMaxX;
	vert_pt[1][Y]=grid_scope.fMinY;
	vert_pt[2][X]=grid_scope.fMaxX;
	vert_pt[2][Y]=grid_scope.fMaxY;
	vert_pt[3][X]=grid_scope.fMinX;
	vert_pt[3][Y]=grid_scope.fMaxY;
	vert_pt[0][Z]=vert_pt[1][Z]=vert_pt[2][Z]=vert_pt[3][Z]=0;
	vert_list=ads_buildlist(RTPOINT,vert_pt[0],RTPOINT,vert_pt[1],RTPOINT,vert_pt[2],RTPOINT,vert_pt[3],RTNONE);
	ent_type=ads_buildlist(RTDXF0,"POINT,MText",0);//选择所有的点集合

	long ll=0;
	AcDbPoint *pPoint;
	AcDbMText *pMText;
	AcDbXrecord *pXrec;
	AcDbDictionary *pDict;
	AcDbObjectId dictObjId, xrecObjId,pointId;
#ifdef _ARX_2007
	if (ads_ssget(L"cp",vert_list,NULL,ent_type,key_pt_set)==RTNORM)
#else
	if (ads_ssget("cp",vert_list,NULL,ent_type,key_pt_set)==RTNORM)
#endif
	{
		AcDbObjectId entId;
		AcDbEntity *pEnt;
		ads_sslength(key_pt_set,&ll);
		for (long j = 0; j < ll; j++)
		{
			ads_ssname(key_pt_set,j,key_pt);
			acdbGetObjectId(entId,key_pt);
			acdbOpenObject(pEnt,entId,AcDb::kForWrite);
			static CTableGridDlg dlg;
			//判断是否是一根直线
			if(pEnt->isKindOf(AcDbPoint::desc()))
			{
				pPoint = (AcDbPoint*)pEnt;
				GRID_DATA_STRU grid_data;
				memset(&grid_data,'\0',sizeof(GRID_DATA_STRU));
				if(!GetGridKey(pPoint,&grid_data))
				{
					pPoint->close();
					continue;
				}
				dlg.m_iDataType = grid_data.data_type;
				dlg.content_id = grid_data.type_id;
				dlg.m_sDisplayContent = grid_data.sContent;
				dlg.m_sDisplayPrefix = grid_data.sContentPrefix;
				dlg.m_sDisplaySuffix = grid_data.sContentSuffix;
				dlg.m_fTextHigh = grid_data.fTextHigh;
				dlg.m_fHighToWide = grid_data.fHighToWide;
				dlg.m_iAlignType=grid_data.align_type-1;
				if(dlg.DoModal()==IDOK)
				{
					grid_data.data_type = dlg.m_iDataType;
					grid_data.type_id = dlg.content_id;
					sprintf(grid_data.sContent,"%s",dlg.m_sDisplayContent);
					sprintf(grid_data.sContentPrefix,"%s",dlg.m_sDisplayPrefix);
					sprintf(grid_data.sContentSuffix,"%s",dlg.m_sDisplaySuffix);
					grid_data.fTextHigh = dlg.m_fTextHigh;
					grid_data.fHighToWide = dlg.m_fHighToWide;
					grid_data.min_x=grid_scope.fMinX;
					grid_data.max_x=grid_scope.fMaxX;
					grid_data.min_y=grid_scope.fMinY;
					grid_data.max_y=grid_scope.fMaxY;
					SetGridData(pPoint,grid_data);
				}
			}
			else if(pEnt->isKindOf(AcDbMText::desc()))
			{
				pMText = (AcDbMText*)pEnt;
				dlg.m_sDisplayContent = pMText->contents();
				dlg.m_iDataType=0;
				dlg.m_fTextHigh = pMText->textHeight();
				AcDbMText::AttachmentPoint align_type=pMText->attachment();
				if(align_type==AcDbMText::kTopLeft)
					dlg.m_iAlignType=0;
				else if(align_type==AcDbMText::kTopCenter)
					dlg.m_iAlignType=1;
				else if(align_type==AcDbMText::kTopRight)
					dlg.m_iAlignType=2;
				else if(align_type==AcDbMText::kMiddleLeft)
					dlg.m_iAlignType=3;
				else if(align_type==AcDbMText::kMiddleCenter)
					dlg.m_iAlignType=4;
				else if(align_type==AcDbMText::kMiddleRight)
					dlg.m_iAlignType=5;
				else if(align_type==AcDbMText::kBottomLeft)
					dlg.m_iAlignType=6;
				else if(align_type==AcDbMText::kBottomCenter)
					dlg.m_iAlignType=7;
				else if(align_type==AcDbMText::kBottomRight)
					dlg.m_iAlignType=8;
				else
					dlg.m_iAlignType=4;
				if(dlg.DoModal()==IDOK)
				{
					AcGePoint3d align_pos;
					align_pos.z = 0;
					if(dlg.m_iAlignType==0)
					{
						align_type=AcDbMText::kTopLeft;
						align_pos.x = grid_scope.fMinX+1.0;
						align_pos.y = grid_scope.fMaxY-1.0;
					}
					else if(dlg.m_iAlignType==1)
					{
						align_type=AcDbMText::kTopCenter;
						align_pos.x = (grid_scope.fMaxX+grid_scope.fMinX)/2.0;
						align_pos.y = grid_scope.fMaxY-1.0;
					}
					else if(dlg.m_iAlignType==2)
					{
						align_type=AcDbMText::kTopRight;
						align_pos.x = grid_scope.fMaxX-1.0;
						align_pos.y = grid_scope.fMaxY-1.0;
					}
					else if(dlg.m_iAlignType==3)
					{
						align_type=AcDbMText::kMiddleLeft;
						align_pos.x = grid_scope.fMinX+1.0;
						align_pos.y = (grid_scope.fMaxY+grid_scope.fMinY)/2.0;
					}
					else if(dlg.m_iAlignType==4)
					{
						align_type=AcDbMText::kMiddleCenter;
						align_pos.x = (grid_scope.fMaxX+grid_scope.fMinX)/2.0;
						align_pos.y = (grid_scope.fMaxY+grid_scope.fMinY)/2.0;
					}
					else if(dlg.m_iAlignType==5)
					{
						align_type=AcDbMText::kMiddleRight;
						align_pos.x = grid_scope.fMaxX-1.0;
						align_pos.y = (grid_scope.fMaxY+grid_scope.fMinY)/2.0;
					}
					else if(dlg.m_iAlignType==6)
					{
						align_type=AcDbMText::kBottomLeft;
						align_pos.x = grid_scope.fMinX+1.0;
						align_pos.y = grid_scope.fMinY+1.0;
					}
					else if(dlg.m_iAlignType==7)
					{
						align_type=AcDbMText::kBottomCenter;
						align_pos.x = (grid_scope.fMaxX+grid_scope.fMinX)/2.0;
						align_pos.y = grid_scope.fMinY+1.0;
					}
					else if(dlg.m_iAlignType==8)
					{
						align_type=AcDbMText::kBottomRight;
						align_pos.x = grid_scope.fMaxX-1.0;
						align_pos.y = grid_scope.fMinY+1.0;
					}
					else
					{
						align_type=AcDbMText::kMiddleCenter;
						align_pos.x = (grid_scope.fMaxX+grid_scope.fMinX)/2.0;
						align_pos.y = (grid_scope.fMaxY+grid_scope.fMinY)/2.0;
					}
#ifdef _ARX_2007
					pMText->setContents(_bstr_t(dlg.m_sDisplayContent.GetBuffer(200)));
#else
					pMText->setContents(dlg.m_sDisplayContent.GetBuffer(200));
#endif
					pMText->setTextHeight(dlg.m_fTextHigh);
					//pMText->setAlignmentPoint(align_pos);
					pMText->setLocation(align_pos);
					pMText->setAttachment(align_type);
				}
			}
			pEnt->close();
		}
		ads_ssfree(key_pt_set);
	}
	ads_relrb(vert_list);
	ads_relrb(ent_type);
	if(ll==0)	//还没有关键点存在,添加关键点
	{
		static CTableGridDlg dlg;
		if(dlg.DoModal()==IDOK)
		{
			AcDbBlockTable *pBlockTable;
			GetCurDwg()->getBlockTable(pBlockTable,AcDb::kForRead);
			//获得当前图形块表记录指针
			AcDbBlockTableRecord *pBlockTableRecord;//定义块表记录指针
			
			//以写方式打开模型空间，获得块表记录指针
			pBlockTable->getAt(ACDB_MODEL_SPACE,pBlockTableRecord,AcDb::kForWrite);
			pBlockTable->close();//关闭块表
			if(dlg.m_iDataType!=0)
			{
				//获得当前图形的块表指针
				AcGePoint3d pos;
				grid_scope.centre(&pos.x,&pos.y,&pos.z);
				pPoint=new AcDbPoint(pos);//创建point对象
				pBlockTableRecord->appendAcDbEntity(pointId,pPoint);//将实体写入块表记录
				pPoint->createExtensionDictionary();
				dictObjId = pPoint->extensionDictionary();
				pPoint->close();
				acdbOpenObject(pDict, dictObjId, AcDb::kForWrite);
				pXrec = new AcDbXrecord;
#ifdef _ARX_2007
				pDict->setAt(L"TOWER_XREC", pXrec, xrecObjId);
#else
				pDict->setAt("TOWER_XREC", pXrec, xrecObjId);
#endif
				pDict->close();
				
				struct resbuf *head = ads_buildlist(AcDb::kDxfInt32, dlg.m_iDataType,
					AcDb::kDxfInt32, dlg.content_id,AcDb::kDxfReal, dlg.m_fTextHigh,
					AcDb::kDxfReal, dlg.m_fHighToWide,AcDb::kDxfReal, grid_scope.fMinX,
					AcDb::kDxfReal, grid_scope.fMinY,AcDb::kDxfReal, grid_scope.fMaxX,
					AcDb::kDxfReal, grid_scope.fMaxY,AcDb::kDxfInt32, dlg.m_iAlignType+1,
#ifdef _ARX_2007
					AcDb::kDxfText, _bstr_t(dlg.m_sDisplayPrefix),AcDb::kDxfText, _bstr_t(dlg.m_sDisplaySuffix), 0);
#else
					AcDb::kDxfText, dlg.m_sDisplayPrefix,AcDb::kDxfText, dlg.m_sDisplaySuffix, 0);
#endif
				pXrec->setFromRbChain(*head);
				ads_relrb(head);
				pXrec->close();
			}
			else	//标题文本
			{
				f3dPoint align_pos;
				char sText[MAX_PATH];
				sprintf(sText,"%s",dlg.m_sDisplayContent);
				AcDbMText::AttachmentPoint align_type;
				align_pos.z = 0;
				if(dlg.m_iAlignType==0)
				{
					align_type=AcDbMText::kTopLeft;
					align_pos.x = grid_scope.fMinX+1.0;
					align_pos.y = grid_scope.fMaxY-1.0;
				}
				else if(dlg.m_iAlignType==1)
				{
					align_type=AcDbMText::kTopCenter;
					align_pos.x = (grid_scope.fMaxX+grid_scope.fMinX)/2.0;
					align_pos.y = grid_scope.fMaxY-1.0;
				}
				else if(dlg.m_iAlignType==2)
				{
					align_type=AcDbMText::kTopRight;
					align_pos.x = grid_scope.fMaxX-1.0;
					align_pos.y = grid_scope.fMaxY-1.0;
				}
				else if(dlg.m_iAlignType==3)
				{
					align_type=AcDbMText::kMiddleLeft;
					align_pos.x = grid_scope.fMinX+1.0;
					align_pos.y = (grid_scope.fMaxY+grid_scope.fMinY)/2.0;
				}
				else if(dlg.m_iAlignType==4)
				{
					align_type=AcDbMText::kMiddleCenter;
					align_pos.x = (grid_scope.fMaxX+grid_scope.fMinX)/2.0;
					align_pos.y = (grid_scope.fMaxY+grid_scope.fMinY)/2.0;
				}
				else if(dlg.m_iAlignType==5)
				{
					align_type=AcDbMText::kMiddleRight;
					align_pos.x = grid_scope.fMaxX-1.0;
					align_pos.y = (grid_scope.fMaxY+grid_scope.fMinY)/2.0;
				}
				else if(dlg.m_iAlignType==6)
				{
					align_type=AcDbMText::kBottomLeft;
					align_pos.x = grid_scope.fMinX+1.0;
					align_pos.y = grid_scope.fMinY+1.0;
				}
				else if(dlg.m_iAlignType==7)
				{
					align_type=AcDbMText::kBottomCenter;
					align_pos.x = (grid_scope.fMaxX+grid_scope.fMinX)/2.0;
					align_pos.y = grid_scope.fMinY+1.0;
				}
				else if(dlg.m_iAlignType==8)
				{
					align_type=AcDbMText::kBottomRight;
					align_pos.x = grid_scope.fMaxX-1.0;
					align_pos.y = grid_scope.fMinY+1.0;
				}
				else
				{
					align_type=AcDbMText::kMiddleCenter;
					align_pos.x = (grid_scope.fMaxX+grid_scope.fMinX)/2.0;
					align_pos.y = (grid_scope.fMaxY+grid_scope.fMinY)/2.0;
				}
				if(dlg.content_id==1001)
					InsertBlock(pBlockTableRecord,align_pos,"M12_HOLE",1.0,0);
				else if(dlg.content_id==1002)
					InsertBlock(pBlockTableRecord,align_pos,"M16_HOLE",1.0,0);
				else if(dlg.content_id==1003)
					InsertBlock(pBlockTableRecord,align_pos,"M20_HOLE",1.0,0);
				else if(dlg.content_id==1004)
					InsertBlock(pBlockTableRecord,align_pos,"M24_HOLE",1.0,0);
				else if(dlg.content_id==1005)
					InsertBlock(pBlockTableRecord,align_pos,"M18_HOLE",1.0,0);
				else if(dlg.content_id==1006)
					InsertBlock(pBlockTableRecord,align_pos,"M22_HOLE",1.0,0);
				else
					DimMText(pBlockTableRecord,sText,
						align_pos,grid_scope.wide(),dlg.m_fTextHigh,TextStyleTable::hzfs.textStyleId,align_type,AcDbMText::kLtoR);
			}
			pBlockTableRecord->close();
		}
	}
}
void DefTable()
{
	ads_point base_pnt;
	ads_name seqent,sss;
	struct resbuf *plist;
	f3dPoint f_pt;
	double scale = sys.general_set.StruMapScale.fRealToDraw;
    // When resource from this ARX app is needed, just
    // instantiate a local CAcModuleResourceOverride
    CAcModuleResourceOverride resOverride;
//---------------------------------------------------------	

#ifdef _ARX_2007
		ads_command(RTSTR,L"undo",RTSTR,L"g",RTNONE);
#else
		ads_command(RTSTR,"undo",RTSTR,"g",RTNONE);
#endif
	
	for(;;)
	{
#ifdef AFX_TARG_ENU_ENGLISH
#ifdef _ARX_2007
		if (ads_getpoint(NULL,L"\n Please click any point on table unit area,<Enter Exit>: ",base_pnt)==RTNORM)
#else
		if (ads_getpoint(NULL,"\n Please click any point on table unit area,<Enter Exit>: ",base_pnt)==RTNORM)
#endif
#else
#ifdef _ARX_2007
		if (ads_getpoint(NULL,L"\n请点表格单元区域内任一点,<Enter退出>: ",base_pnt)==RTNORM)
#else
		if (ads_getpoint(NULL,"\n请点表格单元区域内任一点,<Enter退出>: ",base_pnt)==RTNORM)
#endif
#endif
		{
#ifdef _ARX_2007
			int resCode = ads_command(RTSTR,L"bpoly",RTPOINT,base_pnt,RTSTR,"",RTNONE);
#else
			int resCode = ads_command(RTSTR,"bpoly",RTPOINT,base_pnt,RTSTR,"",RTNONE);
#endif
			ads_ssadd(NULL,NULL,sss);
			
			AcGePoint3d location;
			int nn,vertexNumber;
			if (resCode == RTNORM)
			{
				ads_entlast(seqent);
				//遍历Polyline顶点
				AcDbObjectId plineId;
				acdbGetObjectId(plineId,seqent);
				AcDbPolyline *pPline;
				acdbOpenObject(pPline,plineId,AcDb::kForRead);
				if(pPline==NULL)
				{
#ifdef AFX_TARG_ENU_ENGLISH
					AfxMessageBox("Selected a wrong table unit,please select again");
#else
					AfxMessageBox("选择了错误的表格单元,请重新选择");
#endif
					continue;
				}
				pPline->close();
				acdbOpenObject(pPline,plineId,AcDb::kForWrite);
				nn = pPline->numVerts();
				SCOPE_STRU grid_scope;
				for(vertexNumber = 0;vertexNumber<nn;vertexNumber++)
				{
					pPline->getPointAt(vertexNumber,location);
					Cpy_Pnt(f_pt,location);
					grid_scope.VerifyVertex(f_pt);
				}
				pPline->close();
				plist=ads_buildlist(RTSTR,"erase",RTENAME,seqent,RTSTR,"",RTPOINT,base_pnt,RTREAL,sys.dim_map.fSimTriScale,RTNONE);
				ads_cmd(plist);
				ads_relrb(plist);
				BuildTableGrid(grid_scope);
			}
			else
				continue;
		}
		else
			break;
	}
}

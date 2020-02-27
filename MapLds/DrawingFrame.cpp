#include "StdAfx.h"
#include "RxTools.h"
#include "DrawingToolKits.h"
#include "XerocadDrawing.h"
#include "DrawingFrame.h"
#include "LocalDrawing.h"
#include "dbmtext.h"
//
bool ExtractionDataFromXerofoDrawing(IDrawing *pDrawing,CHashList<GRID_DATA_STRU>& hashAttachDataByPointId,const char *sDwgFileName/*=NULL*/)
{
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	AcDbDatabase blkDb,*pBlkdb=NULL;//定义空的数据库
	Acad::ErrorStatus errorStatus;
	if(sDwgFileName==NULL)
		pBlkdb=GetCurDwg();
	else
	{
#ifdef _ARX_2007
		errorStatus=blkDb.readDwgFile((ACHAR*)_bstr_t(sDwgFileName),_SH_DENYRW,true);//==Acad::eOk)
#else
		errorStatus=blkDb.readDwgFile(sDwgFileName,_SH_DENYRW,true);
#endif
	}
	if(errorStatus==Acad::eOk||pBlkdb)
	{
		AcDbEntity *pEnt=NULL;
		AcDbBlockTable *pTempBlockTable=NULL;
		if(pBlkdb)
			pBlkdb->getBlockTable(pTempBlockTable,AcDb::kForRead);
		else
			blkDb.getBlockTable(pTempBlockTable,AcDb::kForRead);
		//获得当前图形块表记录指针
		AcDbBlockTableRecord *pTempBlockTableRecord;//定义块表记录指针
		//以写方式打开模型空间，获得块表记录指针
		pTempBlockTable->getAt(ACDB_MODEL_SPACE,pTempBlockTableRecord,AcDb::kForWrite);
		pTempBlockTable->close();//关闭块表
		AcDbBlockTableRecordIterator *pIterator=NULL;
		pTempBlockTableRecord->newIterator( pIterator);
		if(pIterator)
		{
			for(;!pIterator->done();pIterator->step())
			{
				pIterator->getEntity(pEnt,AcDb::kForWrite);
				if(pEnt->isKindOf(AcDbPoint::desc()))
				{
					AcDbPoint* pAcDbPoint=(AcDbPoint*)pEnt;
					AcGePoint3d pos=pAcDbPoint->position();
					IDbPoint *pPoint=AppendDbPoint(pDrawing,GEPOINT(pos.x,pos.y,pos.z));
					GRID_DATA_STRU grid_data;
					if(!GetGridKey((AcDbPoint*)pEnt,&grid_data))
					{
						pEnt->close();
						continue;
					}
					hashAttachDataByPointId.SetValue(pPoint->GetId(),grid_data);
				}
				else if(pEnt->isKindOf(AcDbLine::desc()))
				{
					AcDbLine* pKeyLine=(AcDbLine*)pEnt;
					AcGePoint3d pos=pKeyLine->startPoint();
					GEPOINT startPos(pos.x,pos.y,pos.z);
					pos=pKeyLine->endPoint();
					GEPOINT endPos(pos.x,pos.y,pos.z);
					AppendDbLine(pDrawing,startPos,endPos);
				}
				else if (pEnt->isKindOf(AcDbMText::desc()))
				{
					AcDbMText* pMText=(AcDbMText*)pEnt;
					CXhChar100 sText;
				#ifdef _ARX_2007
					sText.Copy(_bstr_t(pMText->contents()));
					if(sText.Length()<=0)
						sText.Copy(_bstr_t(pMText->text()));
				#else
					sText.Copy(pMText->contents());
					if(sText.Length()<=0)
						sText.Copy(pMText->text());
				#endif
					AcGePoint3d pos=pMText->location();
					AppendDbMText(pDrawing,f3dPoint(pos.x,pos.y,pos.z),sText,pMText->rotation(),pMText->textHeight(),pMText->width());
				}
				else if (pEnt->isKindOf(AcDbText::desc()))
				{
					AcDbText* pText=(AcDbText*)pEnt;
					CXhChar100 sText;
			#ifdef _ARX_2007
					sText.Copy(_bstr_t(pText->textString()));
			#else
					sText.Copy(pText->textString());
			#endif
					AcGePoint3d pos=pText->position();
					AppendDbText(pDrawing,f3dPoint(pos.x,pos.y,pos.z),sText,pText->rotation(),pText->height());
				}
				else if (pEnt->isKindOf(AcDbPolyline::desc()))
				{
					AcDbPolyline* pPolyLine=(AcDbPolyline*)pEnt;
					ARRAY_LIST<GEPOINT> ptArr;
					for (int i=0;i<(int)pPolyLine->numVerts();i++)
					{
						AcGePoint2d pt;
						pPolyLine->getPointAt(i,pt);
						ptArr.append(GEPOINT(pt.x,pt.y,0));
					}
					AppendDbPolyline(pDrawing,ptArr.m_pData,ptArr.GetSize());
				}
				else if (pEnt->isKindOf(AcDbCircle::desc()))
				{
					AcDbCircle *pKeyCircle=(AcDbCircle*)pEnt;
					AcGePoint3d center=pKeyCircle->center();
					AcGeVector3d normal=pKeyCircle->normal();
					double dd=pKeyCircle->radius();
					AppendDbCircle(pDrawing,f3dPoint(center.x,center.y,center.z),
								f3dPoint(normal.x,normal.y,normal.z),dd);
				}
				pEnt->close();
			}
		}
		pTempBlockTableRecord->close();
	}
	return true;
}



bool SaveXXDFromDwg(const char* sXDFFileName,const char *sDwgFileName/*=NULL*/)
{
	CXeroDrawing drawing;
	CHashList <GRID_DATA_STRU>hashAttachDataByPointId;
	ExtractionDataFromXerofoDrawing(&drawing,hashAttachDataByPointId,sDwgFileName);
	SaveXeroDrawingAndDataToFile(&drawing,hashAttachDataByPointId,sXDFFileName);
	return true;
}
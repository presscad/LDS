#include "StdAfx.h"
#include "StdArx.h"
#include "RxTools.h"
#include "DrawingToolKits.h"
#include "LocalDrawing.h"
#include "IXeroCad.h"
bool SaveXeroDrawingAndDataToFile(IDrawing *pDrawing,CHashList<GRID_DATA_STRU>& hashAttachDataByPointId,const char* sFileName=NULL); //from XerocadDrawing.h

int CadAlignToXeroAlign(AcDb::TextHorzMode hMode,AcDb::TextVertMode vMode)
{
	if(hMode==AcDb::kTextLeft)
	{
		if(vMode==AcDb::kTextTop)
			return IDbText::AlignTopLeft;
		else if(vMode==AcDb::kTextVertMid)
			return IDbText::AlignMiddleLeft;
		else if(vMode==AcDb::kTextBottom||vMode==AcDb::kTextBase)
			return IDbText::AlignBottomLeft;
	}
	else if(hMode==AcDb::kTextRight)
	{
		if(vMode==AcDb::kTextTop)
			return IDbText::AlignTopRight;
		else if(vMode==AcDb::kTextVertMid)
			return IDbText::AlignMiddleRight;
		else if(vMode==AcDb::kTextBottom||vMode==AcDb::kTextBase)
			return IDbText::AlignBottomRight;
	}
	else if(hMode==AcDb::kTextCenter)
	{
		if(vMode==AcDb::kTextTop)
			return IDbText::AlignTopCenter;
		else if(vMode==AcDb::kTextVertMid)
			return IDbText::AlignMiddleCenter;
		else if(vMode==AcDb::kTextBottom||vMode==AcDb::kTextBase)
			return IDbText::AlignBottomCenter;
	}
	return IDbText::AlignMiddleCenter;
}

int CadAttachmentPointToXeroAlign(AcDbMText::AttachmentPoint attachPt)
{
	if(attachPt==AcDbMText::kTopLeft)
		return IDbText::AlignTopLeft;
	else if(attachPt==AcDbMText::kTopCenter)
		return IDbText::AlignTopCenter;
	else if(attachPt==AcDbMText::kTopRight)
		return IDbText::AlignTopRight;
	else if(attachPt==AcDbMText::kMiddleLeft)
		return IDbText::AlignMiddleLeft;
	else if(attachPt==AcDbMText::kMiddleCenter)
		return IDbText::AlignMiddleCenter;
	else if(attachPt==AcDbMText::kMiddleRight)
		return IDbText::AlignMiddleRight;
	else if(attachPt==AcDbMText::kBottomLeft)
		return IDbText::AlignBottomLeft;
	else if(attachPt==AcDbMText::kBottomCenter)
		return IDbText::AlignBottomCenter;
	else if(attachPt==AcDbMText::kBottomRight)
		return IDbText::AlignBottomRight;
	else
		return IDbText::AlignDefault;
}

bool SavePCDFromDwg(const char* sXDFFileName,const char *sDwgFileName/*=NULL*/)
{
	CXeroDrawing drawing;
	CHashList <GRID_DATA_STRU>hashAttachDataByPointId;
	GetCurDwg()->setClayer(LayerTable::VisibleProfileLayer.layerId);
	AcDbDatabase blkDb(Adesk::kFalse),*pBlkdb=NULL;//定义空的数据库
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
					IDbPoint *pPoint=AppendDbPoint(&drawing,GEPOINT(pos.x,pos.y,pos.z));
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
					AppendDbLine(&drawing,startPos,endPos);
				}
				else if (pEnt->isKindOf(AcDbMText::desc()))
				{
					AcDbMText* pMText=(AcDbMText*)pEnt;
					CXhChar100 sText;
#ifdef _ARX_2007
					sText.Copy(_bstr_t(pMText->contents()));
#else
					sText.Copy(pMText->contents());
#endif
					AcGePoint3d pos=pMText->location();
					int alignType=CadAttachmentPointToXeroAlign(pMText->attachment());
					AppendDbMText(&drawing,f3dPoint(pos.x,pos.y,pos.z),sText,pMText->rotation(),pMText->textHeight(),pMText->width(),alignType);
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
					AcDb::TextHorzMode hMode=pText->horizontalMode();
					AcDb::TextVertMode vMode=pText->verticalMode();
					if(vMode!=AcDb::kTextBase||hMode!=AcDb::kTextLeft)
						pos=pText->alignmentPoint();
					int alignType=CadAlignToXeroAlign(hMode,vMode);
					AppendDbText(&drawing,f3dPoint(pos.x,pos.y,pos.z),sText,pText->rotation(),pText->height(),alignType);
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
					AppendDbPolyline(&drawing,ptArr.m_pData,ptArr.GetSize());
				}
				else if (pEnt->isKindOf(AcDbCircle::desc()))
				{
					AcDbCircle *pKeyCircle=(AcDbCircle*)pEnt;
					AcGePoint3d center=pKeyCircle->center();
					AcGeVector3d normal=pKeyCircle->normal();
					double dd=pKeyCircle->radius();
					AppendDbCircle(&drawing,f3dPoint(center.x,center.y,center.z),
						f3dPoint(normal.x,normal.y,normal.z),dd);
				}
				pEnt->close();
			}
		}
		pTempBlockTableRecord->close();
	}
	SaveXeroDrawingAndDataToFile(&drawing,hashAttachDataByPointId,sXDFFileName);
	return true;
}

void TransformProcessCard()
{
	CFileDialog openFiledlg(TRUE,"DWG",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"工艺卡(*.dwg)|*.dwg|");
	if(openFiledlg.DoModal()!=IDOK)
		return;
	CXhChar200 sOpenFileName=openFiledlg.GetPathName();
	//CFileDialog saveFiledlg(FALSE,"DWG",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"工艺卡(*.pcd)|*.pcd|");
	//if(saveFiledlg.DoModal()!=IDOK)
	//	return;
	CXhChar200 sSaveFileName=sOpenFileName;
	sSaveFileName.Replace(".dwg",".pcd");
	SavePCDFromDwg(sSaveFileName,sOpenFileName);
}
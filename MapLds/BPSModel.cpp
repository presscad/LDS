#include "StdAfx.h"
#include "BPSModel.h"
#include "RxTools.h"
#include "TblDef.h"
#include "DefCard.h"
#include "ArrayList.h"
#include "PartLib.h"
#include "ProcBarDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CBPSModel BPSModel;
//////////////////////////////////////////////////////////////////////////
//
CHashList<f2dRect> CIdentifyManager::hashRectByItemType;
f3dPoint CIdentifyManager::partNoPt;
double CIdentifyManager::fPartNoFontSize;
double CIdentifyManager::fMaxX;
double CIdentifyManager::fMaxY;
double CIdentifyManager::fMinX;
double CIdentifyManager::fMinY;
CXhChar500 CIdentifyManager::sJgCardPath;
CIdentifyManager::CIdentifyManager()
{
	fMaxX=0;
	fMaxY=0;
	fMinX=0;
	fMinY=0;
	fPartNoFontSize=0;
}
CIdentifyManager::~CIdentifyManager()
{

}
bool CIdentifyManager::InitJgCardInfo(const char* sFileName)
{
	sJgCardPath.Copy(sFileName);
	if(strlen(sFileName)<=0)
		return false;
	AcDbDatabase blkDb(Adesk::kFalse);//����յ����ݿ�
#ifdef _ARX_2007
	if(blkDb.readDwgFile((ACHAR*)_bstr_t(sFileName),_SH_DENYRW,true)==Acad::eOk)
#else
	if(blkDb.readDwgFile(sFileName,_SH_DENYRW,true)==Acad::eOk)
#endif
	{
		AcDbEntity *pEnt;
		AcDbBlockTable *pTempBlockTable;
		blkDb.getBlockTable(pTempBlockTable,AcDb::kForRead);
		AcDbBlockTableRecord *pTempBlockTableRecord;//�������¼ָ��
		pTempBlockTable->getAt(ACDB_MODEL_SPACE,pTempBlockTableRecord,AcDb::kForRead);
		pTempBlockTable->close();//�رտ��
		AcDbBlockTableRecordIterator *pIterator=NULL;
		pTempBlockTableRecord->newIterator( pIterator);
		SCOPE_STRU scope;
		CXhChar50 sText;
		for(;!pIterator->done();pIterator->step())
		{
			pIterator->getEntity(pEnt,AcDb::kForRead);
			CAcDbObjLife entObj(pEnt);
			if(pEnt->isKindOf(AcDbLine::desc()))
			{
				AcDbLine* pLine=(AcDbLine*)pEnt;
				f3dPoint startPt,endPt,vec;
				Cpy_Pnt(startPt,pLine->startPoint());
				Cpy_Pnt(endPt,pLine->endPoint());
				scope.VerifyVertex(startPt);
				scope.VerifyVertex(endPt);
				continue;
			}
			else if(pEnt->isKindOf(AcDbPolyline::desc()))
			{
				AcDbPolyline* pPolyLine=(AcDbPolyline*)pEnt;
				AcGePoint3d location;
				int nVertNum = pPolyLine->numVerts();
				for(int iVertIndex = 0;iVertIndex<nVertNum;iVertIndex++)
				{
					pPolyLine->getPointAt(iVertIndex,location);
					scope.VerifyVertex(f3dPoint(location.x,location.y,location.z));
				}
				continue;
			}
			if(pEnt->isKindOf(AcDbMText::desc()))
			{
				AcDbMText* pMText=(AcDbMText*)pEnt;
#ifdef _ARX_2007
				sText.Copy(_bstr_t(pMText->contents()));
#else
				sText.Copy(pMText->contents());
#endif
				if(strstr(sText,"��") && strstr(sText,"��"))
				{
					partNoPt.Set(pMText->location().x,pMText->location().y,0);
#ifdef _ARX_2007
					fPartNoFontSize=pMText->height();
#else
					fPartNoFontSize=pMText->textHeight();
#endif
				}
				continue;
			}
			if(pEnt->isKindOf(AcDbText::desc()))
			{
				AcDbText* pText=(AcDbText*)pEnt;
#ifdef _ARX_2007
				sText.Copy(_bstr_t(pText->textString()));
#else
				sText.Copy(pText->textString());
#endif
				if(strstr(sText,"��") && strstr(sText,"��"))
				{
					partNoPt.Set(pText->position().x,pText->position().y,0);
					fPartNoFontSize=pText->height();
				}
				continue;
			}
			if(!pEnt->isKindOf(AcDbPoint::desc()))
				continue;
			GRID_DATA_STRU grid_data;
			if(!GetGridKey((AcDbPoint*)pEnt,&grid_data))
				continue;
			f2dRect *pRect=hashRectByItemType.Add(grid_data.type_id+1);
			if(pRect)
			{
				pRect->topLeft.Set(grid_data.min_x,grid_data.max_y);
				pRect->bottomRight.Set(grid_data.max_x,grid_data.min_y);
			}
		}
		pTempBlockTableRecord->close();
		//���տ���������
		fMinX=scope.fMinX;
		fMinY=scope.fMinY;
		fMaxX=scope.fMaxX;
		fMaxY=scope.fMaxY;
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
//
CAngleProcessInfo::CAngleProcessInfo()
{
	m_ciType=1;
	m_nWidth=m_nThick=m_nLength=0;
	m_nSumNum=0;
	m_cMaterial='S';
	keyId=NULL;
	m_bInBlockRef=TRUE;
	m_bUploadToServer=FALSE;
	m_nM12=m_nM16=m_nM18=m_nM20=m_nM22=m_nM24=m_nSumHoleCount=0;
	m_bCutRoot=m_bCutBer=m_bMakBend=m_bPushFlat=m_bCutEdge=m_bRollEdge=m_bWeld=m_bKaiJiao=m_bHeJiao=m_bCutAngle=0;
	m_bUpdatePng=FALSE;
	m_iBatchNo=0;
	m_fScale2Template=1;
}
CAngleProcessInfo::~CAngleProcessInfo()
{

}
//��ʼ���빤�տ�ģ��Ķ�Ӧ��ϵ
void CAngleProcessInfo::InitOrig()
{
	if(m_bInBlockRef)
		orig_pt=sign_pt-CIdentifyManager::GetLeftBtmPt()*m_fScale2Template;
	else
		orig_pt=sign_pt-CIdentifyManager::GetPartnoPt()*m_fScale2Template;
}
//���ɽǸֹ��տ�����
void CAngleProcessInfo::CreateRgn(ARRAY_LIST<f3dPoint>& vertexList)
{
	for(int i=0;i<vertexList.GetSize();i++)
		scope.VerifyVertex(vertexList[i]);
	region.CreatePolygonRgn(vertexList.m_pData,vertexList.GetSize());
}
//�ж�������Ƿ��ڽǸֹ��տ���
bool CAngleProcessInfo::PtInAngleRgn(const double* poscoord)
{
	if(region.GetAxisZ().IsZero())
		return false;
	int nRetCode=region.PtInRgn(poscoord);
	return nRetCode==1;
}
//�������ݵ����ͻ�ȡ������������
f2dRect CAngleProcessInfo::GetAngleDataRect(BYTE data_type)
{
	f2dRect rect;
	f2dRect *pRect=CIdentifyManager::GetItemRect(data_type);
	if(pRect)
	{
		rect=*pRect;
		rect.topLeft.x*=m_fScale2Template;
		rect.topLeft.y*=m_fScale2Template;
		rect.bottomRight.x*=m_fScale2Template;
		rect.bottomRight.y*=m_fScale2Template;
		if(data_type==ITEM_TYPE_PART_NO)	//�еļ��Ź������������ַŵ��˷����·����������󷽿������
			rect.bottomRight.y-=rect.Height();
		rect.topLeft.x+=orig_pt.x;
		rect.topLeft.y+=orig_pt.y;
		rect.bottomRight.x+=orig_pt.x;
		rect.bottomRight.y+=orig_pt.y;
	}
	return rect;
}
//�ж�������Ƿ���ָ�����͵����ݿ���
bool CAngleProcessInfo::PtInDataRect(BYTE data_type,f3dPoint pos)
{
	f2dRect rect=GetAngleDataRect(data_type);
	f2dPoint pt(pos.x,pos.y);
	if(rect.PtInRect(pt))
		return true;
	else
		return false;
}
//
void CAngleProcessInfo::CopyProperty(CAngleProcessInfo* pJg)
{
	m_ciType=pJg->m_ciType;
	m_sPartNo=pJg->m_sPartNo;
	m_sSpec=pJg->m_sSpec;
	m_cMaterial=pJg->m_cMaterial;
	m_nWidth=pJg->m_nWidth;
	m_nThick=pJg->m_nThick;
	m_nLength=pJg->m_nLength;
	m_nSumNum=pJg->m_nSumNum;
	scope=pJg->GetCADEntScope();
	m_sCardPngFile.Copy(pJg->m_sCardPngFile);
	m_sCardPngFilePath.Copy(pJg->m_sCardPngFilePath);
	m_sNotes.Copy(pJg->m_sNotes);
	m_sTowerType.Copy(pJg->m_sTowerType);
	m_nSumHoleCount=pJg->m_nSumHoleCount;
	m_nM12=pJg->m_nM12;
	m_nM16=pJg->m_nM16;
	m_nM18=pJg->m_nM18;
	m_nM20=pJg->m_nM20;
	m_nM22=pJg->m_nM22;
	m_nM24=pJg->m_nM24;
	m_bCutRoot=pJg->m_bCutRoot;
	m_bCutBer=pJg->m_bCutBer;	
	m_bMakBend=pJg->m_bMakBend;
	m_bPushFlat=pJg->m_bPushFlat;
	m_bCutEdge=pJg->m_bCutEdge;
	m_bRollEdge=pJg->m_bRollEdge;
	m_bWeld=pJg->m_bWeld;	
	m_bKaiJiao=pJg->m_bKaiJiao;
	m_bHeJiao=pJg->m_bHeJiao;	
	m_bCutAngle=pJg->m_bCutAngle;
	m_bUpdatePng=pJg->m_bUpdatePng;
	m_bUploadToServer=pJg->m_bUploadToServer;
	m_iBatchNo=pJg->m_iBatchNo;
	m_fScale2Template=pJg->m_fScale2Template;
}
//��ʼ���Ǹ���Ϣ
void CAngleProcessInfo::InitAngleInfo(f3dPoint data_pos,const char* sValue)
{
	if(PtInDataRect(ITEM_TYPE_PART_NO,data_pos))	//����
		m_sPartNo.Copy(sValue);
	else if(PtInDataRect(ITEM_TYPE_DES_MAT,data_pos))	//����
		m_cMaterial=CBPSModel::QueryBriefMatMark(sValue);
	else if(PtInDataRect(ITEM_TYPE_DES_GUIGE,data_pos))	//���
	{	
		CXhChar50 sSpec(sValue);
		if(strstr(sSpec,"��")||strstr(sSpec,"L"))
		{	//�Ǹ�
			m_ciType=TYPE_JG;
			sSpec.Replace("��","L");
			if(strstr(sSpec,"��"))
				sSpec.Replace("��","*");
			CBPSModel::RestoreSpec(sSpec,&m_nWidth,&m_nThick);
		}
		else if(strstr(sSpec,"-"))
		{
			m_ciType=TYPE_FLAT;
			CBPSModel::RestoreSpec(sSpec,&m_nWidth,&m_nThick);
		}
		else if(strstr(sSpec,"%c"))
		{	//�ֹ�/Բ��
			sSpec.Replace("%c","��");
			if(strstr(sSpec,"/"))
				m_ciType=TYPE_TUBE;
			else
				m_ciType=TYPE_YG;
		}
		else if(strstr(sSpec,"C"))
			m_ciType=TYPE_JIG;
		else if(strstr(sSpec,"G"))
			m_ciType=TYPE_GGS;
		m_sSpec.Copy(sSpec);
	}
	else if(PtInDataRect(ITEM_TYPE_LENGTH,data_pos))	//����
		m_nLength=atoi(sValue);
	else if(PtInDataRect(ITEM_TYPE_SUM_PART_NUM,data_pos))	//�ӹ���
		m_nSumNum=atoi(sValue);
	else if(PtInDataRect(ITEM_TYPE_PART_NOTES,data_pos))
		m_sNotes.Copy(sValue);
	else if(PtInDataRect(ITEM_TYPE_TA_TYPE,data_pos))
		m_sTowerType.Copy(sValue);
	else if(PtInDataRect(ITEM_TYPE_CUT_ANGLE,data_pos)||
		PtInDataRect(ITEM_TYPE_CUT_ANGLE_E_X,data_pos)||
		PtInDataRect(ITEM_TYPE_CUT_ANGLE_E_Y,data_pos)||
		PtInDataRect(ITEM_TYPE_CUT_ANGLE_S_X,data_pos)||
		PtInDataRect(ITEM_TYPE_CUT_ANGLE_S_Y,data_pos))
		m_bCutAngle=TRUE;
	else if(PtInDataRect(ITEM_TYPE_CUT_BER,data_pos))
		m_bCutBer=TRUE;
	else if(PtInDataRect(ITEM_TYPE_CUT_ROOT,data_pos))
		m_bCutRoot=TRUE;
	else if(PtInDataRect(ITEM_TYPE_MAKE_BEND,data_pos))
		m_bMakBend=TRUE;
	else if(PtInDataRect(ITEM_TYPE_PUSH_FLAT,data_pos)||
		PtInDataRect(ITEM_TYPE_PUSH_FLAT_E_X,data_pos)||
		PtInDataRect(ITEM_TYPE_PUSH_FLAT_E_Y,data_pos)||
		//PtInDataRect(ITEM_TYPE_PUSH_FLAT_M_X,data_pos)||
		//PtInDataRect(ITEM_TYPE_PUSH_FLAT_M_Y,data_pos)||
		PtInDataRect(ITEM_TYPE_PUSH_FLAT_S_X,data_pos)||
		PtInDataRect(ITEM_TYPE_PUSH_FLAT_S_Y,data_pos))
		m_bPushFlat=TRUE;
	else if(PtInDataRect(ITEM_TYPE_KAIJIAO,data_pos))
		m_bKaiJiao=TRUE;
	else if(PtInDataRect(ITEM_TYPE_HEJIAO,data_pos))
		m_bHeJiao=TRUE;
	else if(PtInDataRect(ITEM_TYPE_TA_TYPE,data_pos))
		m_sTowerType.Copy(sValue);
	else if(PtInDataRect(ITEM_TYPE_LSM12_NUM,data_pos))
		m_nM12=atoi(sValue);
	else if(PtInDataRect(ITEM_TYPE_LSM16_NUM,data_pos))
		m_nM16=atoi(sValue);
	else if(PtInDataRect(ITEM_TYPE_LSM18_NUM,data_pos))
		m_nM18=atoi(sValue);
	else if(PtInDataRect(ITEM_TYPE_LSM20_NUM,data_pos))
		m_nM20=atoi(sValue);
	else if(PtInDataRect(ITEM_TYPE_LSM22_NUM,data_pos))
		m_nM22=atoi(sValue);
	else if(PtInDataRect(ITEM_TYPE_LSM24_NUM,data_pos))
		m_nM24=atoi(sValue);
	else if(PtInDataRect(ITEM_TYPE_LSSUM_NUM,data_pos))
		m_nSumHoleCount=atoi(sValue);

}
//��ȡ�Ǹ����ݵ�����
f3dPoint CAngleProcessInfo::GetAngleDataPos(BYTE data_type)
{
	f2dRect rect=GetAngleDataRect(data_type);
	double fx=(rect.topLeft.x+rect.bottomRight.x)*0.5;
	double fy=(rect.topLeft.y+rect.bottomRight.y)*0.5;
	return f3dPoint(fx,fy,0);
}

CXhChar50 CAngleProcessInfo::GetPartName() const
{
	CXhChar50 sName;
	if(m_ciType==1)
		sName.Copy("�Ǹ�");
	else if(m_ciType==2)
		sName.Copy("Բ��");
	else if(m_ciType==3)
		sName.Copy("�ֹ�");
	else if(m_ciType==4)
		sName.Copy("����");
	else if(m_ciType==5)
		sName.Copy("�о�");
	else
		sName.Copy("�ָ�դ");
	return sName;
}
//////////////////////////////////////////////////////////////////////////
//CBPSModel
CBPSModel::CBPSModel(void)
{
	m_hashJgInfo.Empty();
	m_idTowerType=0;
	m_idProject=0;
	m_iRetrieveBatchNo=1;
}
CBPSModel::~CBPSModel(void)
{

}
CXhChar16 CBPSModel::QuerySteelMatMark(char cMat)
{
	CXhChar16 sMatMark;
	if('H'==cMat)
		sMatMark.Copy("Q345");
	else if('G'==cMat)
		sMatMark.Copy("Q390");
	else if('P'==cMat)
		sMatMark.Copy("Q420");
	else if('T'==cMat)
		sMatMark.Copy("Q460");
	else //if('S'==cMat)
		sMatMark.Copy("Q235");
	return sMatMark;
}
char CBPSModel::QueryBriefMatMark(const char* sMatMark)
{
	char cMat='S';
	if(strstr(sMatMark,"Q345"))
		cMat='H';
	else if(strstr(sMatMark,"Q390"))
		cMat='G';
	else if(strstr(sMatMark,"Q420"))
		cMat='P';
	else if(strstr(sMatMark,"Q460"))
		cMat='T';
	return cMat;
}
int CBPSModel::QuerySteelMatIndex(char cMat)
{
	int index=235;
	if(cMat=='H')
		index=345;
	else if(cMat=='G')
		index=390;
	else if(cMat=='P')
		index=420;
	else if(cMat=='T')
		index=460;
	return index;
}
void CBPSModel::RestoreSpec(const char* spec,int *width,int *thick,char *matStr/*=NULL*/)
{
	char sMat[16]="",cMark1=' ',cMark2=' ';
	if(strstr(spec,"Q")==(char*)spec)
	{
		if(strstr(spec,"L"))
			sscanf(spec,"%[^L]%c%d%c%d",sMat,&cMark1,width,&cMark2,thick);
		else if(strstr(spec,"-"))
			sscanf(spec,"%[^-]%c%d",sMat,&cMark1,thick);
	}
	else if(strstr(spec,"L"))
	{
		CXhChar16 sSpec(spec);
		sSpec.Replace("L","");
		sSpec.Replace("*"," ");
		sSpec.Replace("X"," ");
		sscanf(sSpec,"%d%d",width,thick);
	}
	else if (strstr(spec,"-"))
	{
		CXhChar16 sSpec(spec);
		sSpec.Replace("-","");
		sSpec.Replace("*"," ");
		sSpec.Replace("X"," ");
		sscanf(sSpec,"%d%d",thick,width);
		sscanf(spec,"%c%d",sMat,thick);
	}
	if(matStr)
		strcpy(matStr,sMat);
}
//�������ݵ������������Ӧ�Ǹ�
CAngleProcessInfo* CBPSModel::FindAngleByPt(f3dPoint data_pos)
{
	CAngleProcessInfo* pJgInfo=NULL;
	for(pJgInfo=m_hashJgInfo.GetFirst();pJgInfo;pJgInfo=m_hashJgInfo.GetNext())
	{
		if(pJgInfo->PtInAngleRgn(data_pos))
			break;
	}
	return pJgInfo;
}
//���ݼ��Ų��Ҷ�Ӧ�ĽǸ�
CAngleProcessInfo* CBPSModel::FindAngleByPartNo(const char* sPartNo)
{
	CAngleProcessInfo* pJgInfo=NULL;
	for(pJgInfo=m_hashJgInfo.GetFirst();pJgInfo;pJgInfo=m_hashJgInfo.GetNext())
	{
		if(stricmp(pJgInfo->m_sPartNo,sPartNo)==0)
			break;
	}
	return pJgInfo;
}

static CXhChar16 RemoveMaterialBriefMark(CXhChar16 sPartNo,char cMaterial)
{
	if( cMaterial!=CSteelMatLibrary::Q235BriefMark()&&
		cMaterial!=CSteelMatLibrary::Q345BriefMark()&&
		cMaterial!=CSteelMatLibrary::Q390BriefMark()&&
		cMaterial!=CSteelMatLibrary::Q420BriefMark()&&
		cMaterial!=CSteelMatLibrary::Q460BriefMark())
		return sPartNo;
	CXhChar16 sNewPartNo(sPartNo);
	if(sPartNo.At(0)==cMaterial)
		sNewPartNo.Copy((char*)sPartNo+1);
	else if(sPartNo.At(sPartNo.GetLength()-1)==cMaterial)
		sNewPartNo.NCopy(sPartNo,sPartNo.GetLength()-1);
	sNewPartNo=sNewPartNo.Replace(' ',0);
	return sNewPartNo;
}

void CBPSModel::CorrectAngles()
{
	CAngleProcessInfo* pJgInfo=NULL;
	for(pJgInfo=m_hashJgInfo.GetFirst();pJgInfo;pJgInfo=m_hashJgInfo.GetNext())
	{
		if(pJgInfo->m_sPartNo.GetLength()>0)
			RemoveMaterialBriefMark(pJgInfo->m_sPartNo,pJgInfo->m_cMaterial);
	}
	m_hashJgInfo.Clean();
}

//��ȡ�Ǹֲ���
BOOL CBPSModel::RetrieveAngleInfo(CHashSet<AcDbObjectId>& selTextIdSet)
{
	//���ݽǸ�����λ�û�ȡ�Ǹ���Ϣ
	for(AcDbObjectId objId=selTextIdSet.GetFirst();objId;objId=selTextIdSet.GetNext())
	{
		AcDbEntity *pEnt=NULL;
		acdbOpenAcDbEntity(pEnt,objId,AcDb::kForRead);
		if(pEnt==NULL)
			continue;
		pEnt->close();
		CXhChar50 sValue;
		f3dPoint text_pos;
		if(pEnt->isKindOf(AcDbText::desc()))
		{
			AcDbText* pText=(AcDbText*)pEnt;
			text_pos.Set(pText->alignmentPoint().x,pText->alignmentPoint().y,pText->alignmentPoint().z);
			if(text_pos.IsZero())
				text_pos.Set(pText->position().x,pText->position().y,pText->position().z);
#ifdef _ARX_2007
			sValue.Copy(_bstr_t(pText->textString()));
#else
			sValue.Copy(pText->textString());
#endif
		}
		else
		{
			AcDbMText* pMText=(AcDbMText*)pEnt;
			text_pos.Set(pMText->location().x,pMText->location().y,pMText->location().z);//contents
#ifdef _ARX_2007
			sValue.Copy(_bstr_t(pMText->contents()));
#else
			sValue.Copy(pMText->contents());
#endif
		}
		if(strlen(sValue)<=0)	//���˿��ַ�
			continue;
		CAngleProcessInfo* pJgInfo=FindAngleByPt(text_pos);
		if(pJgInfo)
			pJgInfo->InitAngleInfo(text_pos,sValue);
	}
	//����ȡ�ĽǸ���Ϣ���к����Լ��
	CorrectAngles();
	if(m_hashJgInfo.GetNodeNum()<=0)
		return FALSE;
	return TRUE;
}
//��ȡ�Ǹֹ��տ�����
void CBPSModel::RetrieveJgCardRegion(CHashSet<AcDbObjectId>& selPolyLineIdHash)
{
	//��ʼ�����������Ĺ��տ�����
	ARRAY_LIST<f3dPoint> vertex_list;
	for(CAngleProcessInfo* pJgInfo=m_hashJgInfo.GetFirst();pJgInfo;pJgInfo=m_hashJgInfo.GetNext())
	{
		if(pJgInfo->m_bInBlockRef)
		{	//�˽Ǹֹ��տ�Ϊ��ģʽ
			vertex_list.Empty();
			vertex_list.append(CIdentifyManager::GetLeftBtmPt()+pJgInfo->orig_pt);
			vertex_list.append(CIdentifyManager::GetLeftTopPt()+pJgInfo->orig_pt);
			vertex_list.append(CIdentifyManager::GetRightTopPt()+pJgInfo->orig_pt);
			vertex_list.append(CIdentifyManager::GetRightBtmPt()+pJgInfo->orig_pt);
			pJgInfo->CreateRgn(vertex_list);
		}
		else
		{	//�˽Ǹֹ��տ�Ϊ����ģʽ
			double fMaxWidth=0;
			for(AcDbObjectId objId=selPolyLineIdHash.GetFirst();objId;objId=selPolyLineIdHash.GetNext())
			{
				AcDbEntity *pEnt=NULL;
				acdbOpenAcDbEntity(pEnt,objId,AcDb::kForRead);
				if(pEnt==NULL)
					continue;
				pEnt->close();
				if(!pEnt->isKindOf(AcDbPolyline::desc()))
					continue;
				vertex_list.Empty();
				SCOPE_STRU scope;
				AcDbPolyline* pPoly=(AcDbPolyline*)pEnt;
				int nNum=pPoly->numVerts();
				for(int iVertIndex = 0;iVertIndex<nNum;iVertIndex++)
				{
					AcGePoint3d location;
					pPoly->getPointAt(iVertIndex,location);
					vertex_list.append(f3dPoint(location.x,location.y,location.z));
					scope.VerifyVertex(f3dPoint(location.x,location.y,location.z));
				}
				POLYGON region;
				region.CreatePolygonRgn(vertex_list.m_pData,vertex_list.GetSize());
				if(region.PtInRgn(pJgInfo->sign_pt)==1&&scope.wide()>fMaxWidth)
				{
					fMaxWidth=scope.wide();
					pJgInfo->CreateRgn(vertex_list);
					break;
				}
			}
		}
	}
}
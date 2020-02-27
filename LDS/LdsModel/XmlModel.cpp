//<LOCALE_TRANSLATE BY hxr /> 2015-5-12
#include "stdafx.h"
#include "XmlModel.h"
#include "ParseAdaptNo.h"

//////////////////////////////////////////////////////////////////////////
// CXmlSolid
CXmlSolid::CXmlSolid()
{
	m_hHandle=0;
	volume=0;
	m_cTypeId=TYPE_SOLID;
}
//////////////////////////////////////////////////////////////////////////
// CXmlLines
CXmlLines::CXmlLines()
{
	m_hHandle=0;
	m_cTypeId=TYPE_LINES;
}
//////////////////////////////////////////////////////////////////////////
// CXmlBlockRecRef
CXmlBlockRecRef::CXmlBlockRecRef()
{
	m_hHandle=0;
	m_cTypeId=TYPE_BLOCKREC_REF;
	memset(m_arrMatrixd,0,16);
}
//////////////////////////////////////////////////////////////////////////
// CXmlBlockRecord
static CXmlEntity* CreateXmlEntity(int idClsType,DWORD key,void* pContext)	//必设回调函数
{
	CXmlEntity *pEnt=NULL;
	switch(idClsType)
	{
	case CXmlEntity::TYPE_SOLID:
		pEnt=new CXmlSolid();
		break;
	case CXmlEntity::TYPE_LINES:
		pEnt=new CXmlLines();
		break;
	case CXmlEntity::TYPE_BLOCKREC_REF:
		pEnt=new CXmlBlockRecRef();
		break;
	default:
		pEnt=new CXmlEntity();
		break;
	}
	pEnt->SetKey(key);
	return pEnt;
}
static BOOL DeleteXmlEntity(CXmlEntity *pEnt)				//必设回调函数
{
	if(pEnt->GetTypeID()==CXmlEntity::TYPE_SOLID)
		delete (CXmlSolid*)pEnt;
	else if(pEnt->GetTypeID()==CXmlEntity::TYPE_LINES)
		delete (CXmlLines*)pEnt;
	else if(pEnt->GetTypeID()==CXmlEntity::TYPE_BLOCKREC_REF)
		delete (CXmlEntity*)pEnt;
	else
		delete pEnt;
	return TRUE;
}
CXmlBlockRecord::CXmlBlockRecord()
{
	m_arEntity.CreateNewAtom=CreateXmlEntity;
	m_arEntity.DeleteAtom=DeleteXmlEntity;
}
//////////////////////////////////////////////////////////////////////////
// CXmlModel
bool CXmlModel::ImportDataFromXML(CMarkup xml,CString sElemName,CXmlObject *pObj/*=NULL*/)
{
	long handle=0;
	CString sValue=_T("");
	while(xml.FindChildElem(sElemName))   
	{ 
		xml.IntoElem();
		if(sElemName.CompareNoCase(PLINE_JJCH)==0)
		{
			sValue=xml.GetAttrib(YW_NAME);
			m_sName.Copy(sValue);
			//绝缘子、金具
			ImportDataFromXML(xml,PLINE_JYZ);
			ImportDataFromXML(xml,PLINE_JJ);
		}
		//绝缘子、金具
		else if(sElemName.CompareNoCase(PLINE_JJ)==0||
				sElemName.CompareNoCase(PLINE_JYZ)==0)
		{
			sValue=xml.GetAttrib(YW_HANDLE);
			if(sValue.GetLength()==0)
				ImportDataFromXML(xml,sElemName);
			else
			{
				sscanf(sValue,"%X",&handle);
				CXmlFittings *pFittings=XmlFittings.Add(handle);
				if(sElemName.CompareNoCase(PLINE_JYZ)==0)
					pFittings->m_cType=CXmlFittings::TYPE_JYZ;
				else 
					pFittings->m_cType=CXmlFittings::TYPE_JJ;
				pFittings->handle=handle;
				sValue=xml.GetAttrib(PLINE_MODEL);
				pFittings->m_sModel.Copy(sValue);
				ImportDataFromXML(xml,YW_NAME,pFittings);
				ImportDataFromXML(xml,PLINE_POSITION,pFittings);
			}
		}
		else if(sElemName.CompareNoCase(YW_NAME)==0)
		{
			sValue=xml.GetData();
			if(pObj&&pObj->IsFittings())
				((CXmlFittings*)pObj)->m_sName.Copy(sValue);
		}
		else if(sElemName.CompareNoCase(PLINE_POSITION)==0)
		{
			ImportDataFromXML(xml,"X",pObj);
			ImportDataFromXML(xml,"Y",pObj);
			ImportDataFromXML(xml,"Z",pObj);
		}
		else if(sElemName.CompareNoCase("X")==0)
		{
			sValue=xml.GetData();
			if(pObj&&pObj->IsFittings())
				((CXmlFittings*)pObj)->pos.x=atof(sValue);
		}
		else if(sElemName.CompareNoCase("Y")==0)
		{
			sValue=xml.GetData();
			if(pObj&&pObj->IsFittings())
				((CXmlFittings*)pObj)->pos.y=atof(sValue);
		}
		else if(sElemName.CompareNoCase("Z")==0)
		{
			sValue=xml.GetData();
			if(pObj&&pObj->IsFittings())
				((CXmlFittings*)pObj)->pos.z=atof(sValue);
		}
		//属性
		else if(sElemName.CompareNoCase(PLINE_PROIN)==0)
			ImportDataFromXML(xml,PLINE_PROPERTS);
		else if(sElemName.CompareNoCase(PLINE_PROPERTS)==0)
		{
			sValue=xml.GetAttrib(PLINE_MODEL);
			if(sValue.GetLength()>0)
			{
				CXmlObjPropertys *pObjProps=PropertyeTable.Add(sValue);
				pObjProps->m_sModel.Copy(sValue);
				ImportDataFromXML(xml,"property",pObjProps);
			}
		}
		else if(sElemName.CompareNoCase("property")==0)
		{
			if(pObj&&pObj->IsPropObj())
			{
				sValue=xml.GetAttrib(YW_NAME);
				if(sValue.GetLength()>0)
				{
					XML_PROPERTY *pProp=((CXmlObjPropertys*)pObj)->propTable.Add(sValue);
					pProp->sName.Copy(sValue);
					pProp->sType.Copy(xml.GetAttrib(YW_TYPE));
					pProp->sUnit.Copy(xml.GetAttrib("unit"));
					pProp->length=atoi(xml.GetAttrib(YW_LENGTH));
					pProp->sValue.Copy(xml.GetAttrib(YW_VALUE));
				}
			}
		}
		//块表
		else if(sElemName.CompareNoCase(BLOCKTABLE_NODE)==0)
			ImportDataFromXML(xml,BLOCKRECORD_NODE);
		else if(sElemName.CompareNoCase(BLOCKRECORD_NODE)==0)
		{
			sValue=xml.GetAttrib(HANDLE_ATTRIBUTE);
			CXmlBlockRecord *pBolckRec=NULL;
			if(sValue.CompareNoCase(MODEL_SPACE)==0)
			{
				pBolckRec=XmlBlockTable.Add(-1);
				pBolckRec->SetModelSpace(true);
			}
			else if(sValue.GetLength()>0)
			{
				sscanf(sValue,"%X",&handle);
				pBolckRec=XmlBlockTable.Add(handle);
			}
			ImportDataFromXML(xml,ENTITY_NODE,pBolckRec);
		}
		else if(sElemName.CompareNoCase(ENTITY_NODE)==0)
		{
			sValue=xml.GetAttrib(HANDLE_ATTRIBUTE);
			sscanf(sValue,"%X",&handle);
			sValue=xml.GetAttrib(ENTTYPE_ATTRIBUTE);
			if(pObj&&pObj->IsBlockRec()&&
				sValue.CompareNoCase(REFERENCE_TYPE)==0||
				sValue.CompareNoCase(SOLID_TYPE)==0||
				sValue.CompareNoCase(LINES_TYPE)==0)
			{
				CXmlBlockRecord *pRec=(CXmlBlockRecord*)pObj;
				if(sValue.CompareNoCase(REFERENCE_TYPE)==0)
				{
					CXmlBlockRecRef *pBlockRecRef=(CXmlBlockRecRef*)pRec->m_arEntity.Add(handle,CXmlEntity::TYPE_BLOCKREC_REF);
					sValue=xml.GetAttrib(COLOR_ATTRIBUTE);
					pBlockRecRef->m_nRGBColor=atoi(sValue);
					sValue=xml.GetAttrib(BLKHANDLE_ATTRIBUTE);
					sscanf(sValue,"%X",&(pBlockRecRef->m_hBlockRec));
					sValue=xml.GetAttrib(MATRIX_ATTRIBUTE);
					int i=0;
					CXhChar100 sText("%s",sValue);
					for(char* key=strtok(sText,",");key;key=strtok(NULL,","),i++)
					{
						if(i<16)
							pBlockRecRef->m_arrMatrixd[i/4][i%4]=atof(key);
					}
				}
				else if(sValue.CompareNoCase(SOLID_TYPE)==0)
				{
					CXmlSolid *pSolid=(CXmlSolid*)pRec->m_arEntity.Add(handle,CXmlEntity::TYPE_SOLID);
					sValue=xml.GetAttrib(COLOR_ATTRIBUTE);
					pSolid->m_nRGBColor=atoi(sValue);
					sValue=xml.GetAttrib("v");
					pSolid->volume=atof(sValue);
					ImportDataFromXML(xml,POINTLIST_NODE,pSolid);
					ImportDataFromXML(xml,FACELIST_NODE,pSolid);
					ImportDataFromXML(xml,NORMLIST_NODE,pSolid);
				}
				else //if(sValue.CompareNoCase(LINES_TYPE)==0)
				{
					CXmlLines *pLines=(CXmlLines*)pRec->m_arEntity.Add(handle,CXmlEntity::TYPE_LINES);
					sValue=xml.GetAttrib(COLOR_ATTRIBUTE);
					pLines->m_nRGBColor=atoi(sValue);
					ImportDataFromXML(xml,POINTLIST_NODE,pLines);
				}
			}
		}
		else if(sElemName.CompareNoCase(POINTLIST_NODE)==0)
			ImportDataFromXML(xml,POINT_NODE,pObj);
		else if(sElemName.CompareNoCase(FACELIST_NODE)==0)
			ImportDataFromXML(xml,FACE_NODE,pObj);
		else if(sElemName.CompareNoCase(NORMLIST_NODE)==0)
			ImportDataFromXML(xml,NORM_NODE,pObj);
		else if(sElemName.CompareNoCase(POINT_NODE)==0||
				sElemName.CompareNoCase(NORM_NODE)==0)
		{
			f3dPoint *pPt=NULL;
			if(sElemName.CompareNoCase(POINT_NODE)==0)
			{
				if(pObj&&pObj->IsEntity()&&((CXmlEntity*)pObj)->GetTypeID()==CXmlEntity::TYPE_SOLID)
					pPt=((CXmlSolid*)pObj)->m_arPts.append();
				else if(pObj&&pObj->IsEntity()&&((CXmlEntity*)pObj)->GetTypeID()==CXmlEntity::TYPE_LINES)
					pPt=((CXmlLines*)pObj)->m_arPts.append();
			}
			else //if(sElemName.CompareNoCase(NORM_NODE)==0)
			{
				if(pObj&&pObj->IsEntity()&&((CXmlEntity*)pObj)->GetTypeID()==CXmlEntity::TYPE_SOLID)
					pPt=((CXmlSolid*)pObj)->m_arPtNormal.append();
			}
			if(pPt)
			{
				sValue=xml.GetAttrib("x");
				pPt->x=atof(sValue);
				sValue=xml.GetAttrib("y");
				pPt->y=atof(sValue);
				sValue=xml.GetAttrib("z");
				pPt->z=atof(sValue);
			}
		}
		else if(sElemName.CompareNoCase(FACE_NODE)==0)
		{
			if(pObj&&pObj->IsEntity()&&((CXmlEntity*)pObj)->GetTypeID()==CXmlEntity::TYPE_SOLID)
			{
				sValue=xml.GetAttrib("fl");
				ATOM_LIST<int> *pFaceIndexList=((CXmlSolid*)pObj)->m_arIndexs.append();
				for(long i=FindAdaptNo(sValue);!IsAdaptNoEnd();i=FindAdaptNo(NULL))
					pFaceIndexList->append(i);
			}
		}	
		xml.OutOfElem();
	}
	return true;
}
bool CXmlModel::LoadFromXmlFile(const char* sFilePath)
{
	CMarkup xml;
	if(!xml.Load(sFilePath))
	{	//加载XML文件
#ifdef	AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Failed to load file!");
#else
		AfxMessageBox("读取文件失败!");
#endif
		return false;
	}
	xml.ResetMainPos();
	//读取金具串属性
	ImportDataFromXML(xml,PLINE_JJCH);
	//读取属性信息
	ImportDataFromXML(xml,PLINE_PROIN);
	//读取块表
	ImportDataFromXML(xml,BLOCKTABLE_NODE);
	return true;
}
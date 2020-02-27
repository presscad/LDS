// tma_part.cpp : Defines the entry point for the DLL application.
//<LOCALE_TRANSLATE BY wbt />

#include "stdafx.h"
#include "stdio.h"
#include "Tower.h"
#include "PartLib.h"
#include "CreateFace.h"
#include "SolidBodyBuffer.h"
#include "btc.h"
#include "LogFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

PROPLIST_ITEM* _UpdatePropItem(CHashStrList<PROPLIST_ITEM>* propHashtable,char* propKey,char* propName/*=NULL*/)
{
	if(propHashtable==NULL)
		return NULL;
	PROPLIST_ITEM *pItem=propHashtable->GetValue(propKey);
	if(pItem!=NULL&&propName!=NULL)
		pItem->name.Copy(propName);
	return pItem;
}
//////////////////////////////////////////////////////////////////////////
long HANDLE_ADDR::get_HandleId() const {
	if (pHandlVarAddr == NULL)
		return 0;
	if (ciAddrType == 0)
		return *pHandlVarAddr;
	else if (ciAddrType == 2)
	{
		HANDLEPOINT hp = *pHandlVarAddr;
		return hp.handle;
	}
	else
		return 0;
}
long HANDLE_ADDR::set_HandleId(long hHandlValue) {
	if (ciAddrType == 0 && pHandlVarAddr != NULL)
		return *pHandlVarAddr = hHandlValue;
	else
		return 0;
}
CHandleReplacer::SECTION::SECTION()
{
	pSectRod=NULL;
}

CHandleReplacer::CHandleReplacer(long handleOld,long handleNew,f3dLine* line_filter,bool bCollectiveTransition/*=false*/,
									bool bReverseSAndE/*=false*/,bool bReverseXAndY/*=false*/)
{
	_hOld=handleOld;
	m_bVerifyGroupAngleReplaced=true;
	m_bExcludeObjPointer=false;
	memset(_hOldSonAngles,0,sizeof(long)*4);
	hNew=handleNew;
	pOldRod=pNewRod=NULL;
	pTag=NULL;
	m_bReplaceObjSelfHandle=false;
	m_bCollectiveTransition=bCollectiveTransition;
	m_bReverseStartAndEnd=bReverseSAndE;
	m_bReverseWingXAndWingY=bReverseXAndY;
	if(line_filter)
	{
		SECTION* pSect=sections.append();
		pSect->lineFilterStart=line_filter->startPt;
		pSect->lineFilterEnd=line_filter->endPt;
		pSect->lineStdVec=pSect->lineFilterEnd-pSect->lineFilterStart;
		pSect->sect_length=pSect->lineStdVec.mod();
		pSect->hSectRod=hNew;
		if(pSect->sect_length>EPS)
			pSect->lineStdVec/=pSect->sect_length;
	}
}
bool CHandleReplacer::VerifyAndReplace(HANDLEEX* hCurrent,bool bObjSelfHandle/*=false*/)
{
	if(hCurrent->bSimpleNodePoint)
	{
		long hNode=hCurrent->hNodePoint;
		bool replaced=VerifyAndReplace(&hCurrent->handle,bObjSelfHandle);
		if(replaced)
			hCurrent->hNodePoint=hNode;
		return replaced;
	}
	else
		return VerifyAndReplace(&hCurrent->handle,bObjSelfHandle);
}
bool CHandleReplacer::VerifyAndReplace(long* hCurrent,bool bObjSelfHandle/*=false*/)
{
	if(hCurrent==NULL)
		return false;
	if( m_bVerifyGroupAngleReplaced && !m_bCollectiveTransition &&
		hOld>0x20&&(pOldRod==NULL||pOldRod->handle!=hOld)&&CLDSPart::_console!=NULL)
	{
		pOldRod=CLDSPart::_console->FromRodHandle(hOld);
		if(pOldRod&&pOldRod->IsAngle())
			memcpy(_hOldSonAngles,((CLDSLineAngle*)pOldRod)->son_jg_h,sizeof(long)*4);
	}
	if(pOldRod==NULL||pOldRod->GetClassTypeId()!=CLS_GROUPLINEANGLE)
		m_bVerifyGroupAngleReplaced=false;	//ֻ����һ����ϽǸ��滻��֤�ж� wjh-2016.11.10
	if( m_bVerifyGroupAngleReplaced && !m_bCollectiveTransition &&
		hNew>0x20&&(pNewRod==NULL||pNewRod->handle!=hNew)&&CLDSPart::_console!=NULL)
		pNewRod=CLDSPart::_console->FromRodHandle(hNew);
	if(pNewRod==NULL||pNewRod->GetClassTypeId()!=CLS_GROUPLINEANGLE)
		m_bVerifyGroupAngleReplaced=false;	//ֻ����һ����ϽǸ��滻��֤�ж� wjh-2016.11.10
	if( pOldRod&&pNewRod&& (*hCurrent)>0 &&
		pOldRod->GetClassTypeId()==CLS_GROUPLINEANGLE&&pNewRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		for(int i=0;i<4;i++)
		{
			if(((CLDSGroupLineAngle*)pOldRod)->son_jg_h[i]==0)
				break;
			if(_hOldSonAngles[i]==*hCurrent&&(!bObjSelfHandle||IsReplaceObjSelfHandle()))
			{	//�˴�����ʹ��pOldRod->son_jg_h����ΪpOldRod->son_jg_h�п�����֮ǰ�ľ���滻�з����ı� wjh-2016.11.10
				*hCurrent=((CLDSGroupLineAngle*)pNewRod)->son_jg_h[i];
				return true;
			}
		}
	}
	if(*hCurrent==0)
		return false;
	else if(hOld==*hCurrent&&(!bObjSelfHandle||IsReplaceObjSelfHandle()))
		*hCurrent=hNew;
	else if(m_bCollectiveTransition&&hOld==0&&(*hCurrent)<hNew)
		*hCurrent+=hNew;
	else
		return false;
	return true;
}

bool CHandleReplacer::AppendSectRod(const double* lineStartCoord,const double* lineEndCoord,long hSectRod,long priorSerial/*=0*/)
{
	SECTION* pSect;
	for(pSect=sections.GetFirst();pSect;pSect=sections.GetNext())
	{
		if(pSect->hSectRod==hSectRod)
			return false;
	}
	pSect=sections.append();
	pSect->lineFilterStart=GEPOINT(lineStartCoord);
	pSect->lineFilterEnd=GEPOINT(lineEndCoord);
	pSect->lineStdVec=pSect->lineFilterEnd-pSect->lineFilterStart;
	pSect->sect_length=pSect->lineStdVec.mod();
	pSect->hSectRod=hSectRod;
	pSect->priorSerial=priorSerial;
	if(pSect->sect_length>EPS)
		pSect->lineStdVec/=pSect->sect_length;
	return true;
}
long CHandleReplacer::SelectPerfectPartHandle(const double* pos_coord)
{
	if(pos_coord==NULL||sections.GetNodeNum()<=1)
		return hNew;	//�������߶��������
	GEPOINT point(pos_coord);
	CMaxDouble justify;	//�����ص�����ߵĸ˼�
	SECTION* pPerfectSect=NULL;
	for(SECTION* pSect=sections.GetFirst();pSect;pSect=sections.GetNext())
	{
		GEPOINT extend_vec=point-pSect->lineFilterStart;
		double dist=extend_vec*pSect->lineStdVec;
		double oldScalar=justify.number;
		if(!justify.IsInited())
			pPerfectSect=pSect;
		double factor=dist;	//λ��������ε��ص����ж�����
		if(dist<0)
			factor=dist;
		else if(dist>pSect->sect_length)
			factor=pSect->sect_length-dist;
		else
			factor=min(dist,pSect->sect_length-dist);
		justify.Update(factor);
		if(fabs(oldScalar-justify.number)>EPS)	//�������仯(ָ�仯���������㾫�����EPSʱ)
			pPerfectSect=pSect;
		else if(fabs(oldScalar-factor)<EPS&&pPerfectSect->priorSerial<pSect->priorSerial)
			pPerfectSect=pSect;	//�������ֵ�뵱ǰ���������ֵ�������ʱ��ȡ���ȶȽϸߵ������
	}
	if(pPerfectSect)
	{
		hNew=pPerfectSect->hSectRod;
		if(hNew>0x20&&(pNewRod==NULL||pNewRod->handle!=hNew)&&CLDSPart::_console!=NULL)
		{
			if(pPerfectSect->pSectRod==NULL)
				pPerfectSect->pSectRod=CLDSPart::_console->FromRodHandle(hNew);
			pNewRod=pPerfectSect->pSectRod;
		}
		if(hOld>0x20&&(pOldRod==NULL||pOldRod->handle!=hOld)&&CLDSPart::_console!=NULL)
		{
			pOldRod=CLDSPart::_console->FromRodHandle(hOld);
			if(pOldRod&&pOldRod->IsAngle())
				memcpy(_hOldSonAngles,((CLDSLineAngle*)pOldRod)->son_jg_h,sizeof(long)*4);
		}
	}
	return hNew;
}
//////////////////////////////////////////////////////////////////////////
//0.�Զ��� 1.X+ 2.X- 3.Y+ 4.Y- 5.Z+ 6.Z- 
f3dPoint CStdVector::GetVector(int nVecType)	//���ݷ������͵õ���׼����
{
	f3dPoint vec;
	if(nVecType==1)			//X+
		vec.Set(1,0,0);
	else if(nVecType==2)	//X-
		vec.Set(-1,0,0);
	else if(nVecType==3)	//Y+
		vec.Set(0,1,0);
	else if(nVecType==4)	//Y-
		vec.Set(0,-1,0);
	else if(nVecType==5)	//Z+
		vec.Set(0,0,1);
	else if(nVecType==6)	//Z-
		vec.Set(0,0,-1);
	return vec;
}
int CStdVector::GetVectorType(const double* vec_coord)		//���ݷ���õ���׼��������
{
	GEPOINT vec(vec_coord);
	if(fabs(vec.y)<EPS&&fabs(vec.z)<EPS)
	{
		if(vec.x>0)
			return 1;	//X+����
		else if(vec.x<0)
			return 2;	//X-����
	}
	else if(fabs(vec.x)<EPS&&fabs(vec.z)<EPS)
	{
		if(vec.y>0)
			return 3;	//Y+����
		else if(vec.y<0)
			return 4;	//Y-����
	}
	else if(fabs(vec.x)<EPS&&fabs(vec.y)<EPS)
	{
		if(vec.z>0)
			return 5;	//Z+����
		else if(vec.z<0)
			return 6;	//Z-����
	}
	return 0;	//�Զ��巽��
}
CString CStdVector::MakeVectorSetStr(bool bHasEditPara/*=false*/)			//��׼�����ַ���
{
	CString sVec;
#ifdef AFX_TARG_ENU_ENGLISH
	if(bHasEditPara)
		sVec="0.User Specify|1.X+|2.X-|3.Y+|4.Y-|5.Z+|6.Z-|7.Modify Parameter...";
	else
		sVec="0.User Specify|1.X+|2.X-|3.Y+|4.Y-|5.Z+|6.Z-";
#else 
	if(bHasEditPara)
		sVec="0.�Զ���|1.X+|2.X-|3.Y+|4.Y-|5.Z+|6.Z-|7.�޸Ĳ���...";
	else
		sVec="0.�Զ���|1.X+|2.X-|3.Y+|4.Y-|5.Z+|6.Z-";
#endif
	return sVec;
}

CString CStdVector::GetVecTypeString(int vectorType)//�õ��������Ͷ�Ӧ���ַ���
{
	CString sText;
	if(vectorType==0)
#ifdef AFX_TARG_ENU_ENGLISH
		sText="0.User Specify";
#else 
		sText="0.�Զ���";
#endif
	else if(vectorType==1)
		sText="1.X+";
	else if(vectorType==2)
		sText="2.X-";
	else if(vectorType==3)
		sText="3.Y+";
	else if(vectorType==4)
		sText="4.Y-";
	else if(vectorType==5)
		sText="5.Z+";
	else if(vectorType==6)
		sText="6.Z-";
	else if(vectorType==7)
#ifdef AFX_TARG_ENU_ENGLISH
		sText="7.Modify Parameter...";
#else 
		sText="7.�޸Ĳ���...";
#endif
	return sText;
}

CString CStdVector::GetVecTypeString(const double* vec_coord)	//�õ����뷽���Ӧ���ַ���
{
	int nVecType=GetVectorType(vec_coord);
	return GetVecTypeString(nVecType);
}
///////////////////////
IConsole* CLDSObject::_console=NULL;//��������ƽ̨
ISysPara* CLDSObject::sysPara=NULL;	//�������ò���
ILog2File* CLDSObject::log2file=NULL;	//������־�ļ�
//////////////////////////////////////////////////////////////////////
// SLAVE_NODE Construction/Destruction
//////////////////////////////////////////////////////////////////////
SLAVE_NODE::SLAVE_NODE()
{
	memset(this,0,sizeof(SLAVE_NODE));
}
void SLAVE_NODE::SetControlState(char cCoordType/*='X'*/,bool controlled/*=true*/)
{
	switch(cCoordType)
	{
	case 'x':
	case 'X':
		if(controlled)
			cFlag|=0x01;
		else
			cFlag&=0xFE;
		return;
	case 'y':
	case 'Y':
		if(controlled)
			cFlag|=0x02;
		else
			cFlag&=0xFD;
		return;
	case 'z':
	case 'Z':
		if(controlled)
			cFlag|=0x04;
		else
			cFlag&=0xFB;
		return;
	default:
		return;
	}
}
bool SLAVE_NODE::IsControlCoordX()
{
	if(posx_expr[0]==0 || (cFlag&0x01)==0)
		return false;
	else
		return true;
}
bool SLAVE_NODE::IsControlCoordY()
{
	if(posy_expr[0]==0 || (cFlag&0x02)==0)
		return false;
	else
		return true;
}
bool SLAVE_NODE::IsControlCoordZ()
{
	if(posz_expr[0]==0 || (cFlag&0x04)==0)
		return false;
	else
		return true;
}
void SLAVE_NODE::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.ReadByte(&cFlag);
	if( (version==0&&doc_type!=3)||
		(doc_type==1&&version>=5020000)||	//TMA V5.2.0.0
		(doc_type==2&&version>=2010000)||	//LMA V2.1.0.0
		(doc_type==4&&version>=1030804)||	//LDS V1.3.8.4
		(doc_type==5&&version>=1030804))	//TDA V1.3.8.4
		buffer.ReadByte(&cbMirFlag);
	if( version==0||
		(doc_type==1&&version>=5010901)||	//TMA(V5.1.9.1)
		(doc_type==2&&version>=2000901)||	//LMA(V2.0.9.1)
		(doc_type==3&&version>=2010303)||	//TSA(V2.1.3.3)
		(doc_type==4&&version>=1030804)||	//LDS(V1.3.8.4)
		(doc_type==5&&version>=1030804))	//TDA(V1.3.8.4)
	{
		buffer.ReadByte(&ciCtrlType);
		buffer.ReadInteger(&hAcs);
	}
	buffer.ReadString(posx_expr,20);
	buffer.ReadString(posy_expr,20);
	buffer.ReadString(posz_expr,20);
}
void SLAVE_NODE::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteByte(cFlag);
	if( (version==0&&doc_type!=3)||
		(doc_type==1&&version>=5020000)||	//TMA V5.2.0.0
		(doc_type==2&&version>=2010000)||	//LMA V2.1.0.0
		(doc_type==4&&version>=1030804)||	//LDS V1.3.8.4
		(doc_type==5&&version>=1030804))	//TDA V1.3.8.4
		buffer.WriteByte(cbMirFlag);
	if( version==0||
		(doc_type==1&&version>=5010901)||	//TMA(V5.1.9.1)
		(doc_type==2&&version>=2000901)||	//LMA(V2.0.9.1)
		(doc_type==3&&version>=2010303)||	//TSA(V2.1.3.3)
		(doc_type==4&&version>=1030804)||	//LDS(V1.3.8.4)
		(doc_type==5&&version>=1030804))	//TDA(V1.3.8.4)
	{
		buffer.WriteByte(ciCtrlType);
		buffer.WriteInteger(hAcs);
	}
	buffer.WriteString(posx_expr,20);
	buffer.WriteString(posy_expr,20);
	buffer.WriteString(posz_expr,20);
}

///////////////////////
const DWORD HASHTABLESIZE = 500;
const DWORD STATUSHASHTABLESIZE = 50;
CHashStrList<PROPLIST_ITEM> CLDSDbObject::propHashtable;
CXhChar100 CLDSDbObject::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*>CLDSDbObject::hashPropItemById;
CHashTable<DWORD> CLDSDbObject::propStatusHashtable;
int CLDSDbObject::m_iCurDisplayPropGroup;
void CLDSDbObject::InitDbObjectPropHashtable()
{
	int id = 1;
	CLDSDbObject::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSDbObject::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSDbObject::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Data Object Basic Information"));
	CLDSDbObject::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSDbObject::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSDbObject::AddPropItem("layer1",PROPLIST_ITEM(id++,"First Char"));
	CLDSDbObject::AddPropItem("layer2",PROPLIST_ITEM(id++,"Second Char"));
	CLDSDbObject::AddPropItem("layer3",PROPLIST_ITEM(id++,"Third Char"));
	CLDSDbObject::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSDbObject::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSDbObject::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
	//
	CLDSDbObject::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"Lift Arc"));		//Ԥ����
#else 
	CLDSDbObject::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"���ݶ��������Ϣ"));
	CLDSDbObject::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSDbObject::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSDbObject::AddPropItem("layer1",PROPLIST_ITEM(id++,"ͼ���һ��ĸ"));
	CLDSDbObject::AddPropItem("layer2",PROPLIST_ITEM(id++,"ͼ��ڶ���ĸ"));
	CLDSDbObject::AddPropItem("layer3",PROPLIST_ITEM(id++,"ͼ�������ĸ"));
	CLDSDbObject::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSDbObject::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSDbObject::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
	//
	CLDSDbObject::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"Ԥ����"));		//Ԥ����
#endif
	CLDSDbObject::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,""));
	CLDSDbObject::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,""));
	CLDSDbObject::AddPropItem("m_hMotherObject",PROPLIST_ITEM(id++,""));
}
void CLDSDbObject::UpdateRelaObjArcLiftProp()
{
	for(RELATIVE_OBJECT* pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		if(pRelaObj->mirInfo.axis_flag!=0x01&&pRelaObj->mirInfo.axis_flag!=0x02&&pRelaObj->mirInfo.axis_flag!=0x04)
			continue;
		CLDSDbObject* pRelaDataObj=(CLDSDbObject*)BelongModel()->FromHandle(pRelaObj->hObj,TRUE);
		if(pRelaDataObj && pRelaDataObj->GetClassTypeId()==m_nClassTypeId)
		{
			if(m_nClassTypeId==CLS_NODE)
			{
				f3dPoint src_pos=((CLDSNode*)this)->Position(false);
				f3dPoint mirpos=GetMirPos(src_pos,pRelaObj->mirInfo);
				if(!mirpos.IsEqual(((CLDSNode*)pRelaDataObj)->Position(false)))
					continue;
			}
			pRelaDataObj->SetArcLift(ArcLiftHandle());
			pRelaDataObj->CalPosition(true);
		}
	}
}
bool CLDSDbObject::IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr)
{
	PROPLIST_TYPE* pMirSyncTable = pObj->GetSyncPropList();
	if(pMirSyncTable==NULL)
		return false;
	if(pObj==NULL||propKeyStr==NULL||pObj->GetClassTypeId()!=m_nClassTypeId)
		return false;
	MEMORY_BLOCK MPCL = GetDefaultMPCL();
	MEMORY_BLOCK cl=mpcl;
	if(cl.IsZero())
		cl=MPCL;
	MEMORY_BLOCK mir_cl=pObj->mpcl;
	if(mir_cl.IsZero())
		mir_cl=MPCL;
	PROPLIST_ITEM* pItem=pMirSyncTable->GetPropItem(propKeyStr);
	if(pItem!=NULL)
		return cl.GetBitState((WORD)pItem->id-1)&&mir_cl.GetBitState((WORD)pItem->id-1);
	else
	{
		Log2File()->Log("%sͬ���Գ�������ȱʧ",propKeyStr);
		return false;
	}
}
bool CLDSDbObject::SyncMirProp(const char* propKeyStr/*=NULL*/,BYTE ciMirAll0X1Y2Z3/*=0*/,DWORD dwPropFilterFlag/*=0xffffffff*/){return false;}
bool CLDSDbObject::SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/){return false;}
bool CLDSDbObject::SyncMirPropTo(CLDSDbObject* pDestObj,long hMirAcs,short wRotAngle,const char* propKeyStr,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	MIRMSG mirmsg(1);
	if(hMirAcs<=3)
	{
		mirmsg.axis_flag<<=hMirAcs-1;
		if(wRotAngle!=0)
			mirmsg.axis_flag|=0x08;
		mirmsg.rotate_angle=wRotAngle;
		return SyncMirPropTo(pDestObj,mirmsg,propKeyStr,dwPropFilterFlag);
	}
	else
		return false;
}
RELATIVE_OBJECT *CLDSDbObject::AppendRelativeObj(RELATIVE_OBJECT &relaObj)
{
	RELATIVE_OBJECT *pMirObj=NULL;
	if(relaObj.hObj==handle)
		return NULL;	//��֧���ԶԳ�
	long hMirAcs=0;
	if(relaObj.mirInfo.axis_flag&0x01)
		hMirAcs=1;
	else if(relaObj.mirInfo.axis_flag&0x02)
		hMirAcs=2;
	else if(relaObj.mirInfo.axis_flag&0x04)
		hMirAcs=3;
	else if(relaObj.mirInfo.axis_flag==0x08)
		hMirAcs=0;
	for(pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		if(pMirObj->hObj==relaObj.hObj)
		{
			pMirObj->mirInfo=relaObj.mirInfo;
			return pMirObj;
		}
	}
	pMirObj=relativeObjs.SetValue(relaObj.hObj,relaObj);
	if(IsNode())
		((CLDSNode*)this)->SetModified();
	else if(IsPart())
		((CLDSPart*)this)->SetModified();
	return pMirObj;
}
BOOL CLDSDbObject::IsFatherObject()
{
	RELATIVE_OBJECT *pMirObj=NULL;
	for(pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		if(pMirObj->hObj<handle)
			return FALSE;	//�и�С����Ĺ�������
	}
	return TRUE;
}
RELATIVE_OBJECT* CLDSDbObject::GetRelativeObjInfo(long h,bool bIncRotateMir/*=true*/)
{
	RELATIVE_OBJECT *pMirObj=NULL;
	for(pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		if(!bIncRotateMir&&(pMirObj->mirInfo.axis_flag&0x08))
			continue;	//������Ϣ��������ת�Գ�
		if(pMirObj->hObj==h)
			return pMirObj;	//�ǹ�������
	}
	return NULL;
}
long CLDSDbObject::GetRelativeMirObj(MIRMSG msg,bool bIncSelf/*=TRUE*/)
{
	RELATIVE_OBJECT *pMirObj=NULL;
	for(pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		if(pMirObj->mirInfo.IsSameMirMsg(msg))
		{
			if(!bIncSelf&&pMirObj->hObj==handle)
				return NULL;
			else
				return pMirObj->hObj;
		}
	}
	return 0;
}
BOOL CLDSDbObject::GetFatherMirObj(RELATIVE_OBJECT *pRelaObj)
{
	RELATIVE_OBJECT *pMirObj=NULL;
	RELATIVE_OBJECT relaFatherObj;
	relaFatherObj.hObj=handle;

	for(pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		if(pMirObj->hObj<relaFatherObj.hObj)
			relaFatherObj = *pMirObj;	//�и�С����Ĺ�������
	}
	if(relaFatherObj.mirInfo.axis_flag==0)
		return FALSE;
	else if(pRelaObj)
		*pRelaObj=relaFatherObj;
	return TRUE;
}

CHashStrList<PROPLIST_ITEM> CLDSNode::propHashtable;
CXhChar100 CLDSNode::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*> CLDSNode::hashPropItemById;
CHashTable<DWORD> CLDSNode::propStatusHashtable;

BOOL CLDSNode::m_bDisplayBeforeArcLift=TRUE;//��ʾԤ��ǰλ��
int CLDSNode::m_iCurDisplayPropGroup;
long CLDSNode::m_idPropUdf=1;
long CLDSNode::m_idMinPropUdf=1;

void CLDSNode::InitPropMirBitsMap()
{
	int id=1;
	propMirSync.InitHashtableSize(32);
#ifdef AFX_TARG_ENU_ENGLISH
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cfgword",PROPLIST_ITEM(id++,"Model Flag"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("pos",PROPLIST_ITEM(id++,"Coordinate"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("hFatherPart",PROPLIST_ITEM(id++,"Parent Rod"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("xFatherAngleZhunJu",PROPLIST_ITEM(id++,"Father Angle Stadia Setting"));
#else 
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ����"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("pos",PROPLIST_ITEM(id++,"����"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("hFatherPart",PROPLIST_ITEM(id++,"���˼�"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("xFatherAngleZhunJu",PROPLIST_ITEM(id++,"׼���趨"));
#endif
#ifdef __PART_DESIGN_INC_
	#ifdef AFX_TARG_ENU_ENGLISH
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("point_i",PROPLIST_ITEM(id++,"Part Label"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("force_type",PROPLIST_ITEM(id++,"Node Force Judgement"));
	propMirSync.AddPropItem("hangingInfo",PROPLIST_ITEM(id++,"Wire Information"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("restrict",PROPLIST_ITEM(id++,"Restric"));
	#else
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("point_i",PROPLIST_ITEM(id++,"���"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("force_type",PROPLIST_ITEM(id++,"�ڵ㴫���ж�"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("hangingInfo",PROPLIST_ITEM(id++,"������Ϣ"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("restrict",PROPLIST_ITEM(id++,"Լ��"));
#endif
#endif
}
void CLDSNode::InitNodePropHashtable()
{
	int id=1;
	CLDSNode::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);//CreateHashTable(HASHTABLESIZE);
	CLDSNode::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSNode::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Node Basic Information"));
	CLDSNode::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSNode::AddPropItem("wireInfo",PROPLIST_ITEM(id++,"Wire"));
	CLDSNode::AddPropItem("hangingInfo",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Wire Information"));
	CLDSNode::AddPropItem("setupWireInfo",PROPLIST_ITEM(id++,""));
	CLDSNode::AddPropItem("hangingWireType",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Hang Wire Type"));
	CLDSNode::AddPropItem("hangingStyle",PROPLIST_ITEM(id++,"(Existing Explanatory Note)"));
	CLDSNode::AddPropItem("voltage",PROPLIST_ITEM(id++,"(Existing Explanatory Note)"));
	CLDSNode::AddPropItem("splitNumber",PROPLIST_ITEM(id++,"(Existing Explanatory Note)"));
	CLDSNode::AddPropItem("landHigh",PROPLIST_ITEM(id++,"(Existing Explanatory Note)"));
	CLDSNode::AddPropItem("attachmentSpec",PROPLIST_ITEM(id++,"(Existing Explanatory Note)"));
	CLDSNode::AddPropItem("attachmentWeight",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("attachmentWindShieldArea",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("anchorLineAngle",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("tractionLineAngle",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("wireType",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("wireSpec",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("wireDiameter",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("wireArea",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("unitWeight",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("forcePermit",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("saftyCoef",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("maxTension",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("elasticityCoef",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("expansionCoef",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("powerLineInfo",PROPLIST_ITEM(id++,"None"));
	CLDSNode::AddPropItem("horiSpace",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("vertSpace",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("representSpace",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Represent Space"));
	CLDSNode::AddPropItem("HtoL",PROPLIST_ITEM(id++,"(Existing Explanatory Note)High Different Coefficient"));
	CLDSNode::AddPropItem("allocHoriCoef",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Horizontal Span Distribution Coefficient"));
	CLDSNode::AddPropItem("allocVertCoef",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Vertical Span Distribution Coefficient"));
	CLDSNode::AddPropItem("xianluAngle",PROPLIST_ITEM(id++,"Existing Explanatory Note"));
	CLDSNode::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSNode::AddPropItem("layer1",PROPLIST_ITEM(id++,"First Char"));
	CLDSNode::AddPropItem("layer2",PROPLIST_ITEM(id++,"Second Char"));
	CLDSNode::AddPropItem("layer3",PROPLIST_ITEM(id++,"Third Char"));
	CLDSNode::AddPropItem("hFatherPart",PROPLIST_ITEM(id++,"Parent Rod"));
	CLDSNode::AddPropItem("m_bVirtualObj",PROPLIST_ITEM(id++,"Virtual Node"));
	CLDSNode::AddPropItem("pos",PROPLIST_ITEM(id++,"Coordinate"));
	CLDSNode::AddPropItem("pos.x",PROPLIST_ITEM(id++,"X"));
	CLDSNode::AddPropItem("pos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSNode::AddPropItem("pos.z",PROPLIST_ITEM(id++,"Z"));
	CLDSNode::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"Lift Arc Coordinate"));		//Ԥ����
	CLDSNode::AddPropItem("ArcLiftDirection",PROPLIST_ITEM(id++,"Lift Direction"));		//Ԥ����
	CLDSNode::AddPropItem("m_xArcLiftPos",PROPLIST_ITEM(id++,"Prelift Coordinate"));	//Ԥ����λ��
	CLDSNode::AddPropItem("m_xArcLiftPos.x",PROPLIST_ITEM(id++,"X"));
	CLDSNode::AddPropItem("m_xArcLiftPos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSNode::AddPropItem("m_xArcLiftPos.z",PROPLIST_ITEM(id++,"Z"));
	CLDSNode::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSNode::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSNode::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"Relative Node"));
	CLDSNode::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//Ӱ����Ϣ
	CLDSNode::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Project Mother Node"));//Ӱ��ĸ����
	CLDSNode::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//�����Ĳ�������
	CLDSNode::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//�����Ĳ���
	CLDSNode::AddPropItem("xFatherAngleZhunJu",PROPLIST_ITEM(id++,"Stadia Setting"));
	CLDSNode::AddPropItem("attachRelation",PROPLIST_ITEM(id++,"Attach Relation"));
	CLDSNode::AddPropItem("m_cPosCalType",PROPLIST_ITEM(id++,"Coordinate Dependent Type"));
	CLDSNode::AddPropItem("m_cPosCalViceType",PROPLIST_ITEM(id++,"Reference Point Symmetrical Type"));
	CLDSNode::AddPropItem("attachNode",PROPLIST_ITEM(id++,"Dependent Node Handle"));
	CLDSNode::AddPropItem("attachNode1",PROPLIST_ITEM(id++,"Dependent Node1"));
	CLDSNode::AddPropItem("attachNode2",PROPLIST_ITEM(id++,"Dependent Node2"));
	CLDSNode::AddPropItem("attachNode3",PROPLIST_ITEM(id++,"Dependent Node3"));
	CLDSNode::AddPropItem("attachNode4",PROPLIST_ITEM(id++,"Dependent Node4"));
	CLDSNode::AddPropItem("attachPole",PROPLIST_ITEM(id++,"Dependent Rod Handle"));
	CLDSNode::AddPropItem("attachPole1",PROPLIST_ITEM(id++,"Dependent Rod 1"));
	CLDSNode::AddPropItem("attachPole2",PROPLIST_ITEM(id++,"Dependent Rod 2"));
	CLDSNode::AddPropItem("attach_scale",PROPLIST_ITEM(id++,"Relative Whole Angle Scale"));
	CLDSNode::AddPropItem("attach_offset",PROPLIST_ITEM(id++,"Offset Point"));
	CLDSNode::AddPropItem("hRelationPlane",PROPLIST_ITEM(id++,"Datum Plane"));
	CLDSNode::AddPropItem("hDatumPoint",PROPLIST_ITEM(id++,"Datum Point"));
	//����������Ҫ�Ľڵ��źʹ�������
	CLDSNode::AddPropItem("point_i",PROPLIST_ITEM(id++,"Serial Number"));
	CLDSNode::AddPropItem("force_type",PROPLIST_ITEM(id++,"Node force Judgment"));
	//��λ��Ϣ
	CLDSNode::AddPropItem("m_idNodePlace",PROPLIST_ITEM(id++,"Hang Wire Serial Number"));
	//���Ӻ���
	CLDSNode::AddPropItem("attachLoad",PROPLIST_ITEM(id++,"Attached Load"));
	CLDSNode::AddPropItem("attachVolLoad",PROPLIST_ITEM(id++,"Variable Attached Load(kN)"));//���ӿɱ������
	CLDSNode::AddPropItem("attachVolLoad_X",PROPLIST_ITEM(id++,"X Component"));
	CLDSNode::AddPropItem("attachVolLoad_Y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSNode::AddPropItem("attachVolLoad_Z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSNode::AddPropItem("coefCqi",PROPLIST_ITEM(id++,""));//ЧӦϵ��
	CLDSNode::AddPropItem("attachForeverLoad",PROPLIST_ITEM(id++,"Forever Attached Load(kN)"));//�������������
	CLDSNode::AddPropItem("attachForeverLoad_X",PROPLIST_ITEM(id++,"X Component"));
	CLDSNode::AddPropItem("attachForeverLoad_Y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSNode::AddPropItem("attachForeverLoad_Z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSNode::AddPropItem("permanentFz",PROPLIST_ITEM(id++,"Z_2 Component"));
	
	//λ��Լ��
	CLDSNode::AddPropItem("restrict",PROPLIST_ITEM(id++,"Restrict"));
	CLDSNode::AddPropItem("restrict_x",PROPLIST_ITEM(id++,"X Displacement Restrict"));
	CLDSNode::AddPropItem("restrict_y",PROPLIST_ITEM(id++,"Y Displacement Restrict"));
	CLDSNode::AddPropItem("restrict_z",PROPLIST_ITEM(id++,"Z Displacement Restrict"));
	CLDSNode::AddPropItem("restrict_rot_x",PROPLIST_ITEM(id++,"X Rotation Restrict"));
	CLDSNode::AddPropItem("restrict_rot_y",PROPLIST_ITEM(id++,"Y Rotation Restrict"));
	CLDSNode::AddPropItem("restrict_rot_z",PROPLIST_ITEM(id++,"Z Rotation Restrict"));
	CLDSNode::AddPropItem("restrict_all",PROPLIST_ITEM(id++,"All"));
#else 
	CLDSNode::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"������Ϣ"));
	CLDSNode::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSNode::AddPropItem("wireInfo",PROPLIST_ITEM(id++,"����"));
	CLDSNode::AddPropItem("hangingInfo",PROPLIST_ITEM(id++,"���߽ڵ�",NULL,"��|��"));
	CLDSNode::AddPropItem("setupWireInfo",PROPLIST_ITEM(id++,""));
	CLDSNode::AddPropItem("hangingWireType",PROPLIST_ITEM(id++,"(��ע��)�ҽ�����"));
	CLDSNode::AddPropItem("hangingStyle",PROPLIST_ITEM(id++,"(��ע��)"));
	CLDSNode::AddPropItem("voltage",PROPLIST_ITEM(id++,"(��ע��)"));
	CLDSNode::AddPropItem("splitNumber",PROPLIST_ITEM(id++,"(��ע��)"));
	CLDSNode::AddPropItem("landHigh",PROPLIST_ITEM(id++,"(��ע��)"));
	CLDSNode::AddPropItem("attachmentSpec",PROPLIST_ITEM(id++,"(��ע��)"));
	CLDSNode::AddPropItem("attachmentWeight",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("attachmentWindShieldArea",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("anchorLineAngle",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("tractionLineAngle",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("wireType",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("wireSpec",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("wireDiameter",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("wireArea",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("unitWeight",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("forcePermit",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("saftyCoef",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("maxTension",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("elasticityCoef",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("expansionCoef",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("powerLineInfo",PROPLIST_ITEM(id++,"��"));
	CLDSNode::AddPropItem("horiSpace",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("vertSpace",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("representSpace",PROPLIST_ITEM(id++,"(��ע��)������"));
	CLDSNode::AddPropItem("HtoL",PROPLIST_ITEM(id++,"(��ע��)�߲�ϵ��"));
	CLDSNode::AddPropItem("allocHoriCoef",PROPLIST_ITEM(id++,"(��ע��)ˮƽ�������ϵ��"));
	CLDSNode::AddPropItem("allocVertCoef",PROPLIST_ITEM(id++,"(��ע��)��ֱ�������ϵ��"));
	CLDSNode::AddPropItem("xianluAngle",PROPLIST_ITEM(id++,"��ע��"));
	CLDSNode::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ����"));
	CLDSNode::AddPropItem("hFatherPart",PROPLIST_ITEM(id++,"���˼�"));
	CLDSNode::AddPropItem("m_bVirtualObj",PROPLIST_ITEM(id++,"����ڵ�"));
	CLDSNode::AddPropItem("pos",PROPLIST_ITEM(id++,"����"));
	CLDSNode::AddPropItem("pos.x",PROPLIST_ITEM(id++,"X"));
	CLDSNode::AddPropItem("pos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSNode::AddPropItem("pos.z",PROPLIST_ITEM(id++,"Z"));
	CLDSNode::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"Ԥ������"));		//Ԥ����
	CLDSNode::AddPropItem("ArcLiftDirection",PROPLIST_ITEM(id++,"Ԥ������"));		//Ԥ����
	CLDSNode::AddPropItem("m_xArcLiftPos",PROPLIST_ITEM(id++,"Ԥ����ڵ�����"));	//Ԥ����λ��
	CLDSNode::AddPropItem("m_xArcLiftPos.x",PROPLIST_ITEM(id++,"X"));
	CLDSNode::AddPropItem("m_xArcLiftPos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSNode::AddPropItem("m_xArcLiftPos.z",PROPLIST_ITEM(id++,"Z"));
	CLDSNode::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSNode::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSNode::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"�����ڵ�"));
	CLDSNode::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Ӱ����Ϣ"));//Ӱ����Ϣ
	CLDSNode::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Ӱ��ĸ�ڵ�"));//Ӱ��ĸ����
	CLDSNode::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"������������"));//�����Ĳ�������
	CLDSNode::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"��������"));//�����Ĳ���
	CLDSNode::AddPropItem("xFatherAngleZhunJu",PROPLIST_ITEM(id++,"׼���趨"));
	CLDSNode::AddPropItem("attachRelation",PROPLIST_ITEM(id++,"������ϵ"));
	CLDSNode::AddPropItem("m_cPosCalType",PROPLIST_ITEM(id++,"������������"));
	CLDSNode::AddPropItem("m_cPosCalViceType",PROPLIST_ITEM(id++,"���յ�ԳƷ�ʽ"));
	CLDSNode::AddPropItem("attachNode",PROPLIST_ITEM(id++,"�����ڵ���"));
	CLDSNode::AddPropItem("attachNode1",PROPLIST_ITEM(id++,"�����ڵ�1"));
	CLDSNode::AddPropItem("attachNode2",PROPLIST_ITEM(id++,"�����ڵ�2"));
	CLDSNode::AddPropItem("attachNode3",PROPLIST_ITEM(id++,"�����ڵ�3"));
	CLDSNode::AddPropItem("attachNode4",PROPLIST_ITEM(id++,"�����ڵ�4"));
	CLDSNode::AddPropItem("attachPole",PROPLIST_ITEM(id++,"�����˼����"));
	CLDSNode::AddPropItem("attachPole1",PROPLIST_ITEM(id++,"�����˼�1"));
	CLDSNode::AddPropItem("attachPole2",PROPLIST_ITEM(id++,"�����˼�2"));
	CLDSNode::AddPropItem("attach_scale",PROPLIST_ITEM(id++,"��������ռ����"));
	CLDSNode::AddPropItem("attach_offset",PROPLIST_ITEM(id++,"ƫ�Ƶ�ƫ�ƾ���"));
	CLDSNode::AddPropItem("hRelationPlane",PROPLIST_ITEM(id++,"��׼��"));
	CLDSNode::AddPropItem("hPlaneRefNode#0",PROPLIST_ITEM(id++,"���սڵ�1"));
	CLDSNode::AddPropItem("hPlaneRefNode#1",PROPLIST_ITEM(id++,"���սڵ�2"));
	CLDSNode::AddPropItem("hPlaneRefNode#2",PROPLIST_ITEM(id++,"���սڵ�3"));
	CLDSNode::AddPropItem("hDatumPoint",PROPLIST_ITEM(id++,"��׼��"));
	//����������Ҫ�Ľڵ��źʹ�������
	CLDSNode::AddPropItem("point_i",PROPLIST_ITEM(id++,"���"));
	CLDSNode::AddPropItem("force_type",PROPLIST_ITEM(id++,"�ڵ㴫���ж�"));
	//��λ��Ϣ
	CLDSNode::AddPropItem("m_idNodePlace",PROPLIST_ITEM(id++,"��λ���"));
	//���Ӻ���
	CLDSNode::AddPropItem("attachLoad",PROPLIST_ITEM(id++,"���Ӻ���"));
	CLDSNode::AddPropItem("attachVolLoad",PROPLIST_ITEM(id++,"�ɱ������(kN)"));//���ӿɱ������
	CLDSNode::AddPropItem("attachVolLoad_X",PROPLIST_ITEM(id++,"X����"));
	CLDSNode::AddPropItem("attachVolLoad_Y",PROPLIST_ITEM(id++,"Y����"));
	CLDSNode::AddPropItem("attachVolLoad_Z",PROPLIST_ITEM(id++,"Z����"));
	CLDSNode::AddPropItem("coefCqi",PROPLIST_ITEM(id++,""));//ЧӦϵ��
	CLDSNode::AddPropItem("attachForeverLoad",PROPLIST_ITEM(id++,"���������(kN)"));//�������������
	CLDSNode::AddPropItem("attachForeverLoad_X",PROPLIST_ITEM(id++,"X����"));
	CLDSNode::AddPropItem("attachForeverLoad_Y",PROPLIST_ITEM(id++,"Y����"));
	CLDSNode::AddPropItem("attachForeverLoad_Z",PROPLIST_ITEM(id++,"Z����"));
	CLDSNode::AddPropItem("permanentFz",PROPLIST_ITEM(id++,"Z_2����"));
	
	//λ��Լ��
	CLDSNode::AddPropItem("restrict",PROPLIST_ITEM(id++,"Լ��"));
	CLDSNode::AddPropItem("restrict_x",PROPLIST_ITEM(id++,"X����λ��Լ��"));
	CLDSNode::AddPropItem("restrict_y",PROPLIST_ITEM(id++,"Y����λ��Լ��"));
	CLDSNode::AddPropItem("restrict_z",PROPLIST_ITEM(id++,"Z����λ��Լ��"));
	CLDSNode::AddPropItem("restrict_rot_x",PROPLIST_ITEM(id++,"X������תԼ��"));
	CLDSNode::AddPropItem("restrict_rot_y",PROPLIST_ITEM(id++,"Y������תԼ��"));
	CLDSNode::AddPropItem("restrict_rot_z",PROPLIST_ITEM(id++,"Z������תԼ��"));
	CLDSNode::AddPropItem("restrict_all",PROPLIST_ITEM(id++,"ȫ��"));
#endif
	m_idMinPropUdf=id;
	InitPropMirBitsMap();
}

CHashStrList<PROPLIST_ITEM> CLDSPart::propHashtable;
CXhChar100 CLDSPart::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*>CLDSPart::hashPropItemById;
CHashTable<DWORD> CLDSPart::propStatusHashtable;
int CLDSPart::m_iCurDisplayPropGroup;
void CLDSPart::InitPartPropHashtable()
{
	int id = 1;
	CLDSPart::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSPart::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSPart::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Part Basic Information"));
	CLDSPart::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSPart::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSPart::AddPropItem("layer1",PROPLIST_ITEM(id++,"First Char"));
	CLDSPart::AddPropItem("layer2",PROPLIST_ITEM(id++,"Second Char"));
	CLDSPart::AddPropItem("layer3",PROPLIST_ITEM(id++,"Third Char"));
	CLDSPart::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material",NULL));
	CLDSPart::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade","Default|A|B|C|D|"));
	CLDSPart::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSPart::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Serial Number"));
	CLDSPart::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSPart::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//�����Ĳ�������
	CLDSPart::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//�����Ĳ���
	CLDSPart::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Shadow Info"));		//Ӱ����Ϣ
	CLDSPart::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Mother Object"));	//ĸ����
	CLDSPart::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSPart::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"));
	CLDSPart::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	CLDSPart::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Ancillary Part"));
	CLDSPart::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color"));
	CLDSPart::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	CLDSPart::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
#else 
	CLDSPart::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"������Ϣ"));
	CLDSPart::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSPart::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSPart::AddPropItem("layer1",PROPLIST_ITEM(id++,"��һ�ַ�"));
	CLDSPart::AddPropItem("layer2",PROPLIST_ITEM(id++,"�ڶ��ַ�"));
	CLDSPart::AddPropItem("layer3",PROPLIST_ITEM(id++,"�����ַ�"));
	CLDSPart::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	CLDSPart::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�",NULL,"Ĭ��|A|B|C|D|"));
	CLDSPart::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSPart::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	CLDSPart::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSPart::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//�����Ĳ�������
	CLDSPart::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//�����Ĳ���
	CLDSPart::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Shadow Info"));		//Ӱ����Ϣ
	CLDSPart::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Mother Object"));	//ĸ����
	CLDSPart::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	CLDSPart::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����"));
	CLDSPart::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	CLDSPart::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"���⸨������"));
	CLDSPart::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ"));
	CLDSPart::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	CLDSPart::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
#endif
}

CHashStrList<PROPLIST_ITEM> CLDSLinePart::propHashtable;
CXhChar100 CLDSLinePart::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*>CLDSLinePart::hashPropItemById;
CHashTable<DWORD> CLDSLinePart::propStatusHashtable;
int CLDSLinePart::m_iCurDisplayPropGroup;
void CLDSLinePart::InitLinePartPropHashtable()
{
	int id = 1;
	CLDSLinePart::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSLinePart::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSLinePart::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Unit Basic Information"));
	CLDSLinePart::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSLinePart::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSLinePart::AddPropItem("layer1",PROPLIST_ITEM(id++,"First Char"));
	CLDSLinePart::AddPropItem("layer2",PROPLIST_ITEM(id++,"Second Char"));
	CLDSLinePart::AddPropItem("layer3",PROPLIST_ITEM(id++,"Third Char"));
	CLDSLinePart::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	CLDSLinePart::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade",NULL,"Default|A|B|C|D|"));
	CLDSLinePart::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSLinePart::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Serial Number"));
	CLDSLinePart::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSLinePart::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//�����Ĳ���
	CLDSLinePart::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Shadow Info"));		//Ӱ����Ϣ
	CLDSLinePart::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Mother Object"));	//ĸ����
	CLDSLinePart::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"Relative Rod"));
	CLDSLinePart::AddPropItem("size.idClassType",PROPLIST_ITEM(id++,"Rod Type"));
	CLDSLinePart::AddPropItem("specification",PROPLIST_ITEM(id++,"Rod Specification"));
	CLDSLinePart::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSLinePart::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"));
	CLDSLinePart::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	CLDSLinePart::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Ancillary Part"));
	CLDSLinePart::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color"));
	CLDSLinePart::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	CLDSLinePart::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
	CLDSLinePart::AddPropItem("oddMent",PROPLIST_ITEM(id++,""));
	CLDSLinePart::AddPropItem("nodeInfo",PROPLIST_ITEM(id++,"Tip node information"));
	CLDSLinePart::AddPropItem("startInfo",PROPLIST_ITEM(id++,"Start Position"));
	CLDSLinePart::AddPropItem("hStart",PROPLIST_ITEM(id++,"Start Node"));
	CLDSLinePart::AddPropItem("oddStart",PROPLIST_ITEM(id++,"Start"));
	CLDSLinePart::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSLinePart::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSLinePart::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSLinePart::AddPropItem("endInfo",PROPLIST_ITEM(id++,"End Position"));
	CLDSLinePart::AddPropItem("hEnd",PROPLIST_ITEM(id++,"End Node"));
	CLDSLinePart::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"End"));
	CLDSLinePart::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSLinePart::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSLinePart::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	//�������ͼ�������
	CLDSLinePart::AddPropItem("m_sWireType",PROPLIST_ITEM(id++,"Wire Type"));		//��������
	CLDSLinePart::AddPropItem("m_sWireSpec",PROPLIST_ITEM(id++,"Wire Specification"));		//���¹��
	CLDSLinePart::AddPropItem("m_fWireArea",PROPLIST_ITEM(id++,"Section Area"));		//���½����
	CLDSLinePart::AddPropItem("m_fUnitWeight",PROPLIST_ITEM(id++,"Unit Weight(kg/km)"));	//��λ������
	CLDSLinePart::AddPropItem("m_fForcePermit",PROPLIST_ITEM(id++,"Nominal Tensile Strength"));	//�������
	CLDSLinePart::AddPropItem("m_fElasticCoef",PROPLIST_ITEM(id++,"Elastic Coefficient(MPa)"));	//����ϵ��(ģ��)
	CLDSLinePart::AddPropItem("m_fExpansionCoef",PROPLIST_ITEM(id++,"Temperature Expand Coefficient"));//�¶�������ϵ��
	CLDSLinePart::AddPropItem("m_fPreTension",PROPLIST_ITEM(id++,"Pre-tensioning Force"));//�¶�������ϵ��
	//�û����ƽ������
	CLDSLinePart::AddPropItem("userdef",PROPLIST_ITEM(id++,""));
	CLDSLinePart::AddPropItem("userdef.size",PROPLIST_ITEM(id++,"User Specify Specification"));
	CLDSLinePart::AddPropItem("userdef.area",PROPLIST_ITEM(id++,"Section Area"));
	CLDSLinePart::AddPropItem("userdef.E",PROPLIST_ITEM(id++,"Elasticity Modulus"));
	CLDSLinePart::AddPropItem("userdef.fc",PROPLIST_ITEM(id++,"Compression Strength Design"));
	CLDSLinePart::AddPropItem("userdef.ft",PROPLIST_ITEM(id++,"Tension Strength Design"));
	CLDSLinePart::AddPropItem("userdef.pre_stress",PROPLIST_ITEM(id++,"Previous Stress"));
	CLDSLinePart::AddPropItem("userdef.unit_weight",PROPLIST_ITEM(id++,"Unit Weight"));
	CLDSLinePart::AddPropItem("length",PROPLIST_ITEM(id++,"Elem Length"));
	CLDSLinePart::AddPropItem("lengthSingleLine",PROPLIST_ITEM(id++,"Single Line Length"));
	//������������Ҫ�������Ϣ����
	CLDSLinePart::AddPropItem("designInfo",PROPLIST_ITEM(id++,"Design Information"));
	CLDSLinePart::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"Statistic Material Serial Number"));	//ͳ�ı��
	CLDSLinePart::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"Gravity Zoom Out Coefficient"));
	CLDSLinePart::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"Elem Type"));
	CLDSLinePart::AddPropItem("sel_mat_method",PROPLIST_ITEM(id++,"Brace member check rule"));
	CLDSLinePart::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Whether Material Optimization"));
	CLDSLinePart::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"Whether Specification Optimization"));
	CLDSLinePart::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"Calculate Automatic Bolt Hole Out"));
	CLDSLinePart::AddPropItem("hole_out",PROPLIST_ITEM(id++,"Bolt Hole Out"));
	//���㳤����Ϣ
	CLDSLinePart::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,""));
	CLDSLinePart::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,"Calculate Length Type"));
	CLDSLinePart::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLinePart::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,"Simultaneously Compression Reference Rod"));
	CLDSLinePart::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,"Reference Horizon Pole"));
	CLDSLinePart::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,"Minimum Axial Length"));
	CLDSLinePart::AddPropItem("CalLenCoef.minR.hStartNode",PROPLIST_ITEM(id++,"Start Node"));
	CLDSLinePart::AddPropItem("CalLenCoef.minR.hEndNode",PROPLIST_ITEM(id++,"End Node"));
	CLDSLinePart::AddPropItem("CalLenCoef.minR.coef",PROPLIST_ITEM(id++,"Reduce Calculate Coefficient"));
	CLDSLinePart::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,"Parallel Axial Length"));
	CLDSLinePart::AddPropItem("CalLenCoef.paraR.hStartNode",PROPLIST_ITEM(id++,"Start Node"));
	CLDSLinePart::AddPropItem("CalLenCoef.paraR.hEndNode",PROPLIST_ITEM(id++,"End Node"));
	CLDSLinePart::AddPropItem("CalLenCoef.paraR.coef",PROPLIST_ITEM(id++,"Reduce Calculate Coefficient"));
	//ʼ�������Ϣ
	CLDSLinePart::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"Start Information"));
	CLDSLinePart::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"Start Joint Type"));
	CLDSLinePart::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"Start Force Type"));
	CLDSLinePart::AddPropItem("connectStart",PROPLIST_ITEM(id++,"Start Connect"));
	CLDSLinePart::AddPropItem("connectStart.grade",PROPLIST_ITEM(id++,"Bolt Grade"));
	CLDSLinePart::AddPropItem("connectStart.d",PROPLIST_ITEM(id++,"Bolt Diameter"));
	CLDSLinePart::AddPropItem("connectStart.N",PROPLIST_ITEM(id++,"Bolt Number"));
	CLDSLinePart::AddPropItem("connectStart.summBoltN",PROPLIST_ITEM(id++,"Strength Bolt Number"));
	CLDSLinePart::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,"Start Bolt Load Cofficient(No Enable)"));
	CLDSLinePart::AddPropItem("connectStart.maxTension",PROPLIST_ITEM(id++,"Start Maximum connections axial tension"));
	CLDSLinePart::AddPropItem("connectStart.maxCompression",PROPLIST_ITEM(id++,"Start Maximum connections axial pressure"));
	//�ն������Ϣ
	CLDSLinePart::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"End Information"));
	CLDSLinePart::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"End Joint Type"));
	CLDSLinePart::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"End Force Type"));
	CLDSLinePart::AddPropItem("connectEnd",PROPLIST_ITEM(id++,"End Connect"));
	CLDSLinePart::AddPropItem("connectEnd.grade",PROPLIST_ITEM(id++,"Bolt Grade"));
	CLDSLinePart::AddPropItem("connectEnd.d",PROPLIST_ITEM(id++,"Bolt Diameter"));
	CLDSLinePart::AddPropItem("connectEnd.N",PROPLIST_ITEM(id++,"Bolt Number"));
	CLDSLinePart::AddPropItem("connectEnd.summBoltN",PROPLIST_ITEM(id++,"Strength Bolt Number"));
	CLDSLinePart::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,"End Bolt Load Cofficient(No Enable)"));
	CLDSLinePart::AddPropItem("connectEnd.maxTension",PROPLIST_ITEM(id++,"End Maximum connections axial tension"));
	CLDSLinePart::AddPropItem("connectEnd.maxCompression",PROPLIST_ITEM(id++,"End Maximum connections axial pressure"));
#else 
	CLDSLinePart::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"��Ԫ������Ϣ"));
	CLDSLinePart::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSLinePart::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSLinePart::AddPropItem("layer1",PROPLIST_ITEM(id++,"��һ�ַ�"));
	CLDSLinePart::AddPropItem("layer2",PROPLIST_ITEM(id++,"�ڶ��ַ�"));
	CLDSLinePart::AddPropItem("layer3",PROPLIST_ITEM(id++,"�����ַ�"));
	CLDSLinePart::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	CLDSLinePart::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�",NULL,"Ĭ��|A|B|C|D|"));
	CLDSLinePart::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSLinePart::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	CLDSLinePart::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSLinePart::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Ӱ����Ϣ"));//Ӱ����Ϣ
	CLDSLinePart::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Ӱ��ĸ����"));//Ӱ��ĸ����
	CLDSLinePart::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"��������"));//�����Ĳ���
	CLDSLinePart::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"�����˼�"));
	CLDSLinePart::AddPropItem("size.idClassType",PROPLIST_ITEM(id++,"�˼�����"));
	CLDSLinePart::AddPropItem("specification",PROPLIST_ITEM(id++,"�˼����"));
	CLDSLinePart::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	CLDSLinePart::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����"));
	CLDSLinePart::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	CLDSLinePart::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"���⸨������"));
	CLDSLinePart::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ"));
	CLDSLinePart::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	CLDSLinePart::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
	CLDSLinePart::AddPropItem("oddMent",PROPLIST_ITEM(id++,""));
	CLDSLinePart::AddPropItem("nodeInfo",PROPLIST_ITEM(id++,"�˵���Ϣ"));
	CLDSLinePart::AddPropItem("startInfo",PROPLIST_ITEM(id++,"ʼ��λ��"));
	CLDSLinePart::AddPropItem("hStart",PROPLIST_ITEM(id++,"ʼ�˽ڵ�"));
	CLDSLinePart::AddPropItem("oddStart",PROPLIST_ITEM(id++,"ʼ��"));
	CLDSLinePart::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X����"));
	CLDSLinePart::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSLinePart::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSLinePart::AddPropItem("endInfo",PROPLIST_ITEM(id++,"�ն�λ��"));
	CLDSLinePart::AddPropItem("hEnd",PROPLIST_ITEM(id++,"�ն˽ڵ�"));
	CLDSLinePart::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"�ն�"));
	CLDSLinePart::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X����"));
	CLDSLinePart::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSLinePart::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z����"));
	//�������ͼ�������
	CLDSLinePart::AddPropItem("m_sWireType",PROPLIST_ITEM(id++,"��������"));		//��������
	CLDSLinePart::AddPropItem("m_sWireSpec",PROPLIST_ITEM(id++,"���¹��"));		//���¹��
	CLDSLinePart::AddPropItem("m_fWireArea",PROPLIST_ITEM(id++,"�����"));		//���½����
	CLDSLinePart::AddPropItem("m_fUnitWeight",PROPLIST_ITEM(id++,"��λ����(kg/km)"));	//��λ������
	CLDSLinePart::AddPropItem("m_fForcePermit",PROPLIST_ITEM(id++,"�����ǿ��"));	//�������
	CLDSLinePart::AddPropItem("m_fElasticCoef",PROPLIST_ITEM(id++,"����ϵ��(MPa)"));	//����ϵ��(ģ��)
	CLDSLinePart::AddPropItem("m_fExpansionCoef",PROPLIST_ITEM(id++,"�¶�������ϵ��"));//�¶�������ϵ��
	CLDSLinePart::AddPropItem("m_fPreTension",PROPLIST_ITEM(id++,"Ԥ�Ž���"));//�¶�������ϵ��
	//�û����ƽ������
	CLDSLinePart::AddPropItem("userdef",PROPLIST_ITEM(id++,""));
	CLDSLinePart::AddPropItem("userdef.size",PROPLIST_ITEM(id++,"�Զ�����"));
	CLDSLinePart::AddPropItem("userdef.area",PROPLIST_ITEM(id++,"�����"));
	CLDSLinePart::AddPropItem("userdef.E",PROPLIST_ITEM(id++,"����ģ��"));
	CLDSLinePart::AddPropItem("userdef.fc",PROPLIST_ITEM(id++,"��ѹǿ�����ֵ"));
	CLDSLinePart::AddPropItem("userdef.ft",PROPLIST_ITEM(id++,"����ǿ�����ֵ"));
	CLDSLinePart::AddPropItem("userdef.pre_stress",PROPLIST_ITEM(id++,"ԤӦ��"));
	CLDSLinePart::AddPropItem("userdef.unit_weight",PROPLIST_ITEM(id++,"��λ����"));
	CLDSLinePart::AddPropItem("length",PROPLIST_ITEM(id++,"��Ԫ����"));
	CLDSLinePart::AddPropItem("lengthSingleLine",PROPLIST_ITEM(id++,"���߳���"));
	//������������Ҫ�������Ϣ����
	CLDSLinePart::AddPropItem("designInfo",PROPLIST_ITEM(id++,"�����Ϣ"));
	CLDSLinePart::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"ͳ�ı��"));	//ͳ�ı��
	CLDSLinePart::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"�����Ŵ�ϵ��"));
	CLDSLinePart::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"��Ԫ����"));
	CLDSLinePart::AddPropItem("sel_mat_method",PROPLIST_ITEM(id++,"������������"));
	CLDSLinePart::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"(��ע��)�Ƿ���в�����ѡ"));
	CLDSLinePart::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"�Ƿ���й����ѡ"));
	CLDSLinePart::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"�Զ�������˨������"));
	CLDSLinePart::AddPropItem("hole_out",PROPLIST_ITEM(id++,"��˨������"));
	//���㳤����Ϣ
	CLDSLinePart::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,""));
	CLDSLinePart::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,"���㳤������"));
	CLDSLinePart::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLinePart::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,"ͬʱ��ѹ���ո˼�"));
	CLDSLinePart::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,"���պ�����"));
	CLDSLinePart::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,"��С�᳤��"));
	CLDSLinePart::AddPropItem("CalLenCoef.minR.hStartNode",PROPLIST_ITEM(id++,"��ʼ�ڵ�"));
	CLDSLinePart::AddPropItem("CalLenCoef.minR.hEndNode",PROPLIST_ITEM(id++,"��ֹ�ڵ�"));
	CLDSLinePart::AddPropItem("CalLenCoef.minR.coef",PROPLIST_ITEM(id++,"����ϵ��"));
	CLDSLinePart::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,"ƽ���᳤��"));
	CLDSLinePart::AddPropItem("CalLenCoef.paraR.hStartNode",PROPLIST_ITEM(id++,"��ʼ�ڵ�"));
	CLDSLinePart::AddPropItem("CalLenCoef.paraR.hEndNode",PROPLIST_ITEM(id++,"��ֹ�ڵ�"));
	CLDSLinePart::AddPropItem("CalLenCoef.paraR.coef",PROPLIST_ITEM(id++,"����ϵ��"));
	//ʼ�������Ϣ
	CLDSLinePart::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"ʼ����Ϣ"));
	CLDSLinePart::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"ʼ��Լ������"));
	CLDSLinePart::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"ʼ����������"));
	CLDSLinePart::AddPropItem("connectStart",PROPLIST_ITEM(id++,"ʼ������"));
	CLDSLinePart::AddPropItem("connectStart.grade",PROPLIST_ITEM(id++,"��˨����"));
	CLDSLinePart::AddPropItem("connectStart.d",PROPLIST_ITEM(id++,"��˨ֱ��"));
	CLDSLinePart::AddPropItem("connectStart.N",PROPLIST_ITEM(id++,"������˨��"));
	CLDSLinePart::AddPropItem("connectStart.summBoltN",PROPLIST_ITEM(id++,"������˨��"));
	CLDSLinePart::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,"ʼ����˨����ϵ��(δ����)"));
	CLDSLinePart::AddPropItem("connectStart.maxTension",PROPLIST_ITEM(id++,"ʼ�����������������"));
	CLDSLinePart::AddPropItem("connectStart.maxCompression",PROPLIST_ITEM(id++,"ʼ�������������ѹ��"));
	//�ն������Ϣ
	CLDSLinePart::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"�ն���Ϣ"));
	CLDSLinePart::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"�ն�Լ������"));
	CLDSLinePart::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"�ն���������"));
	CLDSLinePart::AddPropItem("connectEnd",PROPLIST_ITEM(id++,"�ն�����"));
	CLDSLinePart::AddPropItem("connectEnd.grade",PROPLIST_ITEM(id++,"��˨����"));
	CLDSLinePart::AddPropItem("connectEnd.d",PROPLIST_ITEM(id++,"��˨ֱ��"));
	CLDSLinePart::AddPropItem("connectEnd.N",PROPLIST_ITEM(id++,"������˨��"));
	CLDSLinePart::AddPropItem("connectEnd.summBoltN",PROPLIST_ITEM(id++,"������˨��"));
	CLDSLinePart::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,"�ն���˨����ϵ��(δ����)"));
	CLDSLinePart::AddPropItem("connectEnd.maxTension",PROPLIST_ITEM(id++,"�ն����������������"));
	CLDSLinePart::AddPropItem("connectEnd.maxCompression",PROPLIST_ITEM(id++,"�ն������������ѹ��"));
#endif
}
CHashStrList<PROPLIST_ITEM> CElemInfo::propHashtable;
CXhChar100 CElemInfo::HELPLINK_URL;
CHashTable<DWORD> CElemInfo::propStatusHashtable;
CHashSet<PROPLIST_ITEM*> CElemInfo::hashPropItemById;
int CElemInfo::m_iCurDisplayPropGroup;
void CElemInfo::InitElemInfoPropHashtable()
{
	int id = 1;
	CElemInfo::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CElemInfo::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CElemInfo::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Unit Basic Information"));
	CElemInfo::AddPropItem("Id",PROPLIST_ITEM(id++,"Identification"));
	CElemInfo::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CElemInfo::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	CElemInfo::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CElemInfo::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CElemInfo::AddPropItem("length",PROPLIST_ITEM(id++,"Elem Length"));
	CElemInfo::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"Relative Rod"));
	CElemInfo::AddPropItem("size.idClassType",PROPLIST_ITEM(id++,"Rod Type"));
	CElemInfo::AddPropItem("specification",PROPLIST_ITEM(id++,"Rod Specification"));
	CElemInfo::AddPropItem("startInfo",PROPLIST_ITEM(id++,"Start Information"));
	CElemInfo::AddPropItem("hStart",PROPLIST_ITEM(id++,"Start Node"));
	//CLDSLinePart::propHashtable.SetValueAt("startPos.x",PROPLIST_ITEM(id++,""));
	//CLDSLinePart::propHashtable.SetValueAt("startPos.y",PROPLIST_ITEM(id++,""));
	//CLDSLinePart::propHashtable.SetValueAt("startPos.z",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("endInfo",PROPLIST_ITEM(id++,"End Position"));
	CElemInfo::AddPropItem("hEnd",PROPLIST_ITEM(id++,"End Node"));
	//CLDSLinePart::propHashtable.SetValueAt("endPos.x",PROPLIST_ITEM(id++,""));
	//CLDSLinePart::propHashtable.SetValueAt("endPos.y",PROPLIST_ITEM(id++,""));
	//CLDSLinePart::propHashtable.SetValueAt("endPos.z",PROPLIST_ITEM(id++,""));
	//�������ͼ�������
	CElemInfo::AddPropItem("m_sWireType",PROPLIST_ITEM(id++,"Wire Type"));		//��������
	CElemInfo::AddPropItem("m_sWireSpec",PROPLIST_ITEM(id++,"Wire Specification"));		//���¹��
	CElemInfo::AddPropItem("m_fWireArea",PROPLIST_ITEM(id++,"Section Area"));		//���½����
	CElemInfo::AddPropItem("m_fUnitWeight",PROPLIST_ITEM(id++,"Unit Weight(kg/km)"));		//��λ������
	CElemInfo::AddPropItem("m_fForcePermit",PROPLIST_ITEM(id++,"Nominal Tensile Strength"));		//�������
	CElemInfo::AddPropItem("m_fElasticCoef",PROPLIST_ITEM(id++,"Elastic Coefficient(MPa)"));		//����ϵ��(ģ��)
	CElemInfo::AddPropItem("m_fExpansionCoef",PROPLIST_ITEM(id++,"Temperature Expand Coefficient"));	//�¶�������ϵ��
	CElemInfo::AddPropItem("m_fPreTension",PROPLIST_ITEM(id++,"Pre-tensioning Force"));		//�¶�������ϵ��
	//�û����ƽ������
	CElemInfo::AddPropItem("userdef",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("userdef.size",PROPLIST_ITEM(id++,"User Specify Specification"));
	CElemInfo::AddPropItem("userdef.area",PROPLIST_ITEM(id++,"Section Area"));
	CElemInfo::AddPropItem("userdef.E",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Elasticity Modulus"));
	CElemInfo::AddPropItem("userdef.fc",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Compression Strength Design"));
	CElemInfo::AddPropItem("userdef.ft",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Tension Strength Design"));
	CElemInfo::AddPropItem("userdef.pre_stress",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Previous Stress"));
	CElemInfo::AddPropItem("userdef.unit_weight",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Unit Weight"));
	CElemInfo::AddPropItem("length",PROPLIST_ITEM(id++,"Elem Length"));
	CElemInfo::AddPropItem("lengthSingleLine",PROPLIST_ITEM(id++,"Single Line Length"));
	//������������Ҫ�������Ϣ����
	CElemInfo::AddPropItem("designInfo",PROPLIST_ITEM(id++,"Design Information"));
	CElemInfo::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"Statistic Material Serial Number"));	//ͳ�ı��
	CElemInfo::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"Gravity Zoom Out Coefficient"));
	CElemInfo::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"Elem Type"));
	//CLDSLinePart::propHashtable.SetValueAt("m_bNeedSelMat",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"Whether Specification Optimization"));
	//�����Ϣ
	CElemInfo::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"Start Joint Type"));
	CElemInfo::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"Start Force Type"));
	CElemInfo::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"End Joint Type"));
	CElemInfo::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"End Force Type"));
	CElemInfo::AddPropItem("boltRelaInfo",PROPLIST_ITEM(id++,"Bolt Relative Information"));
	CElemInfo::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"Calculate Automatic Bolt Hole Out"));
	CElemInfo::AddPropItem("hole_out",PROPLIST_ITEM(id++,"Bolt Hole Out"));
	CElemInfo::AddPropItem("bolt_type",PROPLIST_ITEM(id++,"Bolt Specification"));
	CElemInfo::AddPropItem("bolt_N",PROPLIST_ITEM(id++,"Bolt Number"));
	CElemInfo::AddPropItem("bolt_bearloadcoef",PROPLIST_ITEM(id++,""));
	//���㳤����Ϣ
	CElemInfo::AddPropItem("lamdaInfo",PROPLIST_ITEM(id++,"Calculate Length Information"));
	CElemInfo::AddPropItem("cCalLenType",PROPLIST_ITEM(id++,"Long Thin Compare Calculate Type"));
	CElemInfo::AddPropItem("lamdaRyoInfo",PROPLIST_ITEM(id++,"Minimum Axial"));
	CElemInfo::AddPropItem("LoRyo",PROPLIST_ITEM(id++,"Calculate Length"));
	CElemInfo::AddPropItem("LoRyo.CalType",PROPLIST_ITEM(id++,"Basic Length Type"));
	CElemInfo::AddPropItem("LoRyo.coef",PROPLIST_ITEM(id++,"Reduce Calculate Coefficient"));
	CElemInfo::AddPropItem("LoRyo.hStartNode",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("LoRyo.hEndNode",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("lamdaRxInfo",PROPLIST_ITEM(id++,"Parallel Axial"));
	CElemInfo::AddPropItem("LoRx",PROPLIST_ITEM(id++,"Calculate Length"));
	CElemInfo::AddPropItem("LoRx.CalType",PROPLIST_ITEM(id++,"Basic Length Type"));
	CElemInfo::AddPropItem("LoRx.coef",PROPLIST_ITEM(id++,"Reduce Calculate Coefficient"));
	CElemInfo::AddPropItem("LoRx.hStartNode",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("LoRx.hEndNode",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("lamdaRxcInfo",PROPLIST_ITEM(id++,"Simultaneously Compression"));			//ͬʱ��ѹ��
	//ͬʱ��ѹ��һ��
	CElemInfo::AddPropItem("smcItem1",PROPLIST_ITEM(id++,""));					
	CElemInfo::AddPropItem("smcItem1.cSMCType",PROPLIST_ITEM(id++,"Compression Type"));			//��ѹ����
	CElemInfo::AddPropItem("smcItem1.relaElem",PROPLIST_ITEM(id++,"Reference Elem"));			//���յ�Ԫ
	CElemInfo::AddPropItem("smcItem1.gyreRadius",PROPLIST_ITEM(id++,"Gyre Radius"));		//��ת�뾶
#else 
	CElemInfo::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"��Ԫ������Ϣ"));
	CElemInfo::AddPropItem("Id",PROPLIST_ITEM(id++,"��ʶ"));
	CElemInfo::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CElemInfo::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	CElemInfo::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CElemInfo::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CElemInfo::AddPropItem("length",PROPLIST_ITEM(id++,"��Ԫ����"));
	CElemInfo::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"�����˼�"));
	CElemInfo::AddPropItem("size.idClassType",PROPLIST_ITEM(id++,"�˼�����"));
	CElemInfo::AddPropItem("specification",PROPLIST_ITEM(id++,"�˼����"));
	CElemInfo::AddPropItem("startInfo",PROPLIST_ITEM(id++,"ʼ����Ϣ"));
	CElemInfo::AddPropItem("hStart",PROPLIST_ITEM(id++,"ʼ�˽ڵ�"));
	//CLDSLinePart::propHashtable.SetValueAt("startPos.x",PROPLIST_ITEM(id++,""));
	//CLDSLinePart::propHashtable.SetValueAt("startPos.y",PROPLIST_ITEM(id++,""));
	//CLDSLinePart::propHashtable.SetValueAt("startPos.z",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("endInfo",PROPLIST_ITEM(id++,"�ն�λ��"));
	CElemInfo::AddPropItem("hEnd",PROPLIST_ITEM(id++,"�ն˽ڵ�"));
	//CLDSLinePart::propHashtable.SetValueAt("endPos.x",PROPLIST_ITEM(id++,""));
	//CLDSLinePart::propHashtable.SetValueAt("endPos.y",PROPLIST_ITEM(id++,""));
	//CLDSLinePart::propHashtable.SetValueAt("endPos.z",PROPLIST_ITEM(id++,""));
	//�������ͼ�������
	CElemInfo::AddPropItem("m_sWireType",PROPLIST_ITEM(id++,"��������"));		//��������
	CElemInfo::AddPropItem("m_sWireSpec",PROPLIST_ITEM(id++,"���¹��"));		//���¹��
	CElemInfo::AddPropItem("m_fWireArea",PROPLIST_ITEM(id++,"�����"));		//���½����
	CElemInfo::AddPropItem("m_fUnitWeight",PROPLIST_ITEM(id++,"��λ����(kg/km)"));		//��λ������
	CElemInfo::AddPropItem("m_fForcePermit",PROPLIST_ITEM(id++,"�����ǿ��"));		//�������
	CElemInfo::AddPropItem("m_fElasticCoef",PROPLIST_ITEM(id++,"����ϵ��(MPa)"));		//����ϵ��(ģ��)
	CElemInfo::AddPropItem("m_fExpansionCoef",PROPLIST_ITEM(id++,"�¶�������ϵ��"));	//�¶�������ϵ��
	CElemInfo::AddPropItem("m_fPreTension",PROPLIST_ITEM(id++,"Ԥ�Ž���"));		//�¶�������ϵ��
	//�û����ƽ������
	CElemInfo::AddPropItem("userdef",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("userdef.size",PROPLIST_ITEM(id++,"�Զ�����"));
	CElemInfo::AddPropItem("userdef.area",PROPLIST_ITEM(id++,"�����"));
	CElemInfo::AddPropItem("userdef.E",PROPLIST_ITEM(id++,"(��ע��)����ģ��"));
	CElemInfo::AddPropItem("userdef.fc",PROPLIST_ITEM(id++,"(��ע��)��ѹǿ�����ֵ"));
	CElemInfo::AddPropItem("userdef.ft",PROPLIST_ITEM(id++,"(��ע��)����ǿ�����ֵ"));
	CElemInfo::AddPropItem("userdef.pre_stress",PROPLIST_ITEM(id++,"(��ע��)ԤӦ��"));
	CElemInfo::AddPropItem("userdef.unit_weight",PROPLIST_ITEM(id++,"(��ע��)��λ����"));
	CElemInfo::AddPropItem("length",PROPLIST_ITEM(id++,"��Ԫ����"));
	CElemInfo::AddPropItem("lengthSingleLine",PROPLIST_ITEM(id++,"���߳���"));
	//������������Ҫ�������Ϣ����
	CElemInfo::AddPropItem("designInfo",PROPLIST_ITEM(id++,"�����Ϣ"));
	CElemInfo::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"ͳ�ı��"));	//ͳ�ı��
	CElemInfo::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"�����Ŵ�ϵ��"));
	CElemInfo::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"��Ԫ����"));
	//CLDSLinePart::propHashtable.SetValueAt("m_bNeedSelMat",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"�Ƿ���й����ѡ"));
	//�����Ϣ
	CElemInfo::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"ʼ��Լ������"));
	CElemInfo::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"ʼ����������"));
	CElemInfo::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"�ն�Լ������"));
	CElemInfo::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"�ն���������"));
	CElemInfo::AddPropItem("boltRelaInfo",PROPLIST_ITEM(id++,"��˨�����Ϣ"));
	CElemInfo::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"�Զ�������˨������"));
	CElemInfo::AddPropItem("hole_out",PROPLIST_ITEM(id++,"��˨������"));
	CElemInfo::AddPropItem("bolt_type",PROPLIST_ITEM(id++,"��˨���"));
	CElemInfo::AddPropItem("bolt_N",PROPLIST_ITEM(id++,"������˨��"));
	CElemInfo::AddPropItem("bolt_bearloadcoef",PROPLIST_ITEM(id++,""));
	//���㳤����Ϣ
	CElemInfo::AddPropItem("lamdaInfo",PROPLIST_ITEM(id++,"���㳤����Ϣ"));
	CElemInfo::AddPropItem("cCalLenType",PROPLIST_ITEM(id++,"��ϸ�ȼ�������"));
	CElemInfo::AddPropItem("lamdaRyoInfo",PROPLIST_ITEM(id++,"��С��"));
	CElemInfo::AddPropItem("LoRyo",PROPLIST_ITEM(id++,"���㳤��"));
	CElemInfo::AddPropItem("LoRyo.CalType",PROPLIST_ITEM(id++,"��׼��������"));
	CElemInfo::AddPropItem("LoRyo.coef",PROPLIST_ITEM(id++,"����ϵ��"));
	CElemInfo::AddPropItem("LoRyo.hStartNode",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("LoRyo.hEndNode",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("lamdaRxInfo",PROPLIST_ITEM(id++,"ƽ����"));
	CElemInfo::AddPropItem("LoRx",PROPLIST_ITEM(id++,"���㳤��"));
	CElemInfo::AddPropItem("LoRx.CalType",PROPLIST_ITEM(id++,"��׼��������"));
	CElemInfo::AddPropItem("LoRx.coef",PROPLIST_ITEM(id++,"����ϵ��"));
	CElemInfo::AddPropItem("LoRx.hStartNode",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("LoRx.hEndNode",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("lamdaRxcInfo",PROPLIST_ITEM(id++,"ͬʱ��ѹ"));			//ͬʱ��ѹ��
	//ͬʱ��ѹ��һ��
	CElemInfo::AddPropItem("smcItem1",PROPLIST_ITEM(id++,""));					
	CElemInfo::AddPropItem("smcItem1.cSMCType",PROPLIST_ITEM(id++,"��ѹ����"));			//��ѹ����
	CElemInfo::AddPropItem("smcItem1.relaElem",PROPLIST_ITEM(id++,"���յ�Ԫ"));			//���յ�Ԫ
	CElemInfo::AddPropItem("smcItem1.gyreRadius",PROPLIST_ITEM(id++,"��ת�뾶"));		//��ת�뾶
#endif
	CElemInfo::AddPropItem("smcItem1.LoRxc",PROPLIST_ITEM(id++,""));				//���㳤��
	CElemInfo::AddPropItem("smcItem1.LoRxc.coef",PROPLIST_ITEM(id++,""));		//����ϵ��
	CElemInfo::AddPropItem("smcItem1.LoRxc.CalType",PROPLIST_ITEM(id++,""));		//��׼��������
	CElemInfo::AddPropItem("smcItem1.LoRxc.hStartNode",PROPLIST_ITEM(id++,""));	//ָ�����ȵ���ʼ�ڵ��";
	CElemInfo::AddPropItem("smcItem1.LoRxc.hEndNode",PROPLIST_ITEM(id++,""));	//ָ�����ȵ���ֹ�ڵ��
	//ͬʱ��ѹ�ڶ���
	CElemInfo::AddPropItem("smcItem2",PROPLIST_ITEM(id++,""));					
	CElemInfo::AddPropItem("smcItem2.cSMCType",PROPLIST_ITEM(id++,""));			//��ѹ����
	CElemInfo::AddPropItem("smcItem2.relaElem",PROPLIST_ITEM(id++,""));			//���յ�Ԫ
	CElemInfo::AddPropItem("smcItem2.gyreRadius",PROPLIST_ITEM(id++,""));		//��ת�뾶
	CElemInfo::AddPropItem("smcItem2.LoRxc",PROPLIST_ITEM(id++,""));				//���㳤��
	CElemInfo::AddPropItem("smcItem2.LoRxc.coef",PROPLIST_ITEM(id++,""));		//����ϵ��
	CElemInfo::AddPropItem("smcItem2.LoRxc.CalType",PROPLIST_ITEM(id++,""));		//��׼��������
	CElemInfo::AddPropItem("smcItem2.LoRxc.hStartNode",PROPLIST_ITEM(id++,""));	//ָ�����ȵ���ʼ�ڵ��";
	CElemInfo::AddPropItem("smcItem2.LoRxc.hEndNode",PROPLIST_ITEM(id++,""));	//ָ�����ȵ���ֹ�ڵ��
	//ͬʱ��ѹ������
	CElemInfo::AddPropItem("smcItem3",PROPLIST_ITEM(id++,""));					
	CElemInfo::AddPropItem("smcItem3.cSMCType",PROPLIST_ITEM(id++,""));			//��ѹ����
	CElemInfo::AddPropItem("smcItem3.relaElem",PROPLIST_ITEM(id++,""));			//���յ�Ԫ
	CElemInfo::AddPropItem("smcItem3.gyreRadius",PROPLIST_ITEM(id++,""));		//��ת�뾶
	CElemInfo::AddPropItem("smcItem3.LoRxc",PROPLIST_ITEM(id++,""));				//���㳤��
	CElemInfo::AddPropItem("smcItem3.LoRxc.coef",PROPLIST_ITEM(id++,""));		//����ϵ��
	CElemInfo::AddPropItem("smcItem3.LoRxc.CalType",PROPLIST_ITEM(id++,""));		//��׼��������
	CElemInfo::AddPropItem("smcItem3.LoRxc.hStartNode",PROPLIST_ITEM(id++,""));	//ָ�����ȵ���ʼ�ڵ��";
	CElemInfo::AddPropItem("smcItem3.LoRxc.hEndNode",PROPLIST_ITEM(id++,""));	//ָ�����ȵ���ֹ�ڵ��
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
CHashStrList<PROPLIST_ITEM> CLDSArcPart::propHashtable;
CXhChar100 CLDSArcPart::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*>CLDSArcPart::hashPropItemById;
CHashTable<DWORD> CLDSArcPart::propStatusHashtable;
int CLDSArcPart::m_iCurDisplayPropGroup;
void CLDSArcPart::InitArcPartPropHashtable()
{
	int id = 1;
	CLDSArcPart::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSArcPart::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSArcPart::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Circular Part Basic Information"));
	CLDSArcPart::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSArcPart::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSArcPart::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	CLDSArcPart::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade",NULL,"Default|A|B|C|D|"));
	CLDSArcPart::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSArcPart::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Serial Number"));
	CLDSArcPart::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSArcPart::AddPropItem("m_fZoomLenCoef",PROPLIST_ITEM(id++,"length cutting coef"));
	CLDSArcPart::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,""));
	CLDSArcPart::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSArcPart::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"));
	CLDSArcPart::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	CLDSArcPart::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Ancillary Part"));
	CLDSArcPart::AddPropItem("m_uDisplaySlices",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Display Slices"));
	CLDSArcPart::AddPropItem("neutro_radius",PROPLIST_ITEM(id++,"Neutrosphere Arc Radius "));
	CLDSArcPart::AddPropItem("start_oddment",PROPLIST_ITEM(id++,"Start Odd Length"));
	CLDSArcPart::AddPropItem("end_oddment",PROPLIST_ITEM(id++,"End Odd Length"));
	CLDSArcPart::AddPropItem("offset_h",PROPLIST_ITEM(id++,"Radial Offset"));
	CLDSArcPart::AddPropItem("offset_v",PROPLIST_ITEM(id++,"Normal Offset"));
	CLDSArcPart::AddPropItem("arc_start",PROPLIST_ITEM(id++,""));
	CLDSArcPart::AddPropItem("arc_start.x",PROPLIST_ITEM(id++,""));
	CLDSArcPart::AddPropItem("arc_start.y",PROPLIST_ITEM(id++,""));
	CLDSArcPart::AddPropItem("arc_start.z",PROPLIST_ITEM(id++,""));
	CLDSArcPart::AddPropItem("start_rot_angle",PROPLIST_ITEM(id++,"Start Offset Rotate Angle"));
	CLDSArcPart::AddPropItem("sector_angle",PROPLIST_ITEM(id++,"Arc Sector Angle"));
	CLDSArcPart::AddPropItem("m_siLayoutStyle",PROPLIST_ITEM(id++,"Layout Style"));
	CLDSArcPart::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color"));
	CLDSArcPart::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	CLDSArcPart::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
#else 
	CLDSArcPart::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"���ι���������Ϣ"));
	CLDSArcPart::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSArcPart::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSArcPart::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	CLDSArcPart::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�",NULL,"Ĭ��|A|B|C|D|"));
	CLDSArcPart::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSArcPart::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	CLDSArcPart::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSArcPart::AddPropItem("m_fZoomLenCoef",PROPLIST_ITEM(id++,"����ϵ��"));
	CLDSArcPart::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"�����˼�"));
	CLDSArcPart::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	CLDSArcPart::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����"));
	CLDSArcPart::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	CLDSArcPart::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"���⸨������"));
	CLDSArcPart::AddPropItem("m_uDisplaySlices",PROPLIST_ITEM(id++,"(��ע��)��ʾƬ��"));
	CLDSArcPart::AddPropItem("neutro_radius",PROPLIST_ITEM(id++,"���Բ�Բ���뾶"));
	CLDSArcPart::AddPropItem("start_oddment",PROPLIST_ITEM(id++,"ʼ������ͷ"));
	CLDSArcPart::AddPropItem("end_oddment",PROPLIST_ITEM(id++,"�ն�����ͷ"));
	CLDSArcPart::AddPropItem("offset_h",PROPLIST_ITEM(id++,"����λ��"));
	CLDSArcPart::AddPropItem("offset_v",PROPLIST_ITEM(id++,"����λ��"));
	CLDSArcPart::AddPropItem("arc_start",PROPLIST_ITEM(id++,""));
	CLDSArcPart::AddPropItem("arc_start.x",PROPLIST_ITEM(id++,""));
	CLDSArcPart::AddPropItem("arc_start.y",PROPLIST_ITEM(id++,""));
	CLDSArcPart::AddPropItem("arc_start.z",PROPLIST_ITEM(id++,""));
	CLDSArcPart::AddPropItem("start_rot_angle",PROPLIST_ITEM(id++,"��ʼƫת��"));
	CLDSArcPart::AddPropItem("sector_angle",PROPLIST_ITEM(id++,"Բ�����ν�"));
	CLDSArcPart::AddPropItem("m_siLayoutStyle",PROPLIST_ITEM(id++,"�ڷŷ�ʽ"));
	CLDSArcPart::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ"));
	CLDSArcPart::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	CLDSArcPart::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
#endif
}

CHashStrList<PROPLIST_ITEM> CLDSArcAngle::propHashtable;
CXhChar100 CLDSArcAngle::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*>CLDSArcAngle::hashPropItemById;
CHashTable<DWORD> CLDSArcAngle::propStatusHashtable;
int CLDSArcAngle::m_iCurDisplayPropGroup;
void CLDSArcAngle::InitArcAnglePropHashtable()
{
	int id = 1;
	CLDSArcAngle::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSArcAngle::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSArcAngle::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Circular Angle Basic Information"));
	CLDSArcAngle::AddPropItem("geometryInfo",PROPLIST_ITEM(id++,"Geometry Information"));
	CLDSArcAngle::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSArcAngle::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSArcAngle::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	CLDSArcAngle::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade",NULL,"Default|A|B|C|D|"));
	CLDSArcAngle::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSArcAngle::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Serial Number"));
	CLDSArcAngle::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSArcAngle::AddPropItem("m_fZoomLenCoef",PROPLIST_ITEM(id++,"length cutting coef"));
	CLDSArcAngle::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSArcAngle::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"));
	CLDSArcAngle::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	CLDSArcAngle::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Ancillary Part"));
	CLDSArcAngle::AddPropItem("m_uDisplaySlices",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Display Slices"));
	CLDSArcAngle::AddPropItem("neutro_radius",PROPLIST_ITEM(id++,"Neutrosphere Arc Radius "));
	CLDSArcAngle::AddPropItem("arc_radius",PROPLIST_ITEM(id++,"Arc Radius"));
	CLDSArcAngle::AddPropItem("offset_h",PROPLIST_ITEM(id++,"Radial Offset"));
	CLDSArcAngle::AddPropItem("offset_v",PROPLIST_ITEM(id++,"Normal Offset"));
	CLDSArcAngle::AddPropItem("arc_start",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("arc_start.x",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("arc_start.y",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("arc_start.z",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("start_rot_angle",PROPLIST_ITEM(id++,"Start Offset Rotate Angle"));
	CLDSArcAngle::AddPropItem("sector_angle",PROPLIST_ITEM(id++,"Arc Sector Angle"));
	CLDSArcAngle::AddPropItem("des_arc_center",PROPLIST_ITEM(id++,"Center Position"));
	CLDSArcAngle::AddPropItem("des_arc_start",PROPLIST_ITEM(id++,"Arc Start Point"));
	CLDSArcAngle::AddPropItem("des_arc_norm",PROPLIST_ITEM(id++,"Work Plane Normal"));
	CLDSArcAngle::AddPropItem("m_siLayoutStyle",PROPLIST_ITEM(id++,"Layout Style"));
	CLDSArcAngle::AddPropItem("start_oddment",PROPLIST_ITEM(id++,"Start Odd Length"));
	CLDSArcAngle::AddPropItem("end_oddment",PROPLIST_ITEM(id++,"End Odd Length"));
	CLDSArcAngle::AddPropItem("specification",PROPLIST_ITEM(id++,"Specification Type"));
	CLDSArcAngle::AddPropItem("wing_wide",PROPLIST_ITEM(id++,"Leg Width"));
	CLDSArcAngle::AddPropItem("wing_thick",PROPLIST_ITEM(id++,"Leg Thickness"));
	CLDSArcAngle::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color"));
	CLDSArcAngle::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Specify Part Color"));
	CLDSArcAngle::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
	CLDSArcAngle::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"Assembling Position"));
	CLDSArcAngle::AddPropItem("center",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("center.x",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("center.y",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("center.z",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("workNorm",PROPLIST_ITEM(id++,"Work Normal"));
	CLDSArcAngle::AddPropItem("workNorm.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcAngle::AddPropItem("workNorm.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcAngle::AddPropItem("workNorm.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSArcAngle::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"Position"));
	CLDSArcAngle::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSArcAngle::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSArcAngle::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSArcAngle::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X Axis Direction"));
	CLDSArcAngle::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcAngle::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcAngle::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSArcAngle::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y Axis Direction"));
	CLDSArcAngle::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcAngle::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcAngle::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSArcAngle::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z Axis Direction"));
	CLDSArcAngle::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcAngle::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcAngle::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z Component"));
	//ʼ�ն�
	CLDSArcAngle::AddPropItem("startInfo",PROPLIST_ITEM(id++,"startInfo"));
	CLDSArcAngle::AddPropItem("hStart",PROPLIST_ITEM(id++,"start node handle"));
	CLDSArcAngle::AddPropItem("oddStart",PROPLIST_ITEM(id++,"odd start"));
	CLDSArcAngle::AddPropItem("startPos",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcAngle::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcAngle::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSArcAngle::AddPropItem("endInfo",PROPLIST_ITEM(id++,"endInfo"));
	CLDSArcAngle::AddPropItem("hEnd",PROPLIST_ITEM(id++,"end node handle"));
	CLDSArcAngle::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"oddEnd"));
	CLDSArcAngle::AddPropItem("endPos",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcAngle::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcAngle::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z Component"));
#else 
	CLDSArcAngle::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"���νǸֻ�����Ϣ"));
	CLDSArcAngle::AddPropItem("geometryInfo",PROPLIST_ITEM(id++,"��λ��Ϣ"));
	CLDSArcAngle::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSArcAngle::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSArcAngle::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	CLDSArcAngle::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�",NULL,"Ĭ��|A|B|C|D|"));
	CLDSArcAngle::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSArcAngle::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	CLDSArcAngle::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSArcAngle::AddPropItem("m_fZoomLenCoef",PROPLIST_ITEM(id++,"����ϵ��"));
	CLDSArcAngle::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	CLDSArcAngle::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����"));
	CLDSArcAngle::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	CLDSArcAngle::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"���⸨������"));
	CLDSArcAngle::AddPropItem("m_uDisplaySlices",PROPLIST_ITEM(id++,"(��ע��)��ʾƬ��"));
	CLDSArcAngle::AddPropItem("neutro_radius",PROPLIST_ITEM(id++,"���Բ�Բ���뾶"));
	CLDSArcAngle::AddPropItem("arc_radius",PROPLIST_ITEM(id++,"Բ���뾶"));
	CLDSArcAngle::AddPropItem("offset_h",PROPLIST_ITEM(id++,"����λ��"));
	CLDSArcAngle::AddPropItem("offset_v",PROPLIST_ITEM(id++,"����λ��"));
	CLDSArcAngle::AddPropItem("arc_start",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("arc_start.x",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("arc_start.y",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("arc_start.z",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("start_rot_angle",PROPLIST_ITEM(id++,"��ʼƫת��"));
	CLDSArcAngle::AddPropItem("sector_angle",PROPLIST_ITEM(id++,"Բ�����ν�"));
	CLDSArcAngle::AddPropItem("des_arc_center",PROPLIST_ITEM(id++,"Բ��λ��"));
	CLDSArcAngle::AddPropItem("des_arc_start",PROPLIST_ITEM(id++,"Բ�����"));
	CLDSArcAngle::AddPropItem("des_arc_norm",PROPLIST_ITEM(id++,"�����淨��"));
	CLDSArcAngle::AddPropItem("m_siLayoutStyle",PROPLIST_ITEM(id++,"�ڷŷ�ʽ"));
	CLDSArcAngle::AddPropItem("start_oddment",PROPLIST_ITEM(id++,"ʼ������ͷ"));
	CLDSArcAngle::AddPropItem("end_oddment",PROPLIST_ITEM(id++,"�ն�����ͷ"));
	CLDSArcAngle::AddPropItem("specification",PROPLIST_ITEM(id++,"����ͺ�"));
	CLDSArcAngle::AddPropItem("wing_wide",PROPLIST_ITEM(id++,"֫��"));
	CLDSArcAngle::AddPropItem("wing_thick",PROPLIST_ITEM(id++,"֫��"));
	CLDSArcAngle::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ"));
	CLDSArcAngle::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	CLDSArcAngle::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
	CLDSArcAngle::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"װ�䷽λ"));
	CLDSArcAngle::AddPropItem("center",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("center.x",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("center.y",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("center.z",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("workNorm",PROPLIST_ITEM(id++,"��������"));
	CLDSArcAngle::AddPropItem("workNorm.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSArcAngle::AddPropItem("workNorm.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSArcAngle::AddPropItem("workNorm.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSArcAngle::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"λ��"));
	CLDSArcAngle::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X����"));
	CLDSArcAngle::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSArcAngle::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSArcAngle::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X�᷽��"));
	CLDSArcAngle::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSArcAngle::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSArcAngle::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSArcAngle::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y�᷽��"));
	CLDSArcAngle::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSArcAngle::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSArcAngle::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSArcAngle::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z�᷽��"));
	CLDSArcAngle::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSArcAngle::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSArcAngle::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z�������"));
	//ʼ�ն�
	CLDSArcAngle::AddPropItem("startInfo",PROPLIST_ITEM(id++,"ʼ����Ϣ"));
	CLDSArcAngle::AddPropItem("hStart",PROPLIST_ITEM(id++,"�����"));
	CLDSArcAngle::AddPropItem("startPos",PROPLIST_ITEM(id++,"ʼ���������"));
	CLDSArcAngle::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"ʼ��ʵ������"));
	CLDSArcAngle::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X����"));
	CLDSArcAngle::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSArcAngle::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSArcAngle::AddPropItem("endInfo",PROPLIST_ITEM(id++,"�ն���Ϣ"));
	CLDSArcAngle::AddPropItem("hEnd",PROPLIST_ITEM(id++,"�յ���"));
	CLDSArcAngle::AddPropItem("endPos",PROPLIST_ITEM(id++,"�ն��������"));
	CLDSArcAngle::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"�ն�ʵ������"));
	CLDSArcAngle::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X����"));
	CLDSArcAngle::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSArcAngle::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z����"));
#endif
}

CHashStrList<PROPLIST_ITEM> CLDSArcFlat::propHashtable;
CXhChar100 CLDSArcFlat::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*>CLDSArcFlat::hashPropItemById;
CHashTable<DWORD> CLDSArcFlat::propStatusHashtable;
int CLDSArcFlat::m_iCurDisplayPropGroup;
void CLDSArcFlat::InitArcFlatPropHashtable()
{
	int id = 1;
	CLDSArcFlat::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSArcFlat::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSArcFlat::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Circular Flat Steel Basic Information"));
	CLDSArcFlat::AddPropItem("geometryInfo",PROPLIST_ITEM(id++,"Geometry Information"));
	CLDSArcFlat::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSArcFlat::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSArcFlat::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	CLDSArcFlat::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade",NULL,"Default|A|B|C|D|"));
	CLDSArcFlat::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSArcFlat::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Serial Number"));
	CLDSArcFlat::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSArcFlat::AddPropItem("m_fZoomLenCoef",PROPLIST_ITEM(id++,"length cutting coef"));
	CLDSArcFlat::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSArcFlat::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"));
	CLDSArcFlat::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	CLDSArcFlat::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Ancillary Part"));
	CLDSArcFlat::AddPropItem("m_uDisplaySlices",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Display Slices"));
	CLDSArcFlat::AddPropItem("neutro_radius",PROPLIST_ITEM(id++,"Neutrosphere Arc Radius "));
	CLDSArcFlat::AddPropItem("arc_radius",PROPLIST_ITEM(id++,"Arc Radius"));
	CLDSArcFlat::AddPropItem("offset_h",PROPLIST_ITEM(id++,"Radial Offset"));
	CLDSArcFlat::AddPropItem("offset_v",PROPLIST_ITEM(id++,"Normal Offset"));
	CLDSArcFlat::AddPropItem("arc_start",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("arc_start.x",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("arc_start.y",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("arc_start.z",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("start_rot_angle",PROPLIST_ITEM(id++,"Start Offset Rotate Angle"));
	CLDSArcFlat::AddPropItem("sector_angle",PROPLIST_ITEM(id++,"Arc Sector Angle"));
	CLDSArcFlat::AddPropItem("des_arc_center",PROPLIST_ITEM(id++,"Center Position"));
	CLDSArcFlat::AddPropItem("des_arc_start",PROPLIST_ITEM(id++,"Arc Start Point"));
	CLDSArcFlat::AddPropItem("des_arc_norm",PROPLIST_ITEM(id++,"Work Plane Normal"));
	CLDSArcFlat::AddPropItem("m_siLayoutStyle",PROPLIST_ITEM(id++,"Layout Style"));
	CLDSArcFlat::AddPropItem("start_oddment",PROPLIST_ITEM(id++,"Start Odd Length"));
	CLDSArcFlat::AddPropItem("end_oddment",PROPLIST_ITEM(id++,"End Odd Length"));
	CLDSArcFlat::AddPropItem("specification",PROPLIST_ITEM(id++,"Specification Type"));
	CLDSArcFlat::AddPropItem("wide",PROPLIST_ITEM(id++,"Width"));
	CLDSArcFlat::AddPropItem("thick",PROPLIST_ITEM(id++,"Thickness"));
	CLDSArcFlat::AddPropItem("m_bUSerSpecColor",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	CLDSArcFlat::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
	CLDSArcFlat::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"Assembling Position"));
	CLDSArcFlat::AddPropItem("center",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("center.x",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("center.y",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("center.z",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("workNorm",PROPLIST_ITEM(id++,"Work Normal"));
	CLDSArcFlat::AddPropItem("workNorm.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcFlat::AddPropItem("workNorm.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcFlat::AddPropItem("workNorm.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSArcFlat::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"Position"));
	CLDSArcFlat::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSArcFlat::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSArcFlat::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSArcFlat::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X Axis Direction"));
	CLDSArcFlat::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcFlat::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcFlat::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSArcFlat::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y Axis Direction"));
	CLDSArcFlat::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcFlat::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcFlat::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSArcFlat::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z Axis Direction"));
	CLDSArcFlat::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcFlat::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcFlat::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSArcFlat::AddPropItem("startInfo",PROPLIST_ITEM(id++,"startInfo"));
	CLDSArcFlat::AddPropItem("hStart",PROPLIST_ITEM(id++,"start node handle"));
	CLDSArcFlat::AddPropItem("oddStart",PROPLIST_ITEM(id++,"odd start"));
	CLDSArcFlat::AddPropItem("startPos",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcFlat::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcFlat::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSArcFlat::AddPropItem("endInfo",PROPLIST_ITEM(id++,"endInfo"));
	CLDSArcFlat::AddPropItem("hEnd",PROPLIST_ITEM(id++,"end node handle"));
	CLDSArcFlat::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"oddEnd"));
	CLDSArcFlat::AddPropItem("endPos",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcFlat::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcFlat::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z Component"));
#else 
	CLDSArcFlat::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"���α�ֻ�����Ϣ"));
	CLDSArcFlat::AddPropItem("geometryInfo",PROPLIST_ITEM(id++,"��λ��Ϣ"));
	CLDSArcFlat::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSArcFlat::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSArcFlat::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	CLDSArcFlat::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�",NULL,"Ĭ��|A|B|C|D|"));
	CLDSArcFlat::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSArcFlat::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	CLDSArcFlat::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSArcFlat::AddPropItem("m_fZoomLenCoef",PROPLIST_ITEM(id++,"����ϵ��"));
	CLDSArcFlat::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	CLDSArcFlat::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����"));
	CLDSArcFlat::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	CLDSArcFlat::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"���⸨������"));
	CLDSArcFlat::AddPropItem("m_uDisplaySlices",PROPLIST_ITEM(id++,"(��ע��)��ʾƬ��"));
	CLDSArcFlat::AddPropItem("neutro_radius",PROPLIST_ITEM(id++,"���Բ�Բ���뾶"));
	CLDSArcFlat::AddPropItem("arc_radius",PROPLIST_ITEM(id++,"Բ���뾶"));
	CLDSArcFlat::AddPropItem("offset_h",PROPLIST_ITEM(id++,"����λ��"));
	CLDSArcFlat::AddPropItem("offset_v",PROPLIST_ITEM(id++,"����λ��"));
	CLDSArcFlat::AddPropItem("arc_start",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("arc_start.x",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("arc_start.y",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("arc_start.z",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("start_rot_angle",PROPLIST_ITEM(id++,"��ʼƫת��"));
	CLDSArcFlat::AddPropItem("sector_angle",PROPLIST_ITEM(id++,"Բ�����ν�"));
	CLDSArcFlat::AddPropItem("des_arc_center",PROPLIST_ITEM(id++,"Բ��λ��"));
	CLDSArcFlat::AddPropItem("des_arc_start",PROPLIST_ITEM(id++,"Բ�����"));
	CLDSArcFlat::AddPropItem("des_arc_norm",PROPLIST_ITEM(id++,"�����淨��"));
	CLDSArcFlat::AddPropItem("m_siLayoutStyle",PROPLIST_ITEM(id++,"�ڷŷ�ʽ"));
	CLDSArcFlat::AddPropItem("start_oddment",PROPLIST_ITEM(id++,"ʼ������ͷ"));
	CLDSArcFlat::AddPropItem("end_oddment",PROPLIST_ITEM(id++,"�ն�����ͷ"));
	CLDSArcFlat::AddPropItem("specification",PROPLIST_ITEM(id++,"����ͺ�"));
	CLDSArcFlat::AddPropItem("wide",PROPLIST_ITEM(id++,"���"));
	CLDSArcFlat::AddPropItem("thick",PROPLIST_ITEM(id++,"���"));
	CLDSArcFlat::AddPropItem("m_bUSerSpecColor",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	CLDSArcFlat::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
	CLDSArcFlat::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"װ�䷽λ"));
	CLDSArcFlat::AddPropItem("center",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("center.x",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("center.y",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("center.z",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("workNorm",PROPLIST_ITEM(id++,"��������"));
	CLDSArcFlat::AddPropItem("workNorm.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSArcFlat::AddPropItem("workNorm.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSArcFlat::AddPropItem("workNorm.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSArcFlat::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"λ��"));
	CLDSArcFlat::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X����"));
	CLDSArcFlat::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSArcFlat::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSArcFlat::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X�᷽��"));
	CLDSArcFlat::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSArcFlat::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSArcFlat::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSArcFlat::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y�᷽��"));
	CLDSArcFlat::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSArcFlat::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSArcFlat::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSArcFlat::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z�᷽��"));
	CLDSArcFlat::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSArcFlat::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSArcFlat::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z�������"));
	//ʼ�ն�
	CLDSArcFlat::AddPropItem("startInfo",PROPLIST_ITEM(id++,"ʼ����Ϣ"));
	CLDSArcFlat::AddPropItem("hStart",PROPLIST_ITEM(id++,"�����"));
	CLDSArcFlat::AddPropItem("oddStart",PROPLIST_ITEM(id++,"����ͷ"));
	CLDSArcFlat::AddPropItem("startPos",PROPLIST_ITEM(id++,"ʼ���������"));
	CLDSArcFlat::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"ʼ��ʵ������"));
	CLDSArcFlat::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X����"));
	CLDSArcFlat::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSArcFlat::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSArcFlat::AddPropItem("endInfo",PROPLIST_ITEM(id++,"�ն���Ϣ"));
	CLDSArcFlat::AddPropItem("hEnd",PROPLIST_ITEM(id++,"�յ���"));
	CLDSArcFlat::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"����ͷ"));
	CLDSArcFlat::AddPropItem("endPos",PROPLIST_ITEM(id++,"�ն��������"));
	CLDSArcFlat::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"�ն�ʵ������"));
	CLDSArcFlat::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X����"));
	CLDSArcFlat::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSArcFlat::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z����"));
#endif
}

CHashStrList<PROPLIST_ITEM> CLDSArcSlot::propHashtable;
CXhChar100 CLDSArcSlot::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*>CLDSArcSlot::hashPropItemById;
CHashTable<DWORD> CLDSArcSlot::propStatusHashtable;
int CLDSArcSlot::m_iCurDisplayPropGroup;
void CLDSArcSlot::InitArcSlotPropHashtable()
{
	int id = 1;
	CLDSArcSlot::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSArcSlot::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSArcSlot::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Circular [ steel Basic Information"));
	CLDSArcSlot::AddPropItem("geometryInfo",PROPLIST_ITEM(id++,"Geometry Information"));
	CLDSArcSlot::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSArcSlot::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSArcSlot::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	CLDSArcSlot::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade",NULL,"Default|A|B|C|D|"));
	CLDSArcSlot::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSArcSlot::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Serial Number"));
	CLDSArcSlot::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSArcSlot::AddPropItem("m_fZoomLenCoef",PROPLIST_ITEM(id++,"length cutting coef"));
	CLDSArcSlot::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSArcSlot::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"));
	CLDSArcSlot::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	CLDSArcSlot::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Ancillary Part"));
	CLDSArcSlot::AddPropItem("m_uDisplaySlices",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Display Slices"));
	CLDSArcSlot::AddPropItem("neutro_radius",PROPLIST_ITEM(id++,"Neutrosphere Arc Radius "));
	CLDSArcSlot::AddPropItem("arc_radius",PROPLIST_ITEM(id++,"Arc Radius"));
	CLDSArcSlot::AddPropItem("offset_h",PROPLIST_ITEM(id++,"Radial Offset"));
	CLDSArcSlot::AddPropItem("offset_v",PROPLIST_ITEM(id++,"Normal Offset"));
	CLDSArcSlot::AddPropItem("arc_start",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("arc_start.x",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("arc_start.y",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("arc_start.z",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("start_rot_angle",PROPLIST_ITEM(id++,"Start Offset Rotate Angle"));
	CLDSArcSlot::AddPropItem("sector_angle",PROPLIST_ITEM(id++,"Arc Sector Angle"));
	CLDSArcSlot::AddPropItem("des_arc_center",PROPLIST_ITEM(id++,"Center Position"));
	CLDSArcSlot::AddPropItem("des_arc_start",PROPLIST_ITEM(id++,"Arc Start Point"));
	CLDSArcSlot::AddPropItem("des_arc_norm",PROPLIST_ITEM(id++,"Work Plane Normal"));
	CLDSArcSlot::AddPropItem("m_siLayoutStyle",PROPLIST_ITEM(id++,"Layout Style"));
	CLDSArcSlot::AddPropItem("start_oddment",PROPLIST_ITEM(id++,"Start Odd Length"));
	CLDSArcSlot::AddPropItem("end_oddment",PROPLIST_ITEM(id++,"End Odd Length"));
	CLDSArcSlot::AddPropItem("specification",PROPLIST_ITEM(id++,"Specification Type"));
	CLDSArcSlot::AddPropItem("height",PROPLIST_ITEM(id++,"Section Height"));
	CLDSArcSlot::AddPropItem("wide",PROPLIST_ITEM(id++,"Leg Width"));
	CLDSArcSlot::AddPropItem("thick",PROPLIST_ITEM(id++,"Leg Thickness"));
	CLDSArcSlot::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color"));
	CLDSArcSlot::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	CLDSArcSlot::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
	CLDSArcSlot::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"Assembling Position"));
	CLDSArcSlot::AddPropItem("center",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("center.x",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("center.y",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("center.z",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("workNorm",PROPLIST_ITEM(id++,"Work Normal"));
	CLDSArcSlot::AddPropItem("workNorm.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcSlot::AddPropItem("workNorm.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcSlot::AddPropItem("workNorm.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSArcSlot::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"Position"));
	CLDSArcSlot::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSArcSlot::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSArcSlot::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSArcSlot::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X Axis Direction"));
	CLDSArcSlot::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcSlot::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcSlot::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSArcSlot::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y Axis Direction"));
	CLDSArcSlot::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcSlot::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcSlot::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSArcSlot::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z Axis Direction"));
	CLDSArcSlot::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcSlot::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcSlot::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z Component"));
	//
	CLDSArcSlot::AddPropItem("startInfo",PROPLIST_ITEM(id++,"startInfo"));
	CLDSArcSlot::AddPropItem("hStart",PROPLIST_ITEM(id++,"start node handle"));
	CLDSArcSlot::AddPropItem("oddStart",PROPLIST_ITEM(id++,"odd start"));
	CLDSArcSlot::AddPropItem("startPos",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcSlot::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcSlot::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSArcSlot::AddPropItem("endInfo",PROPLIST_ITEM(id++,"endInfo"));
	CLDSArcSlot::AddPropItem("hEnd",PROPLIST_ITEM(id++,"end node handle"));
	CLDSArcSlot::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"oddEnd"));
	CLDSArcSlot::AddPropItem("endPos",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSArcSlot::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSArcSlot::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z Component"));
#else 
	CLDSArcSlot::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"���β۸ֻ�����Ϣ"));
	CLDSArcSlot::AddPropItem("geometryInfo",PROPLIST_ITEM(id++,"��λ��Ϣ"));
	CLDSArcSlot::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSArcSlot::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSArcSlot::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	CLDSArcSlot::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�",NULL,"Ĭ��|A|B|C|D|"));
	CLDSArcSlot::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSArcSlot::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	CLDSArcSlot::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSArcSlot::AddPropItem("m_fZoomLenCoef",PROPLIST_ITEM(id++,"����ϵ��"));
	CLDSArcSlot::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	CLDSArcSlot::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����"));
	CLDSArcSlot::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	CLDSArcSlot::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"���⸨������"));
	CLDSArcSlot::AddPropItem("m_uDisplaySlices",PROPLIST_ITEM(id++,"(��ע��)��ʾƬ��"));
	CLDSArcSlot::AddPropItem("neutro_radius",PROPLIST_ITEM(id++,"���Բ�Բ���뾶"));
	CLDSArcSlot::AddPropItem("arc_radius",PROPLIST_ITEM(id++,"Բ���뾶"));
	CLDSArcSlot::AddPropItem("offset_h",PROPLIST_ITEM(id++,"����λ��"));
	CLDSArcSlot::AddPropItem("offset_v",PROPLIST_ITEM(id++,"����λ��"));
	CLDSArcSlot::AddPropItem("arc_start",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("arc_start.x",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("arc_start.y",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("arc_start.z",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("start_rot_angle",PROPLIST_ITEM(id++,"��ʼƫת��"));
	CLDSArcSlot::AddPropItem("sector_angle",PROPLIST_ITEM(id++,"Բ�����ν�"));
	CLDSArcSlot::AddPropItem("des_arc_center",PROPLIST_ITEM(id++,"Բ��λ��"));
	CLDSArcSlot::AddPropItem("des_arc_start",PROPLIST_ITEM(id++,"Բ�����"));
	CLDSArcSlot::AddPropItem("des_arc_norm",PROPLIST_ITEM(id++,"�����淨��"));
	CLDSArcSlot::AddPropItem("m_siLayoutStyle",PROPLIST_ITEM(id++,"�ڷŷ�ʽ"));
	CLDSArcSlot::AddPropItem("start_oddment",PROPLIST_ITEM(id++,"ʼ������ͷ"));
	CLDSArcSlot::AddPropItem("end_oddment",PROPLIST_ITEM(id++,"�ն�����ͷ"));
	CLDSArcSlot::AddPropItem("specification",PROPLIST_ITEM(id++,"����ͺ�"));
	CLDSArcSlot::AddPropItem("height",PROPLIST_ITEM(id++,"�����"));
	CLDSArcSlot::AddPropItem("wide",PROPLIST_ITEM(id++,"֫��"));
	CLDSArcSlot::AddPropItem("thick",PROPLIST_ITEM(id++,"֫��"));
	CLDSArcSlot::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ"));
	CLDSArcSlot::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	CLDSArcSlot::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
	CLDSArcSlot::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"װ�䷽λ"));
	CLDSArcSlot::AddPropItem("center",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("center.x",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("center.y",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("center.z",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("workNorm",PROPLIST_ITEM(id++,"��������"));
	CLDSArcSlot::AddPropItem("workNorm.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSArcSlot::AddPropItem("workNorm.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSArcSlot::AddPropItem("workNorm.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSArcSlot::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"λ��"));
	CLDSArcSlot::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X����"));
	CLDSArcSlot::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSArcSlot::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSArcSlot::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X�᷽��"));
	CLDSArcSlot::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSArcSlot::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSArcSlot::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSArcSlot::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y�᷽��"));
	CLDSArcSlot::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSArcSlot::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSArcSlot::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSArcSlot::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z�᷽��"));
	CLDSArcSlot::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSArcSlot::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSArcSlot::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z�������"));
	//
	CLDSArcSlot::AddPropItem("startInfo",PROPLIST_ITEM(id++,"ʼ����Ϣ"));
	CLDSArcSlot::AddPropItem("hStart",PROPLIST_ITEM(id++,"�����"));
	CLDSArcSlot::AddPropItem("oddStart",PROPLIST_ITEM(id++,"����ͷ"));
	CLDSArcSlot::AddPropItem("startPos",PROPLIST_ITEM(id++,"ʼ���������"));
	CLDSArcSlot::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"ʼ��ʵ������"));
	CLDSArcSlot::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X����"));
	CLDSArcSlot::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSArcSlot::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSArcSlot::AddPropItem("endInfo",PROPLIST_ITEM(id++,"�ն���Ϣ"));
	CLDSArcSlot::AddPropItem("hEnd",PROPLIST_ITEM(id++,"�յ���"));
	CLDSArcSlot::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"����ͷ"));
	CLDSArcSlot::AddPropItem("endPos",PROPLIST_ITEM(id++,"�ն��������"));
	CLDSArcSlot::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"�ն�ʵ������"));
	CLDSArcSlot::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X����"));
	CLDSArcSlot::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSArcSlot::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z����"));
#endif
}

MEMORY_BLOCK CLDSLineTube::MPCL;	//Mirror Property Control List
PROPLIST_TYPE CLDSLineTube::propMirSync;
void CLDSLineTube::InitPropMirBitsMap()
{
	int id=1;
	propMirSync.InitHashtableSize(32);
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	//propMirSync.AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("specification",PROPLIST_ITEM(id++,"���"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	propMirSync.AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("Position",PROPLIST_ITEM(id++,"�˵�λ��"));
	propMirSync.AddPropItem("oddStart",PROPLIST_ITEM(id++,"ʼ������ͷ"));
	propMirSync.AddPropItem("oddEnd",PROPLIST_ITEM(id++,"�ն�����ͷ"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("TransectFace",PROPLIST_ITEM(id++,"�����Ϣ"));
}

IMPLEMENT_PROP_FUNC(CLDSLineTube);
void CLDSLineTube::InitLineTubePropHashtable()
{
	return InitPropHashtable();
}
void CLDSLineTube::InitPropHashtable()
{
	int id=1;
	CLDSLineTube::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSLineTube::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSLineTube::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Tube Basic Information"));
	CLDSLineTube::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSLineTube::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSLineTube::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	CLDSLineTube::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade",NULL,"Default|A|B|C|D|"));
	CLDSLineTube::AddPropItem("specification",PROPLIST_ITEM(id++,"Specification"));
	CLDSLineTube::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSLineTube::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Serial Number"));
	CLDSLineTube::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSLineTube::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"Connect Part"));
	CLDSLineTube::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//Ӱ����Ϣ
	CLDSLineTube::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Project Mother Part"));//Ӱ��ĸ����
	CLDSLineTube::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//�����Ĳ�������
	CLDSLineTube::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//�����Ĳ���
	CLDSLineTube::AddPropItem("m_bLiveNeedSupplyPart",PROPLIST_ITEM(id++,"Live cutting"));
	CLDSLineTube::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSLineTube::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"));
	CLDSLineTube::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	CLDSLineTube::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Ancillary Part"));
	CLDSLineTube::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"Pre-Lift"));
	CLDSLineTube::AddPropItem("length",PROPLIST_ITEM(id++,"Physical Length"));
	CLDSLineTube::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color"));
	CLDSLineTube::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	CLDSLineTube::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
	CLDSLineTube::AddPropItem("weldRoadAngle",PROPLIST_ITEM(id++,"Weld Road Angle"));
	CLDSLineTube::AddPropItem("m_iWeldRoadStyle",PROPLIST_ITEM(id++,"Weld Road Line Specified Style"));
	CLDSLineTube::AddPropItem("nearWeldRoadVec",PROPLIST_ITEM(id++,"Weld Direction"));
	CLDSLineTube::AddPropItem("nearWeldRoadVec.x",PROPLIST_ITEM(id++,"Weld Direction X Component"));
	CLDSLineTube::AddPropItem("nearWeldRoadVec.y",PROPLIST_ITEM(id++,"Weld Direction Y Component"));
	CLDSLineTube::AddPropItem("nearWeldRoadVec.z",PROPLIST_ITEM(id++,"Weld Direction Z Component"));
	CLDSLineTube::AddPropItem("workPlaneNorm",PROPLIST_ITEM(id++,""));
	CLDSLineTube::AddPropItem("hWorkNormRefPart",PROPLIST_ITEM(id++,"Normal Reference Rod"));
	CLDSLineTube::AddPropItem("m_iNormCalStyle",PROPLIST_ITEM(id++,"Compute Mode","Directly Specify Or By Parent Rod Specify"));
	CLDSLineTube::AddPropItem("workPlaneNorm.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSLineTube::AddPropItem("workPlaneNorm.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSLineTube::AddPropItem("workPlaneNorm.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSLineTube::AddPropItem("pickWorkPlane",PROPLIST_ITEM(id++,"Datum Plane Near Normal","Tube's near normal generally specified by the user,the system can calculate datum normal according to near normals and tube's axis"));
	CLDSLineTube::AddPropItem("pickWorkPlane.x",PROPLIST_ITEM(id++,"X Component","Tube Normal X Component"));
	CLDSLineTube::AddPropItem("pickWorkPlane.y",PROPLIST_ITEM(id++,"Y Component","Tube Normal Y Component"));
	CLDSLineTube::AddPropItem("pickWorkPlane.z",PROPLIST_ITEM(id++,"Z Component","Tube Normal Z Component"));
	CLDSLineTube::AddPropItem("cutPlane",PROPLIST_ITEM(id++,"Cutting Plane"));
	CLDSLineTube::AddPropItem("cutPlaneNorm",PROPLIST_ITEM(id++," Normal"));
	CLDSLineTube::AddPropItem("cutPlaneNorm.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSLineTube::AddPropItem("cutPlaneNorm.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSLineTube::AddPropItem("cutPlaneNorm.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSLineTube::AddPropItem("cutPlaneOrigin",PROPLIST_ITEM(id++,"Datum Point"));
	CLDSLineTube::AddPropItem("cutPlaneOrigin.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSLineTube::AddPropItem("cutPlaneOrigin.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSLineTube::AddPropItem("cutPlaneOrigin.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSLineTube::AddPropItem("startInfo",PROPLIST_ITEM(id++,"Start Information"));
	CLDSLineTube::AddPropItem("hStart",PROPLIST_ITEM(id++,"Start Point Handle"));
	CLDSLineTube::AddPropItem("oddStart",PROPLIST_ITEM(id++,"Odd Length"));
	CLDSLineTube::AddPropItem("jointStart",PROPLIST_ITEM(id++,"Start Connect"));
	CLDSLineTube::AddPropItem("startPos",PROPLIST_ITEM(id++,"Start Design Coordinate"));
	CLDSLineTube::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"Start Physical Coordinate"));
	CLDSLineTube::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSLineTube::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSLineTube::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSLineTube::AddPropItem("desStartPos",PROPLIST_ITEM(id++,""));
	CLDSLineTube::AddPropItem("desStartPos.endPosType",PROPLIST_ITEM(id++,"Locate Mode"));
	CLDSLineTube::AddPropItem("desStartPos.datum_pos_style",PROPLIST_ITEM(id++,"Datum Point Locate Mode"));
	CLDSLineTube::AddPropItem("desStartPos.hDatumPart",PROPLIST_ITEM(id++,"Datum Rod"));
	CLDSLineTube::AddPropItem("desStartPos.fNormOffset",PROPLIST_ITEM(id++,"Datum Plane Normal Offset"));
	CLDSLineTube::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"Eccentric Distance"));
	CLDSLineTube::AddPropItem("desStartPos.fLenVecOffsetDist",PROPLIST_ITEM(id++,"Datum Point Stretch Direction Offset"));
	CLDSLineTube::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"Offset Plane Normal"));
	CLDSLineTube::AddPropItem("desStartPos.hDatumStartPart",PROPLIST_ITEM(id++,"Project Line Start Datum Tube"));
	CLDSLineTube::AddPropItem("desStartPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"Datum Tube Datum EndPoint"));
	CLDSLineTube::AddPropItem("desStartPos.hDatumEndPart",PROPLIST_ITEM(id++,"Project Line End Datum Tube"));
	CLDSLineTube::AddPropItem("desStartPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"Datum Tube Datum EndPoint"));
	CLDSLineTube::AddPropItem("endInfo",PROPLIST_ITEM(id++,"End Information"));
	CLDSLineTube::AddPropItem("hEnd",PROPLIST_ITEM(id++,"End Point Handle"));
	CLDSLineTube::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"Odd Length"));
	CLDSLineTube::AddPropItem("jointEnd",PROPLIST_ITEM(id++,"End Connect"));
	CLDSLineTube::AddPropItem("endPos",PROPLIST_ITEM(id++,"End Design Coordinate"));
	CLDSLineTube::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"End Physical Coordinate"));
	CLDSLineTube::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSLineTube::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSLineTube::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSLineTube::AddPropItem("desEndPos",PROPLIST_ITEM(id++,""));
	CLDSLineTube::AddPropItem("desEndPos.endPosType",PROPLIST_ITEM(id++,"Locate Mode"));
	CLDSLineTube::AddPropItem("desEndPos.datum_pos_style",PROPLIST_ITEM(id++,"Datum Point Locate Mode"));
	CLDSLineTube::AddPropItem("desEndPos.hDatumPart",PROPLIST_ITEM(id++,"Datum Rod"));
	CLDSLineTube::AddPropItem("desEndPos.fNormOffset",PROPLIST_ITEM(id++,"Datum Plane Normal Offset"));
	CLDSLineTube::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,"Eccentric Distance"));
	CLDSLineTube::AddPropItem("desEndPos.fLenVecOffsetDist",PROPLIST_ITEM(id++,"Datum Point Stretch Direction Offset"));
	CLDSLineTube::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,"Offset Plane Normal"));
	CLDSLineTube::AddPropItem("desEndPos.hDatumStartPart",PROPLIST_ITEM(id++,"Project Line Start Datum Tube"));
	CLDSLineTube::AddPropItem("desEndPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"Datum Tube Datum EndPoint"));
	CLDSLineTube::AddPropItem("desEndPos.hDatumEndPart",PROPLIST_ITEM(id++,"Project Line End Datum Tube"));
	CLDSLineTube::AddPropItem("desEndPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"Datum Tube Datum EndPoint"));
	CLDSLineTube::AddPropItem("m_bHasWeldRoad",PROPLIST_ITEM(id++,"Exist Weld Road Line"));
	CLDSLineTube::AddPropItem("startWorkPlane",PROPLIST_ITEM(id++,"Start Tube Work Plane"));
	CLDSLineTube::AddPropItem("endWorkPlane",PROPLIST_ITEM(id++,"End Tube Work Plane"));
	//������������Ҫ������
	CLDSLineTube::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"Statistic Material Serial Number"));	//ͳ�ı��
	CLDSLineTube::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"Gravity Zoom Out Coefficient"));
	CLDSLineTube::AddPropItem("size.idClassType",PROPLIST_ITEM(id++,"Rod Type"));
	//�����Ϣ
	CLDSLineTube::AddPropItem("designInfo",PROPLIST_ITEM(id++,"Rod Check Information"));
	CLDSLineTube::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"Elem Type"));
	CLDSLineTube::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"Whether Material Optimization"));
	CLDSLineTube::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"Whether Specification Optimization"));
	CLDSLineTube::AddPropItem("hole_out",PROPLIST_ITEM(id++,"Bolt Hole Out"));
	CLDSLineTube::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"Calculate Automatic Bolt Hole Out"));
		//���㳤����Ϣ
	CLDSLineTube::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,"Reduce Calculate Coefficient"));
	CLDSLineTube::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineTube::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,"Calculate Length Type"));
	CLDSLineTube::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,"Reference Horizon Pole"));
	CLDSLineTube::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,"Simultaneously Compression Reference Rod"));
	CLDSLineTube::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,"Minimum Axial Length"));
	CLDSLineTube::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,"Parallel Axial Length"));
		//ʼ�������Ϣ
	CLDSLineTube::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"Start Information"));
	CLDSLineTube::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"Start Joint Type"));
	CLDSLineTube::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"Start Force Type"));
	CLDSLineTube::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
		//�ն������Ϣ
	CLDSLineTube::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"End Information"));
	CLDSLineTube::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"End Joint Type"));
	CLDSLineTube::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"End Force Type"));
	CLDSLineTube::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,""));
#else 
	CLDSLineTube::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"�ֹܻ�����Ϣ"));
	CLDSLineTube::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSLineTube::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSLineTube::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	CLDSLineTube::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�",NULL,"Ĭ��|A|B|C|D|"));
	CLDSLineTube::AddPropItem("specification",PROPLIST_ITEM(id++,"���"));
	CLDSLineTube::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSLineTube::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	CLDSLineTube::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSLineTube::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"��������"));
	CLDSLineTube::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Ӱ����Ϣ"));//Ӱ����Ϣ
	CLDSLineTube::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Ӱ��ĸ����"));//Ӱ��ĸ����
	CLDSLineTube::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"������������"));//�����Ĳ�������
	CLDSLineTube::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"��������"));//�����Ĳ���
	CLDSLineTube::AddPropItem("m_bLiveNeedSupplyPart",PROPLIST_ITEM(id++,"�ֳ�����"));
	CLDSLineTube::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	CLDSLineTube::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����","����˹�����Ҫ���ӵ�����������,��ô�����Ծ��Ǳ������������ĺ��ӹ����ľ��;����˹�������Ҫ����,��ֵΪ0X0��"));
	CLDSLineTube::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��","�Ƿ���ʾ��","��|��"));
	CLDSLineTube::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"���⸨������","���⸨������ֻ���ڸ�������������λ","��|��"));
	CLDSLineTube::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"Ԥ����"));
	CLDSLineTube::AddPropItem("length",PROPLIST_ITEM(id++,"ʵ�ʳ���"));
	CLDSLineTube::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ","������ɫ�Ƿ�Ϊ�û�ָ��","��|��"));
	CLDSLineTube::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	CLDSLineTube::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
	CLDSLineTube::AddPropItem("weldRoadAngle",PROPLIST_ITEM(id++,"�����н�"));
	CLDSLineTube::AddPropItem("m_iWeldRoadStyle",PROPLIST_ITEM(id++,"������ָ����ʽ", "ֱ�Ӹ�������ͨ�����׼�߼нǶ�λ", "0.ֱ��ָ��|1.�Ƕȶ�λ"));
	CLDSLineTube::AddPropItem("nearWeldRoadVec",PROPLIST_ITEM(id++,"��������"));
	CLDSLineTube::AddPropItem("nearWeldRoadVec.x",PROPLIST_ITEM(id++,"��������X����"));
	CLDSLineTube::AddPropItem("nearWeldRoadVec.y",PROPLIST_ITEM(id++,"��������Y����"));
	CLDSLineTube::AddPropItem("nearWeldRoadVec.z",PROPLIST_ITEM(id++,"��������Z����"));
	CLDSLineTube::AddPropItem("workPlaneNorm",PROPLIST_ITEM(id++,"��׼�淨��"));
	CLDSLineTube::AddPropItem("hWorkNormRefPart",PROPLIST_ITEM(id++,"���߲��ո˼�"));
	CLDSLineTube::AddPropItem("m_iNormCalStyle",PROPLIST_ITEM(id++,"���㷽ʽ","ֱ�Ӹ�������ͨ�����˼�����", "0.ֱ��ָ��|1.���ո˼�|2.������յ�"));
	CLDSLineTube::AddPropItem("workPlaneNorm.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSLineTube::AddPropItem("workPlaneNorm.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSLineTube::AddPropItem("workPlaneNorm.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSLineTube::AddPropItem("pickWorkPlane",PROPLIST_ITEM(id++,"��׼����Ʒ���","�ֹܻ�׼����Ʒ���һ�����û�ָ����ϵͳ�ɸ��ݽ��Ʒ��߼��ֹ����߾�ȷ������ֹܵĻ�׼�淨�ߡ�"));
	CLDSLineTube::AddPropItem("pickWorkPlane.x",PROPLIST_ITEM(id++,"X�������","�ֹܻ�׼�淨��X�᷽�������"));
	CLDSLineTube::AddPropItem("pickWorkPlane.y",PROPLIST_ITEM(id++,"Y�������","�ֹܻ�׼�淨��Y�᷽�������"));
	CLDSLineTube::AddPropItem("pickWorkPlane.z",PROPLIST_ITEM(id++,"Z�������","�ֹܻ�׼�淨��Z�᷽�������"));
	CLDSLineTube::AddPropItem("cutPlane",PROPLIST_ITEM(id++,"�и���"));
	CLDSLineTube::AddPropItem("cutPlaneNorm",PROPLIST_ITEM(id++,"����"));
	CLDSLineTube::AddPropItem("cutPlaneNorm.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSLineTube::AddPropItem("cutPlaneNorm.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSLineTube::AddPropItem("cutPlaneNorm.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSLineTube::AddPropItem("cutPlaneOrigin",PROPLIST_ITEM(id++,"����"));
	CLDSLineTube::AddPropItem("cutPlaneOrigin.x",PROPLIST_ITEM(id++,"X����"));
	CLDSLineTube::AddPropItem("cutPlaneOrigin.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSLineTube::AddPropItem("cutPlaneOrigin.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSLineTube::AddPropItem("startInfo",PROPLIST_ITEM(id++,"ʼ����Ϣ"));
	CLDSLineTube::AddPropItem("hStart",PROPLIST_ITEM(id++,"�����"));
	CLDSLineTube::AddPropItem("oddStart",PROPLIST_ITEM(id++,"����ͷ"));
	CLDSLineTube::AddPropItem("oddStart.m_iOddCalStyle", PROPLIST_ITEM(id++, "����ԭ��","","0.��ײ����|1.�����|2.ָ����ֵ"));
	CLDSLineTube::AddPropItem("oddStart.m_fCollideDist", PROPLIST_ITEM(id++, "��ײ��϶"));
	CLDSLineTube::AddPropItem("oddStart.m_hRefPart1", PROPLIST_ITEM(id++, "���չ���1"));
	CLDSLineTube::AddPropItem("oddStart.m_hRefPart2", PROPLIST_ITEM(id++, "���չ���2"));
	CLDSLineTube::AddPropItem("jointStart",PROPLIST_ITEM(id++,"ʼ������"));
	CLDSLineTube::AddPropItem("startPos",PROPLIST_ITEM(id++,"ʼ���������"));
	CLDSLineTube::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"ʼ��ʵ������"));
	CLDSLineTube::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X����"));
	CLDSLineTube::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSLineTube::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSLineTube::AddPropItem("desStartPos",PROPLIST_ITEM(id++,""));
	CLDSLineTube::AddPropItem("desStartPos.endPosType",PROPLIST_ITEM(id++,"��λ��ʽ","","�ڵ㼴����|�������|����ͶӰ|���߽���|ָ������"));
	CLDSLineTube::AddPropItem("desStartPos.datum_pos_style",PROPLIST_ITEM(id++,"���㶨λ��ʽ"));
	CLDSLineTube::AddPropItem("desStartPos.hDatumPart",PROPLIST_ITEM(id++,"��׼�˼�"));
	CLDSLineTube::AddPropItem("desStartPos.fNormOffset",PROPLIST_ITEM(id++,"��׼�淨��ƫ����"));
	CLDSLineTube::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"ƫ�ľ�"));
	CLDSLineTube::AddPropItem("desStartPos.fLenVecOffsetDist",PROPLIST_ITEM(id++,"�������췽��ƫ����"));
	CLDSLineTube::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"ƽ���淨��"));
	CLDSLineTube::AddPropItem("desStartPos.hDatumStartPart",PROPLIST_ITEM(id++,"ͶӰ��ʼ�����ڻ�׼�ֹ�"));
	CLDSLineTube::AddPropItem("desStartPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"��׼�ֹܵĻ�׼�˵�","","ʼ��|�ն�"));
	CLDSLineTube::AddPropItem("desStartPos.hDatumEndPart",PROPLIST_ITEM(id++,"ͶӰ���ն����ڻ�׼�ֹ�"));
	CLDSLineTube::AddPropItem("desStartPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"��׼�ֹܵĻ�׼�˵�", "", "ʼ��|�ն�"));
	CLDSLineTube::AddPropItem("endInfo",PROPLIST_ITEM(id++,"�ն���Ϣ"));
	CLDSLineTube::AddPropItem("hEnd",PROPLIST_ITEM(id++,"�յ���"));
	CLDSLineTube::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"����ͷ"));
	CLDSLineTube::AddPropItem("oddEnd.m_iOddCalStyle", PROPLIST_ITEM(id++, "����ԭ��", "", "0.��ײ����|1.�����|2.ָ����ֵ"));
	CLDSLineTube::AddPropItem("oddEnd.m_fCollideDist", PROPLIST_ITEM(id++, "��ײ��϶"));
	CLDSLineTube::AddPropItem("oddEnd.m_hRefPart1", PROPLIST_ITEM(id++, "���չ���1"));
	CLDSLineTube::AddPropItem("oddEnd.m_hRefPart2", PROPLIST_ITEM(id++, "���չ���2"));
	CLDSLineTube::AddPropItem("jointEnd",PROPLIST_ITEM(id++,"�ն�����"));
	CLDSLineTube::AddPropItem("endPos",PROPLIST_ITEM(id++,"�ն��������"));
	CLDSLineTube::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"�ն�ʵ������"));
	CLDSLineTube::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X����"));
	CLDSLineTube::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSLineTube::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSLineTube::AddPropItem("desEndPos",PROPLIST_ITEM(id++,""));
	CLDSLineTube::AddPropItem("desEndPos.endPosType",PROPLIST_ITEM(id++,"��λ��ʽ","�ֹ��ն˻���λ��", "�ڵ㼴����|�������|����ͶӰ|���߽���|ָ������"));
	CLDSLineTube::AddPropItem("desEndPos.datum_pos_style",PROPLIST_ITEM(id++,"���㶨λ��ʽ"));
	CLDSLineTube::AddPropItem("desEndPos.hDatumPart",PROPLIST_ITEM(id++,"��׼�˼�"));
	CLDSLineTube::AddPropItem("desEndPos.fNormOffset",PROPLIST_ITEM(id++,"��׼�淨��ƫ����"));
	CLDSLineTube::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,"ƫ�ľ�"));
	CLDSLineTube::AddPropItem("desEndPos.fLenVecOffsetDist",PROPLIST_ITEM(id++,"�������췽��ƫ����"));
	CLDSLineTube::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,"ƽ���淨��"));
	CLDSLineTube::AddPropItem("desEndPos.hDatumStartPart",PROPLIST_ITEM(id++,"ͶӰ��ʼ�����ڻ�׼�ֹ�"));
	CLDSLineTube::AddPropItem("desEndPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"��׼�ֹܵĻ�׼�˵�", "", "ʼ��|�ն�"));
	CLDSLineTube::AddPropItem("desEndPos.hDatumEndPart",PROPLIST_ITEM(id++,"ͶӰ���ն����ڻ�׼�ֹ�"));
	CLDSLineTube::AddPropItem("desEndPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"��׼�ֹܵĻ�׼�˵�", "", "ʼ��|�ն�"));
	CLDSLineTube::AddPropItem("m_bHasWeldRoad",PROPLIST_ITEM(id++,"�к�����","�Ƿ���ں�����,�����޷�ֹ���˵�����ں�����","��|��"));
	CLDSLineTube::AddPropItem("startWorkPlane",PROPLIST_ITEM(id++,"�ֹܹ�����"));
	CLDSLineTube::AddPropItem("endWorkPlane",PROPLIST_ITEM(id++,"�ֹܹ�����"));
	CLDSLineTube::AddPropItem("bushTube",PROPLIST_ITEM(id++,"�׹ܶ�λ����", "�ֹ�Ϊ�׹�ʱ,���������ְ�������λ��˨���ж�λ��"));
	CLDSLineTube::AddPropItem("bush.height",PROPLIST_ITEM(id++,"�׹ܸ߶�"));
	CLDSLineTube::AddPropItem("bush.hBushResidePlate",PROPLIST_ITEM(id++,"�����ְ�"));
	CLDSLineTube::AddPropItem("bush.hBushRelyBolt",PROPLIST_ITEM(id++,"��λ��˨"));
	//������������Ҫ������
	CLDSLineTube::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"ͳ�ı��"));	//ͳ�ı��
	CLDSLineTube::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"�����Ŵ�ϵ��"));
	CLDSLineTube::AddPropItem("size.idClassType",PROPLIST_ITEM(id++,"�˼�����"));
	//�����Ϣ
	CLDSLineTube::AddPropItem("designInfo",PROPLIST_ITEM(id++,"�˼�������Ϣ"));
	CLDSLineTube::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"��Ԫ����"));
	CLDSLineTube::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"�Ƿ���в�����ѡ"));
	CLDSLineTube::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"�Ƿ���й����ѡ"));
	CLDSLineTube::AddPropItem("hole_out",PROPLIST_ITEM(id++,"��˨������"));
	CLDSLineTube::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"�Զ�������˨������"));
		//���㳤����Ϣ
	CLDSLineTube::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,"����ϵ��"));
	CLDSLineTube::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineTube::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,"���㳤������"));
	CLDSLineTube::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,"���պ�����"));
	CLDSLineTube::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,"ͬʱ��ѹ���ո˼�"));
	CLDSLineTube::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,"��С�᳤��"));
	CLDSLineTube::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,"ƽ���᳤��"));
		//ʼ�������Ϣ
	CLDSLineTube::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"ʼ����Ϣ"));
	CLDSLineTube::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"ʼ��Լ������"));
	CLDSLineTube::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"ʼ����������"));
	CLDSLineTube::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
		//�ն������Ϣ
	CLDSLineTube::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"�ն���Ϣ"));
	CLDSLineTube::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"�ն�Լ������"));
	CLDSLineTube::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"�ն���������"));
	CLDSLineTube::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,""));
#endif
	InitPropMirBitsMap();
}

CHashStrList<PROPLIST_ITEM> CLDSLineFlat::propHashtable;
CXhChar100 CLDSLineFlat::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*>CLDSLineFlat::hashPropItemById;
CHashTable<DWORD> CLDSLineFlat::propStatusHashtable;
int CLDSLineFlat::m_iCurDisplayPropGroup;
void CLDSLineFlat::InitLineFlatPropHashtable()
{
	int id=1;
	CLDSLineFlat::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSLineFlat::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSLineFlat::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Flat Steel Basic Information"));
	CLDSLineFlat::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSLineFlat::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSLineFlat::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	CLDSLineFlat::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade",NULL,"Default|A|B|C|D|"));
	CLDSLineFlat::AddPropItem("specification",PROPLIST_ITEM(id++,"Specification"));
	CLDSLineFlat::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSLineFlat::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Serial Number"));
	CLDSLineFlat::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSLineFlat::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"Connect Part"));
	CLDSLineFlat::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//Ӱ����Ϣ
	CLDSLineFlat::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Project Mother Part"));//Ӱ��ĸ����
	CLDSLineFlat::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//�����Ĳ�������
	CLDSLineFlat::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//�����Ĳ���
	CLDSLineFlat::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSLineFlat::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"));
	CLDSLineFlat::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	CLDSLineFlat::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Ancillary Part"));
	CLDSLineFlat::AddPropItem("length",PROPLIST_ITEM(id++,"Physical Length"));
	CLDSLineFlat::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color"));
	CLDSLineFlat::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	CLDSLineFlat::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
	CLDSLineFlat::AddPropItem("workPlaneNorm",PROPLIST_ITEM(id++,"Datum Plane Normal"));
	CLDSLineFlat::AddPropItem("hWorkNormRefPart",PROPLIST_ITEM(id++,"Normal Reference Rod"));
	CLDSLineFlat::AddPropItem("workPlaneNorm.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSLineFlat::AddPropItem("workPlaneNorm.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSLineFlat::AddPropItem("workPlaneNorm.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSLineFlat::AddPropItem("nearWorkPlaneNorm",PROPLIST_ITEM(id++,"Datum Plane Near Normal"));
	CLDSLineFlat::AddPropItem("m_iNormCalStyle",PROPLIST_ITEM(id++,"Compute Mode"));
	CLDSLineFlat::AddPropItem("nearWorkPlaneNorm.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSLineFlat::AddPropItem("nearWorkPlaneNorm.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSLineFlat::AddPropItem("nearWorkPlaneNorm.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSLineFlat::AddPropItem("startInfo",PROPLIST_ITEM(id++,"Start Information"));
	CLDSLineFlat::AddPropItem("hStart",PROPLIST_ITEM(id++,"Start Point Handle"));
	CLDSLineFlat::AddPropItem("oddStart",PROPLIST_ITEM(id++,"Odd Length"));
	CLDSLineFlat::AddPropItem("startPos",PROPLIST_ITEM(id++,"Start Design Coordinate"));
	CLDSLineFlat::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"Start Physical Coordinate"));
	CLDSLineFlat::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSLineFlat::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSLineFlat::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSLineFlat::AddPropItem("desStartPos",PROPLIST_ITEM(id++,""));
	CLDSLineFlat::AddPropItem("desStartPos.endPosType",PROPLIST_ITEM(id++,"Locate Mode"));
	CLDSLineFlat::AddPropItem("desStartPos.datum_pos_style",PROPLIST_ITEM(id++,"Datum Point Locate Mode"));
	CLDSLineFlat::AddPropItem("desStartPos.hDatumPart",PROPLIST_ITEM(id++,"Datum Rod"));
	CLDSLineFlat::AddPropItem("desStartPos.fNormOffset",PROPLIST_ITEM(id++,"Datum Plane Normal Offset"));
	CLDSLineFlat::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"Eccentric Distance"));
	CLDSLineFlat::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"Offset Plane Normal"));
	CLDSLineFlat::AddPropItem("desStartPos.fTransverseOffset",PROPLIST_ITEM(id++,"lateral Offset"));
	CLDSLineFlat::AddPropItem("desStartPos.hDatumStartPart",PROPLIST_ITEM(id++,"Project Line Start Datum Rod"));
	CLDSLineFlat::AddPropItem("desStartPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"Datum Rod Datum EndPoint"));
	CLDSLineFlat::AddPropItem("desStartPos.hDatumEndPart",PROPLIST_ITEM(id++,"Project Line End Datum Rod"));
	CLDSLineFlat::AddPropItem("desStartPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"Datum Rod Datum EndPoint"));
	CLDSLineFlat::AddPropItem("endInfo",PROPLIST_ITEM(id++,"End Information"));
	CLDSLineFlat::AddPropItem("hEnd",PROPLIST_ITEM(id++,"End Point Handle"));
	CLDSLineFlat::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"Odd Length"));
	CLDSLineFlat::AddPropItem("endPos",PROPLIST_ITEM(id++,"End Design Coordinate"));
	CLDSLineFlat::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"End Physical Coordinate"));
	CLDSLineFlat::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSLineFlat::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSLineFlat::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSLineFlat::AddPropItem("desEndPos",PROPLIST_ITEM(id++,""));
	CLDSLineFlat::AddPropItem("desEndPos.endPosType",PROPLIST_ITEM(id++,"Locate Mode"));
	CLDSLineFlat::AddPropItem("desEndPos.datum_pos_style",PROPLIST_ITEM(id++,"Datum Point Locate Mode"));
	CLDSLineFlat::AddPropItem("desEndPos.hDatumPart",PROPLIST_ITEM(id++,"Datum Rod"));
	CLDSLineFlat::AddPropItem("desEndPos.fNormOffset",PROPLIST_ITEM(id++,"Datum Plane Normal Offset"));
	CLDSLineFlat::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,"Eccentric Distance"));
	CLDSLineFlat::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,"Offset Plane Normal"));
	CLDSLineFlat::AddPropItem("desEndPos.fTransverseOffset",PROPLIST_ITEM(id++,"lateral Offset"));
	CLDSLineFlat::AddPropItem("desEndPos.hDatumStartPart",PROPLIST_ITEM(id++,"Project Line Start Datum Rod"));
	CLDSLineFlat::AddPropItem("desEndPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"Datum Rod EndPoint"));
	CLDSLineFlat::AddPropItem("desEndPos.hDatumEndPart",PROPLIST_ITEM(id++,"Project Line End Rod"));
	CLDSLineFlat::AddPropItem("desEndPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"Datum Rod EndPoint"));
	//������������Ҫ������
	CLDSLineFlat::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"Statistic Material Serial Number"));	//ͳ�ı��
	CLDSLineFlat::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"Gravity Zoom Out Coefficient"));
	//�����Ϣ
	CLDSLineFlat::AddPropItem("designInfo",PROPLIST_ITEM(id++,"Rod Check Information"));
	CLDSLineFlat::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"Elem Type"));
	CLDSLineFlat::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"Whether Material Optimization"));
	CLDSLineFlat::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"Whether Specification Optimization"));
	CLDSLineFlat::AddPropItem("hole_out",PROPLIST_ITEM(id++,"Bolt Hole Out"));
	CLDSLineFlat::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"Calculate Automatic Bolt Hole Out"));
		//���㳤����Ϣ
	CLDSLineFlat::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,"Reduce Calculate Coefficient"));
	CLDSLineFlat::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineFlat::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,"Calculate Length Type"));
	CLDSLineFlat::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,"Reference Horizon Pole"));
	CLDSLineFlat::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,"Simultaneously Compression Reference Rod"));
	CLDSLineFlat::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,"Minimum Axial Length"));
	CLDSLineFlat::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,"Parallel Axial Length"));
		//ʼ�������Ϣ
	CLDSLineFlat::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"Start Information"));
	CLDSLineFlat::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"Start Joint Type"));
	CLDSLineFlat::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"Start Force Type"));
	CLDSLineFlat::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
		//�ն������Ϣ
	CLDSLineFlat::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"End Information"));
	CLDSLineFlat::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"End Joint Type"));
	CLDSLineFlat::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"End Force Type"));
	CLDSLineFlat::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,""));
#else 
	CLDSLineFlat::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"��ֻ�����Ϣ"));
	CLDSLineFlat::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSLineFlat::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSLineFlat::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	CLDSLineFlat::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�",NULL,"Ĭ��|A|B|C|D|"));
	CLDSLineFlat::AddPropItem("specification",PROPLIST_ITEM(id++,"���"));
	CLDSLineFlat::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSLineFlat::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	CLDSLineFlat::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSLineFlat::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"��������"));
	CLDSLineFlat::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Ӱ����Ϣ"));//Ӱ����Ϣ
	CLDSLineFlat::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Ӱ��ĸ����"));//Ӱ��ĸ����
	CLDSLineFlat::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"������������"));//�����Ĳ�������
	CLDSLineFlat::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"��������"));//�����Ĳ���
	CLDSLineFlat::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	CLDSLineFlat::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����"));
	CLDSLineFlat::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	CLDSLineFlat::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"���⸨������"));
	CLDSLineFlat::AddPropItem("length",PROPLIST_ITEM(id++,"ʵ�ʳ���"));
	CLDSLineFlat::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ"));
	CLDSLineFlat::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	CLDSLineFlat::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
	CLDSLineFlat::AddPropItem("workPlaneNorm",PROPLIST_ITEM(id++,"��׼�淨��"));
	CLDSLineFlat::AddPropItem("hWorkNormRefPart",PROPLIST_ITEM(id++,"���߲��ո˼�"));
	CLDSLineFlat::AddPropItem("workPlaneNorm.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSLineFlat::AddPropItem("workPlaneNorm.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSLineFlat::AddPropItem("workPlaneNorm.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSLineFlat::AddPropItem("nearWorkPlaneNorm",PROPLIST_ITEM(id++,"��׼����Ʒ���"));
	CLDSLineFlat::AddPropItem("m_iNormCalStyle",PROPLIST_ITEM(id++,"���㷽ʽ"));
	CLDSLineFlat::AddPropItem("nearWorkPlaneNorm.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSLineFlat::AddPropItem("nearWorkPlaneNorm.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSLineFlat::AddPropItem("nearWorkPlaneNorm.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSLineFlat::AddPropItem("startInfo",PROPLIST_ITEM(id++,"ʼ����Ϣ"));
	CLDSLineFlat::AddPropItem("hStart",PROPLIST_ITEM(id++,"�����"));
	CLDSLineFlat::AddPropItem("oddStart",PROPLIST_ITEM(id++,"����ͷ"));
	CLDSLineFlat::AddPropItem("startPos",PROPLIST_ITEM(id++,"ʼ���������"));
	CLDSLineFlat::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"ʼ��ʵ������"));
	CLDSLineFlat::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X����"));
	CLDSLineFlat::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSLineFlat::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSLineFlat::AddPropItem("desStartPos",PROPLIST_ITEM(id++,""));
	CLDSLineFlat::AddPropItem("desStartPos.endPosType",PROPLIST_ITEM(id++,"��λ��ʽ"));
	CLDSLineFlat::AddPropItem("desStartPos.datum_pos_style",PROPLIST_ITEM(id++,"���㶨λ��ʽ"));
	CLDSLineFlat::AddPropItem("desStartPos.hDatumPart",PROPLIST_ITEM(id++,"��׼�˼�"));
	CLDSLineFlat::AddPropItem("desStartPos.fNormOffset",PROPLIST_ITEM(id++,"��׼�淨��ƫ����"));
	CLDSLineFlat::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"ƫ�ľ�"));
	CLDSLineFlat::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"ƽ���淨��"));
	CLDSLineFlat::AddPropItem("desStartPos.fTransverseOffset",PROPLIST_ITEM(id++,"����ƫ����"));
	CLDSLineFlat::AddPropItem("desStartPos.hDatumStartPart",PROPLIST_ITEM(id++,"ͶӰ��ʼ�����ڻ�׼�˼�"));
	CLDSLineFlat::AddPropItem("desStartPos.bDatumStartStartEnd", PROPLIST_ITEM(id++, "��׼�˼��Ļ�׼�˵�"));
	CLDSLineFlat::AddPropItem("desStartPos.hDatumEndPart",PROPLIST_ITEM(id++,"ͶӰ���ն����ڻ�׼�˼�"));
	CLDSLineFlat::AddPropItem("desStartPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"��׼�˼��Ļ�׼�˵�"));
	CLDSLineFlat::AddPropItem("endInfo",PROPLIST_ITEM(id++,"�ն���Ϣ"));
	CLDSLineFlat::AddPropItem("hEnd",PROPLIST_ITEM(id++,"�յ���"));
	CLDSLineFlat::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"����ͷ"));
	CLDSLineFlat::AddPropItem("endPos",PROPLIST_ITEM(id++,"�ն��������"));
	CLDSLineFlat::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"�ն�ʵ������"));
	CLDSLineFlat::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X����"));
	CLDSLineFlat::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSLineFlat::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSLineFlat::AddPropItem("desEndPos",PROPLIST_ITEM(id++,""));
	CLDSLineFlat::AddPropItem("desEndPos.endPosType",PROPLIST_ITEM(id++,"��λ��ʽ"));
	CLDSLineFlat::AddPropItem("desEndPos.datum_pos_style",PROPLIST_ITEM(id++,"���㶨λ��ʽ"));
	CLDSLineFlat::AddPropItem("desEndPos.hDatumPart",PROPLIST_ITEM(id++,"��׼�˼�"));
	CLDSLineFlat::AddPropItem("desEndPos.fNormOffset",PROPLIST_ITEM(id++,"��׼�淨��ƫ����"));
	CLDSLineFlat::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,"ƫ�ľ�"));
	CLDSLineFlat::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,"ƽ���淨��"));
	CLDSLineFlat::AddPropItem("desEndPos.fTransverseOffset",PROPLIST_ITEM(id++,"����ƫ����"));
	CLDSLineFlat::AddPropItem("desEndPos.hDatumStartPart",PROPLIST_ITEM(id++,"ͶӰ��ʼ�����ڻ�׼�˼�"));
	CLDSLineFlat::AddPropItem("desEndPos.bDatumStartStartEnd", PROPLIST_ITEM(id++, "��׼�˼��Ļ�׼�˵�"));
	CLDSLineFlat::AddPropItem("desEndPos.hDatumEndPart",PROPLIST_ITEM(id++,"ͶӰ���ն����ڻ�׼�˼�"));
	CLDSLineFlat::AddPropItem("desEndPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"��׼�˼��Ļ�׼�˵�"));
	//������������Ҫ������
	CLDSLineFlat::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"ͳ�ı��"));	//ͳ�ı��
	CLDSLineFlat::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"�����Ŵ�ϵ��"));
	//�����Ϣ
	CLDSLineFlat::AddPropItem("designInfo",PROPLIST_ITEM(id++,"�˼�������Ϣ"));
	CLDSLineFlat::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"��Ԫ����"));
	CLDSLineFlat::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"�Ƿ���в�����ѡ"));
	CLDSLineFlat::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"�Ƿ���й����ѡ"));
	CLDSLineFlat::AddPropItem("hole_out",PROPLIST_ITEM(id++,"��˨������"));
	CLDSLineFlat::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"�Զ�������˨������"));
		//���㳤����Ϣ
	CLDSLineFlat::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,"����ϵ��"));
	CLDSLineFlat::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineFlat::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,"���㳤������"));
	CLDSLineFlat::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,"���պ�����"));
	CLDSLineFlat::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,"ͬʱ��ѹ���ո˼�"));
	CLDSLineFlat::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,"��С�᳤��"));
	CLDSLineFlat::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,"ƽ���᳤��"));
		//ʼ�������Ϣ
	CLDSLineFlat::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"ʼ����Ϣ"));
	CLDSLineFlat::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"ʼ��Լ������"));
	CLDSLineFlat::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"ʼ����������"));
	CLDSLineFlat::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
		//�ն������Ϣ
	CLDSLineFlat::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"�ն���Ϣ"));
	CLDSLineFlat::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"�ն�Լ������"));
	CLDSLineFlat::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"�ն���������"));
	CLDSLineFlat::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,""));
#endif
}

CHashStrList<PROPLIST_ITEM> CLDSLineAngle::propHashtable;
CXhChar100 CLDSLineAngle::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*> CLDSLineAngle::hashPropItemById;
CHashTable<DWORD> CLDSLineAngle::propStatusHashtable;
int CLDSLineAngle::m_iCurDisplayPropGroup;
void CLDSLineAngle::InitPropMirBitsMap()
{
	int id=1;
	propMirSync.InitHashtableSize(32);
	MPCL.SetBitState(id-1);
#ifdef AFX_TARG_ENU_ENGLISH
	propMirSync.AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	//propMirSync.AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("specification",PROPLIST_ITEM(id++,"Specification"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	propMirSync.AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Label"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cfgword",PROPLIST_ITEM(id++,"Model Flag"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("m_bEnableTeG",PROPLIST_ITEM(id++,"Special G"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("Position",PROPLIST_ITEM(id++,"Ridge Vertice Position"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("oddStartOnSameLabel",PROPLIST_ITEM(id++,"Start Ends Same Label"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("oddEndOnSameLabel",PROPLIST_ITEM(id++,"End Ends Same Label"));
	propMirSync.AddPropItem("oddStart",PROPLIST_ITEM(id++,"Start Ends"));
	propMirSync.AddPropItem("oddEnd",PROPLIST_ITEM(id++,"End Ends"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("linkInfo",PROPLIST_ITEM(id++,"Link Bolt Info"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("wingNormInfo",PROPLIST_ITEM(id++,"Leg Normal Info"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("kaihe_base_wing_x0_y1",PROPLIST_ITEM(id++,"Open Close Angle"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cutAngleWing",PROPLIST_ITEM(id++,"Leg Cut"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("push_flat",PROPLIST_ITEM(id++,"Flat Info"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("manuProcess",PROPLIST_ITEM(id++,"Other Process","Cut Root��Cut Back��Weld Process"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("synchronize_scope",PROPLIST_ITEM(id++,"Property Synchronous Scope","Local Object Property Scope"));
#else 
	propMirSync.AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	//propMirSync.AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("specification",PROPLIST_ITEM(id++,"���"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	propMirSync.AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	//CLDSLineAngle::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	//CLDSLineAngle::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	//CLDSLineAngle::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"���⸨������"));
	//CLDSLineAngle::AddPropItem("m_bFootNail",PROPLIST_ITEM(id++,"�Ŷ��Ǹ�"));
	//CLDSLineAngle::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	//CLDSLineAngle::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
	//CLDSLineAngle::AddPropItem("hStart",PROPLIST_ITEM(id++,"�����"));
	//CLDSLineAngle::AddPropItem("hEnd",PROPLIST_ITEM(id++,"�յ���"));
	//CLDSLineAngle::AddPropItem("startInfo",PROPLIST_ITEM(id++,"ʼ����Ϣ"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("m_bEnableTeG",PROPLIST_ITEM(id++,"����׼��"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("Position",PROPLIST_ITEM(id++,"���λ��"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("oddStartOnSameLabel",PROPLIST_ITEM(id++,"ͬ���ʱʼ������"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("oddEndOnSameLabel",PROPLIST_ITEM(id++,"ͬ���ʱ�ն�����"));
	propMirSync.AddPropItem("oddStart",PROPLIST_ITEM(id++,"ʼ������ͷ"));
	//CLDSLineAngle::AddPropItem("endInfo",PROPLIST_ITEM(id++,"�ն���Ϣ"));
	propMirSync.AddPropItem("oddEnd",PROPLIST_ITEM(id++,"�ն�����ͷ"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("linkInfo",PROPLIST_ITEM(id++,"������˨��Ϣ"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("wingNormInfo",PROPLIST_ITEM(id++,"֫������Ϣ"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("kaihe_base_wing_x0_y1",PROPLIST_ITEM(id++,"����֫"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cutAngleWing",PROPLIST_ITEM(id++,"�н���֫"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("push_flat",PROPLIST_ITEM(id++,"ѹ����Ϣ"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("manuProcess",PROPLIST_ITEM(id++,"���๤����Ϣ","��������������ӵȹ���"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("synchronize_scope",PROPLIST_ITEM(id++,"����ͬ����Χ","�ֲ����������ͬ����Χ"));
#endif
	//propMirSync.AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"Ԥ����"));
	//������������Ҫ������
#ifdef __PART_DESIGN_INC_
	propMirSync.AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"ͳ�ı��"));	//ͳ�ı��
	propMirSync.AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"�����Ŵ�ϵ��"));
	propMirSync.AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"��Ԫ����"));//�����Ϣ
	propMirSync.AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"�Ƿ���й����ѡ"));
	propMirSync.AddPropItem("hole_out",PROPLIST_ITEM(id++,"��˨������"));
	propMirSync.AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"�Զ�������˨������"));
	propMirSync.AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,"���㳤��"));	//���㳤����Ϣ
	propMirSync.AddPropItem("joint_type",PROPLIST_ITEM(id++,"Լ������"));	//ʼ�������Ϣ
	propMirSync.AddPropItem("force_type",PROPLIST_ITEM(id++,"��������"));
#endif
}
int CLDSLineAngle::InitLineAnglePropHashtable()
{
	int id=1;
	CLDSLineAngle::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);//CreateHashTable(HASHTABLESIZE);
	CLDSLineAngle::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSLineAngle::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Line Angle Basic Information"));
	CLDSLineAngle::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSLineAngle::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSLineAngle::AddPropItem("layer1",PROPLIST_ITEM(id++,"First Char"));
	CLDSLineAngle::AddPropItem("layer2",PROPLIST_ITEM(id++,"Second Char"));
	CLDSLineAngle::AddPropItem("layer3",PROPLIST_ITEM(id++,"Third Char"));
	CLDSLineAngle::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	CLDSLineAngle::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade",NULL,"Default|A|B|C|D|"));
	CLDSLineAngle::AddPropItem("specification",PROPLIST_ITEM(id++,"Specification"));
	CLDSLineAngle::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSLineAngle::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Serial Number"));
	CLDSLineAngle::AddPropItem("group_father_jg_h",PROPLIST_ITEM(id++,"Compound Father Angle"));
	CLDSLineAngle::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSLineAngle::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"Connect Part"));
	CLDSLineAngle::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//Ӱ����Ϣ
	CLDSLineAngle::AddPropItem("m_bSyncShadowPartNo",PROPLIST_ITEM(id++,"Synchornize part label"));//Ӱ����Ϣ
	CLDSLineAngle::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Project Mother Part"));//Ӱ��ĸ����
	CLDSLineAngle::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//�����Ĳ�������
	CLDSLineAngle::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//�����Ĳ���
	CLDSLineAngle::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSLineAngle::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"));
	CLDSLineAngle::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	CLDSLineAngle::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Ancillary Part"));
	CLDSLineAngle::AddPropItem("m_bFootNail",PROPLIST_ITEM(id++,"Step Angles"));
	CLDSLineAngle::AddPropItem("length",PROPLIST_ITEM(id++,"Physical Length"));
	CLDSLineAngle::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color"));
	CLDSLineAngle::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	CLDSLineAngle::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
	CLDSLineAngle::AddPropItem("hStart",PROPLIST_ITEM(id++,"Start Point Handle"));
	CLDSLineAngle::AddPropItem("hEnd",PROPLIST_ITEM(id++,"End Point Handle"));
	CLDSLineAngle::AddPropItem("startInfo",PROPLIST_ITEM(id++,"Start Information"));
	CLDSLineAngle::AddPropItem("startPos",PROPLIST_ITEM(id++,"Ridge Line Start Point Design Coordinate"));
	CLDSLineAngle::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"Ridge Line Start Point Physical Coordinate"));
	CLDSLineAngle::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z"));

	CLDSLineAngle::AddPropItem("desStartPos.jgber_cal_style",PROPLIST_ITEM(id++,"Ridge Vertex Locate Mode"));	//0.ֱ��ָ��������ꣻ1.��������λ
	CLDSLineAngle::AddPropItem("desStartPos.origin_pos_style",PROPLIST_ITEM(id++,"Start locating Point"));	//0.�ڵ�λ��;1.��׼��;2.��׼�Ǹֹؼ���;3.��һ�˽ڵ�����;4.��һ���������
	CLDSLineAngle::AddPropItem("desStartPos.datum_jg_h",PROPLIST_ITEM(id++,"Lap-joint Position Datum Angle"));	//���λ�û�׼�Ǹ�
	//����origin_pos_style=2ʱ��Ч
	CLDSLineAngle::AddPropItem("desStartPos.cDatumAngleWing",PROPLIST_ITEM(id++,"Key Point Affiliate Leg"));	//�Ǹ�'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset.operStyle",PROPLIST_ITEM(id++,"Key Point Compute Mode"));
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset.gStyle",PROPLIST_ITEM(id++,"Center Line Stadia Style"));
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset.offsetDist",PROPLIST_ITEM(id++,"Center Line Stadia"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint",PROPLIST_ITEM(id++,"Datum Point Position"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSLineAngle::AddPropItem("desStartPos.spatialOperationStyle",PROPLIST_ITEM(id++,"Datum Point Locate Mode"));
	CLDSLineAngle::AddPropItem("desStartPos.len_offset_type",PROPLIST_ITEM(id++,"Datum Plane Offset Type"));		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	CLDSLineAngle::AddPropItem("desStartPos.len_offset_dist",PROPLIST_ITEM(id++,"Datum Plane Offset"));		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	CLDSLineAngle::AddPropItem("desStartPos.datum_offset_dist",PROPLIST_ITEM(id++,"Datum Point Nurmal Offset"));	//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ط��ߵ�ƫ��
	CLDSLineAngle::AddPropItem("desStartPos.cFaceOffsetDatumLine",PROPLIST_ITEM(id++,"Offset Plane Datum Line"));
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"Offset Plane Normal"));
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm.x",PROPLIST_ITEM(id++,"Offset Plane Normal X"));	//ƽ���淨��X����
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm.y",PROPLIST_ITEM(id++,"Offset Plane Normal Y"));	//ƽ���淨��Y����
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm.z",PROPLIST_ITEM(id++,"Offset Plane Normal Z"));	//ƽ���淨��Z����
	CLDSLineAngle::AddPropItem("desStartPos.huoqu_jg_h",PROPLIST_ITEM(id++,"Bending Datum Angle"));		//������׼�Ǹ־��
	CLDSLineAngle::AddPropItem("desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desStartPos.huoqu_dist",PROPLIST_ITEM(id++,"Bending Line Clearance"));		//�����߼�϶
	CLDSLineAngle::AddPropItem("desStartPos.offset_zhun",PROPLIST_ITEM(id++,"Offset Stadia"));		//ƫ��׼�ߵ�ƫ����
	CLDSLineAngle::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"Eccentric Distance"));	//ƫ�ľ�==>���Ǹִ���ڸֹ���ʱ����Ҫ�˱��� wht 09-10-17
	//����spatialOperationStyle=15ʱ��Ч
	CLDSLineAngle::AddPropItem("desStartPos.coordCtrlType",PROPLIST_ITEM(id++,"Master Control Leg Coordinate"));	//0:X-Z����;1:Y-Z����;X-Y����
	CLDSLineAngle::AddPropItem("desStartPos.cMainCoordCtrlWing",PROPLIST_ITEM(id++,"Master Coordinate Control Leg"));		//�Ǹ�����֫ 'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing",PROPLIST_ITEM(id++,""));				//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing.operStyle",PROPLIST_ITEM(id++,"Master Control Point Locate Mode"));		//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing.gStyle",PROPLIST_ITEM(id++,"Master Control Line Stadia Style"));		//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing.offsetDist",PROPLIST_ITEM(id++,"Master Control Line Stadia"));	//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing",PROPLIST_ITEM(id++,""));				//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing.operStyle",PROPLIST_ITEM(id++,"Ancillary Control Point Locate Mode"));	//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing.gStyle",PROPLIST_ITEM(id++,"Ancillary Line Stadia Style"));		//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing.offsetDist",PROPLIST_ITEM(id++,"Ancillary Line Stadia"));	//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desStartPos.fLenOffsetOfCoordWing",PROPLIST_ITEM(id++,"Offset Distance Of Coord_Wing"));
	//����spatialOperationStyle=16,17ʱ��Ч
	CLDSLineAngle::AddPropItem("desStartPos.hDatumLine",PROPLIST_ITEM(id++,"Datum Line Definition"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.startPos",PROPLIST_ITEM(id++,"Datum Line Start Point Coordinate"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.startPos.x",PROPLIST_ITEM(id++,"Datum Line Start Point X"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.startPos.y",PROPLIST_ITEM(id++,"Datum Line Start Point Y"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.startPos.z",PROPLIST_ITEM(id++,"Datum Line Start Point Z"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.endPos",PROPLIST_ITEM(id++,"Datum Line End Point Coordinate"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.endPos.x",PROPLIST_ITEM(id++,"Datum Line End Point X"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.endPos.y",PROPLIST_ITEM(id++,"Datum Line End Point Y"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.endPos.z",PROPLIST_ITEM(id++,"Datum Line End Point Z"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLineStart",PROPLIST_ITEM(id++,"Project Datum Line Start Point"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLineEnd",PROPLIST_ITEM(id++,"Project Datum Line End Point "));
	//�ڶ���ƫ�Ʋ���(����->����ʸ��ƫ��)���������ڴ�Ӹ˼���(��)���λ�ü���
	//BYTE wingXGStyle,wingYGStyle;		//0.����ƫ��;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
	//double offset_x_dist,offset_y_dist;	//����gStyle=4ʱ��Ч
	CLDSLineAngle::AddPropItem("desStartPos.datum_to_ber_style",PROPLIST_ITEM(id++,"Datum Point->Ridge Vertex Locate Mode"));
	CLDSLineAngle::AddPropItem("desStartPos.cWorkWingStyle",PROPLIST_ITEM(id++,"Locate Connect Leg"));//�Ի������ƫ��ʱ��ǰ�ǸֵĻ�׼֫0:�Զ��ж�;1:X֫;2:Y֫;3:˫֫;�����Զ��ж�
	CLDSLineAngle::AddPropItem("desStartPos.wing_x_offset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desStartPos.wing_x_offset.gStyle",PROPLIST_ITEM(id++,"X Leg Normal Offset Type"));
	CLDSLineAngle::AddPropItem("desStartPos.wing_x_offset.offsetDist",PROPLIST_ITEM(id++,"X Leg Normal Offset"));
	CLDSLineAngle::AddPropItem("desStartPos.wing_y_offset",PROPLIST_ITEM(id++,""));	//��ʱ����WING_OFFSET_STYLE::operStyle����
	CLDSLineAngle::AddPropItem("desStartPos.wing_y_offset.gStyle",PROPLIST_ITEM(id++,"Y Leg Normal Offset Type"));
	CLDSLineAngle::AddPropItem("desStartPos.wing_y_offset.offsetDist",PROPLIST_ITEM(id++,"Y Leg Normal Offset"));
	CLDSLineAngle::AddPropItem("endInfo",PROPLIST_ITEM(id++,"End Information"));
	CLDSLineAngle::AddPropItem("endPos",PROPLIST_ITEM(id++,"Ridge Line End Point Design Coordinate"));
	CLDSLineAngle::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"Ridge Line End Point Physical Coordinate"));
	CLDSLineAngle::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z"));

	CLDSLineAngle::AddPropItem("desEndPos.jgber_cal_style",PROPLIST_ITEM(id++,"Ridge Vertex Locate Mode"));	//0.ֱ��ָ��������ꣻ1.��������λ
	CLDSLineAngle::AddPropItem("desEndPos.origin_pos_style",PROPLIST_ITEM(id++,"Start locating Point"));	//0.�ڵ�λ��;1.��׼��;2.��׼�Ǹֹؼ���;3.��һ�˽ڵ�����;4.��һ���������
	CLDSLineAngle::AddPropItem("desEndPos.datum_jg_h",PROPLIST_ITEM(id++,"Lap-joint Position Datum Rod"));	//���λ�û�׼�Ǹ�
	//����origin_pos_style=2ʱ��Ч
	CLDSLineAngle::AddPropItem("desEndPos.cDatumAngleWing",PROPLIST_ITEM(id++,"Key Point Affiliate Leg"));	//�Ǹ�'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
	CLDSLineAngle::AddPropItem("desEndPos.datumWingOffset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desEndPos.datumWingOffset.operStyle",PROPLIST_ITEM(id++,"Key Point Compute Mode"));
	CLDSLineAngle::AddPropItem("desEndPos.datumWingOffset.gStyle",PROPLIST_ITEM(id++,"Center Line Stadia Style"));
	CLDSLineAngle::AddPropItem("desEndPos.datumWingOffset.offsetDist",PROPLIST_ITEM(id++,"Center Line Stadia"));
	CLDSLineAngle::AddPropItem("desEndPos.datumPoint",PROPLIST_ITEM(id++,"Datum Point Position"));
	CLDSLineAngle::AddPropItem("desEndPos.datumPoint.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSLineAngle::AddPropItem("desEndPos.datumPoint.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSLineAngle::AddPropItem("desEndPos.datumPoint.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSLineAngle::AddPropItem("desEndPos.spatialOperationStyle",PROPLIST_ITEM(id++,"Datum Point Locate Mode"));
	CLDSLineAngle::AddPropItem("desStartPos.bendingLenOffsetComplement",PROPLIST_ITEM(id++,"Bending Point Vertical Direction Offset"));
	CLDSLineAngle::AddPropItem("desEndPos.bendingLenOffsetComplement",PROPLIST_ITEM(id++,"Bending Point Vertical Direction Offset"));
	CLDSLineAngle::AddPropItem("desEndPos.len_offset_type",PROPLIST_ITEM(id++,"Datum Plane Offset Type"));		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	CLDSLineAngle::AddPropItem("desEndPos.len_offset_dist",PROPLIST_ITEM(id++,"Datum Plane Offset"));				//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	CLDSLineAngle::AddPropItem("desEndPos.datum_offset_dist",PROPLIST_ITEM(id++,"Datum Point Nurmal Offset"));	//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ط��ߵ�ƫ��
	CLDSLineAngle::AddPropItem("desEndPos.cFaceOffsetDatumLine",PROPLIST_ITEM(id++,"Offset Plane Datum Line"));
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,"Offset Plane Normal "));
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm.x",PROPLIST_ITEM(id++,"X Component"));	//ƽ���淨��X����
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm.y",PROPLIST_ITEM(id++,"Y Component"));	//ƽ���淨��Y����
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm.z",PROPLIST_ITEM(id++,"Z Component"));	//ƽ���淨��Z����
	CLDSLineAngle::AddPropItem("desEndPos.huoqu_jg_h",PROPLIST_ITEM(id++,"Bending Datum Angle"));		//������׼�Ǹ־��
	CLDSLineAngle::AddPropItem("desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desEndPos.huoqu_dist",PROPLIST_ITEM(id++,"Bending Line Clearance"));		//�����߼�϶
	CLDSLineAngle::AddPropItem("desEndPos.offset_zhun",PROPLIST_ITEM(id++,"Offset Stadia"));		//ƫ��׼�ߵ�ƫ����
	CLDSLineAngle::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,"Eccentric Distance"));	//ƫ�ľ�==>���Ǹִ���ڸֹ���ʱ����Ҫ�˱��� wht 09-10-17
	//����spatialOperationStyle=15ʱ��Ч
	CLDSLineAngle::AddPropItem("desEndPos.coordCtrlType",PROPLIST_ITEM(id++,"Master Control Leg Control Coordinate"));	//0:X-Z����;1:Y-Z����;X-Y����
	CLDSLineAngle::AddPropItem("desEndPos.cMainCoordCtrlWing",PROPLIST_ITEM(id++,"Master Coordinate Control Leg"));	//�Ǹ�����֫ 'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing",PROPLIST_ITEM(id++,""));				//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing.operStyle",PROPLIST_ITEM(id++,"Master Control Point Locate Mode"));	//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing.gStyle",PROPLIST_ITEM(id++,"Master Control Line Stadia Style"));		//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing.offsetDist",PROPLIST_ITEM(id++,"Master Control Line Stadia"));	//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing",PROPLIST_ITEM(id++,""));			//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing.operStyle",PROPLIST_ITEM(id++,"Ancillary Point Locate Mode"));	//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing.gStyle",PROPLIST_ITEM(id++,"Ancillary Line Stadia Style"));		//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing.offsetDist",PROPLIST_ITEM(id++,"Ancillary Line Stadia"));	//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desEndPos.fLenOffsetOfCoordWing",PROPLIST_ITEM(id++,"Offset Distance Of Coord_Wing"));
	//����spatialOperationStyle=16,17ʱ��Ч
	CLDSLineAngle::AddPropItem("desEndPos.hDatumLine",PROPLIST_ITEM(id++,"Datum Line Definition"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos",PROPLIST_ITEM(id++,"Datum Line Start Point Coordinate"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos.x",PROPLIST_ITEM(id++,"X Coordinate��"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos.y",PROPLIST_ITEM(id++,"Y Coordinate��"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos.z",PROPLIST_ITEM(id++,"Z Coordinate��"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos",PROPLIST_ITEM(id++,"Datum Line End Point Coordinate"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos.x",PROPLIST_ITEM(id++,"X Coordinate��"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos.y",PROPLIST_ITEM(id++,"Y Coordinate��"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos.z",PROPLIST_ITEM(id++,"Z Coordinate��"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLineStart",PROPLIST_ITEM(id++,"Project Datum Line Start Point"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLineEnd",PROPLIST_ITEM(id++,"Project Datum Line End Point "));
	//�ڶ���ƫ�Ʋ���(����->����ʸ��ƫ��)���������ڴ�Ӹ˼���(��)���λ�ü���
	CLDSLineAngle::AddPropItem("desEndPos.datum_to_ber_style",PROPLIST_ITEM(id++,"Datum Point->Ridge Vertex Locate Mode"));
	//BYTE wingXGStyle,wingYGStyle;		//0.����ƫ��;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
	//double offset_x_dist,offset_y_dist;	//����gStyle=4ʱ��Ч
	CLDSLineAngle::AddPropItem("desEndPos.cWorkWingStyle",PROPLIST_ITEM(id++,"Locate Connect Leg"));//�Ի������ƫ��ʱ��ǰ�ǸֵĻ�׼֫0:�Զ��ж�;1:X֫;2:Y֫;3:˫֫;�����Զ��ж�
	CLDSLineAngle::AddPropItem("desEndPos.wing_x_offset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desEndPos.wing_x_offset.gStyle",PROPLIST_ITEM(id++," Leg Normal Offset Type"));
	CLDSLineAngle::AddPropItem("desEndPos.wing_x_offset.offsetDist",PROPLIST_ITEM(id++," Leg Normal Offset"));
	CLDSLineAngle::AddPropItem("desEndPos.wing_y_offset",PROPLIST_ITEM(id++,""));	//��ʱ����WING_OFFSET_STYLE::operStyle����
	CLDSLineAngle::AddPropItem("desEndPos.wing_y_offset.gStyle",PROPLIST_ITEM(id++,"Y Leg Normal Offset Type"));
	CLDSLineAngle::AddPropItem("desEndPos.wing_y_offset.offsetDist",PROPLIST_ITEM(id++,"Y Leg Normal Offset"));

	CLDSLineAngle::AddPropItem("oddStart",PROPLIST_ITEM(id++,"Odd Length"));
	CLDSLineAngle::AddPropItem("oddStart.m_iOddCalStyle",PROPLIST_ITEM(id++,"Calculate Style"));	//0.��ʾ������ײ���ո˼�����;1.������˨λ�ü���;2.��ʾ����������ͷ
	CLDSLineAngle::AddPropItem("oddStart.m_fCollideDist",PROPLIST_ITEM(id++,"Collision Distance"));
	CLDSLineAngle::AddPropItem("oddStart.m_hRefPart1",PROPLIST_ITEM(id++,"Refence Rod 1"));
	CLDSLineAngle::AddPropItem("oddStart.m_hRefPart2",PROPLIST_ITEM(id++,"Refence Rod 2"));
	CLDSLineAngle::AddPropItem("oddStartExtra",PROPLIST_ITEM(id++,"Fixed Odd Length"));
	CLDSLineAngle::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"Odd Length"));
	CLDSLineAngle::AddPropItem("oddEnd.m_iOddCalStyle",PROPLIST_ITEM(id++,"Calculate Style"));	//0.��ʾ������ײ���ո˼�����;1.������˨λ�ü���;2.��ʾ����������ͷ
	CLDSLineAngle::AddPropItem("oddEnd.m_fCollideDist",PROPLIST_ITEM(id++,"Collision Distance"));
	CLDSLineAngle::AddPropItem("oddEnd.m_hRefPart1",PROPLIST_ITEM(id++,"Refence Rod 1"));
	CLDSLineAngle::AddPropItem("oddEnd.m_hRefPart2",PROPLIST_ITEM(id++,"Refence Rod 2"));
	CLDSLineAngle::AddPropItem("oddEndExtra",PROPLIST_ITEM(id++,"Fixed Odd Length"));
	CLDSLineAngle::AddPropItem("StartOddCanAcrossMiddleSect",PROPLIST_ITEM(id++,"Can shorten across middle","","0.No|1.Yes"));
	CLDSLineAngle::AddPropItem("EndOddCanAcrossMiddleSect",PROPLIST_ITEM(id++,"Can shorten across middle","","0.No|1.Yes"));
	CLDSLineAngle::AddPropItem("linkInfo",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("startBoltInfo",PROPLIST_ITEM(id++,"Previous Set Bolt Information"));
	CLDSLineAngle::AddPropItem("connectStart.grade",PROPLIST_ITEM(id++,"Grade"));
	CLDSLineAngle::AddPropItem("connectStart.d",PROPLIST_ITEM(id++,"Bolt Specification"));
	CLDSLineAngle::AddPropItem("connectStart.N",PROPLIST_ITEM(id++,"Bolt Number"));
	CLDSLineAngle::AddPropItem("connectStart.rows",PROPLIST_ITEM(id++,"Bolt Row Amount"));
	CLDSLineAngle::AddPropItem("endBoltInfo",PROPLIST_ITEM(id++,"Previous Set Bolt Information"));
	CLDSLineAngle::AddPropItem("connectEnd.grade",PROPLIST_ITEM(id++,"Grade"));
	CLDSLineAngle::AddPropItem("connectEnd.d",PROPLIST_ITEM(id++,"Bolt Specification"));
	CLDSLineAngle::AddPropItem("connectEnd.N",PROPLIST_ITEM(id++,"Bolt Number"));
	CLDSLineAngle::AddPropItem("connectEnd.rows",PROPLIST_ITEM(id++,"Bolt Row Amount"));
	CLDSLineAngle::AddPropItem("AttachBolts",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("startAttachBolts",PROPLIST_ITEM(id++,"Decorated Bolt"));
	CLDSLineAngle::AddPropItem("midAttachBolts",PROPLIST_ITEM(id++,"Decorated Center Bolt"));
	CLDSLineAngle::AddPropItem("endAttachBolts",PROPLIST_ITEM(id++,"Decorated Bolt"));
	CLDSLineAngle::AddPropItem("wingNormInfo",PROPLIST_ITEM(id++,"Two Leg Normal Information"));
	CLDSLineAngle::AddPropItem("norm_x_wing",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("norm_x_wing.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("norm_x_wing.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("norm_x_wing.z",PROPLIST_ITEM(id++,"Z"));
	CLDSLineAngle::AddPropItem("norm_y_wing",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("norm_y_wing.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("norm_y_wing.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("norm_y_wing.z",PROPLIST_ITEM(id++,"Z"));
	CLDSLineAngle::AddPropItem("work_wing",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("work_wing.hViceJg",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("XorYMirLayStyle",PROPLIST_ITEM(id++,"Leg Layout Style On X/Y Mirror"));
	CLDSLineAngle::AddPropItem("blScatterBoltLocaTolerance",PROPLIST_ITEM(id++,"Hole location export mode","","0.Space prefer|1.Location prefer"));
	CLDSLineAngle::AddPropItem("decWingAngle",PROPLIST_ITEM(id++,"Two Leg Angle"));
	CLDSLineAngle::AddPropItem("start_wing_plane",PROPLIST_ITEM(id++,"Start Leg Plane"));
	CLDSLineAngle::AddPropItem("end_wing_plane",PROPLIST_ITEM(id++,"End Leg Plane"));
	CLDSLineAngle::AddPropItem("kaihe_base_wing_x0_y1",PROPLIST_ITEM(id++,"Open Close Leg"));
	CLDSLineAngle::AddPropItem("m_cHeJiaoReservedWidthType",PROPLIST_ITEM(id++,"Close Leg Reserve Width Type"));
	CLDSLineAngle::AddPropItem("m_fHeJiaoReservedWidth",PROPLIST_ITEM(id++,"Close Leg Reserve Width"));
	CLDSLineAngle::AddPropItem("SetupPosInfo",PROPLIST_ITEM(id++,"Setup Locate Information"));
	CLDSLineAngle::AddPropItem("wingXG",PROPLIST_ITEM(id++,"X(Red) Leg Execute g"));
	CLDSLineAngle::AddPropItem("wingYG",PROPLIST_ITEM(id++,"Y(Green) Leg Execute g"));
	CLDSLineAngle::AddPropItem("m_bEnableTeG",PROPLIST_ITEM(id++,"Enable Special Stadia"));
	CLDSLineAngle::AddPropItem("teGTable",PROPLIST_ITEM(id++,"Special Stadia Table"));
	CLDSLineAngle::AddPropItem("m_cPosCtrlDatumLineType",PROPLIST_ITEM(id++,"Lap Joint Locate Standard"));
	CLDSLineAngle::AddPropItem("m_cPosCtrlType",PROPLIST_ITEM(id++,"Lap Point Locate Mode"));
	CLDSLineAngle::AddPropItem("startCutWing",PROPLIST_ITEM(id++,"Start Leg Cut"));
	CLDSLineAngle::AddPropItem("startCutWingX",PROPLIST_ITEM(id++,"X Leg"));
	CLDSLineAngle::AddPropItem("startCutWingY",PROPLIST_ITEM(id++,"Y Leg"));
	CLDSLineAngle::AddPropItem("endCutWing",PROPLIST_ITEM(id++,"End Leg Cut"));
	CLDSLineAngle::AddPropItem("endCutWingX",PROPLIST_ITEM(id++,"X Leg"));
	CLDSLineAngle::AddPropItem("endCutWingY",PROPLIST_ITEM(id++,"Y Leg"));
	CLDSLineAngle::AddPropItem("start_push_flat_x1_y2",PROPLIST_ITEM(id++,"Start Flat"));
	CLDSLineAngle::AddPropItem("start_push_flat_length",PROPLIST_ITEM(id++,"Flat Length"));
	CLDSLineAngle::AddPropItem("end_push_flat_x1_y2",PROPLIST_ITEM(id++,"End Flat"));
	CLDSLineAngle::AddPropItem("end_push_flat_length",PROPLIST_ITEM(id++,"Flat Length"));
	CLDSLineAngle::AddPropItem("mid_push_flat_x1_y2",PROPLIST_ITEM(id++,"Center Flat"));
	CLDSLineAngle::AddPropItem("mid_push_flat_start_offset",PROPLIST_ITEM(id++,"Start Offset"));
	CLDSLineAngle::AddPropItem("mid_push_flat_end_offset",PROPLIST_ITEM(id++,"End Offset"));
	CLDSLineAngle::AddPropItem("mid_push_flat_node_handle",PROPLIST_ITEM(id++,"Flat Position Node"));
	CLDSLineAngle::AddPropItem("m_fCutAngleCoefR",PROPLIST_ITEM(id++,"Internal Leg Cut Clearance"));
	CLDSLineAngle::AddPropItem("statMatInfo",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("manuProcess",PROPLIST_ITEM(id++,"Technique Information"));
	CLDSLineAngle::AddPropItem("m_bCutRoot",PROPLIST_ITEM(id++,"Cut Root"));
	CLDSLineAngle::AddPropItem("m_bCutBer",PROPLIST_ITEM(id++,"Cut Ber"));
	CLDSLineAngle::AddPropItem("m_bBendPart",PROPLIST_ITEM(id++,"Bend"));
	CLDSLineAngle::AddPropItem("FilletArcS",PROPLIST_ITEM(id++,"FilletArcS"));
	CLDSLineAngle::AddPropItem("FilletROfSX",PROPLIST_ITEM(id++,"FilletR"));
	CLDSLineAngle::AddPropItem("FilletROfSY",PROPLIST_ITEM(id++,"FilletR"));
	CLDSLineAngle::AddPropItem("FilletArcE",PROPLIST_ITEM(id++,"FilletArcE"));
	CLDSLineAngle::AddPropItem("FilletROfEX",PROPLIST_ITEM(id++,"FilletR"));
	CLDSLineAngle::AddPropItem("FilletROfEY",PROPLIST_ITEM(id++,"FilletR"));
	CLDSLineAngle::AddPropItem("OpenSlotGroup",PROPLIST_ITEM(id++,"OpenSlot"));
	CLDSLineAngle::AddPropItem("OpenSlot1",PROPLIST_ITEM(id++,"OpenSlot1"));
	CLDSLineAngle::AddPropItem("OpenSlot2",PROPLIST_ITEM(id++,"OpenSlot2"));
	CLDSLineAngle::AddPropItem("OpenSlot3",PROPLIST_ITEM(id++,"OpenSlot3"));
	CLDSLineAngle::AddPropItem("OpenSlot4",PROPLIST_ITEM(id++,"OpenSlot4"));
	//������������Ҫ������
	CLDSLineAngle::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"Statistic Material Serial Number"));	//ͳ�ı��
	CLDSLineAngle::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"Gravity Zoom Out Coefficient"));
	CLDSLineAngle::AddPropItem("size.idClassType",PROPLIST_ITEM(id++,""));
	//�����Ϣ
	CLDSLineAngle::AddPropItem("designInfo",PROPLIST_ITEM(id++,"Rod Check Information"));
	CLDSLineAngle::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"Elem Type"));
	CLDSLineAngle::AddPropItem("sel_mat_method",PROPLIST_ITEM(id++,"Brace member check rule"));
	CLDSLineAngle::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Whether Material Optimization"));
	CLDSLineAngle::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"Whether Specification Optimization"));
	CLDSLineAngle::AddPropItem("hole_out",PROPLIST_ITEM(id++,"Bolt Hole Out"));
	CLDSLineAngle::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"Calculate Automatic Bolt Hole Out"));
	//Ԥ����
	CLDSLineAngle::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"Lift Arc"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift",PROPLIST_ITEM(id++,"Start Prelift Coordinate"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift.z",PROPLIST_ITEM(id++,"Z"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift",PROPLIST_ITEM(id++,"End Prelift Coordinate"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift.z",PROPLIST_ITEM(id++,"Z"));
		//���㳤����Ϣ
	CLDSLineAngle::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,"Reduce Calculate Coefficient"));
	CLDSLineAngle::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,"Calculate Length Type"));
	CLDSLineAngle::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,"Reference Horizon Pole"));
	CLDSLineAngle::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,"Simultaneously Compression Reference Rod"));
	CLDSLineAngle::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,"Minimum Axial Length"));
	CLDSLineAngle::AddPropItem("CalLenCoef.minR.hStartNode",PROPLIST_ITEM(id++,"Start Node"));
	CLDSLineAngle::AddPropItem("CalLenCoef.minR.hEndNode",PROPLIST_ITEM(id++,"End Node"));
	CLDSLineAngle::AddPropItem("CalLenCoef.minR.coef",PROPLIST_ITEM(id++,"Reduce Calculate Coefficient"));
	CLDSLineAngle::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,"Parallel Axial Length"));
	CLDSLineAngle::AddPropItem("CalLenCoef.paraR.hStartNode",PROPLIST_ITEM(id++,"Start Node"));
	CLDSLineAngle::AddPropItem("CalLenCoef.paraR.hEndNode",PROPLIST_ITEM(id++,"End Node"));
	CLDSLineAngle::AddPropItem("CalLenCoef.paraR.coef",PROPLIST_ITEM(id++,"Reduce Calculate Coefficient"));
		//ʼ�������Ϣ
	CLDSLineAngle::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"Start Information"));
	CLDSLineAngle::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"Start Joint Type"));
	CLDSLineAngle::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"Start Force Type"));
	CLDSLineAngle::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
		//�ն������Ϣ
	CLDSLineAngle::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"End Information"));
	CLDSLineAngle::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"End Joint Type"));
	CLDSLineAngle::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"End Force Type"));
	CLDSLineAngle::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,""));
#else 
	CLDSLineAngle::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"������Ϣ"));
	CLDSLineAngle::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSLineAngle::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSLineAngle::AddPropItem("layer1",PROPLIST_ITEM(id++,"��һ�ַ�"));
	CLDSLineAngle::AddPropItem("layer2",PROPLIST_ITEM(id++,"�ڶ��ַ�"));
	CLDSLineAngle::AddPropItem("layer3",PROPLIST_ITEM(id++,"�����ַ�"));
	CLDSLineAngle::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	CLDSLineAngle::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�",NULL,"Ĭ��|A|B|C|D|"));
	CLDSLineAngle::AddPropItem("specification",PROPLIST_ITEM(id++,"���"));
	CLDSLineAngle::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSLineAngle::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	CLDSLineAngle::AddPropItem("group_father_jg_h",PROPLIST_ITEM(id++,"��ϸ��Ǹ�"));
	CLDSLineAngle::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSLineAngle::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"��������"));
	CLDSLineAngle::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Ӱ����Ϣ"));//Ӱ����Ϣ
	CLDSLineAngle::AddPropItem("m_bSyncShadowPartNo",PROPLIST_ITEM(id++,"�������±��"));//Ӱ����Ϣ
	CLDSLineAngle::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Ӱ��ĸ����"));//Ӱ��ĸ����
	CLDSLineAngle::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"������������"));//�����Ĳ�������
	CLDSLineAngle::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"��������"));//�����Ĳ���
	CLDSLineAngle::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	CLDSLineAngle::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����"));
	CLDSLineAngle::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	CLDSLineAngle::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"���⸨������"));
	CLDSLineAngle::AddPropItem("m_bFootNail",PROPLIST_ITEM(id++,"�Ŷ��Ǹ�"));
	CLDSLineAngle::AddPropItem("length",PROPLIST_ITEM(id++,"ʵ�ʳ���"));
	CLDSLineAngle::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ"));
	CLDSLineAngle::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	CLDSLineAngle::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
	CLDSLineAngle::AddPropItem("hStart",PROPLIST_ITEM(id++,"�����"));
	CLDSLineAngle::AddPropItem("hEnd",PROPLIST_ITEM(id++,"�յ���"));
	CLDSLineAngle::AddPropItem("startInfo",PROPLIST_ITEM(id++,"ʼ����Ϣ"));
	CLDSLineAngle::AddPropItem("startPos",PROPLIST_ITEM(id++,"��������������"));
	CLDSLineAngle::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"�������ʵ������"));
	CLDSLineAngle::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z"));

	CLDSLineAngle::AddPropItem("desStartPos.jgber_cal_style",PROPLIST_ITEM(id++,"��㶨λ��ʽ"));	//0.ֱ��ָ��������ꣻ1.��������λ
	//�򻯶�λģʽ
	CLDSLineAngle::AddPropItem("desStartPos.SimpleDatumPoint",PROPLIST_ITEM(id++,"��λ����"));		//0.ֱ��ָ��������ꣻ1.��������λ
	CLDSLineAngle::AddPropItem("desStartPos.SimpleDestLineMode",PROPLIST_ITEM(id++,"Ŀ���׼��"));	//0.ֱ��ָ��������ꣻ1.��������λ
	CLDSLineAngle::AddPropItem("desStartPos.SimpleDenpendWing",PROPLIST_ITEM(id++,"����֫"));	//0.ֱ��ָ��������ꣻ1.��������λ
	CLDSLineAngle::AddPropItem("desStartPos.SimpleDependRod",PROPLIST_ITEM(id++,"��������˼�"));	//0.ֱ��ָ��������ꣻ1.��������λ
	CLDSLineAngle::AddPropItem("desStartPos.SimpleLenOffsetType",PROPLIST_ITEM(id++,"����ƫ������"));		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	CLDSLineAngle::AddPropItem("desStartPos.SimpleLenOffset",PROPLIST_ITEM(id++,"����ƫ����"));		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	CLDSLineAngle::AddPropItem("desStartPos.SimpleNormOffset",PROPLIST_ITEM(id++,"����ƫ��"));	//0.ֱ��ָ��������ꣻ1.��������λ
	CLDSLineAngle::AddPropItem("desStartPos.SimpleBendLineAlongAngle",PROPLIST_ITEM(id++,"�����߻�׼�Ǹ�"));
	CLDSLineAngle::AddPropItem("desStartPos.SimpleBendAngleWing",PROPLIST_ITEM(id++,"��ǰ����֫"));
	CLDSLineAngle::AddPropItem("desStartPos.SimpleBendClearance",PROPLIST_ITEM(id++,"������϶"));
	CLDSLineAngle::AddPropItem("desStartPos.SimpleOffsetZhunju",PROPLIST_ITEM(id++,"ƫ��׼��"));
	CLDSLineAngle::AddPropItem("desStartPos.SimpleSelfWorkWing",PROPLIST_ITEM(id++,"������֫"));	//0.ֱ��ָ��������ꣻ1.��������λ
	CLDSLineAngle::AddPropItem("desStartPos.SimpleWingXTipCtrlPoint",PROPLIST_ITEM(id++,"X(��)֫��������"));
	CLDSLineAngle::AddPropItem("desStartPos.SimpleWingYTipCtrlPoint",PROPLIST_ITEM(id++,"Y(��)֫��������"));

	CLDSLineAngle::AddPropItem("desStartPos.origin_pos_style",PROPLIST_ITEM(id++,"��ʼ��λ��"));	//0.�ڵ�λ��;1.��׼��;2.��׼�Ǹֹؼ���;3.��һ�˽ڵ�����;4.��һ���������
	CLDSLineAngle::AddPropItem("desStartPos.datum_jg_h",PROPLIST_ITEM(id++,"���λ�û�׼�Ǹ�"));	//���λ�û�׼�Ǹ�
	//����origin_pos_style=2ʱ��Ч
	CLDSLineAngle::AddPropItem("desStartPos.cDatumAngleWing",PROPLIST_ITEM(id++,"�ؼ������֫"));	//�Ǹ�'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset.operStyle",PROPLIST_ITEM(id++,"�ؼ�����㷽ʽ"));
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset.gStyle",PROPLIST_ITEM(id++,"����׼������"));
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset.offsetDist",PROPLIST_ITEM(id++,"����׼��ֵ"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint",PROPLIST_ITEM(id++,"����λ��"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint.x",PROPLIST_ITEM(id++,"X����"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSLineAngle::AddPropItem("desStartPos.spatialOperationStyle",PROPLIST_ITEM(id++,"���㶨λ��ʽ"));
	CLDSLineAngle::AddPropItem("desStartPos.LappedDatumWingG",PROPLIST_ITEM(id++,"����ľ�"));		
	CLDSLineAngle::AddPropItem("desStartPos.len_offset_type",PROPLIST_ITEM(id++,"����ƫ������"));	//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	CLDSLineAngle::AddPropItem("desStartPos.len_offset_dist",PROPLIST_ITEM(id++,"����ƫ����"));		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	CLDSLineAngle::AddPropItem("desStartPos.datum_offset_dist",PROPLIST_ITEM(id++,"���㷨��ƫ����"));	//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ط��ߵ�ƫ��
	CLDSLineAngle::AddPropItem("desStartPos.cFaceOffsetDatumLine",PROPLIST_ITEM(id++,"ƽ�����׼��"));
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"ƽ���淨��"));
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm.x",PROPLIST_ITEM(id++,"ƽ���淨��X����"));	//ƽ���淨��X����
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm.y",PROPLIST_ITEM(id++,"ƽ���淨��Y����"));	//ƽ���淨��Y����
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm.z",PROPLIST_ITEM(id++,"ƽ���淨��Z����"));	//ƽ���淨��Z����
	CLDSLineAngle::AddPropItem("desStartPos.huoqu_jg_h",PROPLIST_ITEM(id++,"������׼�Ǹ�"));		//������׼�Ǹ־��
	CLDSLineAngle::AddPropItem("desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desStartPos.huoqu_dist",PROPLIST_ITEM(id++,"�����߼�϶"));		//�����߼�϶
	CLDSLineAngle::AddPropItem("desStartPos.offset_zhun",PROPLIST_ITEM(id++,"ƫ��׼��"));		//ƫ��׼�ߵ�ƫ����
	CLDSLineAngle::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"ƫ�ľ�"));	//ƫ�ľ�==>���Ǹִ���ڸֹ���ʱ����Ҫ�˱��� wht 09-10-17
	//����spatialOperationStyle=15ʱ��Ч
	CLDSLineAngle::AddPropItem("desStartPos.coordCtrlType",PROPLIST_ITEM(id++,"����֫��������"));	//0:X-Z����;1:Y-Z����;X-Y����
	CLDSLineAngle::AddPropItem("desStartPos.cMainCoordCtrlWing",PROPLIST_ITEM(id++,"������Ƶ�"));		//�Ǹ�����֫ 'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing",PROPLIST_ITEM(id++,""));				//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing.operStyle",PROPLIST_ITEM(id++,"���ص㶨λ��ʽ"));		//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing.gStyle",PROPLIST_ITEM(id++,"������׼������"));		//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing.offsetDist",PROPLIST_ITEM(id++,"������׼��"));	//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing",PROPLIST_ITEM(id++,""));				//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing.operStyle",PROPLIST_ITEM(id++,"���ص㶨λ��ʽ"));	//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing.gStyle",PROPLIST_ITEM(id++,"������׼������"));		//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing.offsetDist",PROPLIST_ITEM(id++,"������׼��"));	//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desStartPos.fLenOffsetOfCoordWing",PROPLIST_ITEM(id++,"���������λƫ��"));
	//����spatialOperationStyle=16,17ʱ��Ч
	CLDSLineAngle::AddPropItem("desStartPos.hDatumLine",PROPLIST_ITEM(id++,"��׼�߶���"));
	CLDSLineAngle::AddPropItem("desStartPos.cMirTransType",PROPLIST_ITEM(id++,"���նԳ�����"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.startPos",PROPLIST_ITEM(id++,"��׼���������"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.startPos.x",PROPLIST_ITEM(id++,"��׼�����X����"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.startPos.y",PROPLIST_ITEM(id++,"��׼�����Y����"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.startPos.z",PROPLIST_ITEM(id++,"��׼�����Z����"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.endPos",PROPLIST_ITEM(id++,"��׼���յ�����"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.endPos.x",PROPLIST_ITEM(id++,"��׼���յ�X����"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.endPos.y",PROPLIST_ITEM(id++,"��׼���յ�Y����"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.endPos.z",PROPLIST_ITEM(id++,"��׼���յ�Z����"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLineStart",PROPLIST_ITEM(id++,"ͶӰ��׼�����"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLineEnd",PROPLIST_ITEM(id++,"ͶӰ��׼���յ�"));
	//�ڶ���ƫ�Ʋ���(����->����ʸ��ƫ��)���������ڴ�Ӹ˼���(��)���λ�ü���
	//BYTE wingXGStyle,wingYGStyle;		//0.����ƫ��;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
	//double offset_x_dist,offset_y_dist;	//����gStyle=4ʱ��Ч
	CLDSLineAngle::AddPropItem("desStartPos.datum_to_ber_style",PROPLIST_ITEM(id++,"����->��㶨λ��ʽ"));
	CLDSLineAngle::AddPropItem("desStartPos.cWorkWingStyle",PROPLIST_ITEM(id++,"��λ����֫"));//�Ի������ƫ��ʱ��ǰ�ǸֵĻ�׼֫0:�Զ��ж�;1:X֫;2:Y֫;3:˫֫;�����Զ��ж�
	CLDSLineAngle::AddPropItem("desStartPos.wing_x_offset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desStartPos.wing_x_offset.gStyle",PROPLIST_ITEM(id++,"X֫������ƫ�Ʒ�ʽ"));
	CLDSLineAngle::AddPropItem("desStartPos.wing_x_offset.offsetDist",PROPLIST_ITEM(id++,"X֫������ƫ����"));
	CLDSLineAngle::AddPropItem("desStartPos.wing_y_offset",PROPLIST_ITEM(id++,""));	//��ʱ����WING_OFFSET_STYLE::operStyle����
	CLDSLineAngle::AddPropItem("desStartPos.wing_y_offset.gStyle",PROPLIST_ITEM(id++,"Y֫������ƫ�Ʒ�ʽ"));
	CLDSLineAngle::AddPropItem("desStartPos.wing_y_offset.offsetDist",PROPLIST_ITEM(id++,"Y֫������ƫ����"));
	/*CLDSLineAngle::AddPropItem("des_start_pos.jgber_cal_style",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.datum_jg_h",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.datum_pos_style",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.datum_offset_dist",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.len_offset_dist",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.offset_zhun",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.huoqu_jg_h",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.huoqu_dist",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.datum_point",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.datum_point.x",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.datum_point.y",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.datum_point.z",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.projectDatumLineStartInfo",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.start_jg_h",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.iStartJgBerStyle",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.fStartJgLengOffsetX",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.fStartJgLengOffsetY",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.projectDatumLineEndInfo",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.end_jg_h",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.iEndJgBerStyle",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.fEndJgLengOffsetX",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.fEndJgLengOffsetY",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.cFaceOffsetDatumLine",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.face_offset_norm1",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.face_offset_norm1.x",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.face_offset_norm1.y",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.face_offset_norm1.z",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.bByOtherEndOffset1",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.face_offset_norm2",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.face_offset_norm2.x",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.face_offset_norm2.y",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.face_offset_norm2.z",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.bByOtherEndOffset2",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.datum_to_ber_style",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.work_wing_auto0_x1_y2_both3",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.offset_X_dist_style",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.offset_X_dist",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.offset_Y_dist_style",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.offset_Y_dist",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_start_pos.fEccentricDist",PROPLIST_ITEM(id++,""));*/
	CLDSLineAngle::AddPropItem("endInfo",PROPLIST_ITEM(id++,"�ն���Ϣ"));
	CLDSLineAngle::AddPropItem("endPos",PROPLIST_ITEM(id++,"�����յ��������"));
	CLDSLineAngle::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"�����յ�ʵ������"));
	CLDSLineAngle::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z"));

	CLDSLineAngle::AddPropItem("desEndPos.jgber_cal_style",PROPLIST_ITEM(id++,"��㶨λ��ʽ"));	//0.ֱ��ָ��������ꣻ1.��������λ
	//�򻯶�λģʽ
	CLDSLineAngle::AddPropItem("desEndPos.SimpleDatumPoint",PROPLIST_ITEM(id++,"��λ����"));	//0.ֱ��ָ��������ꣻ1.��������λ
	CLDSLineAngle::AddPropItem("desEndPos.SimpleDestLineMode",PROPLIST_ITEM(id++,"Ŀ���׼��"));	//0.ֱ��ָ��������ꣻ1.��������λ
	CLDSLineAngle::AddPropItem("desEndPos.SimpleDenpendWing",PROPLIST_ITEM(id++,"����֫"));	//0.ֱ��ָ��������ꣻ1.��������λ
	CLDSLineAngle::AddPropItem("desEndPos.SimpleDependRod",PROPLIST_ITEM(id++,"��������˼�"));	//0.ֱ��ָ��������ꣻ1.��������λ
	CLDSLineAngle::AddPropItem("desEndPos.SimpleLenOffsetType",PROPLIST_ITEM(id++,"����ƫ������"));		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	CLDSLineAngle::AddPropItem("desEndPos.SimpleLenOffset",PROPLIST_ITEM(id++,"����ƫ����"));		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	CLDSLineAngle::AddPropItem("desEndPos.SimpleNormOffset",PROPLIST_ITEM(id++,"����ƫ��"));	//0.ֱ��ָ��������ꣻ1.��������λ
	CLDSLineAngle::AddPropItem("desEndPos.SimpleBendLineAlongAngle",PROPLIST_ITEM(id++,"�����߻�׼�Ǹ�"));
	CLDSLineAngle::AddPropItem("desEndPos.SimpleBendAngleWing",PROPLIST_ITEM(id++,"��ǰ����֫"));
	CLDSLineAngle::AddPropItem("desEndPos.SimpleBendClearance",PROPLIST_ITEM(id++,"������϶"));
	CLDSLineAngle::AddPropItem("desEndPos.SimpleOffsetZhunju",PROPLIST_ITEM(id++,"ƫ��׼��"));
	CLDSLineAngle::AddPropItem("desEndPos.SimpleSelfWorkWing",PROPLIST_ITEM(id++,"������֫"));	//0.ֱ��ָ��������ꣻ1.��������λ
	CLDSLineAngle::AddPropItem("desEndPos.SimpleWingXTipCtrlPoint",PROPLIST_ITEM(id++,"X(��)֫��������"));
	CLDSLineAngle::AddPropItem("desEndPos.SimpleWingYTipCtrlPoint",PROPLIST_ITEM(id++,"Y(��)֫��������"));

	CLDSLineAngle::AddPropItem("desEndPos.origin_pos_style",PROPLIST_ITEM(id++,"��ʼ��λ��"));	//0.�ڵ�λ��;1.��׼��;2.��׼�Ǹֹؼ���;3.��һ�˽ڵ�����;4.��һ���������
	CLDSLineAngle::AddPropItem("desEndPos.datum_jg_h",PROPLIST_ITEM(id++,"���λ�û�׼�˼�"));	//���λ�û�׼�Ǹ�
	//����origin_pos_style=2ʱ��Ч
	CLDSLineAngle::AddPropItem("desEndPos.cDatumAngleWing",PROPLIST_ITEM(id++,"�ؼ������֫"));	//�Ǹ�'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
	CLDSLineAngle::AddPropItem("desEndPos.datumWingOffset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desEndPos.datumWingOffset.operStyle",PROPLIST_ITEM(id++,"�ؼ�����㷽ʽ"));
	CLDSLineAngle::AddPropItem("desEndPos.datumWingOffset.gStyle",PROPLIST_ITEM(id++,"����׼������"));
	CLDSLineAngle::AddPropItem("desEndPos.datumWingOffset.offsetDist",PROPLIST_ITEM(id++,"����׼��ֵ"));
	CLDSLineAngle::AddPropItem("desEndPos.datumPoint",PROPLIST_ITEM(id++,"����λ��"));
	CLDSLineAngle::AddPropItem("desEndPos.datumPoint.x",PROPLIST_ITEM(id++,"X����"));
	CLDSLineAngle::AddPropItem("desEndPos.datumPoint.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSLineAngle::AddPropItem("desEndPos.datumPoint.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSLineAngle::AddPropItem("desEndPos.spatialOperationStyle",PROPLIST_ITEM(id++,"���㶨λ��ʽ"));
	CLDSLineAngle::AddPropItem("desStartPos.bendingLenOffsetComplement",PROPLIST_ITEM(id++,"���������ƫ����"));
	CLDSLineAngle::AddPropItem("desEndPos.bendingLenOffsetComplement",PROPLIST_ITEM(id++,"���������ƫ����"));
	CLDSLineAngle::AddPropItem("desEndPos.LappedDatumWingG",PROPLIST_ITEM(id++,"����ľ�"));		
	CLDSLineAngle::AddPropItem("desEndPos.len_offset_type",PROPLIST_ITEM(id++,"����ƫ������"));		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	CLDSLineAngle::AddPropItem("desEndPos.len_offset_dist",PROPLIST_ITEM(id++,"����ƫ����"));		//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ػ�׼�Ǹ�ʼ�������췽���ƫ��
	CLDSLineAngle::AddPropItem("desEndPos.datum_offset_dist",PROPLIST_ITEM(id++,"���㷨��ƫ����"));	//��ӽǸ�ͶӰ��ʽʱ,��ӵ��Ի�׼֫�ط��ߵ�ƫ��
	CLDSLineAngle::AddPropItem("desEndPos.cFaceOffsetDatumLine",PROPLIST_ITEM(id++,"ƽ�����׼��"));
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,"ƽ���淨��"));
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm.x",PROPLIST_ITEM(id++,"X����"));	//ƽ���淨��X����
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm.y",PROPLIST_ITEM(id++,"Y����"));	//ƽ���淨��Y����
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm.z",PROPLIST_ITEM(id++,"Z����"));	//ƽ���淨��Z����
	CLDSLineAngle::AddPropItem("desEndPos.huoqu_jg_h",PROPLIST_ITEM(id++,"������׼�Ǹ�"));		//������׼�Ǹ־��
	CLDSLineAngle::AddPropItem("desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desEndPos.huoqu_dist",PROPLIST_ITEM(id++,"�����߼�϶"));		//�����߼�϶
	CLDSLineAngle::AddPropItem("desEndPos.offset_zhun",PROPLIST_ITEM(id++,"ƫ��׼��"));		//ƫ��׼�ߵ�ƫ����
	CLDSLineAngle::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,"ƫ�ľ�"));	//ƫ�ľ�==>���Ǹִ���ڸֹ���ʱ����Ҫ�˱��� wht 09-10-17
	//����spatialOperationStyle=15ʱ��Ч
	CLDSLineAngle::AddPropItem("desEndPos.coordCtrlType",PROPLIST_ITEM(id++,"����֫��������"));	//0:X-Z����;1:Y-Z����;X-Y����
	CLDSLineAngle::AddPropItem("desEndPos.cMainCoordCtrlWing",PROPLIST_ITEM(id++,"������Ƶ�"));	//�Ǹ�����֫ 'X'֫�����Ϲؼ���;'Y'֫�����Ϲؼ���;���������Ϲؼ���
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing",PROPLIST_ITEM(id++,""));				//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing.operStyle",PROPLIST_ITEM(id++,"���ص㶨λ��ʽ"));	//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing.gStyle",PROPLIST_ITEM(id++,"������׼������"));		//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing.offsetDist",PROPLIST_ITEM(id++,"������׼��"));	//�Ǹ���Ҫ����֫(�����������������֫)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing",PROPLIST_ITEM(id++,""));			//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing.operStyle",PROPLIST_ITEM(id++,"���ص㶨λ��ʽ"));	//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing.gStyle",PROPLIST_ITEM(id++,"������׼������"));		//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing.offsetDist",PROPLIST_ITEM(id++,"������׼��"));	//�Ǹָ�������֫(����һ�����������֫)
	CLDSLineAngle::AddPropItem("desEndPos.fLenOffsetOfCoordWing",PROPLIST_ITEM(id++,"���������λƫ��"));
	//����spatialOperationStyle=16,17ʱ��Ч
	CLDSLineAngle::AddPropItem("desEndPos.hDatumLine",PROPLIST_ITEM(id++,"��׼�߶���"));
	CLDSLineAngle::AddPropItem("desEndPos.cMirTransType",PROPLIST_ITEM(id++,"���նԳ�����"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos",PROPLIST_ITEM(id++,"��׼���������"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos.x",PROPLIST_ITEM(id++,"X���꣺"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos.y",PROPLIST_ITEM(id++,"Y���꣺"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos.z",PROPLIST_ITEM(id++,"Z���꣺"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos",PROPLIST_ITEM(id++,"��׼���յ�����"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos.x",PROPLIST_ITEM(id++,"X���꣺"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos.y",PROPLIST_ITEM(id++,"Y���꣺"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos.z",PROPLIST_ITEM(id++,"Z���꣺"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLineStart",PROPLIST_ITEM(id++,"ͶӰ��׼�����"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLineEnd",PROPLIST_ITEM(id++,"ͶӰ��׼���յ�"));
	//�ڶ���ƫ�Ʋ���(����->����ʸ��ƫ��)���������ڴ�Ӹ˼���(��)���λ�ü���
	CLDSLineAngle::AddPropItem("desEndPos.datum_to_ber_style",PROPLIST_ITEM(id++,"����->��㶨λ��ʽ"));
	//BYTE wingXGStyle,wingYGStyle;		//0.����ƫ��;1.X-Yƽ��ƽ��;2.Y-Zƽ��ƽ��;3.X-Zƽ��ƽ��;4.�Զ���ƽ����
	//double offset_x_dist,offset_y_dist;	//����gStyle=4ʱ��Ч
	CLDSLineAngle::AddPropItem("desEndPos.cWorkWingStyle",PROPLIST_ITEM(id++,"��λ����֫"));//�Ի������ƫ��ʱ��ǰ�ǸֵĻ�׼֫0:�Զ��ж�;1:X֫;2:Y֫;3:˫֫;�����Զ��ж�
	CLDSLineAngle::AddPropItem("desEndPos.wing_x_offset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desEndPos.wing_x_offset.gStyle",PROPLIST_ITEM(id++,"֫������ƫ�Ʒ�ʽ"));
	CLDSLineAngle::AddPropItem("desEndPos.wing_x_offset.offsetDist",PROPLIST_ITEM(id++,"֫������ƫ����"));
	CLDSLineAngle::AddPropItem("desEndPos.wing_y_offset",PROPLIST_ITEM(id++,""));	//��ʱ����WING_OFFSET_STYLE::operStyle����
	CLDSLineAngle::AddPropItem("desEndPos.wing_y_offset.gStyle",PROPLIST_ITEM(id++,"Y֫������ƫ�Ʒ�ʽ"));
	CLDSLineAngle::AddPropItem("desEndPos.wing_y_offset.offsetDist",PROPLIST_ITEM(id++,"Y֫������ƫ����"));
	/*CLDSLineAngle::AddPropItem("des_end_pos.jgber_cal_style",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.datum_jg_h",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.datum_pos_style",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.datum_offset_dist",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.len_offset_dist",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.offset_zhun",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.huoqu_jg_h",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.huoqu_dist",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.datum_point",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.datum_point.x",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.datum_point.y",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.datum_point.z",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.projectDatumLineStartInfo",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.start_jg_h",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.iStartJgBerStyle",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.fStartJgLengOffsetX",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.fStartJgLengOffsetY",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.projectDatumLineEndInfo",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.end_jg_h",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.iEndJgBerStyle",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.fEndJgLengOffsetX",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.fEndJgLengOffsetY",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.face_offset_norm1",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.face_offset_norm1.x",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.face_offset_norm1.y",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.face_offset_norm1.z",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.bByOtherEndOffset1",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.cFaceOffsetDatumLine",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.face_offset_norm2",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.face_offset_norm2.x",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.face_offset_norm2.y",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.face_offset_norm2.z",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.bByOtherEndOffset2",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.datum_to_ber_style",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.work_wing_auto0_x1_y2_both3",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.offset_X_dist_style",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.offset_X_dist",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.offset_Y_dist_style",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.offset_Y_dist",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("des_end_pos.fEccentricDist",PROPLIST_ITEM(id++,""));*/

	CLDSLineAngle::AddPropItem("oddStart",PROPLIST_ITEM(id++,"����ͷ"));
	CLDSLineAngle::AddPropItem("oddStart.m_iOddCalStyle",PROPLIST_ITEM(id++,"����ԭ��"));	//0.��ʾ������ײ���ո˼�����;1.������˨λ�ü���;2.��ʾ����������ͷ
	CLDSLineAngle::AddPropItem("oddStart.m_fCollideDist",PROPLIST_ITEM(id++,"��ײ��϶"));
	CLDSLineAngle::AddPropItem("oddStart.m_hRefPart1",PROPLIST_ITEM(id++,"���ո˼�1"));
	CLDSLineAngle::AddPropItem("oddStart.m_hRefPart2",PROPLIST_ITEM(id++,"���ո˼�2"));
	CLDSLineAngle::AddPropItem("oddStartExtra",PROPLIST_ITEM(id++,"�̶�����ͷ"));
	CLDSLineAngle::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"����ͷ"));
	CLDSLineAngle::AddPropItem("oddEnd.m_iOddCalStyle",PROPLIST_ITEM(id++,"����ԭ��"));	//0.��ʾ������ײ���ո˼�����;1.������˨λ�ü���;2.��ʾ����������ͷ
	CLDSLineAngle::AddPropItem("oddEnd.m_fCollideDist",PROPLIST_ITEM(id++,"��ײ��϶"));
	CLDSLineAngle::AddPropItem("oddEnd.m_hRefPart1",PROPLIST_ITEM(id++,"���ո˼�1"));
	CLDSLineAngle::AddPropItem("oddEnd.m_hRefPart2",PROPLIST_ITEM(id++,"���ո˼�2"));
	CLDSLineAngle::AddPropItem("oddEndExtra",PROPLIST_ITEM(id++,"�̶�����ͷ"));
	CLDSLineAngle::AddPropItem("StartOddCanAcrossMiddleSect",PROPLIST_ITEM(id++,"��ͷ�ɿ���е�","","0.������|1.����"));
	CLDSLineAngle::AddPropItem("EndOddCanAcrossMiddleSect",PROPLIST_ITEM(id++,"��ͷ�ɿ���е�","","0.������|1.����"));
	CLDSLineAngle::AddPropItem("linkInfo",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("startBoltInfo",PROPLIST_ITEM(id++,"Ԥ����˨��Ϣ"));
	CLDSLineAngle::AddPropItem("connectStart.grade",PROPLIST_ITEM(id++,"����"));
	CLDSLineAngle::AddPropItem("connectStart.d",PROPLIST_ITEM(id++,"��˨���"));
	CLDSLineAngle::AddPropItem("connectStart.m_iConnectWing",PROPLIST_ITEM(id++,"����֫"));
	CLDSLineAngle::AddPropItem("connectStart.N",PROPLIST_ITEM(id++,"������˨��"));
	CLDSLineAngle::AddPropItem("connectStart.summBoltN",PROPLIST_ITEM(id++,"������˨��"));
	CLDSLineAngle::AddPropItem("connectStart.rows",PROPLIST_ITEM(id++,"��˨����"));
	CLDSLineAngle::AddPropItem("endBoltInfo",PROPLIST_ITEM(id++,"Ԥ����˨��Ϣ"));
	CLDSLineAngle::AddPropItem("connectEnd.grade",PROPLIST_ITEM(id++,"����"));
	CLDSLineAngle::AddPropItem("connectEnd.d",PROPLIST_ITEM(id++,"��˨���"));
	CLDSLineAngle::AddPropItem("connectEnd.m_iConnectWing",PROPLIST_ITEM(id++,"����֫"));
	CLDSLineAngle::AddPropItem("connectEnd.N",PROPLIST_ITEM(id++,"������˨��"));
	CLDSLineAngle::AddPropItem("connectEnd.summBoltN",PROPLIST_ITEM(id++,"������˨��"));
	CLDSLineAngle::AddPropItem("connectEnd.rows",PROPLIST_ITEM(id++,"��˨����"));
	CLDSLineAngle::AddPropItem("AttachBolts",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("startAttachBolts",PROPLIST_ITEM(id++,"�Ѳ���˨"));
	CLDSLineAngle::AddPropItem("midAttachBolts",PROPLIST_ITEM(id++,"�Ѳ��м���˨��"));
	CLDSLineAngle::AddPropItem("endAttachBolts",PROPLIST_ITEM(id++,"�Ѳ���˨"));
	CLDSLineAngle::AddPropItem("wingNormInfo",PROPLIST_ITEM(id++,"��֫������Ϣ"));
	CLDSLineAngle::AddPropItem("norm_x_wing",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("norm_x_wing.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("norm_x_wing.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("norm_x_wing.z",PROPLIST_ITEM(id++,"Z"));
	CLDSLineAngle::AddPropItem("norm_x_wing.hViceJg",PROPLIST_ITEM(id++,"X֫������ո˼�"));
	CLDSLineAngle::AddPropItem("norm_y_wing",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("norm_y_wing.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("norm_y_wing.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("norm_y_wing.z",PROPLIST_ITEM(id++,"Z"));
	CLDSLineAngle::AddPropItem("norm_y_wing.hViceJg",PROPLIST_ITEM(id++,"Y֫������ո˼�"));
	CLDSLineAngle::AddPropItem("work_wing",PROPLIST_ITEM(id++,"����֫���߷���"));
	CLDSLineAngle::AddPropItem("work_wing.hViceJg",PROPLIST_ITEM(id++,"������ո˼�"));
	CLDSLineAngle::AddPropItem("XorYMirLayStyle",PROPLIST_ITEM(id++,"X/Y���������ԳƷ�ʽ"));
	CLDSLineAngle::AddPropItem("blScatterBoltLocaTolerance",PROPLIST_ITEM(id++,"��λ����","","0.��֤���|1.��֤��λ"));
	CLDSLineAngle::AddPropItem("decWingAngle",PROPLIST_ITEM(id++,"��֫�н�"));
	CLDSLineAngle::AddPropItem("start_wing_plane",PROPLIST_ITEM(id++,"�Ǹ�֫ƽ��"));
	CLDSLineAngle::AddPropItem("end_wing_plane",PROPLIST_ITEM(id++,"�Ǹ�֫ƽ��"));
	CLDSLineAngle::AddPropItem("kaihe_base_wing_x0_y1",PROPLIST_ITEM(id++,"����֫"));
	CLDSLineAngle::AddPropItem("m_cHeJiaoReservedWidthType",PROPLIST_ITEM(id++,"�ϽǱ����������"));
	CLDSLineAngle::AddPropItem("m_fHeJiaoReservedWidth",PROPLIST_ITEM(id++,"�ϽǱ������"));
	CLDSLineAngle::AddPropItem("SetupPosInfo",PROPLIST_ITEM(id++,"��װ��λ��Ϣ"));
	CLDSLineAngle::AddPropItem("wingXG",PROPLIST_ITEM(id++,"X(��)ִ֫��׼��"));
	CLDSLineAngle::AddPropItem("wingYG",PROPLIST_ITEM(id++,"Y(��)ִ֫��׼��"));
	CLDSLineAngle::AddPropItem("m_bEnableTeG",PROPLIST_ITEM(id++,"��������׼��"));
	CLDSLineAngle::AddPropItem("teGTable",PROPLIST_ITEM(id++,"����׼���"));
	CLDSLineAngle::AddPropItem("m_cPosCtrlDatumLineType",PROPLIST_ITEM(id++,"��Ӷ�λ��׼"));
	CLDSLineAngle::AddPropItem("m_cPosCtrlType",PROPLIST_ITEM(id++,"��ӵ㶨λ��ʽ"));
	CLDSLineAngle::AddPropItem("startCutWing",PROPLIST_ITEM(id++,"ʼ���н���֫"));
	CLDSLineAngle::AddPropItem("startCutWingX",PROPLIST_ITEM(id++,"X֫"));
	CLDSLineAngle::AddPropItem("startCutWingY",PROPLIST_ITEM(id++,"Y֫"));
	/*
	CLDSLineAngle::AddPropItem("start_cut_angle_type",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("m_bUserSpecStartCutAngle",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("cut_angle[0][0]",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("cut_angle[0][1]",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("cut_angle[1][0]",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("cut_angle[1][1]",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("cut_wing_para[0][0]",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("cut_wing_para[0][1]",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("cut_wing_para[0][2]",PROPLIST_ITEM(id++,""));
	*/
	CLDSLineAngle::AddPropItem("endCutWing",PROPLIST_ITEM(id++,"�ն��н���֫"));
	CLDSLineAngle::AddPropItem("endCutWingX",PROPLIST_ITEM(id++,"X֫"));
	CLDSLineAngle::AddPropItem("endCutWingY",PROPLIST_ITEM(id++,"Y֫"));
	/*
	CLDSLineAngle::AddPropItem("m_bUserSpecEndCutAngle",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("end_cut_angle_type",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("cut_angle[2][0]",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("cut_angle[2][1]",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("cut_angle[3][0]",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("cut_angle[3][1]",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("cut_wing_para[1][0]",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("cut_wing_para[1][1]",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("cut_wing_para[1][2]",PROPLIST_ITEM(id++,""));
	*/
	CLDSLineAngle::AddPropItem("start_push_flat_x1_y2",PROPLIST_ITEM(id++,"ʼ��ѹ��"));
	CLDSLineAngle::AddPropItem("start_push_flat_length",PROPLIST_ITEM(id++,"ѹ�ⳤ��"));
	CLDSLineAngle::AddPropItem("end_push_flat_x1_y2",PROPLIST_ITEM(id++,"�ն�ѹ��"));
	CLDSLineAngle::AddPropItem("end_push_flat_length",PROPLIST_ITEM(id++,"ѹ�ⳤ��"));
	CLDSLineAngle::AddPropItem("mid_push_flat_x1_y2",PROPLIST_ITEM(id++,"�м�ѹ��"));
	CLDSLineAngle::AddPropItem("mid_push_flat_length",PROPLIST_ITEM(id++,"ѹ�ⳤ��"));
	CLDSLineAngle::AddPropItem("mid_push_flat_start_offset",PROPLIST_ITEM(id++,"ʼ��ƫ����"));
	CLDSLineAngle::AddPropItem("mid_push_flat_end_offset",PROPLIST_ITEM(id++,"�ն�ƫ����"));
	CLDSLineAngle::AddPropItem("mid_push_flat_node_handle",PROPLIST_ITEM(id++,"ѹ��λ�ýڵ�"));
	CLDSLineAngle::AddPropItem("m_fCutAngleCoefR",PROPLIST_ITEM(id++,"�ڲ��нǼ�϶"));
	CLDSLineAngle::AddPropItem("statMatInfo",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("manuProcess",PROPLIST_ITEM(id++,"������Ϣ"));
	CLDSLineAngle::AddPropItem("m_bCutRoot",PROPLIST_ITEM(id++,"���"));
	CLDSLineAngle::AddPropItem("m_bCutBer",PROPLIST_ITEM(id++,"����"));
	CLDSLineAngle::AddPropItem("m_bBendPart",PROPLIST_ITEM(id++,"����"));
	CLDSLineAngle::AddPropItem("m_bWeldPart",PROPLIST_ITEM(id++,"����"));
	CLDSLineAngle::AddPropItem("FilletArcS",PROPLIST_ITEM(id++,"ʼ�˵�Բ��"));
	CLDSLineAngle::AddPropItem("FilletROfSX",PROPLIST_ITEM(id++,"X֫�뾶R"));
	CLDSLineAngle::AddPropItem("FilletROfSY",PROPLIST_ITEM(id++,"Y֫�뾶R"));
	CLDSLineAngle::AddPropItem("FilletArcE",PROPLIST_ITEM(id++,"�ն˵�Բ��"));
	CLDSLineAngle::AddPropItem("FilletROfEX",PROPLIST_ITEM(id++,"X֫�뾶R"));
	CLDSLineAngle::AddPropItem("FilletROfEY",PROPLIST_ITEM(id++,"Y֫�뾶R"));
	CLDSLineAngle::AddPropItem("OpenSlotGroup",PROPLIST_ITEM(id++,"����"));
	CLDSLineAngle::AddPropItem("OpenSlot1",PROPLIST_ITEM(id++,"����1"));
	CLDSLineAngle::AddPropItem("OpenSlot2",PROPLIST_ITEM(id++,"����2"));
	CLDSLineAngle::AddPropItem("OpenSlot3",PROPLIST_ITEM(id++,"����3"));
	CLDSLineAngle::AddPropItem("OpenSlot4",PROPLIST_ITEM(id++,"����4"));
	//������������Ҫ������
	CLDSLineAngle::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"ͳ�ı��"));	//ͳ�ı��
	CLDSLineAngle::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"�����Ŵ�ϵ��"));
	CLDSLineAngle::AddPropItem("size.idClassType",PROPLIST_ITEM(id++,""));
	//�����Ϣ
	CLDSLineAngle::AddPropItem("designInfo",PROPLIST_ITEM(id++,"�˼�������Ϣ"));
	CLDSLineAngle::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"��Ԫ����"));
	CLDSLineAngle::AddPropItem("sel_mat_method",PROPLIST_ITEM(id++,"������������"));
	CLDSLineAngle::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"(��ע��)�Ƿ���в�����ѡ"));
	CLDSLineAngle::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"�Ƿ���й����ѡ"));
	CLDSLineAngle::AddPropItem("hole_out",PROPLIST_ITEM(id++,"��˨������"));
	CLDSLineAngle::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"�Զ�������˨������"));
	//Ԥ����
	CLDSLineAngle::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"Ԥ����"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift",PROPLIST_ITEM(id++,"ʼ��Ԥ��ǰ����"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift.z",PROPLIST_ITEM(id++,"Z"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift",PROPLIST_ITEM(id++,"�ն�Ԥ��ǰ����"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift.z",PROPLIST_ITEM(id++,"Z"));
		//���㳤����Ϣ
	CLDSLineAngle::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,"����ϵ��"));
	CLDSLineAngle::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,"���㳤������"));
	CLDSLineAngle::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,"���պ�����"));
	CLDSLineAngle::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,"ͬʱ��ѹ���ո˼�"));
	CLDSLineAngle::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,"��С�᳤��"));
	CLDSLineAngle::AddPropItem("CalLenCoef.minR.hStartNode",PROPLIST_ITEM(id++,"��ʼ�ڵ�"));
	CLDSLineAngle::AddPropItem("CalLenCoef.minR.hEndNode",PROPLIST_ITEM(id++,"��ֹ�ڵ�"));
	CLDSLineAngle::AddPropItem("CalLenCoef.minR.coef",PROPLIST_ITEM(id++,"����ϵ��"));
	CLDSLineAngle::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,"ƽ���᳤��"));
	CLDSLineAngle::AddPropItem("CalLenCoef.paraR.hStartNode",PROPLIST_ITEM(id++,"��ʼ�ڵ�"));
	CLDSLineAngle::AddPropItem("CalLenCoef.paraR.hEndNode",PROPLIST_ITEM(id++,"��ֹ�ڵ�"));
	CLDSLineAngle::AddPropItem("CalLenCoef.paraR.coef",PROPLIST_ITEM(id++,"����ϵ��"));
		//ʼ�������Ϣ
	CLDSLineAngle::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"ʼ����Ϣ"));
	CLDSLineAngle::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"ʼ��Լ������"));
	CLDSLineAngle::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"ʼ����������"));
	CLDSLineAngle::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
		//�ն������Ϣ
	CLDSLineAngle::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"�ն���Ϣ"));
	CLDSLineAngle::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"�ն�Լ������"));
	CLDSLineAngle::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"�ն���������"));
	CLDSLineAngle::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,""));
#endif
	InitPropMirBitsMap();
	return id;
}
CHashStrList<PROPLIST_ITEM> CLDSGroupLineAngle::propHashtable;
CXhChar100 CLDSGroupLineAngle::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*> CLDSGroupLineAngle::hashPropItemById;
CHashTable<DWORD> CLDSGroupLineAngle::propStatusHashtable;
int CLDSGroupLineAngle::m_iCurDisplayPropGroup;
void CLDSGroupLineAngle::InitGroupLineAnglePropHashtable()
{
	CLDSGroupLineAngle::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);//CreateHashTable(HASHTABLESIZE);
	CLDSGroupLineAngle::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	int id=CLDSLineAngle::InitLineAnglePropHashtable();
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSGroupLineAngle::AddPropItem("group_style",PROPLIST_ITEM(id++,"Group Style"));
	CLDSGroupLineAngle::AddPropItem("jg_space",PROPLIST_ITEM(id++,"Group Clearance"));
	CLDSGroupLineAngle::AddPropItem("m_bUpdateSonAngleOddment",PROPLIST_ITEM(id++,"Update Child Angel Odd Length"));
	CLDSGroupLineAngle::AddPropItem("ciMasterIndex",PROPLIST_ITEM(id++,"Same direct angle"));
	CLDSGroupLineAngle::AddPropItem("ciMasterDirectIndex",PROPLIST_ITEM(id++,"Direct relation of angles"));
#else 
	CLDSGroupLineAngle::AddPropItem("group_style",PROPLIST_ITEM(id++,"�����ʽ"));
	CLDSGroupLineAngle::AddPropItem("jg_space",PROPLIST_ITEM(id++,"��ϼ�϶"));
	CLDSGroupLineAngle::AddPropItem("m_bUpdateSonAngleOddment",PROPLIST_ITEM(id++,"�����ӽǸ�����ͷ"));
	CLDSGroupLineAngle::AddPropItem("ciMasterIndex",PROPLIST_ITEM(id++,"ͬ���ӽǸ�"));
	CLDSGroupLineAngle::AddPropItem("ciMasterDirectIndex",PROPLIST_ITEM(id++,"֫�����ϵ"));
#endif
}

CHashStrList<PROPLIST_ITEM> CLDSPolyJoint::propHashtable;
CXhChar100 CLDSPolyJoint::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*>CLDSPolyJoint::hashPropItemById;
CHashTable<DWORD> CLDSPolyJoint::propStatusHashtable;
int CLDSPolyJoint::m_iCurDisplayPropGroup;
void CLDSPolyJoint::InitPolyJointPropHashtable()
{
	int id=1;
	CLDSPolyJoint::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSPolyJoint::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
	CLDSPolyJoint::AddPropItem("basicInfo",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("handle",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("layer",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("cMaterial",PROPLIST_ITEM(id++,""));
	//CLDSPolyJoint::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade",NULL,"A|B|C|D|"));
	CLDSPolyJoint::AddPropItem("specification",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("iSeg",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("sPartNo",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("cfgword",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("pPrevLinePart",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("pNextLinePart",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("rBending",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("crMaterial",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("sNotes",PROPLIST_ITEM(id++,""));
	CLDSPolyJoint::AddPropItem("huoqu_base_wing_x0_y1_both2",PROPLIST_ITEM(id++,""));
}

CHashStrList<PROPLIST_ITEM> CLDSLineSlot::propHashtable;
CXhChar100 CLDSLineSlot::HELPLINK_URL;
CHashSet <PROPLIST_ITEM*> CLDSLineSlot::hashPropItemById;
CHashTable<DWORD> CLDSLineSlot::propStatusHashtable;
int CLDSLineSlot::m_iCurDisplayPropGroup;
void CLDSLineSlot::InitLineSlotPropHashtable()
{
	int id=1;
	CLDSLineSlot::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSLineSlot::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSLineSlot::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"[ steel Basic Information"));
	CLDSLineSlot::AddPropItem("handle",PROPLIST_ITEM(id++,"handle"));
	CLDSLineSlot::AddPropItem("layer",PROPLIST_ITEM(id++,"layer"));
	CLDSLineSlot::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	CLDSLineSlot::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade",NULL,"Default|A|B|C|D|"));
	CLDSLineSlot::AddPropItem("specification",PROPLIST_ITEM(id++,"specification"));
	CLDSLineSlot::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSLineSlot::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Number"));
	CLDSLineSlot::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSLineSlot::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"Relative Object"));
	CLDSLineSlot::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//Ӱ����Ϣ
	CLDSLineSlot::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Shadow Mother Object"));//Ӱ��ĸ����
	CLDSLineSlot::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//�����Ĳ�������
	CLDSLineSlot::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block"));//�����Ĳ���
	CLDSLineSlot::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSLineSlot::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"));
	CLDSLineSlot::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	CLDSLineSlot::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Auxiliary Part"));
	CLDSLineSlot::AddPropItem("width",PROPLIST_ITEM(id++,"Width"));
	CLDSLineSlot::AddPropItem("thick",PROPLIST_ITEM(id++,"thick"));
	CLDSLineSlot::AddPropItem("height",PROPLIST_ITEM(id++,"height"));
	CLDSLineSlot::AddPropItem("length",PROPLIST_ITEM(id++,"length"));
	CLDSLineSlot::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color"));
	CLDSLineSlot::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	CLDSLineSlot::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
	CLDSLineSlot::AddPropItem("specialSlot",PROPLIST_ITEM(id++,"Special Position Parameter"));
	CLDSLineSlot::AddPropItem("hAttachTube",PROPLIST_ITEM(id++,"Attached Tube"));
	CLDSLineSlot::AddPropItem("fLen",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desDatumPoint",PROPLIST_ITEM(id++,"Location Postion"));
	CLDSLineSlot::AddPropItem("desDatumPoint.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineSlot::AddPropItem("desDatumPoint.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineSlot::AddPropItem("desDatumPoint.z",PROPLIST_ITEM(id++,"Z"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.hOffsetDatumNode",PROPLIST_ITEM(id++,"Offset from node"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.lenOffsetType",PROPLIST_ITEM(id++,"Offset Direction"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.lenOffset",PROPLIST_ITEM(id++,"Offset Distance"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVecStyle",PROPLIST_ITEM(id++,"Ray Vector"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVecStdStyle",PROPLIST_ITEM(id++,"Specify"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVec.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVec.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVec.z",PROPLIST_ITEM(id++,"Z"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVecRotateAngle",PROPLIST_ITEM(id++,"Rotate Angle"));
	CLDSLineSlot::AddPropItem("workPlaneNorm",PROPLIST_ITEM(id++,"Work Plane Normal"));
	CLDSLineSlot::AddPropItem("hWorkNormRefPart",PROPLIST_ITEM(id++,"Normal Reference Rod"));
	CLDSLineSlot::AddPropItem("workPlaneNorm.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSLineSlot::AddPropItem("workPlaneNorm.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSLineSlot::AddPropItem("workPlaneNorm.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSLineSlot::AddPropItem("nearWorkPlaneNorm",PROPLIST_ITEM(id++,"Datum Plane Near Normal"));
	CLDSLineSlot::AddPropItem("m_iNormCalStyle",PROPLIST_ITEM(id++,"Datum Plane Near Normal"));
	CLDSLineSlot::AddPropItem("nearWorkPlaneNorm.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSLineSlot::AddPropItem("nearWorkPlaneNorm.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSLineSlot::AddPropItem("nearWorkPlaneNorm.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSLineSlot::AddPropItem("startInfo",PROPLIST_ITEM(id++,"Start Information"));
	CLDSLineSlot::AddPropItem("hStart",PROPLIST_ITEM(id++,"Start Handle"));
	CLDSLineSlot::AddPropItem("oddStart",PROPLIST_ITEM(id++,"Rod Oddment"));
	CLDSLineSlot::AddPropItem("startPos",PROPLIST_ITEM(id++,"Start Design Coordinate"));
	CLDSLineSlot::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"Start Actual Coordinate"));
	CLDSLineSlot::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSLineSlot::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSLineSlot::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSLineSlot::AddPropItem("desStartPos",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desStartPos.endPosType",PROPLIST_ITEM(id++,"Locate Mode"));
	CLDSLineSlot::AddPropItem("desStartPos.datum_pos_style",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desStartPos.hDatumPart",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desStartPos.fNormOffset",PROPLIST_ITEM(id++,"Normal Offset"));
	CLDSLineSlot::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"Offset Distance"));
	CLDSLineSlot::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"Offset Normal"));
	CLDSLineSlot::AddPropItem("desStartPos.fTransverseOffset",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desStartPos.hDatumStartPart",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desStartPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desStartPos.hDatumEndPart",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desStartPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("endInfo",PROPLIST_ITEM(id++,"End Information"));
	CLDSLineSlot::AddPropItem("hEnd",PROPLIST_ITEM(id++,"End Handle"));
	CLDSLineSlot::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"Rod Oddment"));
	CLDSLineSlot::AddPropItem("endPos",PROPLIST_ITEM(id++,"[ steel End Design Coordinate"));
	CLDSLineSlot::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"End Actual Coordinate"));
	CLDSLineSlot::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSLineSlot::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSLineSlot::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSLineSlot::AddPropItem("desEndPos",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.endPosType",PROPLIST_ITEM(id++,"Locate Mode"));
	CLDSLineSlot::AddPropItem("desEndPos.datum_pos_style",PROPLIST_ITEM(id++,"Datum Point Locate Mode"));
	CLDSLineSlot::AddPropItem("desEndPos.hDatumPart",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.fNormOffset",PROPLIST_ITEM(id++,"Normal Offset"));
	CLDSLineSlot::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,"Offset Distance"));
	CLDSLineSlot::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,"Offset Normal"));
	CLDSLineSlot::AddPropItem("desEndPos.fTransverseOffset",PROPLIST_ITEM(id++,"Lateral Offset"));
	CLDSLineSlot::AddPropItem("desEndPos.hDatumStartPart",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"Datum Endpoint"));
	CLDSLineSlot::AddPropItem("desEndPos.hDatumEndPart",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,""));
	//������������Ҫ������
	CLDSLineSlot::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,""));	//ͳ�ı��
	CLDSLineSlot::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,""));
	//�����Ϣ
	CLDSLineSlot::AddPropItem("designInfo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("hole_out",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,""));
	//���㳤����Ϣ
	CLDSLineSlot::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,""));
	//ʼ�������Ϣ
	CLDSLineSlot::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("start_force_type",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
	//�ն������Ϣ
	CLDSLineSlot::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("end_force_type",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,""));
#else
	CLDSLineSlot::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"�۸ֻ�����Ϣ"));
	CLDSLineSlot::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSLineSlot::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSLineSlot::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	CLDSLineSlot::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�",NULL,"Ĭ��|A|B|C|D|"));
	CLDSLineSlot::AddPropItem("specification",PROPLIST_ITEM(id++,"���"));
	CLDSLineSlot::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSLineSlot::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	CLDSLineSlot::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSLineSlot::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"��������"));
	CLDSLineSlot::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Ӱ����Ϣ"));//Ӱ����Ϣ
	CLDSLineSlot::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Ӱ��ĸ����"));//Ӱ��ĸ����
	CLDSLineSlot::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"�����Ĳ�������"));//�����Ĳ�������
	CLDSLineSlot::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"�����Ĳ���"));//�����Ĳ���
	CLDSLineSlot::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	CLDSLineSlot::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����"));
	CLDSLineSlot::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	CLDSLineSlot::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"���⸨������"));
	CLDSLineSlot::AddPropItem("width",PROPLIST_ITEM(id++,"���"));
	CLDSLineSlot::AddPropItem("thick",PROPLIST_ITEM(id++,"���"));
	CLDSLineSlot::AddPropItem("height",PROPLIST_ITEM(id++,"�߶�"));
	CLDSLineSlot::AddPropItem("length",PROPLIST_ITEM(id++,"ʵ�ʳ���"));
	CLDSLineSlot::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ"));
	CLDSLineSlot::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	CLDSLineSlot::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
	CLDSLineSlot::AddPropItem("specialSlot",PROPLIST_ITEM(id++,"����۸ֶ�λ����"));
	CLDSLineSlot::AddPropItem("hAttachTube",PROPLIST_ITEM(id++,"�����ֹ�"));
	CLDSLineSlot::AddPropItem("fLen",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desDatumPoint",PROPLIST_ITEM(id++,"��λ��׼��"));
	CLDSLineSlot::AddPropItem("desDatumPoint.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSLineSlot::AddPropItem("desDatumPoint.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSLineSlot::AddPropItem("desDatumPoint.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.hOffsetDatumNode",PROPLIST_ITEM(id++,"ƫ����ʼ�ڵ�"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.lenOffsetType",PROPLIST_ITEM(id++,"����ƫ�Ʒ���"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.lenOffset",PROPLIST_ITEM(id++,"����ƫ����"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVecStyle",PROPLIST_ITEM(id++,"���߷���"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVecStdStyle",PROPLIST_ITEM(id++,"ָ������"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVec.x",PROPLIST_ITEM(id++,"���߷���X����"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVec.y",PROPLIST_ITEM(id++,"���߷���Y����"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVec.z",PROPLIST_ITEM(id++,"���߷���Z����"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVecRotateAngle",PROPLIST_ITEM(id++,"��ת�Ƕ�"));
	CLDSLineSlot::AddPropItem("workPlaneNorm",PROPLIST_ITEM(id++,"��׼�淨��"));
	CLDSLineSlot::AddPropItem("hWorkNormRefPart",PROPLIST_ITEM(id++,"���߲��ո˼�"));
	CLDSLineSlot::AddPropItem("workPlaneNorm.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSLineSlot::AddPropItem("workPlaneNorm.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSLineSlot::AddPropItem("workPlaneNorm.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSLineSlot::AddPropItem("nearWorkPlaneNorm",PROPLIST_ITEM(id++,"��׼����Ʒ���"));
	CLDSLineSlot::AddPropItem("m_iNormCalStyle",PROPLIST_ITEM(id++,"���㷽ʽ"));
	CLDSLineSlot::AddPropItem("nearWorkPlaneNorm.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSLineSlot::AddPropItem("nearWorkPlaneNorm.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSLineSlot::AddPropItem("nearWorkPlaneNorm.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSLineSlot::AddPropItem("startInfo",PROPLIST_ITEM(id++,"ʼ����Ϣ"));
	CLDSLineSlot::AddPropItem("hStart",PROPLIST_ITEM(id++,"�����"));
	CLDSLineSlot::AddPropItem("oddStart",PROPLIST_ITEM(id++,"����ͷ"));
	CLDSLineSlot::AddPropItem("startPos",PROPLIST_ITEM(id++,"ʼ���������"));
	CLDSLineSlot::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"ʼ��ʵ������"));
	CLDSLineSlot::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X����"));
	CLDSLineSlot::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSLineSlot::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSLineSlot::AddPropItem("desStartPos",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desStartPos.endPosType",PROPLIST_ITEM(id++,"��λ��ʽ"));
	CLDSLineSlot::AddPropItem("desStartPos.datum_pos_style",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desStartPos.hDatumPart",PROPLIST_ITEM(id++,"���λ�û�׼�Ǹ�"));
	CLDSLineSlot::AddPropItem("desStartPos.fNormOffset",PROPLIST_ITEM(id++,"��׼�淨��ƫ����"));
	CLDSLineSlot::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"ƫ�ľ�"));
	CLDSLineSlot::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"ƽ���淨��"));
	CLDSLineSlot::AddPropItem("desStartPos.fTransverseOffset",PROPLIST_ITEM(id++,"����ƫ����"));
	CLDSLineSlot::AddPropItem("desStartPos.hDatumStartPart",PROPLIST_ITEM(id++,"ͶӰ��ʼ�����ڻ�׼�˼�"));
	CLDSLineSlot::AddPropItem("desStartPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"��׼��ֵĻ�׼�˵�"));
	CLDSLineSlot::AddPropItem("desStartPos.hDatumEndPart",PROPLIST_ITEM(id++,"ͶӰ���ն����ڻ�׼�˼�"));
	CLDSLineSlot::AddPropItem("desStartPos.bDatumEndStartEnd", PROPLIST_ITEM(id++, ""));
	CLDSLineSlot::AddPropItem("endInfo",PROPLIST_ITEM(id++,"�ն���Ϣ"));
	CLDSLineSlot::AddPropItem("hEnd",PROPLIST_ITEM(id++,"�յ���"));
	CLDSLineSlot::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"����ͷ"));
	CLDSLineSlot::AddPropItem("endPos",PROPLIST_ITEM(id++,"�ն��������"));
	CLDSLineSlot::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"�ն�ʵ������"));
	CLDSLineSlot::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X����"));
	CLDSLineSlot::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSLineSlot::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSLineSlot::AddPropItem("desEndPos",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.endPosType",PROPLIST_ITEM(id++,"��λ��ʽ"));
	CLDSLineSlot::AddPropItem("desEndPos.datum_pos_style",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.hDatumPart",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.fNormOffset",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.fTransverseOffset",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.hDatumStartPart",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.hDatumEndPart",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,""));
	//������������Ҫ������
	CLDSLineSlot::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,""));	//ͳ�ı��
	CLDSLineSlot::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,""));
	//�����Ϣ
	CLDSLineSlot::AddPropItem("designInfo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("hole_out",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,""));
	//���㳤����Ϣ
	CLDSLineSlot::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,""));
	//ʼ�������Ϣ
	CLDSLineSlot::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("start_force_type",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
	//�ն������Ϣ
	CLDSLineSlot::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("end_force_type",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,""));
#endif
}

MEMORY_BLOCK CLDSGeneralPlate::MPCL;	//Mirror Property Control List
PROPLIST_TYPE CLDSGeneralPlate::propMirSync;
CHashList<PLATEEDIT_STATE> CLDSGeneralPlate::m_hashDisplayPlateStates;
void CLDSGeneralPlate::InitPropMirBitsMap()
{
	int id=1;
	propMirSync.InitHashtableSize(32);
	MPCL.SetBitState(id-1);
#ifdef AFX_TARG_ENU_ENGLISH
	propMirSync.AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	//propMirSync.AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("specification",PROPLIST_ITEM(id++,"Specification"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	propMirSync.AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Label"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cfgword",PROPLIST_ITEM(id++,"Model Flag"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("Profile",PROPLIST_ITEM(id++,"Profile"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("thick",PROPLIST_ITEM(id++,"Thickness"));
#else 
	propMirSync.AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	//propMirSync.AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("specification",PROPLIST_ITEM(id++,"���"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	propMirSync.AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("Profile",PROPLIST_ITEM(id++,"����"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("thick",PROPLIST_ITEM(id++,"���"));
#endif
}

long CLDSParamPlate::m_idMinPropUdf;						//��ʹ�õ���С�û��Զ�������Id
CHashPtrList<DYNA_PROPITEM_GROUP> CLDSParamPlate::hashParamItemPropGroup;
IMPLEMENT_PROP_FUNC(CLDSParamPlate);
void CLDSParamPlate::InitPropHashtable()
{
	int id=1;
	CLDSParamPlate::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSParamPlate::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSParamPlate::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Parametric Plate Basic Information"));
	CLDSParamPlate::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSParamPlate::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSParamPlate::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	CLDSParamPlate::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade",NULL,"Default|A|B|C|D|"));
	CLDSParamPlate::AddPropItem("thick",PROPLIST_ITEM(id++,"Thickness"));
	CLDSParamPlate::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSParamPlate::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Serial Number"));
	CLDSParamPlate::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSParamPlate::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSParamPlate::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"));
	CLDSParamPlate::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	CLDSParamPlate::AddPropItem("m_cPickedDisplayMode",PROPLIST_ITEM(id++,"Profile Visible"));
	CLDSParamPlate::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Plate"));
	CLDSParamPlate::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
	CLDSParamPlate::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"Assembling Position"));
	CLDSParamPlate::AddPropItem("m_fNormOffset",PROPLIST_ITEM(id++,"Datum Node Normal Offset"));
	CLDSParamPlate::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color"));
	CLDSParamPlate::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	CLDSParamPlate::AddPropItem("ucs",PROPLIST_ITEM(id++,""));
	CLDSParamPlate::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"Position"));
	CLDSParamPlate::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSParamPlate::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSParamPlate::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSParamPlate::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X Axis Direction"));
	CLDSParamPlate::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSParamPlate::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSParamPlate::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSParamPlate::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y Axis Direction"));
	CLDSParamPlate::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSParamPlate::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSParamPlate::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSParamPlate::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z Axis Direction"));
	CLDSParamPlate::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSParamPlate::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSParamPlate::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSParamPlate::AddPropItem("plate_work_plane",PROPLIST_ITEM(id++,"Plate Work Plane"));
	CLDSParamPlate::AddPropItem("plateProfile",PROPLIST_ITEM(id++,"Plane Configuration"));
	CLDSParamPlate::AddPropItem("designInfo.hNode",PROPLIST_ITEM(id++,"Dependant node"));
	CLDSParamPlate::AddPropItem("designInfo.hRod",PROPLIST_ITEM(id++,"Dependant rod"));
	CLDSParamPlate::AddPropItem("m_iParamType",PROPLIST_ITEM(id++,"Plate Type"));
	CLDSParamPlate::AddPropItem("m_cRollProcessType",PROPLIST_ITEM(id++,"Rolling Style of Plate Edge"));
	CLDSParamPlate::AddPropItem("m_cRollOffsetType",PROPLIST_ITEM(id++,"RollLine Cal Type","","0.Outside|1.Inside"));
	CLDSParamPlate::AddPropItem("m_cDeformType",PROPLIST_ITEM(id++,"Deform Type","","0.Assembly Plane|1.Middle Plane|2.Raised Plane|3.Sunk Plane|4.Default"));
	CLDSParamPlate::AddPropItem("m_fDeformCoef",PROPLIST_ITEM(id++,"Deform Coef",""));
	CLDSParamPlate::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"Connect Part"));
	CLDSParamPlate::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//Ӱ����Ϣ
	CLDSParamPlate::AddPropItem("m_bSyncShadowPartNo",PROPLIST_ITEM(id++,"Synchornize part label"));//Ӱ����Ϣ
	CLDSParamPlate::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Project Mother Part"));//Ӱ��ĸ����
	CLDSParamPlate::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//�����Ĳ�������
	CLDSParamPlate::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//�����Ĳ���
	CLDSParamPlate::AddPropItem("plateDesignParam",PROPLIST_ITEM(id++,"Design Parameter"));
	CLDSParamPlate::AddPropItem("plateAttachBolts",PROPLIST_ITEM(id++,"Connect Bolt"));
	CLDSParamPlate::AddPropItem("plateAttachElbowPlate",PROPLIST_ITEM(id++,"Connect Elbow Plate"));
#else 
	CLDSParamPlate::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"�������ְ������Ϣ"));
	CLDSParamPlate::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSParamPlate::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSParamPlate::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	CLDSParamPlate::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�",NULL,"Ĭ��|A|B|C|D|"));
	CLDSParamPlate::AddPropItem("thick",PROPLIST_ITEM(id++,"���"));
	CLDSParamPlate::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSParamPlate::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	CLDSParamPlate::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSParamPlate::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	CLDSParamPlate::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����"));
	CLDSParamPlate::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	CLDSParamPlate::AddPropItem("m_cPickedDisplayMode",PROPLIST_ITEM(id++,"��ʾ������"));
	CLDSParamPlate::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"���⸨���ְ�"));
	CLDSParamPlate::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
	CLDSParamPlate::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"װ�䷽λ"));
	CLDSParamPlate::AddPropItem("m_fNormOffset",PROPLIST_ITEM(id++,"���㷨��ƫ����"));
	CLDSParamPlate::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ"));
	CLDSParamPlate::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	CLDSParamPlate::AddPropItem("ucs",PROPLIST_ITEM(id++,""));
	CLDSParamPlate::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"λ��"));
	CLDSParamPlate::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X����"));
	CLDSParamPlate::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSParamPlate::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSParamPlate::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X�᷽��"));
	CLDSParamPlate::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSParamPlate::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSParamPlate::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSParamPlate::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y�᷽��"));
	CLDSParamPlate::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSParamPlate::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSParamPlate::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSParamPlate::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z�᷽��"));
	CLDSParamPlate::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSParamPlate::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSParamPlate::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSParamPlate::AddPropItem("plate_work_plane",PROPLIST_ITEM(id++,"�ְ幤����"));
	CLDSParamPlate::AddPropItem("plateProfile",PROPLIST_ITEM(id++,"�ְ�����"));
	CLDSParamPlate::AddPropItem("designInfo.hNode",PROPLIST_ITEM(id++,"�����ڵ���"));
	CLDSParamPlate::AddPropItem("designInfo.hRod",PROPLIST_ITEM(id++,"�����˼����"));
	CLDSParamPlate::AddPropItem("m_iParamType",PROPLIST_ITEM(id++,"�ְ�����"));
	CLDSParamPlate::AddPropItem("m_cRollProcessType",PROPLIST_ITEM(id++,"���Ͳ���ߴ���ʽ"));
	CLDSParamPlate::AddPropItem("m_cRollOffsetType",PROPLIST_ITEM(id++,"���ƫ�Ʒ�ʽ","","0.���|1.�ڲ�"));
	CLDSParamPlate::AddPropItem("m_cDeformType",PROPLIST_ITEM(id++,"�������δ���ʽ","","0.װ����|1.���Բ�|2.͹��|3.����|4.Ĭ��ֵ"));
	CLDSParamPlate::AddPropItem("m_fDeformCoef",PROPLIST_ITEM(id++,"���Բ�ϵ��",""));
	CLDSParamPlate::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"��������"));
	CLDSParamPlate::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Ӱ����Ϣ"));//Ӱ����Ϣ
	CLDSParamPlate::AddPropItem("m_bSyncShadowPartNo",PROPLIST_ITEM(id++,"�������±��"));//Ӱ����Ϣ
	CLDSParamPlate::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Ӱ��ĸ����"));//Ӱ��ĸ����
	CLDSParamPlate::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"������������"));//�����Ĳ�������
	CLDSParamPlate::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"��������"));//�����Ĳ���
	CLDSParamPlate::AddPropItem("plateDesignParam",PROPLIST_ITEM(id++,"��Ʋ���"));
	CLDSParamPlate::AddPropItem("plateAttachBolts",PROPLIST_ITEM(id++,"������˨"));
	CLDSParamPlate::AddPropItem("plateAttachElbowPlate",PROPLIST_ITEM(id++,"�������"));
	CLDSParamPlate::AddPropItem("PlateEdit",PROPLIST_ITEM(id++,"�༭����"));
	CLDSParamPlate::AddPropItem("WorkUcs",PROPLIST_ITEM(id++,"��������ϵ"));
	CLDSParamPlate::AddPropItem("wcs.origin",PROPLIST_ITEM(id++,"ԭ��λ��"));
	CLDSParamPlate::AddPropItem("wcs.origin.x",PROPLIST_ITEM(id++,"X����","��������ϵX�������"));
	CLDSParamPlate::AddPropItem("wcs.origin.y",PROPLIST_ITEM(id++,"Y����","��������ϵY�������"));
	CLDSParamPlate::AddPropItem("wcs.axis_x",PROPLIST_ITEM(id++,"X�᷽��"));
	CLDSParamPlate::AddPropItem("wcs.axis_x.a",PROPLIST_ITEM(id++,"������нǦ�"));
	CLDSParamPlate::AddPropItem("wcs.axis_y",PROPLIST_ITEM(id++,"Y�᷽��"));
	CLDSParamPlate::AddPropItem("InsertProfileVertex",PROPLIST_ITEM(id++,"����������","",""));
	CLDSParamPlate::AddPropItem("AddNewHole",PROPLIST_ITEM(id++,"��ӿ�","",""));
	CLDSParamPlate::AddPropItem("FilletVertex",PROPLIST_ITEM(id++,"����","",""));
	CLDSParamPlate::AddPropItem("DeleteFeature",PROPLIST_ITEM(id++,"ɾ��"));
	CLDSParamPlate::AddPropItem("FilletVertex",PROPLIST_ITEM(id++,"����"));
	InitObjParamItemsPropHashTable(id);
	m_idMinPropUdf=id;
#endif
	InitPropMirBitsMap();
}

IMPLEMENT_PROP_FUNC(CLDSPlate);
void CLDSPlate::InitPropHashtable()
{
	int id=1;
	CXhChar100 helpStr;
	CLDSPlate::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSPlate::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSPlate::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Plate Basic Information"));
	CLDSPlate::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSPlate::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSPlate::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	CLDSPlate::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade",NULL,"Default|A|B|C|D|"));
	CLDSPlate::AddPropItem("thick",PROPLIST_ITEM(id++,"Thickness"));
	CLDSPlate::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSPlate::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Serial Number"));
	CLDSPlate::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSPlate::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSPlate::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"));
	CLDSPlate::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	CLDSPlate::AddPropItem("m_cPickedDisplayMode",PROPLIST_ITEM(id++,"Profile Visible"));
	CLDSPlate::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color"));
	CLDSPlate::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	CLDSPlate::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
	CLDSPlate::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"Assembling Position"));
	CLDSPlate::AddPropItem("m_fNormOffset",PROPLIST_ITEM(id++,"Datum Point Normal Position"));
	CLDSPlate::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"Connect Part"));
	CLDSPlate::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//Ӱ����Ϣ
	CLDSPlate::AddPropItem("m_bSyncShadowPartNo",PROPLIST_ITEM(id++,"Synchornize part label"));//Ӱ����Ϣ
	CLDSPlate::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Project Mother Part"));//Ӱ��ĸ����
	CLDSPlate::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//�����Ĳ�������
	CLDSPlate::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//�����Ĳ���
	//���ڵ������ȷֵ�
	CLDSPlate::AddPropItem("designinfo.des_para",PROPLIST_ITEM(id++,"Plate Locate Parameter"));
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.hLinePart",PROPLIST_ITEM(id++,"Datum Rod Handle"));			//��׼�˼����
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.hStartNode",PROPLIST_ITEM(id++,"Start Datum Node Handle"));			//ʼ�˽ڵ���
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.hEndNode",PROPLIST_ITEM(id++,"End Datum Node Handle"));			//�ն˽ڵ���
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.start_offset_dist",PROPLIST_ITEM(id++,"Start Offset"));	//ʼ��ƫ����
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.end_offset_dist",PROPLIST_ITEM(id++,"End Offset"));	//�ն�ƫ����
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.offset_scale",PROPLIST_ITEM(id++,"Offset Proportion"));		//ƫ�Ʊ���
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.offset_dist",PROPLIST_ITEM(id++,"Offset Distance"));		//�ڵȷֻ����ϵ�ƫ��ֵ

	CLDSPlate::AddPropItem("ucs",PROPLIST_ITEM(id++,""));
	CLDSPlate::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"Position"));
	CLDSPlate::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSPlate::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSPlate::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSPlate::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X Axis Direction"));
	CLDSPlate::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSPlate::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSPlate::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSPlate::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y Axis Direction"));
	CLDSPlate::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSPlate::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSPlate::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSPlate::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z Axis Direction"));
	CLDSPlate::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSPlate::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSPlate::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSPlate::AddPropItem("plate_work_plane",PROPLIST_ITEM(id++,"Plate Work Plane"));
	CLDSPlate::AddPropItem("plateProfile",PROPLIST_ITEM(id++,"Plane Configuration"));
	CLDSPlate::AddPropItem("designInfo.hNode",PROPLIST_ITEM(id++,"Dependant node"));
	CLDSPlate::AddPropItem("designInfo.hRod",PROPLIST_ITEM(id++,"Dependant rod"));
	CLDSPlate::AddPropItem("m_fInnerRadius",PROPLIST_ITEM(id++,"Datum Inner Circle Radius"));
	CLDSPlate::AddPropItem("m_tInnerColumnNorm",PROPLIST_ITEM(id++,"Inner Circle Axis Direction "));
	CLDSPlate::AddPropItem("m_tInnerColumnNorm.x",PROPLIST_ITEM(id++,"Inner Circle Axis X Component"));
	CLDSPlate::AddPropItem("m_tInnerColumnNorm.y",PROPLIST_ITEM(id++,"Inner Circle Axis Y Component"));
	CLDSPlate::AddPropItem("m_tInnerColumnNorm.z",PROPLIST_ITEM(id++,"Inner Circle Axis Z Component"));
	CLDSPlate::AddPropItem("m_tInnerOrigin",PROPLIST_ITEM(id++,"Inner Circle Position"));
	CLDSPlate::AddPropItem("m_tInnerOrigin.x",PROPLIST_ITEM(id++,"Inner Circle X Component"));
	CLDSPlate::AddPropItem("m_tInnerOrigin.y",PROPLIST_ITEM(id++,"Inner Circle Y Component"));
	CLDSPlate::AddPropItem("m_tInnerOrigin.z",PROPLIST_ITEM(id++,"Inner Circle Z Component"));
	CLDSPlate::AddPropItem("iProfileStyle0123",PROPLIST_ITEM(id++,"Configuration Create Style"));
	CLDSPlate::AddPropItem("m_cRollProcessType",PROPLIST_ITEM(id++,"Rolling Style of Slot Insert Plate"));
	CLDSPlate::AddPropItem("m_cRollOffsetType",PROPLIST_ITEM(id++,"RollLine Cal Type","","0.Outside|1.Inside"));
	CLDSPlate::AddPropItem("m_cDeformType",PROPLIST_ITEM(id++,"Deform Type","","0.Assembly Plane|1.Middle Plane|2.Raised Plane|3.Sunk Plane|4.Default"));
	CLDSPlate::AddPropItem("m_fDeformCoef",PROPLIST_ITEM(id++,"Defom Coef",""));
	CLDSPlate::AddPropItem("face_N",PROPLIST_ITEM(id++,"Plate Type"));
	CLDSPlate::AddPropItem("basicFace",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Datum Plane Definition"));
	CLDSPlate::AddPropItem("m_bSelfCorrectBendline",PROPLIST_ITEM(id++,"Bending line defination"));
	CLDSPlate::AddPropItem("huoquFace1",PROPLIST_ITEM(id++,"The First Bending Plane Definition"));
	CLDSPlate::AddPropItem("huoquline_start[0]",PROPLIST_ITEM(id++,"Bending Line Start Point"));
	CLDSPlate::AddPropItem("huoquline_end[0]",PROPLIST_ITEM(id++,"Bending Line End Point"));
	CLDSPlate::AddPropItem("huoqufacenorm[0]",PROPLIST_ITEM(id++,"Bending Plane Normal Definition"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].norm_style",PROPLIST_ITEM(id++,"Leg Normal Compute Mode"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].vector",PROPLIST_ITEM(id++,"Bending Normal "));
	CLDSPlate::AddPropItem("huoqufacenorm[0].vector.x",PROPLIST_ITEM(id++,"Bending Normal X Axis Coordinate"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].vector.y",PROPLIST_ITEM(id++,"Bending Normal Y Axis Coordinate"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].vector.z",PROPLIST_ITEM(id++,"Bending Normal Z Axis Coordinate"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].hVicePart",PROPLIST_ITEM(id++,"Datum Normal Part"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].norm_wing",PROPLIST_ITEM(id++,"Datum Leg"));	
	CLDSPlate::AddPropItem("huoqufacenorm[0].direction",PROPLIST_ITEM(id++,"Normal"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].hCrossPart",PROPLIST_ITEM(id++,"Cross-bracing Reference Rod"));
	CLDSPlate::AddPropItem("huoquFace2",PROPLIST_ITEM(id++,"Second Bending Plane Definition"));
	CLDSPlate::AddPropItem("huoquline_start[1]",PROPLIST_ITEM(id++,"Bending Line Start Point"));
	CLDSPlate::AddPropItem("huoquline_end[1]",PROPLIST_ITEM(id++,"Bending Line End Point"));
	CLDSPlate::AddPropItem("huoqufacenorm[1]",PROPLIST_ITEM(id++,"Bending Plane Normal Definition"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].norm_style",PROPLIST_ITEM(id++,"Leg Normal Compute Mode"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].vector",PROPLIST_ITEM(id++,"Bending Normal"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].vector.x",PROPLIST_ITEM(id++,"Bending Normal X Axis Coordinate"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].vector.y",PROPLIST_ITEM(id++,"Bending Normal Y Axis Coordinate"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].vector.z",PROPLIST_ITEM(id++,"Bending Normal Z Axis Coordinate"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].hVicePart",PROPLIST_ITEM(id++,"Datum Normal Part"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].norm_wing",PROPLIST_ITEM(id++,"Datum Leg"));	
	CLDSPlate::AddPropItem("huoqufacenorm[1].direction",PROPLIST_ITEM(id++,"Normal"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].hCrossPart",PROPLIST_ITEM(id++,"Cross-bracing Reference Rod"));
	CLDSPlate::AddPropItem("plateParam",PROPLIST_ITEM(id++,"Plate Parameter"));
	CLDSPlate::AddPropItem("plateLinkPoles",PROPLIST_ITEM(id++,"Connecting Rod"));
	CLDSPlate::AddPropItem("plateAttachBolts",PROPLIST_ITEM(id++,"Connect Bolt"));
	CLDSPlate::AddPropItem("fixedVertex",PROPLIST_ITEM(id++,"Fixed Vertex"));
	CLDSPlate::AddPropItem("manuProcess",PROPLIST_ITEM(id++,"Process Technology"));
	CLDSPlate::AddPropItem("m_bWeldPart",PROPLIST_ITEM(id++,"Weld Technology","Whether to need to welding in the machining process.","Yes|No"));
	CLDSPlate::AddPropItem("iWeldLineStyle",PROPLIST_ITEM(id++,"Weld Line Style","Weld line size calculation type","0.Calculate freely|1.Specified Weld Line|2.Extention Aligned"));
	CLDSPlate::AddPropItem("m_bNeedFillet",PROPLIST_ITEM(id++,"IF Slot Process","The thicker plate is often need to slot in advance during the welding process.","Yes|No"));
	CLDSPlate::AddPropItem("designInfo.m_bEnableFlexibleDesign",PROPLIST_ITEM(id++,""));
	CLDSPlate::AddPropItem("designInfo.weld_start_high",PROPLIST_ITEM(id++,"Weld Start Height"));
	CLDSPlate::AddPropItem("designInfo.weld_end_high",PROPLIST_ITEM(id++,"Weld End Height"));
	CLDSPlate::AddPropItem("designInfo.weld_width",PROPLIST_ITEM(id++,"Weld Width"));
	CLDSPlate::AddPropItem("m_bAttachPartByWeldRoad",PROPLIST_ITEM(id++,""));
	CLDSPlate::AddPropItem("VertexList",PROPLIST_ITEM(id++,"Outline Vertext"));
#else 
	CLDSPlate::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"�ְ������Ϣ"));
	CLDSPlate::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSPlate::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSPlate::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	CLDSPlate::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�",NULL,"Ĭ��|A|B|C|D|"));
	CLDSPlate::AddPropItem("thick",PROPLIST_ITEM(id++,"���"));
	CLDSPlate::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSPlate::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	CLDSPlate::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSPlate::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	CLDSPlate::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����"));
	CLDSPlate::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	CLDSPlate::AddPropItem("m_cPickedDisplayMode",PROPLIST_ITEM(id++,"��ʾ������"));
	CLDSPlate::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"���⸨���ְ�"));
	CLDSPlate::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ"));
	CLDSPlate::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	CLDSPlate::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
	CLDSPlate::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"װ�䷽λ"));
	CLDSPlate::AddPropItem("m_fNormOffset",PROPLIST_ITEM(id++,"���㷨��λ��"));
	CLDSPlate::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"��������"));
	CLDSPlate::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Ӱ����Ϣ"));//Ӱ����Ϣ
	CLDSPlate::AddPropItem("m_bSyncShadowPartNo",PROPLIST_ITEM(id++,"�������±��"));//Ӱ����Ϣ
	CLDSPlate::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Ӱ��ĸ����"));//Ӱ��ĸ����
	CLDSPlate::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"������������"));//�����Ĳ�������
	CLDSPlate::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"��������"));//�����Ĳ���
	//���ڵ������ȷֵ�
	CLDSPlate::AddPropItem("designinfo.des_para",PROPLIST_ITEM(id++,"�ְ嶨λ����"));
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.hLinePart",PROPLIST_ITEM(id++,"��׼�˼����"));			//��׼�˼����
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.hStartNode",PROPLIST_ITEM(id++,"ʼ�˻�׼�ڵ���"));			//ʼ�˽ڵ���
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.hEndNode",PROPLIST_ITEM(id++,"�ն˻�׼�ڵ���"));			//�ն˽ڵ���
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.start_offset_dist",PROPLIST_ITEM(id++,"ʼ��ƫ����"));	//ʼ��ƫ����
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.end_offset_dist",PROPLIST_ITEM(id++,"�ն�ƫ����"));	//�ն�ƫ����
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.offset_scale",PROPLIST_ITEM(id++,"ƫ�Ʊ���"));		//ƫ�Ʊ���
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.offset_dist",PROPLIST_ITEM(id++,"ƫ�ƾ���"));		//�ڵȷֻ����ϵ�ƫ��ֵ

	CLDSPlate::AddPropItem("ucs",PROPLIST_ITEM(id++,""));
	CLDSPlate::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"λ��"));
	CLDSPlate::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X����"));
	CLDSPlate::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSPlate::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSPlate::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X�᷽��"));
	CLDSPlate::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSPlate::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSPlate::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSPlate::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y�᷽��"));
	CLDSPlate::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSPlate::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSPlate::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSPlate::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z�᷽��"));
	CLDSPlate::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSPlate::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSPlate::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSPlate::AddPropItem("plate_work_plane",PROPLIST_ITEM(id++,"�ְ幤����"));
	CLDSPlate::AddPropItem("plateProfile",PROPLIST_ITEM(id++,"�ְ�����"));
	CLDSPlate::AddPropItem("designInfo.hNode",PROPLIST_ITEM(id++,"�����ڵ���"));
	CLDSPlate::AddPropItem("designInfo.hRod",PROPLIST_ITEM(id++,"�����˼����"));
	CLDSPlate::AddPropItem("m_fInnerRadius",PROPLIST_ITEM(id++,"�����ڻ��뾶"));
	CLDSPlate::AddPropItem("m_tInnerColumnNorm",PROPLIST_ITEM(id++,"�ڻ����߷���"));
	CLDSPlate::AddPropItem("m_tInnerColumnNorm.x",PROPLIST_ITEM(id++,"�ڻ�����X����"));
	CLDSPlate::AddPropItem("m_tInnerColumnNorm.y",PROPLIST_ITEM(id++,"�ڻ�����Y����"));
	CLDSPlate::AddPropItem("m_tInnerColumnNorm.z",PROPLIST_ITEM(id++,"�ڻ�����Z����"));
	CLDSPlate::AddPropItem("m_tInnerOrigin",PROPLIST_ITEM(id++,"�ڻ�λ��"));
	CLDSPlate::AddPropItem("m_tInnerOrigin.x",PROPLIST_ITEM(id++,"�ڻ�λ��X����"));
	CLDSPlate::AddPropItem("m_tInnerOrigin.y",PROPLIST_ITEM(id++,"�ڻ�λ��Y����"));
	CLDSPlate::AddPropItem("m_tInnerOrigin.z",PROPLIST_ITEM(id++,"�ڻ�λ��Z����"));
	CLDSPlate::AddPropItem("iProfileStyle0123",PROPLIST_ITEM(id++,"�������ɷ�ʽ"));
	helpStr.Copy("�������:չ���󵹽Ǵ�Ϊ����ֱ��, ���ٵ���1:��֤������߳���ΪL-N,���ٵ���2:��֤����ڲ�߳���ΪL-N���ְ�ֻ�� ���ٵ���1�����ٵ���1=���ٵ���2");
	CLDSPlate::AddPropItem("m_cRollProcessType",PROPLIST_ITEM(id++,"��ߴ���ʽ",helpStr,"0.�������|1.���ٵ���1|2.���ٵ���2"));
	CLDSPlate::AddPropItem("m_cRollOffsetType",PROPLIST_ITEM(id++,"���ƫ�Ʒ�ʽ","","0.���|1.�ڲ�"));
	CLDSPlate::AddPropItem("m_cDeformType",PROPLIST_ITEM(id++,"�������δ���ʽ","","0.װ����|1.���Բ�|2.͹��|3.����|4.Ĭ��ֵ"));
	CLDSPlate::AddPropItem("m_fDeformCoef",PROPLIST_ITEM(id++,"���Բ�ϵ��",""));
	CLDSPlate::AddPropItem("m_fDeformCoef2",PROPLIST_ITEM(id++,"���Բ�ϵ��2",""));
	CLDSPlate::AddPropItem("m_siProfileAlgor",PROPLIST_ITEM(id++,"���������㷨"));
	CLDSPlate::AddPropItem("face_N",PROPLIST_ITEM(id++,"�ְ�����"));
	CLDSPlate::AddPropItem("basicFace",PROPLIST_ITEM(id++,"(��ע��)��׼�涨��"));
	CLDSPlate::AddPropItem("m_bSelfCorrectBendline",PROPLIST_ITEM(id++,"�����߼��㷽ʽ"));
	CLDSPlate::AddPropItem("huoquFace1",PROPLIST_ITEM(id++,"��һ�����涨��"));
	CLDSPlate::AddPropItem("huoquline_start[0]",PROPLIST_ITEM(id++,"��������ʼ��"));
	CLDSPlate::AddPropItem("huoquline_end[0]",PROPLIST_ITEM(id++,"��������ֹ��"));
	CLDSPlate::AddPropItem("huoqufacenorm[0]",PROPLIST_ITEM(id++,"�����淨�߶���"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].norm_style",PROPLIST_ITEM(id++,"֫���߼��㷽ʽ"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].vector",PROPLIST_ITEM(id++,"��������"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].vector.x",PROPLIST_ITEM(id++,"��������X�᷽������"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].vector.y",PROPLIST_ITEM(id++,"��������Y�᷽������"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].vector.z",PROPLIST_ITEM(id++,"��������Z�᷽������"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].hVicePart",PROPLIST_ITEM(id++,"��׼���߹���"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].norm_wing",PROPLIST_ITEM(id++,"��׼֫"));	
	CLDSPlate::AddPropItem("huoqufacenorm[0].direction",PROPLIST_ITEM(id++,"���߳���"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].hCrossPart",PROPLIST_ITEM(id++,"����ο��˼�"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].angle",PROPLIST_ITEM(id++,"������(��)"));
	CLDSPlate::AddPropItem("huoquFace2",PROPLIST_ITEM(id++,"�ڶ������涨��"));
	CLDSPlate::AddPropItem("huoquline_start[1]",PROPLIST_ITEM(id++,"��������ʼ��"));
	CLDSPlate::AddPropItem("huoquline_end[1]",PROPLIST_ITEM(id++,"��������ֹ��"));
	CLDSPlate::AddPropItem("huoqufacenorm[1]",PROPLIST_ITEM(id++,"�����淨�߶���"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].norm_style",PROPLIST_ITEM(id++,"֫���߼��㷽ʽ"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].vector",PROPLIST_ITEM(id++,"��������"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].vector.x",PROPLIST_ITEM(id++,"��������X�᷽������"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].vector.y",PROPLIST_ITEM(id++,"��������Y�᷽������"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].vector.z",PROPLIST_ITEM(id++,"��������Z�᷽������"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].hVicePart",PROPLIST_ITEM(id++,"��׼���߹���"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].norm_wing",PROPLIST_ITEM(id++,"��׼֫"));	
	CLDSPlate::AddPropItem("huoqufacenorm[1].direction",PROPLIST_ITEM(id++,"���߳���"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].hCrossPart",PROPLIST_ITEM(id++,"����ο��˼�"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].angle",PROPLIST_ITEM(id++,"�����ǣ���)"));
	CLDSPlate::AddPropItem("peakVertexOffsetY",PROPLIST_ITEM(id++,"������ƫ����"));
	CLDSPlate::AddPropItem("extendVertexOffsetLen",PROPLIST_ITEM(id++,"��ڳ���"));
	CLDSPlate::AddPropItem("peakPointMargin",PROPLIST_ITEM(id++,"��ڼ�϶ֵ"));
	CLDSPlate::AddPropItem("plateParam",PROPLIST_ITEM(id++,"�ְ����"));
	CLDSPlate::AddPropItem("plateLinkPoles",PROPLIST_ITEM(id++,"���Ӹ˼�"));
	CLDSPlate::AddPropItem("plateAttachBolts",PROPLIST_ITEM(id++,"������˨"));
	CLDSPlate::AddPropItem("fixedVertex",PROPLIST_ITEM(id++,"�̶�����"));
	CLDSPlate::AddPropItem("manuProcess",PROPLIST_ITEM(id++,"�ӹ�����"));
	CLDSPlate::AddPropItem("m_bWeldPart",PROPLIST_ITEM(id++,"���ӹ���","�ӹ���������Ҫ����","��|��"));
	CLDSPlate::AddPropItem("iWeldLineStyle",PROPLIST_ITEM(id++,"����������","�����߳ߴ��������","0.���ɼ���|1.ָ������|2.�������"));
	CLDSPlate::AddPropItem("m_bNeedFillet",PROPLIST_ITEM(id++,"�¿ڹ���","���ӹ����ж��ڽϺ��һ����Ҫ��ǰ�����¿�","��|��"));
	CLDSPlate::AddPropItem("designInfo.m_bEnableFlexibleDesign",PROPLIST_ITEM(id++,""));
	CLDSPlate::AddPropItem("designInfo.weld_start_high",PROPLIST_ITEM(id++,"������ʼ��"));
	CLDSPlate::AddPropItem("designInfo.weld_end_high",PROPLIST_ITEM(id++,"������ֹ��"));
	CLDSPlate::AddPropItem("designInfo.weld_width",PROPLIST_ITEM(id++,"�������"));
	CLDSPlate::AddPropItem("m_bAttachPartByWeldRoad",PROPLIST_ITEM(id++,""));
	CLDSPlate::AddPropItem("VertexList",PROPLIST_ITEM(id++,"������"));
	CLDSPlate::AddPropItem("PlateEdit",PROPLIST_ITEM(id++,"�༭����"));
	CLDSPlate::AddPropItem("WorkUcs",PROPLIST_ITEM(id++,"��������ϵ"));
	CLDSPlate::AddPropItem("wcs.origin",PROPLIST_ITEM(id++,"ԭ��λ��"));
	CLDSPlate::AddPropItem("wcs.origin.x",PROPLIST_ITEM(id++,"X����","��������ϵX�������"));
	CLDSPlate::AddPropItem("wcs.origin.y",PROPLIST_ITEM(id++,"Y����","��������ϵY�������"));
	CLDSPlate::AddPropItem("wcs.axis_x",PROPLIST_ITEM(id++,"X�᷽��"));
	CLDSPlate::AddPropItem("wcs.axis_x.a",PROPLIST_ITEM(id++,"������нǦ�"));
	CLDSPlate::AddPropItem("wcs.axis_y",PROPLIST_ITEM(id++,"Y�᷽��"));
	CLDSPlate::AddPropItem("InsertProfileVertex",PROPLIST_ITEM(id++,"����������","",""));
	CLDSPlate::AddPropItem("AddNewHole",PROPLIST_ITEM(id++,"��ӿ�","",""));
	CLDSPlate::AddPropItem("FilletVertex",PROPLIST_ITEM(id++,"����","",""));
	CLDSPlate::AddPropItem("DeleteFeature",PROPLIST_ITEM(id++,"ɾ��"));
#endif
	for(int i=0;i<26;i++)
	{
		char sName[50]="",sPos[50]="";
		sprintf(sName,"%c",'A'+i);
		CLDSPlate::AddPropItem(sName,PROPLIST_ITEM(id++,CXhChar50("����%s",(char*)sName)));
		sprintf(sPos,"%s.x",sName);
		CLDSPlate::AddPropItem(sPos,PROPLIST_ITEM(id++,"X����"));
		sprintf(sPos,"%s.y",sName);
		CLDSPlate::AddPropItem(sPos,PROPLIST_ITEM(id++,"Y����"));
	}
}

// ��˨
MEMORY_BLOCK CLDSBolt::MPCL;	//Mirror Property Control List
PROPLIST_TYPE CLDSBolt::propMirSync;
void CLDSBolt::InitPropMirBitsMap()
{
	int id=1;
	propMirSync.InitHashtableSize(32);
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("specification",PROPLIST_ITEM(id++,"���"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("workNormInfo",PROPLIST_ITEM(id++,"��������"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("Position",PROPLIST_ITEM(id++,"��׼λ��"));
}
IMPLEMENT_PROP_FUNC(CLDSBolt);
void CLDSBolt::InitBoltPropHashtable()
{
	return InitPropHashtable();
}
void CLDSBolt::InitPropHashtable()
{
	int id=1;
	CLDSBolt::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSBolt::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSBolt::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Bolt Basic Information"));
	CLDSBolt::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSBolt::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSBolt::AddPropItem("m_hFamily",PROPLIST_ITEM(id++,"Bolt Specification Serial"));		//��˨���ϵ��
	CLDSBolt::AddPropItem("grade",PROPLIST_ITEM(id++,"Bolt Grade","","4.8|5.8|6.8|8.8"));
	CLDSBolt::AddPropItem("specification",PROPLIST_ITEM(id++,"BoltSpecification"));
	CLDSBolt::AddPropItem("L0",PROPLIST_ITEM(id++,"L0"));
	CLDSBolt::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSBolt::AddPropItem("statSegStr",PROPLIST_ITEM(id++,"Seg Number Statistic Coverage"));
	CLDSBolt::AddPropItem("dwRayNo",PROPLIST_ITEM(id++,"Ray Number"));
	CLDSBolt::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSBolt::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSBolt::AddPropItem("hole_d_increment",PROPLIST_ITEM(id++,"Bolt Hole clearance"));
	CLDSBolt::AddPropItem("gasketInfo",PROPLIST_ITEM(id++,"Washer Information"));
	CLDSBolt::AddPropItem("gasketThick",PROPLIST_ITEM(id++,"Washer Thickness","","Autho matched|Specified"));
	CLDSBolt::AddPropItem("gasketN",PROPLIST_ITEM(id++,"Washer Number"));
	CLDSBolt::AddPropItem("gasketOffset",PROPLIST_ITEM(id++,"Washer Displacement"));
	CLDSBolt::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Bolt","","Yes|No"));
	CLDSBolt::AddPropItem("m_bAntiLooseWasher",PROPLIST_ITEM(id++,"Anti-Loose Washer","","Yes|No"));
	CLDSBolt::AddPropItem("m_bAntiTheft",PROPLIST_ITEM(id++,"Anti-Theft Bolt","","Yes|No"));
	CLDSBolt::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color","","Yes|No"));
	CLDSBolt::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	CLDSBolt::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
	CLDSBolt::AddPropItem("m_bCalL0IncGapByInters", PROPLIST_ITEM(id++, "Computing Model", "", "0.Sum Part Thick|1.Include Part Gap"));
	CLDSBolt::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"Connect Part"));
	CLDSBolt::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//Ӱ����Ϣ
	CLDSBolt::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Project Mother Part"));//Ӱ��ĸ����
	CLDSBolt::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//�����Ĳ�������
	CLDSBolt::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//�����Ĳ���
	CLDSBolt::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"Assembling Position"));
	CLDSBolt::AddPropItem("des_base_pos",PROPLIST_ITEM(id++,"Assembling Position Design Parameter","Simpleness Mode|Senior Mode"));
	CLDSBolt::AddPropItem("des_base_pos.cLocationStyle",PROPLIST_ITEM(id++,"Location Coord Type","","Model Coords|Local Coords"));
	CLDSBolt::AddPropItem("des_base_pos.cResidePlane",PROPLIST_ITEM(id++,"Reside plane"));
	CLDSBolt::AddPropItem("des_base_pos.datumPoint",PROPLIST_ITEM(id++,"Locate datum point"));
	CLDSBolt::AddPropItem("des_base_pos.datumPoint.x",PROPLIST_ITEM(id++,"Datum Point X Coordinate"));
	CLDSBolt::AddPropItem("des_base_pos.datumPoint.y",PROPLIST_ITEM(id++,"Datum Point Y Coordinate"));
	CLDSBolt::AddPropItem("des_base_pos.datumPoint.z",PROPLIST_ITEM(id++,"Datum Point Z Coordinate"));
	CLDSBolt::AddPropItem("des_base_pos.hPart",PROPLIST_ITEM(id++,"Locate Datum Part"));
	CLDSBolt::AddPropItem("des_base_pos.m_biWingOffsetStyle",PROPLIST_ITEM(id++,"Lateral offset style","","g|g1|g2|g3|User-Defined"));
	CLDSBolt::AddPropItem("des_base_pos.wing_offset_dist",PROPLIST_ITEM(id++,"lateral Offset"));
	CLDSBolt::AddPropItem("des_base_pos.len_offset_dist",PROPLIST_ITEM(id++,"Angle Vertical Offset"));
	CLDSBolt::AddPropItem("RotationStartEdge",PROPLIST_ITEM(id++,"Rotating reference edge","","0.Datum Edge|1.Weld Edge"));
	CLDSBolt::AddPropItem("des_base_pos.offset_angle",PROPLIST_ITEM(id++,"Offset Angle"));
	CLDSBolt::AddPropItem("des_base_pos.R",PROPLIST_ITEM(id++,"Offset Distance"));
	CLDSBolt::AddPropItem("des_base_pos.offset_wing",PROPLIST_ITEM(id++,"Offset Leg","","X Leg|Y Leg"));
	CLDSBolt::AddPropItem("des_base_pos.direction",PROPLIST_ITEM(id++,"(Angle|Bolt)Vertical Offset Direction","","Start->End|End->Start"));
	CLDSBolt::AddPropItem("des_base_pos.norm_offset",PROPLIST_ITEM(id++,"Normal Offset"));
	CLDSBolt::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"Position"));
	CLDSBolt::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSBolt::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSBolt::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSBolt::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X Axis Direction"));
	CLDSBolt::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSBolt::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSBolt::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSBolt::AddPropItem("des_work_norm",PROPLIST_ITEM(id++,"Work Normal Design Parameter"));
	CLDSBolt::AddPropItem("des_work_norm.norm_style",PROPLIST_ITEM(id++,"Work Normal Compute Mode"));
	CLDSBolt::AddPropItem("des_work_norm.hVicePart",PROPLIST_ITEM(id++,"Datum Normal Part"));
	CLDSBolt::AddPropItem("des_work_norm.norm_wing",PROPLIST_ITEM(id++,"Datum Leg","","X leg normal|Y leg normal"));
	CLDSBolt::AddPropItem("des_work_norm.reside_plane",PROPLIST_ITEM(id++,"Work Plane"));
	CLDSBolt::AddPropItem("des_work_norm.direction",PROPLIST_ITEM(id++,"Direction Type|Normal"));
	CLDSBolt::AddPropItem("des_work_norm.hCrossPart",PROPLIST_ITEM(id++,"Cross-bracing Reference Rod"));
	CLDSBolt::AddPropItem("des_work_norm.nearVector",PROPLIST_ITEM(id++,"(Approximate|Relative)Work Normal"));
	CLDSBolt::AddPropItem("des_work_norm.nearVector.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSBolt::AddPropItem("des_work_norm.nearVector.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSBolt::AddPropItem("des_work_norm.nearVector.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSBolt::AddPropItem("des_work_norm.rot_ang",PROPLIST_ITEM(id++,"Rotation Angle","","0|90|180|270"));
	CLDSBolt::AddPropItem("work_norm",PROPLIST_ITEM(id++,"Work Normal"));
	CLDSBolt::AddPropItem("work_norm.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSBolt::AddPropItem("work_norm.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSBolt::AddPropItem("work_norm.z",PROPLIST_ITEM(id++,"Z Component"));
	//��ģʽ��λ����
	CLDSBolt::AddPropItem("des_base_pos.datum_pos_style",PROPLIST_ITEM(id++,"��λ��ʽ","","1.�Ǹ���㶨λ|2.�Ǹֽڵ㶨λ|3.�Ǹ����߽���"));
	CLDSBolt::AddPropItem("des_base_pos.hBasePart",PROPLIST_ITEM(id++,"��׼�Ǹ�"));
	CLDSBolt::AddPropItem("des_base_pos.hCrossPart",PROPLIST_ITEM(id++,"����Ǹ�"));
	CLDSBolt::AddPropItem("des_base_pos.hBaseNode",PROPLIST_ITEM(id++,"��׼�ڵ�"));
	CLDSBolt::AddPropItem("des_base_pos.iWorkWing",PROPLIST_ITEM(id++,"����֫","","X֫|Y֫"));
	CLDSBolt::AddPropItem("des_base_pos.fWingOffDist1",PROPLIST_ITEM(id++,"g by angle leg","","g|g1|g2|g3"));
	CLDSBolt::AddPropItem("des_base_pos.fWingOffDist2",PROPLIST_ITEM(id++,"g by cross leg","","g|g1|g2|g3"));
	CLDSBolt::AddPropItem("des_base_pos.fWingOffDist3",PROPLIST_ITEM(id++,"g on plane","","g|g1|g2|g3"));
	CLDSBolt::AddPropItem("des_base_pos.iDirection",PROPLIST_ITEM(id++,"longitude direction","","start->end|end->start"));
	CLDSBolt::AddPropItem("des_base_pos.fLenOffDist",PROPLIST_ITEM(id++,"longitude offset"));
#else 
	CLDSBolt::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"��˨������Ϣ"));
	CLDSBolt::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSBolt::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSBolt::AddPropItem("m_hFamily",PROPLIST_ITEM(id++,"��˨���ϵ��"));		//��˨���ϵ��
	CLDSBolt::AddPropItem("grade",PROPLIST_ITEM(id++,"��˨����","","4.8|5.8|6.8|8.8"));
	CLDSBolt::AddPropItem("specification",PROPLIST_ITEM(id++,"��˨���"));
	CLDSBolt::AddPropItem("L0",PROPLIST_ITEM(id++,"ͨ�����","ͨ�����(mm),������˨��Ȧ�ĺ���Լ�ͨ���ַ�������õ��ĺ�������֡�",""));
	CLDSBolt::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSBolt::AddPropItem("statSegStr",PROPLIST_ITEM(id++,"�κ�ͳ�Ʒ�Χ","�ڶ���Ȼ��Ӵ�����˨ͬʱ��Ҫ�ڶ������ͳ�ƣ������Ҫ�ر�ָ��ͳ�ƶκŷ�Χ"));
	CLDSBolt::AddPropItem("dwRayNo",PROPLIST_ITEM(id++,"���ߺ�"));
	CLDSBolt::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSBolt::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	CLDSBolt::AddPropItem("hole_d_increment",PROPLIST_ITEM(id++,"�ݿ׼�϶","����װ����Ҫ,һ����˨��ֱ��Ҫ������˨��ֱ�������ֱ���Ĳ�ֵ��Ϊ�ݿ׼�϶(mm)��",""));
	//������
	CLDSBolt::AddPropItem("xHolePropGroup",PROPLIST_ITEM(id++,"������","",""));
	CLDSBolt::AddPropItem("dfHoleDiameter",PROPLIST_ITEM(id++,"�׾�","�����ϵĿ�ֱ��(mm)��",""));
	CLDSBolt::AddPropItem("siWaistLength",PROPLIST_ITEM(id++,"��Բ����","��˨��Ϊ��Բʱ������Բ����Բ�ļ����(mm)��",""));
	CLDSBolt::AddPropItem("hHoleResidePart",PROPLIST_ITEM(id++,"��������","��˨�����ڹ����ľ����",""));
	CLDSBolt::AddPropItem("ciHoleFuncType",PROPLIST_ITEM(id++,"��������","��˨�׵Ĺ����������͡�","0.��˨��|1.�Ŷ���|2.���߿�|3.�ӵؿ�|4.װ���|5.���ƿ�|6.������|7.���׽Ŷ�|8.�ؽſ�"));
	CLDSBolt::AddPropItem("ciManuProcessType",PROPLIST_ITEM(id++,"�ƿ׹���","","���|���"));
	CLDSBolt::AddPropItem("blEnlargeHole",PROPLIST_ITEM(id++,"��Ҫ����","��ԭ�п׾�������ʩ�����׹���","��|��"));
	//
	CLDSBolt::AddPropItem("gasketInfo",PROPLIST_ITEM(id++,"��Ȧ��Ϣ"));
	CLDSBolt::AddPropItem("gasketThick",PROPLIST_ITEM(id++,"��Ȧ���","","�Զ�ƥ��|ָ�����"));
	CLDSBolt::AddPropItem("gasketN",PROPLIST_ITEM(id++,"��Ȧ����"));
	CLDSBolt::AddPropItem("gasketOffset",PROPLIST_ITEM(id++,"��Ȧλ��"));
	CLDSBolt::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"������˨","","��|��"));
	CLDSBolt::AddPropItem("m_bAntiLooseWasher",PROPLIST_ITEM(id++,"ͳ�Ʒ��ɵ�Ȧ","","��|��"));
	CLDSBolt::AddPropItem("m_bAntiTheft",PROPLIST_ITEM(id++,"ͳ��Ϊ������˨","","��|��"));
	CLDSBolt::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ","","��|��"));
	CLDSBolt::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	CLDSBolt::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
	CLDSBolt::AddPropItem("m_bCalL0IncGapByInters", PROPLIST_ITEM(id++, "ͨ�����ִ�й���", "0.�ۼ�ͨ���ۼӴ���������ȼ���ͨ��1.����ȱʧ����϶���ۼӴ���������ȼ����ڹ�����>2mm�ļ�϶ֵ����ͨ��", "0.�ۼ�ͨ��|1.����ȱʧ����϶"));
	CLDSBolt::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"��������"));
	CLDSBolt::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Ӱ����Ϣ"));//Ӱ����Ϣ
	CLDSBolt::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Ӱ��ĸ����","��ǰ������Ӧ��Ӱ��ĸ����������þ��Ϊ0x0ʱ��ʾ��ǰ����Ϊԭʼ������"));//Ӱ��ĸ����
	CLDSBolt::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"������������"));//�����Ĳ�������
	CLDSBolt::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"��������"));//�����Ĳ���
	CLDSBolt::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"װ�䷽λ"));
	CLDSBolt::AddPropItem("des_base_pos",PROPLIST_ITEM(id++,"װ��λ����Ʋ���","","��ģʽ|�߼�ģʽ"));
	CLDSBolt::AddPropItem("des_base_pos.cLocationStyle",PROPLIST_ITEM(id++,"������������","��˨��λ������������","ģ������|�������"));
	CLDSBolt::AddPropItem("des_base_pos.cResidePlane",PROPLIST_ITEM(id++,"����ƽ��"));
	CLDSBolt::AddPropItem("des_base_pos.datumPoint",PROPLIST_ITEM(id++,"��λ����"));
	CLDSBolt::AddPropItem("des_base_pos.datumPoint.x",PROPLIST_ITEM(id++,"����X����"));
	CLDSBolt::AddPropItem("des_base_pos.datumPoint.y",PROPLIST_ITEM(id++,"����Y����"));
	CLDSBolt::AddPropItem("des_base_pos.datumPoint.z",PROPLIST_ITEM(id++,"����Z����"));
	CLDSBolt::AddPropItem("des_base_pos.hPart",PROPLIST_ITEM(id++,"��λ��׼����"));
	CLDSBolt::AddPropItem("des_base_pos.m_biWingOffsetStyle",PROPLIST_ITEM(id++,"����ƫ�Ʒ�ʽ","","g|g1|g2|g3|�Զ���"));
	CLDSBolt::AddPropItem("des_base_pos.wing_offset_dist",PROPLIST_ITEM(id++,"����ƫ����"));
	CLDSBolt::AddPropItem("des_base_pos.len_offset_dist",PROPLIST_ITEM(id++,"����ƫ����"));
	CLDSBolt::AddPropItem("RotationStartEdge",PROPLIST_ITEM(id++,"��ת��ʼ��","","0.��׼��|1.������"));
	CLDSBolt::AddPropItem("des_base_pos.offset_angle",PROPLIST_ITEM(id++,"ƫ�ƽǶ�"));
	CLDSBolt::AddPropItem("des_base_pos.R",PROPLIST_ITEM(id++,"ƫ�ƾ���"));
	CLDSBolt::AddPropItem("des_base_pos.offset_wing",PROPLIST_ITEM(id++,"ƫ��֫","","X֫|Y֫"));
	CLDSBolt::AddPropItem("des_base_pos.direction",PROPLIST_ITEM(id++,"����ƫ�Ʒ���","","ʼ->��|��->ʼ"));
	CLDSBolt::AddPropItem("des_base_pos.norm_offset",PROPLIST_ITEM(id++,"Z��ƫ��","��˨�Զ�λ������װ������ϵ���᷽�����������߷��򣩵�ƫ������"));
	CLDSBolt::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"λ��"));
	CLDSBolt::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X����"));
	CLDSBolt::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSBolt::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSBolt::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X�᷽��"));
	CLDSBolt::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSBolt::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSBolt::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSBolt::AddPropItem("des_work_norm",PROPLIST_ITEM(id++,"����������Ʋ���"));
	CLDSBolt::AddPropItem("des_work_norm.norm_style",PROPLIST_ITEM(id++,"�������߼��㷽ʽ","",CLDSVector::GetNormStyleCmbItems()));
	CLDSBolt::AddPropItem("des_work_norm.hVicePart",PROPLIST_ITEM(id++,"��׼���߹���"));
	CLDSBolt::AddPropItem("des_work_norm.norm_wing",PROPLIST_ITEM(id++,"��׼֫","","X֫����|Y֫����"));
	CLDSBolt::AddPropItem("des_work_norm.reside_plane",PROPLIST_ITEM(id++,"������"));
	CLDSBolt::AddPropItem("des_work_norm.direction",PROPLIST_ITEM(id++,"��������|���߳���"));
	CLDSBolt::AddPropItem("des_work_norm.hCrossPart",PROPLIST_ITEM(id++,"����ο��˼�"));
	CLDSBolt::AddPropItem("des_work_norm.nearVector",PROPLIST_ITEM(id++,"(����|���)��������"));
	CLDSBolt::AddPropItem("des_work_norm.nearVector.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSBolt::AddPropItem("des_work_norm.nearVector.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSBolt::AddPropItem("des_work_norm.nearVector.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSBolt::AddPropItem("des_work_norm.rot_ang",PROPLIST_ITEM(id++,"��ת�Ƕ�","","0|90|180|270"));
	CLDSBolt::AddPropItem("work_norm",PROPLIST_ITEM(id++,"��������"));
	CLDSBolt::AddPropItem("work_norm.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSBolt::AddPropItem("work_norm.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSBolt::AddPropItem("work_norm.z",PROPLIST_ITEM(id++,"Z�������"));
	//��ģʽ��λ����
	CLDSBolt::AddPropItem("des_base_pos.datum_pos_style",PROPLIST_ITEM(id++,"��λ��ʽ","","1.�Ǹ���㶨λ|2.�Ǹֽڵ㶨λ|3.�Ǹ����߽���"));
	CLDSBolt::AddPropItem("des_base_pos.hBasePart",PROPLIST_ITEM(id++,"��׼�Ǹ�"));
	CLDSBolt::AddPropItem("des_base_pos.hCrossPart",PROPLIST_ITEM(id++,"����Ǹ�"));
	CLDSBolt::AddPropItem("des_base_pos.hBaseNode",PROPLIST_ITEM(id++,"��׼�ڵ�"));
	CLDSBolt::AddPropItem("des_base_pos.iWorkWing",PROPLIST_ITEM(id++,"����֫","","X֫|Y֫"));
	CLDSBolt::AddPropItem("des_base_pos.fWingOffDist1",PROPLIST_ITEM(id++,"�����ľ�","��˨�ڻ�׼�Ǹ��ϵ�ʵ���ľ�","g|g1|g2|g3"));
	CLDSBolt::AddPropItem("des_base_pos.fWingOffDist2",PROPLIST_ITEM(id++,"�����ľ�2","��˨�ڽ���Ǹ��ϵ��ľ�","g|g1|g2|g3"));
	CLDSBolt::AddPropItem("des_base_pos.fWingOffDist3",PROPLIST_ITEM(id++,"ƽ���ľ�","��λ��ƽ�ƻ����ڻ�׼�Ǹ��ϵ��ľ�","g|g1|g2|g3"));
	CLDSBolt::AddPropItem("des_base_pos.iDirection",PROPLIST_ITEM(id++,"����ƫ�Ʒ���","","ʼ->��|��->ʼ"));
	CLDSBolt::AddPropItem("des_base_pos.fLenOffDist",PROPLIST_ITEM(id++,"����ƫ�ƾ���","�Զ�λ�����ػ�׼�Ǹ������ƫ����"));
#endif
	InitPropMirBitsMap();
}

CHashStrList<PROPLIST_ITEM> CLDSSphere::propHashtable;
CXhChar100 CLDSSphere::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*>CLDSSphere::hashPropItemById;
CHashTable<DWORD> CLDSSphere::propStatusHashtable;
int CLDSSphere::m_iCurDisplayPropGroup;
void CLDSSphere::InitSpherePropHashtable()
{
	int id=1;
	CLDSSphere::propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	CLDSSphere::propStatusHashtable.CreateHashTable(STATUSHASHTABLESIZE);
#ifdef AFX_TARG_ENU_ENGLISH
	CLDSSphere::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"Sphere Basic Informaiton"));
	CLDSSphere::AddPropItem("handle",PROPLIST_ITEM(id++,"Handle"));
	CLDSSphere::AddPropItem("layer",PROPLIST_ITEM(id++,"Layer"));
	CLDSSphere::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"Material"));
	CLDSSphere::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"Quality Grade",NULL,"Default|A|B|C|D|"));
	CLDSSphere::AddPropItem("specification",PROPLIST_ITEM(id++,"Specification"));
	CLDSSphere::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSSphere::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"Part Serial Number"));
	CLDSSphere::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSSphere::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSSphere::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"));
	CLDSSphere::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	CLDSSphere::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,""));
	CLDSSphere::AddPropItem("D",PROPLIST_ITEM(id++,"External Diameter"));
	CLDSSphere::AddPropItem("d",PROPLIST_ITEM(id++,"Internal Diameter"));
	CLDSSphere::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color"));
	CLDSSphere::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	CLDSSphere::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
	CLDSSphere::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"Assembling Position"));
	CLDSSphere::AddPropItem("ucs",PROPLIST_ITEM(id++,"Assembling Coordinate System"));
	CLDSSphere::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"Position"));
	CLDSSphere::AddPropItem("hDatumNode",PROPLIST_ITEM(id++,"Datum Node"));
	CLDSSphere::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSSphere::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSSphere::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSSphere::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X Axis Direction"));
	CLDSSphere::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSSphere::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSSphere::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSSphere::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y Axis Direction"));
	CLDSSphere::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSSphere::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSSphere::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSSphere::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z Axis Direction"));
	CLDSSphere::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSSphere::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSSphere::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSSphere::AddPropItem("sphereLinkPoles",PROPLIST_ITEM(id++,"Connecting Rod"));
#else 
	CLDSSphere::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"���������Ϣ"));
	CLDSSphere::AddPropItem("handle",PROPLIST_ITEM(id++,"���"));
	CLDSSphere::AddPropItem("layer",PROPLIST_ITEM(id++,"ͼ��"));
	CLDSSphere::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"����"));
	CLDSSphere::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"�����ȼ�",NULL,"Ĭ��|A|B|C|D|"));
	CLDSSphere::AddPropItem("specification",PROPLIST_ITEM(id++,"���"));
	CLDSSphere::AddPropItem("iSeg",PROPLIST_ITEM(id++,"�ֶκ�"));
	CLDSSphere::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"�������"));
	CLDSSphere::AddPropItem("cfgword",PROPLIST_ITEM(id++,"��ĺ�"));
	CLDSSphere::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"ͳ��ϵ��"));
	CLDSSphere::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"���Ӹ�����"));
	CLDSSphere::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"��ʾ��"));
	CLDSSphere::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,""));
	CLDSSphere::AddPropItem("D",PROPLIST_ITEM(id++,"��ֱ��"));
	CLDSSphere::AddPropItem("d",PROPLIST_ITEM(id++,"��ֱ��"));
	CLDSSphere::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"ָ��������ɫ"));
	CLDSSphere::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"������ɫ"));
	CLDSSphere::AddPropItem("sNotes",PROPLIST_ITEM(id++,"��ע"));
	CLDSSphere::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"װ�䷽λ"));
	CLDSSphere::AddPropItem("ucs",PROPLIST_ITEM(id++,"װ������ϵ"));
	CLDSSphere::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"λ��"));
	CLDSSphere::AddPropItem("hDatumNode",PROPLIST_ITEM(id++,"��׼�ڵ�"));
	CLDSSphere::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X����"));
	CLDSSphere::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSSphere::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z����"));
	CLDSSphere::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X�᷽��"));
	CLDSSphere::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSSphere::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSSphere::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSSphere::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y�᷽��"));
	CLDSSphere::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSSphere::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSSphere::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSSphere::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z�᷽��"));
	CLDSSphere::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X�������"));
	CLDSSphere::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y�������"));
	CLDSSphere::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z�������"));
	CLDSSphere::AddPropItem("sphereLinkPoles",PROPLIST_ITEM(id++,"���Ӹ˼�"));
#endif
}
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef __PART_DESIGN_INC_

void WEATHER::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.ReadInteger(&m_nHighestT);	//�������
	buffer.ReadInteger(&m_nLowestT);	//�������
	buffer.ReadInteger(&m_nAverageT);	//ƽ������
	buffer.ReadDouble(&m_fMaxWindV);		//������
	buffer.ReadInteger(&m_nMaxWindT);	//������ʱ�¶�
	buffer.ReadDouble(&m_fMaxIceThick);			//��󸲱���(mm)
	buffer.ReadInteger(&m_nMaxIceT);			//��󸲱�ʱ�¶�
	buffer.ReadDouble(&m_fMaxIceWindV);			//��󸲱�ʱ����
	buffer.ReadInteger(&m_nSetupT);				//��װʱ�¶�
	buffer.ReadDouble(&m_fSetupWindV);	//��װʱ����
}
void WEATHER::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(m_nHighestT);	//�������
	buffer.WriteInteger(m_nLowestT);	//�������
	buffer.WriteInteger(m_nAverageT);	//ƽ������
	buffer.WriteDouble(m_fMaxWindV);	//������
	buffer.WriteInteger(m_nMaxWindT);	//������ʱ�¶�
	buffer.WriteDouble(m_fMaxIceThick);	//��󸲱���(mm)
	buffer.WriteInteger(m_nMaxIceT);	//��󸲱�ʱ�¶�
	buffer.WriteDouble(m_fMaxIceWindV);	//��󸲱�ʱ����
	buffer.WriteInteger(m_nSetupT);		//��װʱ�¶�
	buffer.WriteDouble(m_fSetupWindV);	//��װʱ����
}
void WEATHER::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[50]="";
	fprintf(fp,"<������Ϣ>\n");
	fprintf(fp,"<������� HighestT=\"%d\"/>\n",m_nHighestT);
	fprintf(fp,"<������� LowestT=\"%d\"/>\n",m_nLowestT);
	fprintf(fp,"<ƽ������ AvergeT=\"%d\"/>\n",m_nAverageT);
	sprintf(sText,"%f",m_fMaxWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<������ MaxWindV=\"%s\"/>\n",sText);
	fprintf(fp,"<������ʱ�¶� MaxWindT=\"%d\"/>\n",m_nMaxWindT);
	sprintf(sText,"%f",m_fMaxIceThick);
	SimplifiedNumString(sText);
	fprintf(fp,"<��󸲱��� MaxIceThick=\"%s\"/>\n",sText);
	fprintf(fp,"<��󸲱�ʱ�¶� MaxIceT=\"%d\"/>\n",m_nMaxIceT);
	sprintf(sText,"%f",m_fMaxIceWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<��󸲱�ʱ���� MaxIceWindV=\"%s\"/>\n",sText);
	fprintf(fp,"<��װʱ�¶� SetupT=\"%d\"/>\n",m_nSetupT);
	sprintf(sText,"%f",m_fSetupWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<��װʱ���� SetupWindV=\"%s\"/>\n",sText);
	fprintf(fp,"</������Ϣ>\n");
}
void WEATHER_REGION::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.ReadString(weatherCode);
	WEATHER::FromBuffer(buffer,version);
}
void WEATHER_REGION::ToBuffer(CBuffer &buffer)
{
	buffer.WriteString(weatherCode);
	WEATHER::ToBuffer(buffer);
}
void WEATHER_REGION::ToXmlFile(FILE* fp,DWORD schema)
{
	fprintf(fp,"<�������� weatherCode=\"%s\">\n",weatherCode);
	WEATHER::ToXmlFile(fp,schema);
	fprintf(fp,"</��������>\n");
}
long CElemInfo::globalId;
long CElemInfo::GetNewId(){
	globalId++;
	return globalId;
}
static bool IsMirPos(char cMirType,GEPOINT pos1,GEPOINT pos2)
{
	if(cMirType=='X'&&pos1.IsEqual(pos2.x,-pos2.y,pos2.z))
		return true;
	else if(cMirType=='Y'&&pos1.IsEqual(-pos2.x,pos2.y,pos2.z))
		return true;
	else if(cMirType=='Z'&&pos1.IsEqual(-pos2.x,-pos2.y,pos2.z))
		return true;
	else
		return false;
}
void CElemInfo::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	/*buffer.WriteInteger(m_hParentRod);
	buffer.WriteByte(cCalLenType);
	buffer.WriteByte(smcItems[0].cSMCType);
	buffer.WriteInteger(horiElem.hStart);
	buffer.WriteInteger(horiElem.hEnd);
	buffer.WriteInteger(smcItems[0].relaElem.hStart);
	buffer.WriteInteger(smcItems[0].relaElem.hEnd);
	buffer.WriteInteger(smcItems[1].relaElem.hStart);
	buffer.WriteInteger(smcItems[1].relaElem.hEnd);
	buffer.WriteInteger(smcItems[2].relaElem.hStart);
	buffer.WriteInteger(smcItems[2].relaElem.hEnd);
	buffer.WriteInteger(LoRyo.bCalByElemLen);
	buffer.WriteInteger(LoRyo.hStartNode);
	buffer.WriteInteger(LoRyo.hEndNode);
	buffer.WriteDouble(LoRyo.coef);
	buffer.WriteInteger(LoRx.bCalByElemLen);
	buffer.WriteInteger(LoRx.hStartNode);
	buffer.WriteInteger(LoRx.hEndNode);
	buffer.WriteDouble(LoRx.coef);
	buffer.WriteInteger(smcItems[0].LoRc.bCalByElemLen);
	buffer.WriteInteger(smcItems[0].LoRc.hStartNode);
	buffer.WriteInteger(smcItems[0].LoRc.hEndNode);
	buffer.WriteDouble(smcItems[0].LoRc.coef);*/
	buffer.WriteInteger(m_hParentRod);
	buffer.WriteByte(cCalLenType);
	buffer.WriteInteger(horiElem.hStart);
	buffer.WriteInteger(horiElem.hEnd);
	buffer.WriteInteger(LoRyo.bCalByElemLen);
	buffer.WriteInteger(LoRyo.hStartNode);
	buffer.WriteInteger(LoRyo.hEndNode);
	buffer.WriteDouble(LoRyo.coef);
	buffer.WriteInteger(LoRx.bCalByElemLen);
	buffer.WriteInteger(LoRx.hStartNode);
	buffer.WriteInteger(LoRx.hEndNode);
	buffer.WriteDouble(LoRx.coef);
	for(int i=0;i<3;i++)
	{
		buffer.WriteByte(smcItems[i].cSMCType);
		buffer.WriteInteger(smcItems[i].relaElem.hStart);
		buffer.WriteInteger(smcItems[i].relaElem.hEnd);
		buffer.WriteDouble (smcItems[i].LoRc.coef);
		buffer.WriteInteger(smcItems[i].LoRc.bCalByElemLen);
		buffer.WriteInteger(smcItems[i].LoRc.hStartNode);
		buffer.WriteInteger(smcItems[i].LoRc.hEndNode);
	}

	if(version==0||((doc_type==3&&version>=2000101)||(doc_type==4&&version>=1020101)))
	{	//�洢��Ԫ�Ĺ����Գ���Ϣ�������޷��ڵ�Ԫģʽ������Ԥ���ԳƵ�Ԫ�����Ч��
		buffer.WriteBooleanThin(relaElem.bFatherElem);
		buffer.WriteInteger(relaElem.xFatherElem.hStartNode);
		buffer.WriteInteger(relaElem.xFatherElem.hEndNode);
		buffer.WriteInteger(relaElem.xElemMirX.hStartNode);
		buffer.WriteInteger(relaElem.xElemMirX.hEndNode);
		buffer.WriteInteger(relaElem.xElemMirY.hStartNode);
		buffer.WriteInteger(relaElem.xElemMirY.hEndNode);
		buffer.WriteInteger(relaElem.xElemMirZ.hStartNode);
		buffer.WriteInteger(relaElem.xElemMirZ.hEndNode);
	}
}
void CElemInfo::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	buffer.ReadInteger(&m_hParentRod);
	buffer.ReadByte(&cCalLenType);
	if(version>0&&((doc_type==3&&version<2000007)||(doc_type==4&&version<1020007)))
	{
		if(cCalLenType!=0)	//m_bSpecLoInfo=true
			cCalLenType=5;	//5.ָ��ƽ�������С�����㳤��
		buffer.ReadInteger(&LoRyo.bCalByElemLen);
		buffer.ReadInteger(&LoRyo.hStartNode);
		buffer.ReadInteger(&LoRyo.hEndNode);
		buffer.ReadDouble(&LoRyo.coef);
		buffer.ReadInteger(&LoRx.bCalByElemLen);
		buffer.ReadInteger(&LoRx.hStartNode);
		buffer.ReadInteger(&LoRx.hEndNode);
		buffer.ReadDouble(&LoRx.coef);
		buffer.ReadInteger(&LoRx.bCalByElemLen);
		buffer.ReadInteger(&LoRx.hStartNode);
		buffer.ReadInteger(&LoRx.hEndNode);
		buffer.ReadDouble(&LoRx.coef);
		long hRefRod=0;
		buffer.ReadInteger(&hRefRod);
	}
	else
	{
		buffer.ReadInteger(&horiElem.hStart);
		buffer.ReadInteger(&horiElem.hEnd);
		buffer.ReadInteger(&LoRyo.bCalByElemLen);
		buffer.ReadInteger(&LoRyo.hStartNode);
		buffer.ReadInteger(&LoRyo.hEndNode);
		buffer.ReadDouble(&LoRyo.coef);
		buffer.ReadInteger(&LoRx.bCalByElemLen);
		buffer.ReadInteger(&LoRx.hStartNode);
		buffer.ReadInteger(&LoRx.hEndNode);
		buffer.ReadDouble(&LoRx.coef);
		for(int i=0;i<3;i++)
		{
			buffer.ReadByte(&smcItems[i].cSMCType);
			buffer.ReadInteger(&smcItems[i].relaElem.hStart);
			buffer.ReadInteger(&smcItems[i].relaElem.hEnd);
			buffer.ReadDouble (&smcItems[i].LoRc.coef);
			buffer.ReadInteger(&smcItems[i].LoRc.bCalByElemLen);
			buffer.ReadInteger(&smcItems[i].LoRc.hStartNode);
			buffer.ReadInteger(&smcItems[i].LoRc.hEndNode);
		}
	}
	if(version==0||((doc_type==3&&version>=2000101)||(doc_type==4&&version>=1020101)))
	{
		buffer.ReadBooleanThin(&relaElem.bFatherElem);
		buffer.ReadInteger(&relaElem.xFatherElem.hStartNode);
		buffer.ReadInteger(&relaElem.xFatherElem.hEndNode);
		buffer.ReadInteger(&relaElem.xElemMirX.hStartNode);
		buffer.ReadInteger(&relaElem.xElemMirX.hEndNode);
		buffer.ReadInteger(&relaElem.xElemMirY.hStartNode);
		buffer.ReadInteger(&relaElem.xElemMirY.hEndNode);
		buffer.ReadInteger(&relaElem.xElemMirZ.hStartNode);
		buffer.ReadInteger(&relaElem.xElemMirZ.hEndNode);
	}
}
int CElemInfo::LoRyoCalType()
{
	return LoCalType(LoRyo);
}
int CElemInfo::LoRxCalType()
{
	return LoCalType(LoRx);
}
int CElemInfo::LoCalType(Lo_CALINFO& LoInfo)
{
	if( (LoInfo.hStartNode==m_hStartNode&&LoInfo.hEndNode==m_hEndNode)||
		(LoInfo.hStartNode==m_hEndNode&&LoInfo.hEndNode==m_hStartNode))
		return 0;	//��Ԫ����
	else if(LoInfo.bCalByElemLen)
		return 0;	//��Ԫ����
	else if(LoInfo.hStartNode<=0||LoInfo.hEndNode<=0)
		return 2;	//���迼��
	else
		return 1;	//ָ���ڼ�
}
int CElemInfo::GetSmcItemsNum(int *scmItemsTag/* =NULL */)
{	//�õ�ͬʱ��ѹ���ǵ���������Ǿ������
	int num=0,iTag=0;
	if(smcItems[0].cSMCType!=SMC_ITEM::SMC_NONE)
	{
		num++;
		iTag+=1;
	}
	if(smcItems[1].cSMCType!=SMC_ITEM::SMC_NONE)
	{
		num++;
		iTag+=2;
	}
	if(smcItems[2].cSMCType!=SMC_ITEM::SMC_NONE)
	{	
		num++;
		iTag+=3;
	}
	if(scmItemsTag)
		*scmItemsTag=iTag;
	return num;
}
BYTE SMC_ITEM::RetrieveSmcType(long hElemStart,long hElemEnd,IModel* pModel)	//����б��ͬʱ��ѹ����
{
	if(cSMCType>0&&cSMCType<=SMC_REFER_2ENDS)
		return cSMCType;
	else
	{
		cSMCType=SMC_NONE;
		CLDSNode* pStart=pModel->FromNodeHandle(hElemStart);
		CLDSNode* pEnd=pModel->FromNodeHandle(hElemEnd);
		CLDSNode* pReferStart=pModel->FromNodeHandle(relaElem.hStart);
		CLDSNode* pReferEnd=pModel->FromNodeHandle(relaElem.hEnd);
		if(pStart&&pEnd&&pReferStart&&pReferEnd)
		{
			if(hElemStart==relaElem.hStart&&fabs(pEnd->Position(false).z-pReferEnd->Position(false).z)<EPS)
				cSMCType=SMC_REFER_START;
			else if(hElemEnd==relaElem.hEnd&&fabs(pStart->Position(false).z-pReferStart->Position(false).z)<EPS)
				cSMCType=SMC_REFER_END;
			else if( IsMirPos('X',pStart->Position(false),pReferStart->Position(false))&&
				IsMirPos('X',pEnd->Position(false),pReferEnd->Position(false)))
				cSMCType=SMC_REFER_MIRX;
			else if(IsMirPos('X',pStart->Position(false),pReferEnd->Position(false))&&
					IsMirPos('X',pEnd->Position(false),pReferStart->Position(false)))
				cSMCType=SMC_REFER_MIRX;
			else if(IsMirPos('Y',pStart->Position(false),pReferStart->Position(false))&&
					IsMirPos('Y',pEnd->Position(false),pReferEnd->Position(false)))
				cSMCType=SMC_REFER_MIRY;
			else if(IsMirPos('Y',pStart->Position(false),pReferEnd->Position(false))&&
					IsMirPos('Y',pEnd->Position(false),pReferStart->Position(false)))
				cSMCType=SMC_REFER_MIRY;
			else
				cSMCType=SMC_REFER_UDF;
		}
		return cSMCType;
	}
}

int CElemInfo::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	char sText[100]="";
#ifdef __LDS_CONTEXT_
	CLDSLinePart *pParentRod=(CLDSLinePart*)m_pModel->FromPartHandle(m_hParentRod,CLS_LINEPART);
	if(pParentRod==NULL)
		return 0;
	if(GetPropID("Id")==id)
	{
		int start_point_i=0,end_point_i=0;
		CLDSNode *pStart=m_pModel->FromNodeHandle(m_hStartNode);
		if(pStart)
			start_point_i=pStart->point_i;
		CLDSNode *pEnd=m_pModel->FromNodeHandle(m_hEndNode);
		if(pEnd)
			end_point_i=pEnd->point_i;
		//������С�ţ������ʽ��ʾ����Ϊ�˱����뵥ԪУ�󴰿��е�ʼĩ�˲�����ͻ��wjh-2014.1.14
		sprintf(sText,"%d-%d",start_point_i,end_point_i);
	}
	else if(GetPropID("layer")==id)
	{
		strcpy(sText,pParentRod->layer());
	}
	/*else if(GetPropID("iSeg")==id)
	{
		CLDSLinePart *pParentRod=(CLDSLinePart*)m_pModel->FromPartHandle(m_hParentRod,CLS_LINEPART);
		if(pParentRod)
			sprintf(sText,"%d",pParentRod->iSeg);
	}*/
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(pParentRod->cfgword,valueStr);
	{
		if(pParentRod->layer(0)=='L')	//���ȹ���
			return pParentRod->cfgword.GetLegScopeStr(valueStr,50,true);
		else
			return pParentRod->cfgword.GetBodyScopeStr(valueStr);
	}
	else if(GetPropID("cMaterial")==id)
	{
		char matStr[20]="";
		QuerySteelMatMark(pParentRod->cMaterial,matStr);
		strcpy(sText,matStr);
	}
	else if(GetPropID("m_uStatMatNo")==id)
	{
		sprintf(sText,"%d",pParentRod->m_uStatMatNo);
	}
	else if(GetPropID("length")==id)
	{
		CLDSNode *pStart=m_pModel->FromNodeHandle(m_hStartNode);
		CLDSNode *pEnd=m_pModel->FromNodeHandle(m_hEndNode);
		if(pStart&&pEnd)
		{
			double len=DISTANCE(pStart->Position(false),pEnd->Position(false));
			sprintf(sText,"%f",len);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("size.idClassType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pParentRod->size_idClassType==CLS_LINEANGLE)
			strcpy(sText, "Angle with equal leg");
		else if(pParentRod->size_idClassType==CLS_GROUPLINEANGLE)
		{
			if(pParentRod->size_cSubClassType=='T')
				strcpy(sText, "T-Type combine");
			else if(pParentRod->size_cSubClassType=='D')
				strcpy(sText, "Diagonal combine");
			else if(pParentRod->size_cSubClassType=='X')
				strcpy(sText, "Cross Combine");
		}
		else if(pParentRod->size_idClassType==CLS_LINETUBE)
			strcpy(sText, "Tube");
		else if(pParentRod->size_idClassType==CLS_LINEFLAT)
			strcpy(sText, "Flat Angle");
		else if(pParentRod->size_idClassType==CLS_LINESLOT)
			strcpy(sText, "[ steel");
		else if(pParentRod->size_idClassType==CLS_CABLE)
			strcpy(sText, "Wire Cable");
		else if(pParentRod->size_idClassType==0)
			strcpy(sText, "User appoint");
#else 
		if(pParentRod->size_idClassType==CLS_LINEANGLE)
			strcpy(sText, "��֫�Ǹ�");
		else if(pParentRod->size_idClassType==CLS_GROUPLINEANGLE)
		{
			if(pParentRod->size_cSubClassType=='T')
				strcpy(sText, "T�����");
			else if(pParentRod->size_cSubClassType=='D')
				strcpy(sText, "�Խ����");
			else if(pParentRod->size_cSubClassType=='X')
				strcpy(sText, "ʮ�����");
		}
		else if(pParentRod->size_idClassType==CLS_LINETUBE)
			strcpy(sText, "�ֹ�");
		else if(pParentRod->size_idClassType==CLS_LINEFLAT)
			strcpy(sText, "���");
		else if(pParentRod->size_idClassType==CLS_LINESLOT)
			strcpy(sText, "�۸�");
		else if(pParentRod->size_idClassType==CLS_CABLE)
			strcpy(sText, "����");
		else if(pParentRod->size_idClassType==0)
			strcpy(sText, "�û�����");
#endif
	}
	else if(GetPropID("specification")==id)
	{
		if(pParentRod->GetClassTypeId()==CLS_LINEPART&&pParentRod->size_idClassType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Cross-section Constant");
#else 
			strcpy(sText,"���泣��");
#endif
		else
		{
			char s_wing_wide[200],s_wing_thick[200];
			sprintf(s_wing_wide,"%f",pParentRod->size_wide);
			sprintf(s_wing_thick,"%f",pParentRod->size_thick);
			SimplifiedNumString(s_wing_wide);
			SimplifiedNumString(s_wing_thick);
			sprintf(sText,"%sX%s", s_wing_wide,s_wing_thick);
		}
	}
	else if(GetPropID("m_sWireType")==id)
	{
		strcpy(sText,pParentRod->GetWireType());
	}
	else if(GetPropID("m_bNeedSelSize")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pParentRod->m_bNeedSelSize)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(pParentRod->m_bNeedSelSize)
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("m_bAutoCalHoleOutCoef")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pParentRod->m_bAutoCalHoleOutCoef)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(pParentRod->m_bAutoCalHoleOutCoef)
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("hole_out")==id)
	{
		sprintf(sText,"%f",pParentRod->hole_out);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("bolt_type")==id)
	{
		if(pParentRod->pStart->handle==m_hStartNode)
			sprintf(sText,"%sM%d",pParentRod->connectStart.grade,pParentRod->connectStart.d);
		else if(pParentRod->pEnd->handle==m_hEndNode)
			sprintf(sText,"%sM%d",pParentRod->connectEnd.grade,pParentRod->connectEnd.d);
	}
	else if(GetPropID("bolt_N")==id)
	{
		if(pParentRod->pStart->handle==m_hStartNode)
			sprintf(sText,"%d",pParentRod->connectStart.wnSummBoltN);
		else if(pParentRod->pEnd->handle==m_hEndNode)
			sprintf(sText,"%d",pParentRod->connectEnd.wnSummBoltN);
	}
	/*else if(GetPropID("bolt_bearloadcoef")==id)
	{
		sprintf(sText,"%f",connectStart.bearloadcoef);
		SimplifiedNumString(sText);
	}*/
	else if(GetPropID("m_iElemType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(pParentRod->m_iElemType==0)
			strcpy(sText,"0.Default");
		else if(pParentRod->m_iElemType==1)
			strcpy(sText,"1.Truss Elem");
		else if(pParentRod->m_iElemType==2)
			strcpy(sText,"2.Classical Beam");
		else if(pParentRod->m_iElemType==3)
			strcpy(sText,"3.Cable Elem");
#else 
		if(ciFreedomStart==3&&ciFreedomEnd==3)
			strcpy(sText,"1.������");
		else if(ciFreedomStart>3||ciFreedomEnd>3)
			strcpy(sText,"2.������");
		else if(pParentRod->m_iElemType==0)
			strcpy(sText,"0.�Զ��ж�");
		else if(pParentRod->m_iElemType==1)
			strcpy(sText,"1.������");
		else if(pParentRod->m_iElemType==2)
			strcpy(sText,"2.������");
		else if(pParentRod->m_iElemType==3)
			strcpy(sText,"3.������Ԫ");
#endif
	}
	else if(GetPropID("start_joint_type")==id)
	{
		int jointType=JOINT_RIGID;
		if(pParentRod->pStart->handle==m_hStartNode)
			jointType=pParentRod->start_joint_type;
#ifdef AFX_TARG_ENU_ENGLISH
		if(jointType==JOINT_HINGE)
			strcpy(sText,"Articulate");
		else if(jointType==JOINT_RIGID)
			strcpy(sText,"Rigid Connect");
		else //if(jointType==JOINT_AUTOJUSTIFY)
			strcpy(sText,"Default");
#else 
		if(jointType==JOINT_HINGE||ciFreedomStart==3)
			strcpy(sText,"�½�");
		else if(jointType==JOINT_RIGID||ciFreedomStart>3)
			strcpy(sText,"�ս�");
		else //if(jointType==JOINT_AUTOJUSTIFY)
			strcpy(sText,"�Զ��ж�");
#endif
	}
	else if(GetPropID("start_force_type")==id)
	{
		int forceType=StartForceType(pParentRod);
#ifdef AFX_TARG_ENU_ENGLISH
		if(forceType==CENTRIC_FORCE)
			strcpy(sText,"Center Force");
		else //if(forceType==ECCENTRIC_FORCE)
			strcpy(sText,"Eccentric Force");
#else 
		if(forceType==CENTRIC_FORCE)
			strcpy(sText,"��������");
		else //if(forceType==ECCENTRIC_FORCE)
			strcpy(sText,"ƫ������");
#endif
	}
	else if(GetPropID("end_joint_type")==id)
	{
		int jointType=EndJointState(pParentRod);//JOINT_RIGID;
		if(pParentRod->pEnd->handle==m_hEndNode)
			jointType=pParentRod->start_joint_type;
#ifdef AFX_TARG_ENU_ENGLISH
		if(jointType==JOINT_HINGE)
			strcpy(sText,"Articulate");
		else if(jointType==JOINT_RIGID)
			strcpy(sText,"Rigid Connect");
		else //if(jointType==JOINT_AUTOJUSTIFY)
			strcpy(sText,"Default");
#else 
		if(jointType==JOINT_HINGE||ciFreedomEnd==3)
			strcpy(sText,"�½�");
		else if(jointType==JOINT_RIGID||ciFreedomEnd>3)
			strcpy(sText,"�ս�");
		else //if(jointType==JOINT_AUTOJUSTIFY)
			strcpy(sText,"�Զ��ж�");
#endif
	}
	else if(GetPropID("end_force_type")==id)
	{
		int forceType=EndForceType(pParentRod);
#ifdef AFX_TARG_ENU_ENGLISH
		if(forceType==CENTRIC_FORCE)
			strcpy(sText,"Center Force");
		else //if(forceType==ECCENTRIC_FORCE)
			strcpy(sText,"Eccentric Force");
#else 
		if(forceType==CENTRIC_FORCE)
			strcpy(sText,"��������");
		else //if(forceType==ECCENTRIC_FORCE)
			strcpy(sText,"ƫ������");
#endif
	}
	else if(GetPropID("cCalLenType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(cCalLenType==0)
			strcpy(sText,"0.Intelligent Computing");
		else if(cCalLenType==1)
			strcpy(sText,"1.V tilted material(crossing foot material connect)");
		else if(cCalLenType==2)
			strcpy(sText,"2.V tilted material(V-plane connect)");
		else if(cCalLenType==3)
			strcpy(sText,"3.V tilted material 3 Section Horizon Pole");
		else if(cCalLenType==4)
			strcpy(sText,"4.V tilted material 4 Section Horizon Pole");
		else if(cCalLenType==6)
			strcpy(sText,"6.Specified Cross Rod Calculate Length");
		else //if(cCalLenType==5)
			strcpy(sText,"5.Specified Element Calculate Length");
#else 
		if(cCalLenType==0)
			strcpy(sText,"0.���ܼ���");
		else if(cCalLenType==1)
			strcpy(sText,"1.Vб��(��Ų�����)");
		else if(cCalLenType==2)
			strcpy(sText,"2.Vб��(V������)");
		else if(cCalLenType==3)
			strcpy(sText,"3.Vб��3�ڼ��������");
		else if(cCalLenType==4)
			strcpy(sText,"4.Vб��4�ڼ��������");
		else if(cCalLenType==6)
			strcpy(sText,"6.ָ������ļ��㳤��");
		else //if(cCalLenType==5)
			strcpy(sText,"5.ָ����Ԫ���㳤��");
#endif
	}
	else if(GetPropID("lamdaRyoInfo")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(LoRyoCalType()==2)
			strcpy(sText,"0.Unconsideration");
		else			  
			strcpy(sText,"1.Consideration");
#else 
		if(LoRyoCalType()==2)
			strcpy(sText,"0.������");
		else			  
			strcpy(sText,"1.����");
#endif
	}
	else if(GetPropID("lamdaRxInfo")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(LoRxCalType()==2)
			strcpy(sText,"0.Unconsideration");
		else			  
			strcpy(sText,"1.Consideration");
#else 
		if(LoRxCalType()==2)
			strcpy(sText,"0.������");
		else			  
			strcpy(sText,"1.����");
#endif
	}
	else if(GetPropID("lamdaRxcInfo")==id)
	{
		int iNum=GetSmcItemsNum();
#ifdef AFX_TARG_ENU_ENGLISH
		if(iNum==0)
			strcpy(sText,"0.Unconsideration");
		else if(iNum==1)
			strcpy(sText,"1.Single Item");
		else if(iNum==2)
			strcpy(sText,"2.Double Item");
		else
			strcpy(sText,"3.Three Item");
#else 
		if(iNum==0)
			strcpy(sText,"0.������");
		else if(iNum==1)
			strcpy(sText,"1.����");
		else if(iNum==2)
			strcpy(sText,"2.����");
		else
			strcpy(sText,"3.����");
#endif
	}
	else if(GetPropID("LoRyo.CalType")==id)
	{
		int type=LoRyoCalType();
#ifdef AFX_TARG_ENU_ENGLISH
		if(type==0)
			strcpy(sText,"0.Elem Length");
		else if(type==1)
			strcpy(sText,"1.Specify Direction");
		else
			strcpy(sText,"2.Unconsideration");
#else 
		if(type==0)
			strcpy(sText,"0.��Ԫ����");
		else if(type==1)
			strcpy(sText,"1.ָ���ڼ�");
		else
			strcpy(sText,"2.������");
#endif
	}
	else if(GetPropID("LoRx.CalType")==id)
	{
		int type=LoRxCalType();
#ifdef AFX_TARG_ENU_ENGLISH
		if(type==0)
			strcpy(sText,"0.Elem Length");
		else if(type==1)
			strcpy(sText,"1.Specify Direction");
		else
			strcpy(sText,"2.Unconsideration");
#else 
		if(type==0)
			strcpy(sText,"0.��Ԫ����");
		else if(type==1)
			strcpy(sText,"1.ָ���ڼ�");
		else
			strcpy(sText,"2.������");
#endif
	}
	else if(GetPropID("smcItem1.LoRxc.CalType")==id)
	{
		int type=LoCalType(smcItems[0].LoRc);
#ifdef AFX_TARG_ENU_ENGLISH
		if(type==0)
			strcpy(sText,"0.Elem Length");
		else if(type==1)
			strcpy(sText,"1.Specify Direction");
		else
			strcpy(sText,"2.Unconsideration");
#else 
		if(type==0)
			strcpy(sText,"0.��Ԫ����");
		else if(type==1)
			strcpy(sText,"1.ָ���ڼ�");
		else
			strcpy(sText,"2.������");
#endif
	}
	else if(GetPropID("smcItem2.LoRxc.CalType")==id)
	{
		int type=LoCalType(smcItems[1].LoRc);
#ifdef AFX_TARG_ENU_ENGLISH
		if(type==0)
			strcpy(sText,"0.Elem Length");
		else if(type==1)
			strcpy(sText,"1.Specify Direction");
		else
			strcpy(sText,"2.Unconsideration");
#else 
		if(type==0)
			strcpy(sText,"0.��Ԫ����");
		else if(type==1)
			strcpy(sText,"1.ָ���ڼ�");
		else
			strcpy(sText,"2.������");
#endif
	}
	else if(GetPropID("smcItem3.LoRxc.CalType")==id)
	{
		int type=LoCalType(smcItems[2].LoRc);
#ifdef AFX_TARG_ENU_ENGLISH
		if(type==0)
			strcpy(sText,"0.Elem Length");
		else if(type==1)
			strcpy(sText,"1.Specify Direction");
		else
			strcpy(sText,"2.Unconsideration");
#else 
		if(type==0)
			strcpy(sText,"0.��Ԫ����");
		else if(type==1)
			strcpy(sText,"1.ָ���ڼ�");
		else
			strcpy(sText,"2.������");
#endif
	}
	else if(GetPropID("LoRyo.hStartNode")==id)
	{
		CLDSNode *pNode=m_pModel->FromNodeHandle(LoRyo.hStartNode);
		if(pNode)
			sprintf(sText,"%d",pNode->point_i);
	}
	else if(GetPropID("LoRyo.hEndNode")==id)
	{
		CLDSNode *pNode=m_pModel->FromNodeHandle(LoRyo.hEndNode);
		if(pNode)
			sprintf(sText,"%d",pNode->point_i);
	}
	else if(GetPropID("LoRyo.coef")==id)
	{
		sprintf(sText,"%f",LoRyo.coef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("LoRx.hStartNode")==id)
	{
		CLDSNode *pNode=m_pModel->FromNodeHandle(LoRx.hStartNode);
		if(pNode)
			sprintf(sText,"%d",pNode->point_i);
	}
	else if(GetPropID("LoRx.hEndNode")==id)
	{
		CLDSNode *pNode=m_pModel->FromNodeHandle(LoRx.hEndNode);
		if(pNode)
			sprintf(sText,"%d",pNode->point_i);
	}
	else if(GetPropID("LoRx.coef")==id)
	{
		sprintf(sText,"%f",LoRx.coef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("smcItem1.LoRxc.hStartNode")==id)
	{
		CLDSNode *pNode=m_pModel->FromNodeHandle(smcItems[0].LoRc.hStartNode);
		if(pNode)
			sprintf(sText,"%d",pNode->point_i);
	}
	else if(GetPropID("smcItem1.LoRxc.hEndNode")==id)
	{
		CLDSNode *pNode=m_pModel->FromNodeHandle(smcItems[0].LoRc.hEndNode);
		if(pNode)
			sprintf(sText,"%d",pNode->point_i);
	}
	else if(GetPropID("smcItem1.LoRxc.coef")==id)
	{
		sprintf(sText,"%f",smcItems[0].LoRc.coef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("smcItem2.LoRxc.hStartNode")==id)
	{
		CLDSNode *pNode=m_pModel->FromNodeHandle(smcItems[1].LoRc.hStartNode);
		if(pNode)
			sprintf(sText,"%d",pNode->point_i);
	}
	else if(GetPropID("smcItem2.LoRxc.hEndNode")==id)
	{
		CLDSNode *pNode=m_pModel->FromNodeHandle(smcItems[1].LoRc.hEndNode);
		if(pNode)
			sprintf(sText,"%d",pNode->point_i);
	}
	else if(GetPropID("smcItem2.LoRxc.coef")==id)
	{
		sprintf(sText,"%f",smcItems[1].LoRc.coef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("smcItem3.LoRxc.hStartNode")==id)
	{
		CLDSNode *pNode=m_pModel->FromNodeHandle(smcItems[2].LoRc.hStartNode);
		if(pNode)
			sprintf(sText,"%d",pNode->point_i);
	}
	else if(GetPropID("smcItem3.LoRxc.hEndNode")==id)
	{
		CLDSNode *pNode=m_pModel->FromNodeHandle(smcItems[2].LoRc.hEndNode);
		if(pNode)
			sprintf(sText,"%d",pNode->point_i);
	}
	else if(GetPropID("smcItem3.LoRxc.coef")==id)
	{
		sprintf(sText,"%f",smcItems[2].LoRc.coef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("LoRyo")==id)
	{
		long hStartNode=LoRyo.hStartNode,hEndNode=LoRyo.hEndNode;
		if(LoRyoCalType()==0)	//ָ����Ԫ����
		{
			hStartNode=m_hStartNode;
			hEndNode=m_hEndNode;
		}
		CLDSNode *pStart=m_pModel->FromNodeHandle(hStartNode);
		CLDSNode *pEnd=m_pModel->FromNodeHandle(hEndNode);
		double len=0;
		if(pStart&&pEnd)
			len=DISTANCE(pStart->Position(false),pEnd->Position(false))*LoRyo.coef;
		sprintf(sText,"%f",len);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("LoRx")==id)
	{
		long hStartNode=LoRx.hStartNode,hEndNode=LoRx.hEndNode;
		if(LoRxCalType()==0)	//ָ����Ԫ����
		{
			hStartNode=m_hStartNode;
			hEndNode=m_hEndNode;
		}
		CLDSNode *pStart=m_pModel->FromNodeHandle(hStartNode);
		CLDSNode *pEnd=m_pModel->FromNodeHandle(hEndNode);
		double len=0;
		if(pStart&&pEnd)
			len=DISTANCE(pStart->Position(false),pEnd->Position(false))*LoRx.coef;
		sprintf(sText,"%f",len);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("smcItem1.LoRxc")==id)
	{
		long hStartNode=smcItems[0].LoRc.hStartNode,hEndNode=smcItems[0].LoRc.hEndNode;
		if(LoCalType(smcItems[0].LoRc)==0)	//ָ����Ԫ����
		{
			hStartNode=m_hStartNode;
			hEndNode=m_hEndNode;
		}
		CLDSNode *pStart=m_pModel->FromNodeHandle(hStartNode);
		CLDSNode *pEnd=m_pModel->FromNodeHandle(hEndNode);
		double len=0;
		if(pStart&&pEnd)
			len=DISTANCE(pStart->Position(false),pEnd->Position(false))*smcItems[0].LoRc.coef;
		sprintf(sText,"%f",len);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("smcItem1.relaElem")==id)
	{
		CLDSNode *pStart=m_pModel->FromNodeHandle(smcItems[0].relaElem.hStart);
		CLDSNode *pEnd=m_pModel->FromNodeHandle(smcItems[0].relaElem.hEnd);
		if(pStart&&pEnd)
			sprintf(sText,"%d-%d",pStart->point_i,pEnd->point_i);
		else if(pStart)
			sprintf(sText,"%d-?",pStart->point_i);
		else if(pEnd)
			sprintf(sText,"?-%d",pEnd->point_i);
	}
	else if(GetPropID("smcItem2.LoRxc")==id)
	{
		long hStartNode=smcItems[1].LoRc.hStartNode,hEndNode=smcItems[1].LoRc.hEndNode;
		if(LoCalType(smcItems[1].LoRc)==0)	//ָ����Ԫ����
		{
			hStartNode=m_hStartNode;
			hEndNode=m_hEndNode;
		}
		CLDSNode *pStart=m_pModel->FromNodeHandle(hStartNode);
		CLDSNode *pEnd=m_pModel->FromNodeHandle(hEndNode);
		double len=0;
		if(pStart&&pEnd)
			len=DISTANCE(pStart->Position(false),pEnd->Position(false))*smcItems[1].LoRc.coef;
		sprintf(sText,"%f",len);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("smcItem2.relaElem")==id)
	{
		CLDSNode *pStart=m_pModel->FromNodeHandle(smcItems[1].relaElem.hStart);
		CLDSNode *pEnd=m_pModel->FromNodeHandle(smcItems[1].relaElem.hEnd);
		if(pStart&&pEnd)
			sprintf(sText,"%d-%d",pStart->point_i,pEnd->point_i);
		else if(pStart)
			sprintf(sText,"%d-?",pStart->point_i);
		else if(pEnd)
			sprintf(sText,"?-%d",pEnd->point_i);
	}
	else if(GetPropID("smcItem3.LoRxc")==id)
	{
		long hStartNode=smcItems[2].LoRc.hStartNode,hEndNode=smcItems[2].LoRc.hEndNode;
		if(LoCalType(smcItems[2].LoRc)==0)	//ָ����Ԫ����
		{
			hStartNode=m_hStartNode;
			hEndNode=m_hEndNode;
		}
		CLDSNode *pStart=m_pModel->FromNodeHandle(hStartNode);
		CLDSNode *pEnd=m_pModel->FromNodeHandle(hEndNode);
		double len=0;
		if(pStart&&pEnd)
			len=DISTANCE(pStart->Position(false),pEnd->Position(false))*smcItems[2].LoRc.coef;
		sprintf(sText,"%f",len);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("smcItem3.relaElem")==id)
	{
		CLDSNode *pStart=m_pModel->FromNodeHandle(smcItems[2].relaElem.hStart);
		CLDSNode *pEnd=m_pModel->FromNodeHandle(smcItems[2].relaElem.hEnd);
		if(pStart&&pEnd)
			sprintf(sText,"%d-%d",pStart->point_i,pEnd->point_i);
		else if(pStart)
			sprintf(sText,"%d-?",pStart->point_i);
		else if(pEnd)
			sprintf(sText,"?-%d",pEnd->point_i);
	}
	//��ѹ����
	else if(GetPropID("smcItem1.cSMCType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_MIRX)
			strcpy(sText,"0.X axis symmetry");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_MIRY)	
			strcpy(sText,"1.Y axis symmetry");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_START)
			strcpy(sText,"2.Start Same Endpoint");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_END)
			strcpy(sText,"3.End Same Endpoint");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_2ENDS)
			strcpy(sText,"4.Both Ends Same Endpoint");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_UDF)	
			strcpy(sText,"5.Specify Simultaneously Compression Item");
#else 
		if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_MIRX)
			strcpy(sText,"0.X��Գ�");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_MIRY)	
			strcpy(sText,"1.Y��Գ�");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_START)
			strcpy(sText,"2.ʼ��ͬ�˵�");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_END)
			strcpy(sText,"3.�ն�ͬ�˵�");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_2ENDS)
			strcpy(sText,"4.����ͬ�˵�");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_UDF)	
			strcpy(sText,"5.ָ��ͬʱ��ѹ��Ԫ");
#endif
	}
	else if(GetPropID("smcItem2.cSMCType")==id)	
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_MIRX)	
			strcpy(sText,"0.X axis symmetry");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_MIRY)	
			strcpy(sText,"1.Y axis symmetry");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_START)
			strcpy(sText,"2.Start Same Endpoint");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_END)
			strcpy(sText,"3.End Same Endpoint");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_2ENDS)
			strcpy(sText,"4.Both Ends Same Endpoint");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_UDF)	
			strcpy(sText,"5.Specify Simultaneously Compression Item");
#else 
		if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_MIRX)	
			strcpy(sText,"0.X��Գ�");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_MIRY)	
			strcpy(sText,"1.Y��Գ�");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_START)
			strcpy(sText,"2.ʼ��ͬ�˵�");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_END)
			strcpy(sText,"3.�ն�ͬ�˵�");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_2ENDS)
			strcpy(sText,"4.����ͬ�˵�");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_UDF)	
			strcpy(sText,"5.ָ��ͬʱ��ѹ��Ԫ");
#endif
	}
	else if(GetPropID("smcItem3.cSMCType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_MIRX)	
			strcpy(sText,"0.X axis symmetry");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_MIRY)	
			strcpy(sText,"1.Y axis symmetry");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_START)
			strcpy(sText,"2.Start Same Endpoint");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_END)	
			strcpy(sText,"3.End Same Endpoint");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_2ENDS)
			strcpy(sText,"4.Both Ends Same Endpoint");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_UDF)
			strcpy(sText,"5.Specify Simultaneously Compression Item");
#else 
		if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_MIRX)	
			strcpy(sText,"0.X��Գ�");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_MIRY)	
			strcpy(sText,"1.Y��Գ�");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_START)
			strcpy(sText,"2.ʼ��ͬ�˵�");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_END)	
			strcpy(sText,"3.�ն�ͬ�˵�");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_2ENDS)
			strcpy(sText,"4.����ͬ�˵�");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_UDF)
			strcpy(sText,"5.ָ��ͬʱ��ѹ��Ԫ");
#endif
	}
	//��ת�뾶
	else if(GetPropID("smcItem1.gyreRadius")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(smcItems[0].cSMCType&0x80)	//��С��
			strcpy(sText,"1.minimum axial");
		else 
			strcpy(sText,"0.parallel axial");
#else 
		if(smcItems[0].cSMCType&0x80)	//��С��
			strcpy(sText,"1.��С��");
		else 
			strcpy(sText,"0.ƽ����");
#endif
	}
	else if(GetPropID("smcItem2.gyreRadius")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(smcItems[1].cSMCType&0x80)	//��С��
			strcpy(sText,"1.minimum axial");
		else 
			strcpy(sText,"0.parallel axial");
#else 
		if(smcItems[1].cSMCType&0x80)	//��С��
			strcpy(sText,"1.��С��");
		else 
			strcpy(sText,"0.ƽ����");
#endif
	}
	else if(GetPropID("smcItem3.gyreRadius")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(smcItems[1].cSMCType&0x80)	//��С��
			strcpy(sText,"1.minimum axial");
		else 
			strcpy(sText,"0.parallel axial");
#else 
		if(smcItems[1].cSMCType&0x80)	//��С��
			strcpy(sText,"1.��С��");
		else 
			strcpy(sText,"0.ƽ����");
#endif
	}
#ifdef AFX_TARG_ENU_ENGLISH
	if(strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("0X%X-0X%X Element's property##d string value out of length",m_hStartNode,m_hEndNode,id);
#else
	if(strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("��Ԫ0X%X-0X%X����#%d�ַ���������������",m_hStartNode,m_hEndNode,id);
#endif
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
#endif
	return strlen(sText);
}
long CElemInfo::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem =CElemInfo::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
	{
		/*char prop_str[500];
		strcpy(prop_str,"û�ҵ���Ϊ\"");
		strcat(prop_str,propStr);
		strcat(prop_str,"\"������ID��");
		AfxMessageBox(prop_str);*/
		return 0;
	}
}

BOOL CElemInfo::IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSObject *pFirstObj = selectObjs.GetFirst();
	if(pFirstObj->GetClassTypeId()!=CLS_FEMELEMENT)
	{
		selectObjs.pop_stack();
		return FALSE;
	}
	CElemInfo *pFirstElem=(CElemInfo*)pFirstObj;
	pFirstElem->GetPropValueStr(idProp,valueStr);
	for(CLDSObject *pObject=selectObjs.GetNext();pObject;pObject=selectObjs.GetNext())
	{
		char valueStr2[500]="";
		if(pObject->GetClassTypeId()!=CLS_FEMELEMENT)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
		CElemInfo *pElemInfo=(CElemInfo*)pObject;
		pElemInfo->GetPropValueStr(idProp,valueStr2);
		if(_stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CElemInfo::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CElemInfo::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}

BOOL CElemInfo::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CElemInfo::propStatusHashtable.SetValueAt(id,dwStatus);
}
int compare_eleminfo(const ELEMINFO_PTR& pElemInfo1,const ELEMINFO_PTR& pElemInfo2)
{
	long arrStartPointI[2]={0},arrEndPointI[2]={0};
	CLDSNode* pStartNode1=pElemInfo1->BelongModel()->FromNodeHandle(pElemInfo1->m_hStartNode);
	if(pStartNode1)
		arrStartPointI[0]=pStartNode1->point_i;
	CLDSNode* pStartNode2=pElemInfo1->BelongModel()->FromNodeHandle(pElemInfo2->m_hStartNode);
	if(pStartNode2)
		arrStartPointI[1]=pStartNode2->point_i;
	CLDSNode* pEndNode1=pElemInfo1->BelongModel()->FromNodeHandle(pElemInfo1->m_hEndNode);
	if(pEndNode1)
		arrEndPointI[0]=pEndNode1->point_i;
	CLDSNode* pEndNode2=pElemInfo1->BelongModel()->FromNodeHandle(pElemInfo2->m_hEndNode);
	if(pEndNode2)
		arrEndPointI[1]=pEndNode2->point_i;
	long minPointI1=min(arrStartPointI[0],arrEndPointI[0]);
	long minPointI2=min(arrStartPointI[1],arrEndPointI[1]);
	long maxPointI1=max(arrStartPointI[0],arrEndPointI[0]);
	long maxPointI2=max(arrStartPointI[1],arrEndPointI[1]);
	if(minPointI1<minPointI2)
		return -1;
	else if(minPointI1>minPointI2)
		return 1;
	else if(maxPointI1<maxPointI2)
		return -1;
	else if(maxPointI1>maxPointI2)
		return 1;
	else
		return 0;
}
CElemInfo::CElemInfo()
{
	ciFreedomStart=ciFreedomEnd=0;
	memset(&relaElem,0,sizeof(RELATIVE_ELEM));
	m_nClassTypeId = CLS_FEMELEMENT;
	//strcpy(m_sClassName,"CElemInfo");
	//start_joint_state=end_joint_state=0;
	m_pModel=NULL;
	horiElem.hStart=horiElem.hEnd=0;
	cCalLenType=0;
	memset(&smcItems,0,sizeof(SMC_ITEM)*3);
}
short CElemInfo::ElemType(CLDSLinePart* pRod)	//0:�����˻򾭵�����1:����
{
	if(m_pModel&&pRod==NULL)
		pRod=(CLDSLinePart*)m_pModel->FromPartHandle(m_hParentRod,CLS_LINEPART);
	if(pRod&&pRod->size_idClassType==CLS_CABLE)
		return 1;
	else
		return 0;
}
int CElemInfo::StartJointState(CLDSLinePart* pRod)
{
	if(m_pModel&&pRod==NULL)
		pRod=(CLDSLinePart*)m_pModel->FromPartHandle(m_hParentRod,CLS_LINEPART);
	if(pRod&&pRod->pStart&&pRod->pStart->handle==m_hStartNode)	//�˼��˽ڵ㣬��˼��˽ڵ���������һ��
	{
		if(pRod->start_joint_type==JOINT_HINGE)
			return 2;	//ǿ�ƽ½�
		else if(pRod->start_joint_type==JOINT_RIGID)
			return 1;	//�ս�
		else
			return 0;
	}
	else
		return 1;
}
int CElemInfo::EndJointState(CLDSLinePart* pRod)
{
	if(m_pModel&&pRod==NULL)
		pRod=(CLDSLinePart*)m_pModel->FromPartHandle(m_hParentRod,CLS_LINEPART);
	if(pRod&&pRod->pEnd&&pRod->pEnd->handle==m_hEndNode)	//�˼��˽ڵ㣬��˼��˽ڵ���������һ��
	{
		if(pRod->end_joint_type==JOINT_HINGE)
			return 2;	//ǿ�ƽ½�
		else if(pRod->end_joint_type==JOINT_RIGID)
			return 1;	//�ս�
		else
			return 0;
	}
	else
		return 1;
}
int CElemInfo::StartForceType(CLDSLinePart* pRod)
{
	if(m_pModel&&pRod==NULL)
		pRod=(CLDSLinePart*)m_pModel->FromPartHandle(m_hParentRod,CLS_LINEPART);
	if(pRod&&pRod->pStart&&pRod->pStart->handle==m_hStartNode)	//�˼��˽ڵ㣬��˼��˽ڵ���������һ��
		return pRod->start_force_type;
	if(pRod&&pRod->pStart&&pRod->pStart->handle==m_hEndNode)	//�˼��˽ڵ㣬��˼��˽ڵ���������һ��
		return pRod->end_force_type;
	else						//�˼��м�ڵ㣬�϶�����������
		return CENTRIC_FORCE;
}
int CElemInfo::EndForceType(CLDSLinePart* pRod)
{
	if(m_pModel&&pRod==NULL)
		pRod=(CLDSLinePart*)m_pModel->FromPartHandle(m_hParentRod,CLS_LINEPART);
	if(pRod&&pRod->pEnd&&pRod->pEnd->handle==m_hEndNode)	//�˼��˽ڵ㣬��˼��˽ڵ���������һ��
		return pRod->end_force_type;
	if(pRod&&pRod->pEnd&&pRod->pEnd->handle==m_hStartNode)	//�˼��˽ڵ㣬��˼��˽ڵ���������һ��
		return pRod->start_force_type;
	else						//�˼��м�ڵ㣬�϶�����������
		return CENTRIC_FORCE;
}
/////////////////////////////////////////////////////////////////////////////
//�빹��ֱ���йصĹ����������ϵ��alfa1
double QueryIceReviseCoefAlfa1(double diameter)
{
	if(diameter<5.0)
		return 1.1;
	else if(diameter<10.0)
		return 1.2-0.02*diameter;
	else if(diameter<20.0)
		return 1.1-0.01*diameter;
	else if(diameter<30.0)
		return 1.0-0.01*diameter;
	else if(diameter<40.0)
		return 0.95-0.005*diameter;
	else if(diameter<50.0)
		return 0.95-0.005*diameter;
	else if(diameter<60.0)
		return 1.05-0.007*diameter;
	else if(diameter<70.0)
		return 0.81-0.003*diameter;
	else
		return 0.6;
}
//high_pos(m)�߶�λ��,������ȵĸ߶ȵ���ϵ��alfa2
double QueryIceReviseCoefAlfa2(double high_pos)
{
	if(high_pos<10.0)
		return 1.0;
	else if(high_pos<50.0)
		return 0.85+0.015*high_pos;
	else if(high_pos<100.0)
		return 1.2 +0.008*high_pos;
	else if(high_pos<150.0)
		return 1.6+0.004*high_pos;
	else if(high_pos<200.0)
		return 1.6+0.004*high_pos;
	else if(high_pos<250.0)
		return 1.6+0.004*high_pos;
	else if(high_pos<300.0)
		return 2.1+0.002*high_pos;
	else if(high_pos<350.0)
		return 2.1+0.002*high_pos;
	else
		return 2.8;
}

bool WINDSEG_FILTER::ModuleTogetherWith(CLDSDbObject* pDbObj) const
{
	if(pDbObj->IsLegObj())
		return legword.And(pDbObj->cfgword)!=FALSE;
	else
		return bodyword.And(pDbObj->GetUnifiedCfgword())!=FALSE;
}
bool WINDSEG_FILTER::InitFilter(CWindSegment* pWindSeg)
{
	ARRAY_LIST<CLDSNodePtr> nodes(0,pWindSeg->ValidNodeCount());
	int i,count=pWindSeg->ValidNodeCount();
	for(i=0;i<count;i++)
		nodes.append(pWindSeg->nodePtrArr[i]);
	if(pWindSeg->cType==0&&pWindSeg->ValidNodeCount()==2)
	{
		CLDSNode* pMirNode=nodes[1]->GetMirNode(MIRMSG(2));
		if(pMirNode)
			nodes.append(pMirNode);
		pMirNode=nodes[0]->GetMirNode(MIRMSG(2));
		if(pMirNode)
			nodes.append(pMirNode);
	}
	return InitFilter(nodes.m_pData,nodes.GetSize(),pWindSeg->xProjPlaneOrg);
}
bool WINDSEG_FILTER::InitFilter(CLDSNodePtr* nodes,int count,f3dPoint prjOrg)
{
	double Af=0.0;	//�������
	CLDSNode *pNode=NULL, *pStart=NULL, *pEnd=NULL;
	f3dPoint pos,jg_vec;
	f3dLine line;
	int i;
	f3dPoint origin;
	if(count<3)
		return false;
	bool hasbody=false,hasleg=false;
	ARRAY_LIST<f3dPoint> vertexList(0,count);
	pNode=nodes[0];
	for(i=0;i<6;i++)
		bodyword.flag.word[i]=legword.flag.word[i]=0xffffffff;
	for(i=0;i<count;i++)
	{
		pNode=nodes[i];
		if(pNode->IsLegObj())
		{
			legword &= pNode->cfgword;
			hasleg=true;
		}
		else
		{
			hasbody=true;
			bodyword&= pNode->cfgword;
		}
		vertexList.append(pNode->Position(false));
		if(i==0)
			cs.origin=vertexList[i];
		else if(cs.axis_x.IsZero())
		{
			cs.axis_x=vertexList[i]-vertexList[i-1];
			normalize(cs.axis_x);
		}
		else if(cs.axis_y.IsZero())
		{
			cs.axis_y=vertexList[i]-vertexList[i-1];
			normalize(cs.axis_y);
			if(fabs(cs.axis_y*cs.axis_x)>EPS_COS)	//��X�Ṳ��Ϊ��ЧY�᷽��,��ռ���Ѱ��
				cs.axis_y.Set();
			else
			{	//�ҵ���������ϵ
				cs.axis_z = cs.axis_x^cs.axis_y;
				cs.axis_y = cs.axis_z^cs.axis_x;
				normalize(cs.axis_y);
				normalize(cs.axis_z);
			}
		}
	}
	if(!hasbody)
		bodyword.Clear();
	if(!hasleg)
		legword.Clear();
	//�����൲����
	backcs=cs;
	//�����ѹͶӰ��
	prjucs.origin=prjOrg;
	if(fabs(cs.axis_z.x)>fabs(cs.axis_z.y))
	{
		prjucs.axis_z.Set(1,0,0);
		prjucs.axis_x.Set(0,cs.axis_x.y,cs.axis_x.z);
		backcs.origin.y=prjOrg.y*2-backcs.origin.y;
		backcs.axis_x.y*=-1;
		backcs.axis_y.y*=-1;
		backcs.axis_z.y*=-1;
	}
	else
	{
		prjucs.axis_z.Set(0,1,0);
		prjucs.axis_x.Set(cs.axis_x.x,0,cs.axis_x.z);
		backcs.origin.x=prjOrg.x*2-backcs.origin.x;
		backcs.axis_x.x*=-1;
		backcs.axis_y.x*=-1;
		backcs.axis_z.x*=-1;
	}
	normalize(prjucs.axis_x);
	prjucs.axis_y=prjucs.axis_z^prjucs.axis_x;
	minZ=maxZ=0;
	for(i=0;i<count;i++)
	{
		vertexList[i]=cs.TransPToCS(vertexList[i]);
		if(minZ>vertexList[i].z)
			minZ=vertexList[i].z;
		if(maxZ<vertexList[i].z)
			maxZ=vertexList[i].z;
		vertexList[i].z=0;
	}
	//��������ΰ���
	int n=vertexList.GetSize();
	for(i=0;i<n;i++)
	{
		double result=DistOf2dPtLine(vertexList[(i+2)%n],vertexList[i],vertexList[(i+1)%n]);
		if(result<0)	// ��������Ҳ࣬�а��ǳ���
		{
			vertexList.RemoveAt((i+1)%n);
			n--;
		}
	}
	frontrgn.CreatePolygonRgn(vertexList.m_pData,vertexList.GetSize());
	DYN_ARRAY<f3dPoint> prjvertexarr(vertexList.GetSize());
	for(i=0;i<vertexList.GetSize();i++)
	{
		pos=cs.TransPFromCS(vertexList[i]);
		vertexList[i]=prjucs.TransPToCS(pos);
		vertexList[i].z=0;
	}
	region.CreatePolygonRgn(vertexList.m_pData,vertexList.GetSize());
	return true;
}
bool WINDSEG_FILTER::IsIncludePoint(const double* point3d)
{
	GEPOINT prjpick=prjucs.TransPToCS(point3d);
	prjpick.z=0;
	if(region.PtInRgn(prjpick)!=1)
		return false;	//����ͶӰ�����������
	GEPOINT frontpick=cs.TransPToCS(point3d);
	GEPOINT justify_norm_vec(cs.origin.x-prjucs.origin.x,cs.origin.y-prjucs.origin.y);
	double justify=justify_norm_vec.x*cs.axis_z.x+justify_norm_vec.y*cs.axis_z.y;
	if((justify>0&&frontpick.z>maxZ+EPS)||(justify<0&&frontpick.z<minZ-EPS))
		return false;	//λ��ǰ�൲������ݺи����
	GEPOINT backpick=backcs.TransPToCS(point3d);
	if((justify>0&&backpick.z>maxZ+EPS)||(justify<0&&backpick.z<minZ-EPS))
		return false;	//λ�ں�൲������ݺи����
	return true;
}
bool WINDSEG_FILTER::IsIncludeLine(const double* lineStart,const double* lineEnd)
{
	return IsIncludePoint(lineStart)&&IsIncludePoint(lineEnd);
}
bool WINDSEG_FILTER::IsIncludeRod(CLDSLinePart* pRod)
{
	if(!ModuleTogetherWith(pRod)||pRod->pStart==NULL||pRod->pEnd==NULL)
		return false;
	GEPOINT lineStart=pRod->pStart->Position(false);
	GEPOINT lineEnd=pRod->pEnd->Position(false);
	return IsIncludePoint(lineStart)&&IsIncludePoint(lineEnd);
}
bool WINDSEG_FILTER::IsLineInFrontWindBox(CLDSLinePart* pRod)
{
	if(!ModuleTogetherWith(pRod)||pRod->pStart==NULL||pRod->pEnd==NULL)
		return false;
	GEPOINT lineStart=cs.TransPToCS(pRod->pStart->Position(false));
	GEPOINT lineEnd  =cs.TransPToCS(pRod->pEnd->Position(false));
	if(lineStart.z>maxZ+EPS||lineStart.z<minZ-EPS||lineEnd.z>maxZ+EPS||lineEnd.z<minZ-EPS)
		return false;
	lineStart.z=lineEnd.z=0;
	return (frontrgn.PtInRgn(lineStart)==1&&frontrgn.PtInRgn(lineEnd)==1);
}
#endif
CWindSegment::CWindSegment()
{
	iNo=0;
	m_nValidNodeCount=0;
	cType=cMirType=cCalStyle=0;
	memset(nodeArr,0,sizeof(nodeArr));
	memset(nodePtrArr,0,sizeof(nodeArr));
	m_bSpecB_A=FALSE;
	AfFront=AfSide=FaiFront=FaiSide=0;
	B_AFront=B_ASide=1.0;
	K=1.0;
	BetaZ=1.2;
	BetaZForFoundation=1.0;
	CqiFront=CqiSide=1.2;
	m_fProfileModeCoef=1.3;
	dfMuSa = dfMuSb = 1.3;
	m_bAutoCalProfileCoef=TRUE;
	pSideElemPicker=pFrontElemPicker=NULL;
	pMirArmWindSeg = NULL;
}
bool CWindSegment::MirToWindSeg(CWindSegment* pMirWindSeg, int ciMirY1X2,IModel* pModel)
{
	pMirWindSeg->iNo = -iNo;
	pMirWindSeg->m_nValidNodeCount = 0;
	pMirWindSeg->cType = cType;
	pMirWindSeg->cMirType = cMirType;
	pMirWindSeg->cCalStyle = cCalStyle;
	memset(pMirWindSeg->nodeArr, 0, sizeof(nodeArr));
	memset(pMirWindSeg->nodePtrArr, 0, sizeof(nodeArr));
	CLDSNode* pNode, *pMirNode;
	for (int i = 0; i < 8; i++)
	{
		if (nodeArr[i] <= 0)
			break;
		if ((pNode = pModel->FromNodeHandle(nodeArr[i])) == NULL)
			continue;
		if (ciMirY1X2 != 1 && ciMirY1X2 != 2)
		{
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "%d�ŷ�ѹ�ζԳƹ����з��֣�ȱ�ٺ��ʵĶԳ���������", iNo);
			return false;
		}
		MIRMSG mirmsg(3 - ciMirY1X2);
		if ((pMirNode = pNode->GetMirNode(mirmsg)) != NULL)
			pMirWindSeg->nodeArr[i] = pMirNode->handle;
		if ((pMirWindSeg->nodePtrArr[i] = pMirNode) == NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "%d�ŷ�ѹ�Σ������ڵ�0x%X�Գ�ʧ��", iNo, nodeArr[i]);
	}
	pMirWindSeg->m_bSpecB_A = m_bSpecB_A;
	pMirWindSeg->AfFront = AfFront;
	pMirWindSeg->AfSide = AfSide;
	pMirWindSeg->FaiFront = FaiFront;
	pMirWindSeg->FaiSide = FaiSide;
	pMirWindSeg->B_AFront = B_AFront;
	pMirWindSeg->B_ASide = B_ASide;
	pMirWindSeg->K = K;
	pMirWindSeg->BetaZ = BetaZ;
	pMirWindSeg->BetaZForFoundation = BetaZForFoundation;
	pMirWindSeg->CqiFront = CqiFront;
	pMirWindSeg->CqiSide = CqiSide;
	pMirWindSeg->m_fProfileModeCoef = m_fProfileModeCoef;
	pMirWindSeg->dfMuSa = dfMuSa;
	pMirWindSeg->dfMuSb = dfMuSb;
	pMirWindSeg->m_bAutoCalProfileCoef = m_bAutoCalProfileCoef;
	pMirWindSeg->pSideElemPicker = pMirWindSeg->pFrontElemPicker = NULL;
	pMirArmWindSeg = NULL;
	return true;
}
const DWORD HASHTABLESIZE1 = 500;
IMPLEMENT_PROP_FUNC(CWindSegment);
void CWindSegment::InitPropHashtable()
{
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE1);
	propStatusHashtable.CreateHashTable(HASHTABLESIZE1);
	int id=1;
#ifdef AFX_TARG_ENU_ENGLISH
	AddPropItem("attribute",PROPLIST_ITEM(id++,"Wind load seg properties","Wind load seg properties"));
	AddPropItem("type",PROPLIST_ITEM(id++,"Working condition of the name","","Distribution of 0.8 rectangular tower section of the|1. Custom multi-node contour|2. 6 triangular tower section distribution"));
	AddPropItem("cMirType",PROPLIST_ITEM(id++,"Symmetrical way","","No symmetry axisymmetric|X axial symmetry|Y axial symmetry"));
	AddPropItem("cCalStyle",PROPLIST_ITEM(id++,"calculate way","","The tower|X to the cross arm Y to cross arm"));
	AddPropItem("BetaZ",PROPLIST_ITEM(id++,"The wind vibration coefficient (tower)",""));
	AddPropItem("BetaZForFoundation",PROPLIST_ITEM(id++,"The wind vibration coefficient (base)",""));
	AddPropItem("CqiFront",PROPLIST_ITEM(id++,"Positive effect coefficient",""));
	AddPropItem("CqiSide",PROPLIST_ITEM(id++,"Side effect coefficient",""));
	AddPropItem("m_bAutoCalProfileCoef",PROPLIST_ITEM(id++,"Specify a value","","YES|NO"));
	AddPropItem("m_fProfileModeCoef",PROPLIST_ITEM(id++,"Tower shape coefficient",""));
	AddPropItem("m_bSpecB_A",PROPLIST_ITEM(id++,"Specify a value","Specify a value","YES|NO"));
	AddPropItem("B_AFront",PROPLIST_ITEM(id++,"Positive depth/width",""));
	AddPropItem("K",PROPLIST_ITEM(id++,"The towers are cross section shape coefficient",""));
	AddPropItem("basis_points",PROPLIST_ITEM(id++,"Illustrating the basis points",""));
	AddPropItem("X",PROPLIST_ITEM(id++,"X","X"));
	AddPropItem("Y",PROPLIST_ITEM(id++,"Y","Y"));
	AddPropItem("Z",PROPLIST_ITEM(id++,"Z","Z"));
#else
	AddPropItem("attribute",PROPLIST_ITEM(id++,"��ѹ������","��ѹ������"));
	AddPropItem("type",PROPLIST_ITEM(id++,"��ѹ������","��ѹ������","0.�˵������������|1.�Զ����ڵ�����|2.����������������"));
	AddPropItem("cMirType",PROPLIST_ITEM(id++,"�ԳƷ�ʽ","�ԳƷ�ʽ","�޶Գ�|X��Գ�|Y��Գ�"));
	AddPropItem("cCalStyle",PROPLIST_ITEM(id++,"���㷽ʽ","���㷽ʽ","����|X��ᵣ|Y��ᵣ"));
	AddPropItem("BetaZ",PROPLIST_ITEM(id++,"����ϵ��(����)","����ϵ��(����)"));
	AddPropItem("BetaZForFoundation",PROPLIST_ITEM(id++,"����ϵ��(����)","����ϵ��(����)"));
	AddPropItem("CqiFront",PROPLIST_ITEM(id++,"����ЧӦϵ��","����ЧӦϵ��"));
	AddPropItem("CqiSide",PROPLIST_ITEM(id++,"����ЧӦϵ��","����ЧӦϵ��"));
	AddPropItem("m_bAutoCalProfileCoef",PROPLIST_ITEM(id++,"��������ϵ��","�Ƿ��Զ�������������ϵ��","�Զ�����|�ֶ�ָ��"));
	AddPropItem("m_fProfileModeCoef",PROPLIST_ITEM(id++,"ϵ��","��������ϵ��"));
	AddPropItem("m_bSpecB_A",PROPLIST_ITEM(id++,"�������/���","�û�ָ��'�������/���'ֵ","�Զ�����|�ֶ�ָ��"));
	AddPropItem("B_AFront",PROPLIST_ITEM(id++,"ϵ��","�������/���"));
	AddPropItem("K",PROPLIST_ITEM(id++,"���������״ϵ��","���������״ϵ��"));
	AddPropItem("basis_points",PROPLIST_ITEM(id++,"ͶӰ�����","ͶӰ�����"));
	AddPropItem("X",PROPLIST_ITEM(id++,"X","ͶӰ�����X����"));
	AddPropItem("Y",PROPLIST_ITEM(id++,"Y","ͶӰ�����Y����"));
	AddPropItem("Z",PROPLIST_ITEM(id++,"Z","ͶӰ�����Z����"));
#endif
}
int CWindSegment::GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(GetPropID("type")==id)
	{
		if(cType==0)
			sText.Printf("0.�˵������������");
		else if(cType==1)
			sText.Printf("1.�Զ����ڵ�����");
		else if(cType==2)
			sText.Printf("2.����������������");
	}
	else if(GetPropID("cMirType")==id)
	{
		if(cType==0||cType==2||cMirType==0)
			sText.Printf("�޶Գ�");
		else if(cMirType==1)
			sText.Printf("X��Գ�");
		else if(cMirType==2)
			sText.Printf("Y��Գ�");
	}
	else if(GetPropID("cCalStyle")==id)
	{
		if(cType==0||cType==2||cCalStyle==0)
			sText.Printf("����");
		else if(cCalStyle==1)
			sText.Printf("X��ᵣ");
		else if(cCalStyle==2)
			sText.Printf("Y��ᵣ");
	}
	else if(GetPropID("BetaZ")==id)
		sText.Printf("%lg",BetaZ);
	else if(GetPropID("BetaZForFoundation")==id)
		sText.Printf("%lg",BetaZForFoundation);
	else if(GetPropID("CqiFront")==id)
		sText.Printf("%lg",CqiFront);
	else if(GetPropID("CqiSide")==id)
		sText.Printf("%lg",CqiSide);
	else if(GetPropID("m_fProfileModeCoef")==id)
		sText.Printf("%lg",m_fProfileModeCoef);
	else if(GetPropID("m_bAutoCalProfileCoef")==id)
	{
		if(m_bAutoCalProfileCoef)
			sText.Copy("�Զ�����");
		else
			sText.Copy("�ֶ�ָ��");
	}
	else if(GetPropID("B_AFront")==id)
		sText.Printf("%lg",B_AFront);
	else if(GetPropID("m_bSpecB_A")==id)
	{
		if(m_bSpecB_A)
			sText.Copy("�ֶ�ָ��");
		else
			sText.Copy("�Զ�����");
	}
	else if(GetPropID("K")==id)
		sText.Printf("%lg",K);
	else if(GetPropID("X")==id)
		sText.Printf("%lg", xProjPlaneOrg.x);
	else if(GetPropID("Y")==id)
		sText.Printf("%lg", xProjPlaneOrg.x);
	else if(GetPropID("Z")==id)
		sText.Printf("%lg", xProjPlaneOrg.x);
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
bool CWindSegment::ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType/*=0*/)
{
	bool replaced=false;
	for(int i=0;i<8;i++)
		replaced|=replacer.VerifyAndReplace(&nodeArr[i]);
	return replaced;
}
long CWindSegment::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (nodeArr[i] > 0)
			pListHandlers->Append(HANDLE_ADDR(&nodeArr[i]));
		else
			break;
	}
	return i;
}
void CWindSegment::CleanInvalidObjRef(IModel* pBelongModel)
{
	for(int i=0;i<8;i++)
	{
		if(nodeArr[i]<=0x20)
			continue;
		pBelongModel->ValidateObjRefH(&nodeArr[i]);
	}
}
void CWindSegment::ToBuffer(CBuffer& buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteInteger(iNo);
	//buffer.WriteInteger(iType);
	buffer.WriteByte(cType);
	buffer.WriteByte(cMirType);
	buffer.WriteByte(cCalStyle);
	buffer.LogPosition();
	buffer.WriteInteger(0);
	int i,n=GetMaxNodeCount();
	for(i=0;i<n;i++)
	{
		if(nodeArr[i]>0)
			buffer.WriteInteger(nodeArr[i]);
		else
			break;
	}
	buffer.RecallPosition();
	buffer.WriteInteger(i);
	buffer.RecallPosition();
	buffer.WriteDouble(CqiFront);
	buffer.WriteDouble(CqiSide);
	buffer.WriteDouble(K);
	buffer.WriteDouble(AfFront);
	buffer.WriteDouble(AfSide);
	buffer.WritePoint(xProjPlaneOrg);			//ͶӰ�����
	buffer.WriteInteger(m_bSpecB_A);			//���û�ָ��ӭ����������ȱ�ֵ
	buffer.WriteInteger(m_bAutoCalProfileCoef);	//�Զ������������幹������ϵ��
	buffer.WriteDouble(m_fProfileModeCoef);		//�������幹������ϵ��
	buffer.WriteDouble(FaiFront);
	buffer.WriteDouble(B_AFront);
	buffer.WriteDouble(FaiSide);
	buffer.WriteDouble(B_ASide);
	buffer.WriteDouble(BetaZ);
	buffer.WriteDouble(BetaZForFoundation);
}
void CWindSegment::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<��ѹ����Ϣ>\n");
	fprintf(fp,"<��� iNo=\"%d\"/>\n",iNo);
	if (cType==0)
		fprintf(fp,"<��׼�κ� cType=\"0.�˵���������\"/>\n");
	else if (cType==1)
		fprintf(fp,"<��׼�κ� cType=\"1.�Զ����ڵ�����\"/>\n");
	else if (cType==2)
		fprintf(fp,"<��׼�κ� cType=\"2.���������������\"/>\n");
	if (cMirType==0)
		fprintf(fp,"<�Գ���Ϣ cMirType=\"0.�޶Գ�\"/>\n");
	else if (cMirType==1)
		fprintf(fp,"<�Գ���Ϣ cMirType=\"1.Y��Գ�\"/>\n");
	else if (cMirType==2)
		fprintf(fp,"<�Գ���Ϣ cMirType=\"2.X��Գ�\"/>\n");
	if (cCalStyle==0)
		fprintf(fp,"<���㷽ʽ cCalStyle=\"0.������ʽ�����ѹ\"/>\n");
	else if (cCalStyle==1)
		fprintf(fp,"<���㷽ʽ cCalStyle=\"1.��X����ˮƽ�ᵣ��ʽ�����ѹ\"/>\n");
	else if (cCalStyle==2)
		fprintf(fp,"<���㷽ʽ cCalStyle=\"2.��Y����ˮƽ�ᵣ��ʽ�����ѹ\"/>\n");

	//buffer.LogPosition();
	//buffer.WriteInteger(0);
	int i,n=GetMaxNodeCount();
	fprintf(fp,"<�˸������ڵ�ľ�� nodeArr=\"[");
	for(i=0;i<n;i++)
	{
		if(nodeArr[i]>0)
		{
			if (i==0)
				fprintf(fp,"0X%X",nodeArr[i]);
			else
				fprintf(fp,",0X%X",nodeArr[i]);
		}
		else
			break;
	}
	fprintf(fp,"]\"/>\n");
	//buffer.RecallPosition();
	fprintf(fp,"<iֵ i=\"%d\"/>\n",i);
	//buffer.RecallPosition();
	sprintf(sText,"%f",CqiFront);
	SimplifiedNumString(sText);
	fprintf(fp,"<��������ϵ�� CqiFront=\"%s\"/>\n",sText);
	sprintf(sText,"%f",CqiSide);
	SimplifiedNumString(sText);
	fprintf(fp,"<��������ϵ�� CqiSide=\"%s\"/>\n",sText);
	sprintf(sText,"%f",K);
	SimplifiedNumString(sText);
	fprintf(fp,"<�������ض�����״ϵ�� K=\"%s\"/>\n",sText);
	sprintf(sText,"%f",AfFront);
	SimplifiedNumString(sText);
	fprintf(fp,"<m2����������ܷ�ѹ��ͶӰ��� AfFront=\"%s\"/>\n",sText);
	sprintf(sText,"%f",AfSide);
	SimplifiedNumString(sText);
	fprintf(fp,"<m2����������ܷ�ѹ��ͶӰ��� AfSide=\"%s\"/>\n",sText);
	sprintf(sText,"%f",xProjPlaneOrg.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",xProjPlaneOrg.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",xProjPlaneOrg.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<ͶӰ����� xProjPlaneOrg=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",m_bSpecB_A);
	SimplifiedNumString(sText);
	fprintf(fp,"<���û�ָ��ӭ����������ȱ�ֵ m_bSpecB_A=\"%s\"/>\n",sText);
	sprintf(sText,"%f",m_bAutoCalProfileCoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<�Զ������������幹������ϵ�� m_bAutoCalProfileCoef=\"%s\"/>\n",sText);
	sprintf(sText,"%f",m_fProfileModeCoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<�������幹������ϵ�� m_fProfileModeCoef=\"%s\"/>\n",sText);
	sprintf(sText,"%f",FaiFront);
	SimplifiedNumString(sText);
	fprintf(fp,"<���浲��ϵ�� FaiFront=\"%s\"/>\n",sText);
	sprintf(sText,"%f",B_AFront);
	SimplifiedNumString(sText);
	fprintf(fp,"<��������ӭ�����뱳����֮����������ӭ������ B_AFront=\"%s\"/>\n",sText);
	sprintf(sText,"%f",FaiSide);
	SimplifiedNumString(sText);
	fprintf(fp,"<���浲��ϵ�� FaiSide=\"%s\"/>\n",sText);
	sprintf(sText,"%f",B_ASide);
	SimplifiedNumString(sText);
	fprintf(fp,"<��������ӭ�����뱳����֮����������ӭ������ B_ASide=\"%s\"/>\n",sText);
	sprintf(sText,"%f",BetaZ);
	SimplifiedNumString(sText);
	fprintf(fp,"<����ϵ����Ʒ���ص���ϵ�� BetaZ=\"%s\"/>\n",sText);
	sprintf(sText,"%f",BetaZForFoundation);
	SimplifiedNumString(sText);
	fprintf(fp,"<���ڻ�����Ƶķ���ϵ����Ʒ���ص���ϵ�� BetaZForFoundation=\"%s\"/>\n",sText);
	fprintf(fp,"</��ѹ����Ϣ>\n");
}
void CWindSegment::FromBuffer(CBuffer& buffer, long version/*= NULL*/,long doc_type/*=3*/)
{
	buffer.ReadInteger(&iNo);
	if(version==0||version>=1000008)//version==NULL||compareVersion(version,"1.0.0.8")>=0)
	{
		buffer.ReadByte(&cType);
		buffer.ReadByte(&cMirType);
		buffer.ReadByte(&cCalStyle);
		int n=4;
		buffer.ReadInteger(&n);
		if(n>8)
#ifdef AFX_TARG_ENU_ENGLISH
			throw "The node number of wind pressure is more than 8,so the data file is wrong��";
#else 
			throw "��ѹ�νڵ������������������ļ�����";
#endif
		for(int i=0;i<n;i++)
			buffer.ReadInteger(&nodeArr[i]);
	}
	else
	{
		long iType;
		buffer.ReadInteger(&iType);
		cType=(BYTE)iType%100;
		if(cType==1)	//0.�˽ڵ���������
			cType=0;
		else if(cType<8)
			cType=1;	//1.�Զ����ڵ�����
		else
			cType=2;	//2.���ڵ����������
		cCalStyle=(BYTE)(iType/1000);	//0.����ʽ����1.X����ˮƽ�ᵣ2.Y����ˮƽ�ᵣ
		cMirType=(BYTE)((iType%1000)/100);	//�Գ���Ϣ0.�޶Գ�1.Y��Գ�2.X��Գ�
		for(int i=0;i<4;i++)
			buffer.ReadInteger(&nodeArr[i]);
	}
	buffer.ReadDouble(&CqiFront);
	buffer.ReadDouble(&CqiSide);
	buffer.ReadDouble(&K);
	buffer.ReadDouble(&AfFront);
	buffer.ReadDouble(&AfSide);
	if(version==0||(doc_type==3&&version>=1080101)||(doc_type==4&&version>=1010101))
	{
		buffer.ReadPoint(xProjPlaneOrg);			//ͶӰ�����
		buffer.ReadInteger(&m_bSpecB_A);			//���û�ָ��ӭ����������ȱ�ֵ
		buffer.ReadInteger(&m_bAutoCalProfileCoef);	//�Զ������������幹������ϵ��
		buffer.ReadDouble(&m_fProfileModeCoef);		//�������幹������ϵ��
	}
	buffer.ReadDouble(&FaiFront);
	buffer.ReadDouble(&B_AFront);
	if(B_AFront<=0)	//0�϶����������һᵼ�·�ѹ�ζԻ��򱨴������ݸ�ֵ1.0��������Զ�����
		B_AFront=1.0;
	buffer.ReadDouble(&FaiSide);
	buffer.ReadDouble(&B_ASide);
	buffer.ReadDouble(&BetaZ);
	if(version==0||(doc_type==3&&version>=1070309)||(doc_type==4&&version>=1000309))
		buffer.ReadDouble(&BetaZForFoundation);
}

void CWindSegment::InitNodePtrArr(IModel *pModel)
{
	if(pModel==NULL)
		return;
	int n=sizeof(nodeArr)/4;
	memset(nodePtrArr,0,sizeof(nodePtrArr));
	/*
	CHashTable<CLDSNode*> hashTable;
	hashTable.CreateHashTable(pTower->Node.GetNodeNum());
	BOOL bPush=pTower->Node.push_stack();
	CLDSNode *pNode;
	for(pNode=pTower->Node.GetFirst();pNode;pNode=pTower->Node.GetNext())
		hashTable.SetValueAt(pNode->handle,pNode);
	for(int i=0;i<n;i++)
	{
		if(nodeArr[i]>0)
		{
			if(hashTable.GetValueAt(nodeArr[i],pNode))
				nodePtrArr[i]=(CLDSNode*)pNode;
		}
	}
	if(bPush)
		pTower->Node.pop_stack();
	*/
	// ����Ta.Node�Ѱ�����ϣ���������������Ϊ�������С�wjh-2012.8.11
	for(int i=0;i<n;i++)
	{
		if(nodeArr[i]>0)
			nodePtrArr[i]=pModel->FromNodeHandle(nodeArr[i]);
	}
}
bool CWindSegment::StatProfileNodeSet(IModel* pTower,NODESET &nodeset,BOOL bFront/*=TRUE*/)
{
	if(pTower==NULL)
		return false;
	int i,n=sizeof(nodeArr)/4;
	memset(nodePtrArr,0,sizeof(nodePtrArr));
	//CHashTable<CLDSNode*> hashTable;
	//hashTable.CreateHashTable(pTower->Node.GetNodeNum());
	int nValidNode=StatValidNodeCount(pTower);
	if(cType==0)	//�˵���������
	{
		if(nodePtrArr[0]==NULL||nodePtrArr[1]==NULL)
			return false;
		nodeset.append(nodePtrArr[0]);
		nodeset.append(nodePtrArr[1]);
		if(bFront)
		{	//����Y��Գ�������
			nodePtrArr[2]=nodePtrArr[1]->GetMirNode(MIRMSG(2));
			nodePtrArr[3]=nodePtrArr[0]->GetMirNode(MIRMSG(2));
		}
		else
		{	//����X��Գ�������
			nodePtrArr[2]=nodePtrArr[1]->GetMirNode(MIRMSG(1));
			nodePtrArr[3]=nodePtrArr[0]->GetMirNode(MIRMSG(1));
		}
		if(nodePtrArr[2]==NULL||nodePtrArr[3]==NULL)
			return false;
		nodeset.append(nodePtrArr[2]);
		nodeset.append(nodePtrArr[3]);
	}
	else if(cType==1)
	{	//�ᵣ��ڵ�������
		if(nValidNode<3)
			return false;
		for(i=0;i<nValidNode;i++)
			nodeset.append(nodePtrArr[i]);
	}
	else if(cType==2)
	{	//���������
		if(nValidNode==4)
		{
			nodeset.append(nodePtrArr[0]);
			nodeset.append(nodePtrArr[1]);
			nodeset.append(nodePtrArr[2]);
			nodeset.append(nodePtrArr[3]);
		}
		else
			return false;
	}
	return true;
}
bool CWindSegment::IsIncludePoint(IModel* pTower, const double* pxPoint, double tolerance /*= 1.0*/)
{
#if defined(__PART_DESIGN_INC_)&&defined(__LDS_CONTEXT_)
	if (pTower == NULL)
		return false;
	int i, nValidNode, n = sizeof(nodeArr) / 4;
	if (xTaperScope.nCount == 0)
	{
		memset(nodePtrArr, 0, sizeof(nodePtrArr));
		nValidNode = StatValidNodeCount(pTower);
	}
	else
		return xTaperScope.IsIncludePoint(pxPoint, tolerance);
	if (cType == 0)	//�˵���������
	{
		if (nodePtrArr[0] == NULL || nodePtrArr[1] == NULL)
			return false;
		//����Y��Գ�������
		nodePtrArr[2] = nodePtrArr[1]->GetMirNode(MIRMSG(2));
		nodePtrArr[3] = nodePtrArr[0]->GetMirNode(MIRMSG(2));
		if (nodePtrArr[2] == NULL || nodePtrArr[3] == NULL)
			return false;
		GEPOINT xarrPoints[4];
		xarrPoints[0] = nodePtrArr[0]->xOriginalPos;
		xarrPoints[1] = nodePtrArr[1]->xOriginalPos;
		xarrPoints[2] = nodePtrArr[2]->xOriginalPos;
		xarrPoints[3] = nodePtrArr[3]->xOriginalPos;
		xTaperScope.InitPolyVertexes(this->xProjPlaneOrg, GEPOINT(0, 1, 0), xarrPoints, 4);
		return xTaperScope.IsIncludePoint(pxPoint, tolerance);
	}
	else if (cType == 1)
	{	//�ᵣ��ڵ�������
		if (nValidNode < 3)
			return false;
		GEPOINT _arrPoints[8];
		ARRAY_LIST<GEPOINT> xarrPolyVertexes(_arrPoints, 8);
		for (i = 0; i < nValidNode; i++)
			xarrPolyVertexes.append(nodePtrArr[i]->xOriginalPos);
		GEPOINT vBasePlaneNormal(0, 1, 0);
		if (this->cCalStyle == 2)	//Y��ˮƽ�ᵣ
			vBasePlaneNormal.Set(1, 0, 0);
		xTaperScope.InitPolyVertexes(this->xProjPlaneOrg, vBasePlaneNormal, 
			xarrPolyVertexes.m_pData, xarrPolyVertexes.GetSize());
		return xTaperScope.IsIncludePoint(pxPoint, tolerance);
	}
	else if (cType == 2)
	{	//���������
		/*if (nValidNode == 4)
		{
			nodeset.append(nodePtrArr[0]);
			nodeset.append(nodePtrArr[1]);
			nodeset.append(nodePtrArr[2]);
			nodeset.append(nodePtrArr[3]);
		}
		else*/
		return false;
	}
#endif
	return true;
}
bool CWindSegment::IsIncludeElem(IModel* pTower, const double* pxElemStart, const double* pxElemEnd,double tolerance/*=1*/)
{
#if defined(__PART_DESIGN_INC_)&&defined(__LDS_CONTEXT_)
	if (pTower == NULL)
		return false;
	int i, nValidNode,n = sizeof(nodeArr) / 4;
	if (xTaperScope.nCount == 0)
	{
		memset(nodePtrArr, 0, sizeof(nodePtrArr));
		nValidNode = StatValidNodeCount(pTower);
	}
	else
		return xTaperScope.IsIncludeLine(pxElemStart, pxElemEnd, tolerance);
	if (cType == 0)	//�˵���������
	{
		if (nodePtrArr[0] == NULL || nodePtrArr[1] == NULL)
			return false;
		//����Y��Գ�������
		nodePtrArr[2] = nodePtrArr[1]->GetMirNode(MIRMSG(2));
		nodePtrArr[3] = nodePtrArr[0]->GetMirNode(MIRMSG(2));
		if (nodePtrArr[2] == NULL || nodePtrArr[3] == NULL)
			return false;
		GEPOINT xarrPoints[4];
		xarrPoints[0] = nodePtrArr[0]->xOriginalPos;
		xarrPoints[1] = nodePtrArr[1]->xOriginalPos;
		xarrPoints[2] = nodePtrArr[2]->xOriginalPos;
		xarrPoints[3] = nodePtrArr[3]->xOriginalPos;
		xTaperScope.InitPolyVertexes(this->xProjPlaneOrg, GEPOINT(0, 1, 0), xarrPoints, 4);
		return xTaperScope.IsIncludeLine(pxElemStart, pxElemEnd, tolerance);
	}
	else if (cType == 1)
	{	//�ᵣ��ڵ�������
		if (nValidNode < 3)
			return false;
		GEPOINT _arrPoints[8];
		ARRAY_LIST<GEPOINT> xarrPolyVertexes(_arrPoints, 8);
		for (i = 0; i < nValidNode; i++)
			xarrPolyVertexes.append(nodePtrArr[i]->xOriginalPos);
		GEPOINT vBasePlaneNormal(0, 1, 0);
		if (this->cCalStyle == 2)	//Y��ˮƽ�ᵣ
			vBasePlaneNormal.Set(1, 0, 0);
		xTaperScope.InitPolyVertexes(this->xProjPlaneOrg, vBasePlaneNormal,
			xarrPolyVertexes.m_pData, xarrPolyVertexes.GetSize());
		return xTaperScope.IsIncludeLine(pxElemStart, pxElemEnd, tolerance);
	}
	else if (cType == 2)
	{	//���������
		/*if (nValidNode == 4)
		{
			nodeset.append(nodePtrArr[0]);
			nodeset.append(nodePtrArr[1]);
			nodeset.append(nodePtrArr[2]);
			nodeset.append(nodePtrArr[3]);
		}
		else*/
			return false;
	}
#endif
	return true;
}
bool CWindSegment::IsHasLegObj(IModel* pTower)
{
	if(nodePtrArr[0]==NULL)
		InitNodePtrArr(pTower);
	for(int i=0;i<8;i++)
	{
		if(nodePtrArr[i]==NULL)
			return false;
		if(nodePtrArr[i]->IsLegObj())
			return true;
	}
	return false;
}
CFGWORD CWindSegment::StatCfgword(IModel* pTower)
{
	if(nodePtrArr[0]==NULL)
		InitNodePtrArr(pTower);
	CFGWORD cfgword;
	int i;
	for(i=0;i<6;i++)
		cfgword.flag.word[i]=0xffffffff;
	for(i=0;i<8;i++)
	{
		if(nodePtrArr[i]==NULL)
			break;
		cfgword&=nodePtrArr[i]->GetUnifiedCfgword();
	}
	return cfgword;
}

int CWindSegment::StatValidNodeCount(IModel *pTower)
{
	InitNodePtrArr(pTower);
	int n=GetMaxNodeCount();
	for(m_nValidNodeCount=0;m_nValidNodeCount<n;m_nValidNodeCount++)
	{
		if(nodePtrArr[m_nValidNodeCount]!=NULL)
			continue;
		else
			return m_nValidNodeCount;
	}
	return n;
}
///////////////////////////////////////////////////////////////////////////////////
CConnectInfo::CConnectInfo()
{
	strcpy(grade,"6.8");//��˨����4.8;5.8;6.8;8.8;10.9
	d=20;				//��˨ֱ�������
	uiLegacyN=1;	//��˨���е���˨����
	rows=1;				//��˨��������
	bearloadcoef=0;		//��˨����ϵ��
	m_iConnectWing=0;	//�Ǹ�����֫����Ǹ�˫֫:0��X֫:1��Y֫:2���ӣ�
	m_iConnectWing=0;
	m_iConnectType=0;
	//strcpy(flNo,"");
	idAngleJoint=0;
	m_hCoupleRod=0;
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	maxTension=maxCompression=maxMoment=0;
#endif
}

CConnectInfo::~CConnectInfo()
{
	//boltList.Empty();
}
WORD CConnectInfo::get_wnSummBoltN()
{
	WORD* pwnSummBoltCount=((WORD*)&uiLegacyN)+1;
	WORD wnBoltCount=(*pwnSummBoltCount);//&0x7fff;
	return (wnBoltCount!=0)?wnBoltCount:wnConnBoltN;
}
WORD CConnectInfo::set_wnSummBoltN(WORD wnBoltCount)
{
	WORD* pwnSummBoltCount=((WORD*)&uiLegacyN)+1;
	*pwnSummBoltCount=wnBoltCount;
	return wnBoltCount;
}
WORD CConnectInfo::get_wnDualShearConnBoltN()
{
	if ((uiLegacyN&0xffff0000) == 0)
		return wnConnBoltN;	//δ���������˨������ʾ�����Ƿ���ģ���ֹ������ʵ�ʵ���������˨��
	else
		return wnConnBoltN / 2 + wnConnBoltN % 2;
}

void CConnectInfo::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
/*#if defined(__TMA_FILE_)||defined(__TMA_)
	if(version==0||version>=4000022)//version==NULL||compareVersion(version,"4.0.0.22")>=0)
#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version==0||version>=1000077)//version==NULL||compareVersion(version,"1.0.0.77")>=0)
#elif defined(__TDA_FILE_)
	if(true)//version==NULL||compareVersion(version,"1.0.0.77")>=0)
#else
	if(version==0||version>=1000003)//version==NULL||compareVersion(version,"1.0.0.3")>=0)
#endif*/
	if( version==0||((doc_type==1&&version>=4000022)||
		(doc_type==2&&version>=1000077)||(doc_type>2&&version>=1000003)||doc_type==5))
		buffer.ReadString(grade);
	buffer.ReadInteger(&d);//��˨���ڴ������˨�Ĺ��
	buffer.Read(&uiLegacyN,4);			//��˨������˨������
	buffer.ReadInteger(&rows);		//��˨������˨������
	if( version==0||
		(doc_type==1&&version>=4010300)||	//TMA(V4,1,3,0)
		(doc_type==2&&version>=1010300)||	//LMA(V1,1,3,0)
		(doc_type==4&&version>=1030002)||	//LDS(V1,3,0,2)
		(doc_type==5&&version>=1030002))	//TDA(V1,3,0,2)
	{
		buffer.ReadByte(&m_iConnectWing);
		buffer.ReadByte(&m_iConnectType);
	}
/*#if defined(__TMA_FILE_)||defined(__TMA_)
	if(version>0&&version<4000023)//version!=NULL&&compareVersion(version,"4.0.0.23")<0)
#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version>0&&version<1000078)//version!=NULL&&compareVersion(version,"1.0.0.78")<0)
#elif defined(__TDA_FILE_)
	if(false)//version==NULL||compareVersion(version,"1.0.0.77")>=0)
#else
	if(version>0&&version<1000005)//version!=NULL&&compareVersion(version,"1.0.0.5")<0)
#endif*/
	if( version>0&&doc_type!=5&&((doc_type==1&&version<4000023)||
		(doc_type==2&&version<1000078)||(doc_type>2&&version<1000005)))
	{
		JGZJ jgzj;
		long layout_style;
		f3dLine ber_ls_line,wing_ls_line;
		buffer.ReadInteger(&layout_style);		//��˨���з�ʽ(�ڽǸ��ϵ����λ��)
		buffer.ReadInteger(&jgzj.g);		//��˨���е���˨����(�Ǹ�׼��)��
		buffer.ReadInteger(&jgzj.g1);		//��˨���е���˨����(�Ǹ�׼��)��
		buffer.ReadInteger(&jgzj.g2);		//��˨���е���˨����(�Ǹ�׼��)��
		buffer.ReadInteger(&jgzj.g3);		//��˨���е���˨����(�Ǹ�׼��)��
		buffer.ReadDouble(&ber_ls_line.startPt.x);
		buffer.ReadDouble(&ber_ls_line.startPt.y);
		buffer.ReadDouble(&ber_ls_line.startPt.z);
		buffer.ReadDouble(&ber_ls_line.endPt.x);
		buffer.ReadDouble(&ber_ls_line.endPt.y);
		buffer.ReadDouble(&ber_ls_line.endPt.z);
		buffer.ReadDouble(&wing_ls_line.startPt.x);
		buffer.ReadDouble(&wing_ls_line.startPt.y);
		buffer.ReadDouble(&wing_ls_line.startPt.z);
		buffer.ReadDouble(&wing_ls_line.endPt.x);
		buffer.ReadDouble(&wing_ls_line.endPt.y);
		buffer.ReadDouble(&wing_ls_line.endPt.z);
	}
//#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
#ifndef __PART_DESIGN_INC_
	double maxTension=0,maxCompression=0,maxMoment=0;
#endif
	if( (doc_type==3||doc_type==4)&&
		(version==0||(doc_type==3&&version>=1070306)||(doc_type==4&&version>=1000306)))
	{
		buffer.ReadDouble(&maxTension);
		buffer.ReadDouble(&maxCompression);
		buffer.ReadDouble(&maxMoment);
	}
//#endif
}
void CConnectInfo::ToBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	buffer.WriteString(grade);
	buffer.WriteInteger(d);//��˨���ڴ������˨�Ĺ��
	buffer.Write(&uiLegacyN,4);//WriteInteger(N);	//��˨������˨������
	buffer.WriteInteger(rows);		//��˨������˨������
	if( version==0||
		(doc_type==1&&version>=4010300)||	//TMA(V4,1,3,0)
		(doc_type==2&&version>=1010300)||	//LMA(V1,1,3,0)
		(doc_type==4&&version>=1030002)||	//LDS(V1,3,0,2)
		(doc_type==5&&version>=1030002))	//TDA(V1,3,0,2)
	{
		buffer.WriteByte(m_iConnectWing);
		buffer.WriteByte(m_iConnectType);
	}
#ifndef __PART_DESIGN_INC_
	double maxTension=0,maxCompression=0,maxMoment=0;
#endif
	if(doc_type==3||doc_type==4)
	{
		buffer.WriteDouble(maxTension);
		buffer.WriteDouble(maxCompression);
		buffer.WriteDouble(maxMoment);
	}
}
void CConnectInfo::CopyBasicBoltInfo(CConnectInfo &src)
{
	strcpy(grade,src.grade);
	d=src.d;
	uiLegacyN=src.uiLegacyN;
	rows=src.rows;
}
//
void SIZE_WTM::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	buffer.ReadDouble(&width);
	buffer.ReadDouble(&thick);
	buffer.ReadByte(&cMaterial);
	buffer.ReadByte(&idClsType);
	buffer.ReadByte(&cSubClassType);
}
void SIZE_WTM::ToBuffer(CBuffer &buffer)
{
	buffer.WriteDouble(width);
	buffer.WriteDouble(thick);
	buffer.WriteByte(cMaterial);
	buffer.WriteByte(idClsType);
	buffer.WriteByte(cSubClassType);
}
//Object
long HexStrToLong(const char* idstr)
{
	CXhChar16 str(idstr);
	int i,index=0,len=str.GetLength();
	BYTE halfbytes[8]={0};
	for(i=len-1,index=0;i>=0&&index<8;i--)
	{
		if(str[i]>='0'&&str[i]<='9')
			halfbytes[index++] = str[i]-'0';
		else if(str[i]>='A'&&str[i]<='F')
			halfbytes[index++] = str[i]-'A'+10;
		else if(str[i]>='a'&&str[i]<='f')
			halfbytes[index++] = str[i]-'a'+10;
		if(str[i]=='x'||str[i]=='X'||str[i]=='*')
			break;
	}
	long h=0;
	BYTE* byteArr=(BYTE*)&h;
	for(i=0;i<4;i++)
		byteArr[i]=halfbytes[i*2]+(halfbytes[i*2+1]<<4);
	return h;
}
CLDSObject::CLDSObject(){	// ���캯��
	m_nClassTypeId = CLS_OBJECT;
	//strcpy(m_sClassName,"CLDSObject");
	feature = 0;
	handle = 0x0;
	m_bLocked = FALSE;
	m_pModified=NULL;
	m_pModel = NULL;
	m_hBlock=0;
	m_iShadowObjType=0;
	m_pMotherObj=NULL;
	shadow.BLOCK_OBJ.m_hBlockRef=0;
	shadow.BLOCK_OBJ.m_hMotherObj=0;
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN��������Ա���;
}
CLDSObject::~CLDSObject()
{
}
long CLDSObject::get_Id(){return handle;}
long CLDSObject::set_Id(long hId)
{
	return handle =hId;
}
CXhChar50 CLDSObject::GetBlockIdName()
{
	CXhChar50 idname;
	CBlockModel* pBlock=NULL;
	if(m_hBlock>0x20 && BelongModel() && BelongModel()->IsTowerModel())
		pBlock=((CTower*)BelongModel())->Block.FromHandle(m_hBlock);
	if(pBlock)
	{
		idname.Copy(pBlock->name);
		idname.Append('#');
		idname.Append(CXhChar16("0X%X",m_hBlock));
	}
	else
#ifdef AFX_TARG_ENU_ENGLISH
		idname.Copy("None");
#else 
		idname.Copy("��");
#endif
	return idname;
}
static CLogFile emptylogfile;
ILog2File* CLDSObject::Log2File()
{
	if(log2file)
		return log2file;
	else
		return &emptylogfile;
}
IModel* CLDSObject::BelongModel() const{
	if(m_pModel)
		return m_pModel;
	else if(CLDSDbObject::_console)
		return CLDSDbObject::_console->GetActiveModel();
	else
		return NULL;
}

void CLDSObject::GetClassType(char class_type[])
{
	//strcpy(class_type,m_sClassName);
}
bool CLDSObject::IsAcceptableTypeId(int type_id)
{
	if(type_id==m_nClassTypeId||type_id==CLS_OBJECT)
		return true;	//��ȫһ�»�Ϊ�������������
	else
		return false;
}

DWORD CLDSObject::SetPermission(DWORD dwPerm)
{
	dwPermission=dwPerm;
	return dwPerm;
}

void CLDSObject::SetShadowPara(CLDSObject *pMotherObj,long hObj,int iShadowObjType/*=SHADOW_ISOLATED*/)
{
	m_iShadowObjType=SHADOW_NONE;
	if(m_pModel==NULL||!m_pModel->IsTowerModel())
		return;
	if(pMotherObj==NULL)
	{
		shadow.COMMON_OBJ.m_hAcs=0;
		shadow.COMMON_OBJ.m_hMotherObj=0;
		return;
	}
	if(pMotherObj==this||pMotherObj->GetClassTypeId()!=GetClassTypeId())
		return;
	CTower *pTower=(CTower*)m_pModel;
	//hObj=0ʱ��ʾ���ȴ�ԭλӰ�俽�����Ĺ��� wjh-2015.7.23
	//if((iShadowObjType==SHADOW_ISOLATED&&pTower->FromAcsHandle(hObj)==NULL)
	//	||(iShadowObjType==SHADOW_ISOLATED_RELA_LOCATION&&pTower->FromPartHandle(hObj)==NULL))
	//	return;
	m_iShadowObjType=iShadowObjType;
	shadow.COMMON_OBJ.m_hAcs=hObj;
	shadow.COMMON_OBJ.m_hMotherObj=pMotherObj->handle;
	m_pMotherObj=pMotherObj;
}

void CLDSObject::SetShadowPara(CLDSObject *pMotherObj,CBlockReference *pBlockRef)
{
	m_iShadowObjType=SHADOW_NONE;
	if(m_pModel==NULL||!m_pModel->IsTowerModel())
		return;
	if(pMotherObj==NULL||pBlockRef==NULL)
	{
		shadow.BLOCK_OBJ.m_hBlockRef=0;
		shadow.BLOCK_OBJ.m_hMotherObj=0;
		return;
	}
	if(pMotherObj==this||pMotherObj->GetClassTypeId()!=GetClassTypeId())
		return;
	CTower *pTower=(CTower*)m_pModel;
	CBlockModel *pBlock=pTower->Block.FromHandle(pBlockRef->m_hBlock);
	if(pBlock==NULL)
		return;
	if(pBlock->IsEmbeded())
		m_iShadowObjType=SHADOW_EMBEDDED;
	else 
		m_iShadowObjType=SHADOW_EXTERBLK;
	shadow.BLOCK_OBJ.m_hBlockRef=pBlockRef->handle;
	shadow.BLOCK_OBJ.m_hMotherObj=pMotherObj->handle;
	m_pMotherObj=pMotherObj;
	if(IsDbObject())
	{
		CLDSPart* selfp=NULL;
		CLDSDbObject* self=(CLDSDbObject*)this;
		self->cfgword=((CLDSDbObject*)pMotherObj)->cfgword;
		self->iSeg=((CLDSDbObject*)pMotherObj)->iSeg;
		if(self->IsPart())
		{
			selfp=(CLDSPart*)self;
			selfp->m_bVirtualPart=((CLDSPart*)pMotherObj)->m_bVirtualPart;
			selfp->SetPartNo(((CLDSPart*)pMotherObj)->GetPartNo(),true);
		}
		if(pBlockRef!=NULL)//m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
		{
			if(selfp)
				selfp->ModifyPartNoSegI(pBlockRef->iSeg);
			self->cfgword=pBlockRef->cfgword;
			//������岿��װ��ʱ���޷�����ͳһ�κž�Ĭ��Ϊ0�Σ���ʱ��Ӧ�����Ӷζκ� wjh-2015.8.4
			if(pBlockRef->iSeg!=0)
				self->iSeg=pBlockRef->iSeg;
		}
	}
}
void CLDSObject::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( version==0||
		(doc_type==1&&version>=4020004)||	//TMA(V4.2.0.4)
		(doc_type==2&&version>=1020004)||	//LMA(V1.2.0.4)
		(doc_type==3&&version>=2000004)||	//TSA(V2.0.0.4)
		(doc_type==4&&version>=1020004)||	//LDS(V1.2.0.4)
		(doc_type==5&&version>=1020004))	//TDA(V1.2.0.4)
	{
		buffer.ReadDword(&dwPermission);
	}
}
void CLDSObject::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteDword(dwPermission);
}

CLDSObject* CLDSObject::UpdateMotherObject()
{
	m_pMotherObj=NULL;
	if(m_pModel==NULL||m_iShadowObjType==SHADOW_NONE)
		return NULL;
	if(m_iShadowObjType==SHADOW_ISOLATED)
	{
		if(shadow.COMMON_OBJ.m_hMotherObj==handle||
			m_pModel->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs)==NULL)
			return NULL;
		m_pMotherObj=m_pModel->FromHandle(shadow.COMMON_OBJ.m_hMotherObj);
	}
	else if(!m_pModel->IsTowerModel())
		return NULL;
	else if(m_iShadowObjType==SHADOW_EMBEDDED)
	{
		CTower *pTower=(CTower*)m_pModel;
		CBlockReference *pBlockRef=pTower->BlockRef.FromHandle(shadow.BLOCK_OBJ.m_hBlockRef);
		if(pBlockRef==NULL)
			return NULL;
		CBlockModel *pBlock=pTower->Block.FromHandle(pBlockRef->m_hBlock);
		if(pBlock==NULL||!pBlock->IsEmbeded()||shadow.BLOCK_OBJ.m_hMotherObj==handle)
			return NULL;
		m_pMotherObj=pTower->FromHandle(shadow.BLOCK_OBJ.m_hMotherObj);
	}
	else if(m_iShadowObjType==SHADOW_EXTERBLK)
	{
		CTower *pTower=(CTower*)m_pModel;
		CBlockReference *pBlockRef=pTower->BlockRef.FromHandle(shadow.BLOCK_OBJ.m_hBlockRef);
		if(pBlockRef==NULL)
			return NULL;
		CBlockModel *pBlock=pTower->Block.FromHandle(pBlockRef->m_hBlock);
		if(pBlock==NULL||pBlock->IsEmbeded())
			return NULL;
		m_pMotherObj=pBlock->FromHandle(shadow.BLOCK_OBJ.m_hMotherObj);;
	}
	if(m_pMotherObj&&m_pMotherObj->GetClassTypeId()!=GetClassTypeId())
		m_pMotherObj=NULL;
	return m_pMotherObj;
}
void CLDSObject::EmptyShadowPara()
{
	m_iShadowObjType=0;
	shadow.BLOCK_OBJ.m_hBlockRef=0;
	shadow.BLOCK_OBJ.m_hMotherObj=0;
	m_pMotherObj=NULL;
}
CLDSObject* CLDSObject::MotherObject(bool ancestor/*=true*/)
{
	if(m_pMotherObj==NULL&&m_iShadowObjType!=SHADOW_NONE)
		UpdateMotherObject();
	if(m_pMotherObj==NULL||m_pMotherObj==this)
		return this;
	else if(ancestor)
		return m_pMotherObj->MotherObject(ancestor);
	else
		return m_pMotherObj;
}
bool CLDSObject::IsBlockRefShadowObject()
{
	if(m_iShadowObjType!=SHADOW_EMBEDDED&&m_iShadowObjType!=SHADOW_EXTERBLK)
		return false;
	else
		return IsShadowObject();
}
BYTE CLDSObject::ShadowMode()
{
	if(m_iShadowObjType==SHADOW_ISOLATED||m_iShadowObjType==SHADOW_ISOLATED_RELA_LOCATION)
		return SHADOWMODE_COMMON;
	else if(m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
		return SHADOWMODE_BLOCK;
	else //if(m_iShadowObjType==SHADOW_NONE)
		return 0;
}
bool CLDSObject::IsShadowObject()
{
	if(m_pMotherObj==NULL&&m_iShadowObjType>0)
		UpdateMotherObject();
	if(m_pMotherObj&&m_pMotherObj!=this)
		return true;
	else 
		return false;
}
bool CLDSObject::IsMotherObject()
{
	bool bRetCode=false;
	if(IsDbObject())
	{
		CLDSDbObject *pDbObj=(CLDSDbObject*)this;
		if(pDbObj->IsNode())
		{
			int nPush=m_pModel->PushNodeStack();
			for(CLDSNode *pNode=m_pModel->EnumNodeFirst();pNode;pNode=m_pModel->EnumNodeNext())
			{
				if(!pNode->IsShadowObject())
					continue;
				if(pNode->MotherObject(false)->handle==handle)
				{
					bRetCode=true;
					break;
				}
			}
			m_pModel->PopNodeStack(nPush);
		}
		else 
		{
			int nPush=m_pModel->PushPartStack();
			for(CLDSPart *pPart=m_pModel->EnumPartFirst(m_nClassTypeId);pPart;pPart=m_pModel->EnumPartNext(m_nClassTypeId))
			{
				if(!pPart->IsShadowObject())
					continue;
				if(pPart->MotherPart(false)->handle==handle)
				{
					bRetCode=true;
					break;
				}
			}
			m_pModel->PopPartStack(nPush);
		}
	}
	else 
	{
		if(m_nClassTypeId==CLS_GEPOINT)
		{
			for(CLDSPoint *pPt=m_pModel->EnumPointFirst();pPt;pPt=m_pModel->EnumPointNext())
			{
				if(!pPt->IsShadowObject())
					continue;
				if(pPt->MotherObject(false)->handle==handle)
				{
					bRetCode=true;
					break;
				}
			}
		}
		else if(m_nClassTypeId==CLS_GELINE)
		{
			for(CLDSLine *pLine=m_pModel->EnumLineFirst();pLine;pLine=m_pModel->EnumLineNext())
			{
				if(!pLine->IsShadowObject())
					continue;
				if(pLine->MotherObject(false)->handle==handle)
				{
					bRetCode=true;
					break;
				}
			}
		}
		else if(m_nClassTypeId==CLS_GEPLANE)
		{
			for(CLDSPlane *pPlane=m_pModel->EnumPlaneFirst();pPlane;pPlane=m_pModel->EnumPlaneNext())
			{
				if(!pPlane->IsShadowObject())
					continue;
				if(pPlane->MotherObject(false)->handle==handle)
				{
					bRetCode=true;
					break;
				}
			}
		}
	}
	return bRetCode;
}
int CLDSObject::GetMotherClassTypeId() const
{
	if(m_pMotherObj)
		return m_pMotherObj->GetClassTypeId();
	else
		return m_nClassTypeId;
}

bool CLDSObject::IsMemberOfBlockRef(CBlockReference *pBlockRef)
{
	if(m_iShadowObjType==SHADOW_EMBEDDED)
	{
		if(shadow.BLOCK_OBJ.m_hBlockRef==pBlockRef->handle)
			return true;
	}
	else if(m_iShadowObjType==SHADOW_EXTERBLK)
	{
		if(shadow.BLOCK_OBJ.m_hBlockRef==pBlockRef->handle)
			return true;
	}
	return false;
}
bool CLDSObject::IsBlockRefPart()
{
	if((m_iShadowObjType==2&&shadow.BLOCK_OBJ.m_hBlockRef>0x20)||
		(m_iShadowObjType==3&&shadow.BLOCK_OBJ.m_hBlockRef>0x20))
		return true;
	return false;
}

bool CLDSObject::GetBlockParameter(CBlockParameter &blockPara)
{
	if(m_pModel==NULL||!m_pModel->IsTowerModel())
		return false;
	CBlockModel *pBlock=NULL;
	CBlockReference *pBlockRef=NULL;
	CTower *pTower=(CTower*)m_pModel;
	if(m_iShadowObjType==SHADOW_EMBEDDED)
	{
		pBlockRef=pTower->BlockRef.FromHandle(shadow.BLOCK_OBJ.m_hBlockRef);
		if(pBlockRef==NULL)
			return false;
		pBlock=pTower->Block.FromHandle(pBlockRef->m_hBlock);
		if(pBlock==NULL||!pBlock->IsEmbeded())
			return false;
	}
	else if(m_iShadowObjType==SHADOW_EXTERBLK)
	{
		pBlockRef=pTower->BlockRef.FromHandle(shadow.BLOCK_OBJ.m_hBlockRef);
		if(pBlockRef==NULL)
			return false;
		pBlock=pTower->Block.FromHandle(pBlockRef->m_hBlock);
		if(pBlock==NULL||pBlock->IsEmbeded())
			return false;
	}
	else 
		return false;
	blockPara.m_pBlock=pBlock;
	blockPara.m_pBlockRef=pBlockRef;
	blockPara.m_hMotherObj=shadow.BLOCK_OBJ.m_hMotherObj;
	return true;
}
CParaCS* CLDSObject::GetParaCS()
{
	if(m_iShadowObjType==SHADOW_ISOLATED)
		return m_pModel->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
	else 
		return NULL;
}
bool CLDSObject::ShadowParaAndBlockIdToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(  version>0&&(	//����֮ǰ������趨��__COMPLEX_PART_INC_��ʹT/MA�޷���¼Ӱ��������ʰ汾�Ÿ�Ϊ5.0.0.1 wjh-2015.3.9
		(doc_type==1&&version<5000001)||//version<4020003)||
		(doc_type==2&&version<1020003)||
		(doc_type==3&&version<1080103)||
		(doc_type==4&&version<1020003)||
		(doc_type==5&&version<1020003)))
		return false;
	buffer.WriteByte(m_iShadowObjType);
	if(m_iShadowObjType==SHADOW_ISOLATED)
	{
		buffer.WriteInteger(shadow.COMMON_OBJ.m_hAcs);
		buffer.WriteInteger(shadow.COMMON_OBJ.m_hMotherObj);
	}
	else if(m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
	{
		buffer.WriteInteger(shadow.BLOCK_OBJ.m_hBlockRef);
		buffer.WriteInteger(shadow.BLOCK_OBJ.m_hMotherObj);
	}
	else //������������,��Ӱ�����ͬʱ���Ϊ��������ڴ˴�����
	{
		buffer.WriteInteger(m_hBlock);
		return false;
	}
	return true;
}
bool CLDSObject::ShadowParaAndBlockIdFromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	/*
	//ʵ����LDS V1.2.0.4֮ǰ�汾�в�δ�����Ϣ,��ShadowParaAndBlockIdFromBuffer�����жϵ���LDS V1.2.0.3
	//����LDS V1.2.0.3�汾ʹ�ü���,�Ҹ��ٻ�ʹ�õ����͹���,��˳��ڼ򻯴���δ���д˰汾�ж�
	if(GetClassTypeId()==CLS_ARCPART)
	{
		if( version==0||
			(doc_type==1&&version>=4020004)||
			(doc_type==2&&version>=1020004)||
			(doc_type==3&&version>=1080104)||
			(doc_type==4&&version>=1020004)||
			(doc_type==5&&version>=1020004))
		{
			buffer.ReadByte(&m_iShadowObjType);
			if(m_iShadowObjType==SHADOW_ISOLATED)
			{
				buffer.ReadInteger(&shadow.COMMON_OBJ.m_hAcs);
				buffer.ReadInteger(&shadow.COMMON_OBJ.m_hMotherObj);
			}
			else if(m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
			{
				buffer.ReadInteger(&shadow.BLOCK_OBJ.m_hBlockRef);
				buffer.ReadInteger(&shadow.BLOCK_OBJ.m_hMotherObj);
			}
			else
			{
				buffer.ReadInteger(&m_hBlock);
				return false;
			}
			return true;
		}
	} else*/
	if( version==0||	//����֮ǰ������趨��__COMPLEX_PART_INC_��ʹT/MA�޷���¼Ӱ��������ʰ汾�Ÿ�Ϊ5.0.0.1 wjh-2015.3.9
		(doc_type==1&&version>=5000001)||//version>=4020003)||
		(doc_type==2&&version>=1020003)||
		(doc_type==3&&version>=1080103)||
		(doc_type==4&&version>=1020003)||
		(doc_type==5&&version>=1020003))
	{
		buffer.ReadByte(&m_iShadowObjType);
		if(m_iShadowObjType==SHADOW_ISOLATED)
		{
			buffer.ReadInteger(&shadow.COMMON_OBJ.m_hAcs);
			buffer.ReadInteger(&shadow.COMMON_OBJ.m_hMotherObj);
		}
		else if(m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
		{
			buffer.ReadInteger(&shadow.BLOCK_OBJ.m_hBlockRef);
			buffer.ReadInteger(&shadow.BLOCK_OBJ.m_hMotherObj);
		}
		else
		{
			buffer.ReadInteger(&m_hBlock);
			return false;
		}
		return true;
	}
	return false;
}

MEMORY_BLOCK::MEMORY_BLOCK(MEMORY_BLOCK& memblock)
{
	wSize=0;	//�����ʼ�����㣬�����ڸ�ֵ�ǻ����wjh-2015.3.6
	u4Bits=0;
	*this=memblock;
}
MEMORY_BLOCK& MEMORY_BLOCK::operator=(const MEMORY_BLOCK& memblock)
{
	wSize=memblock.Size();
	if(wSize>4)
	{
		if(block)
			delete []block;
		block=new char[wSize];
		memcpy(block,memblock.Data(),wSize);
	}
	else
		memcpy(&u4Bits,memblock.Data(),4);
	return *this;
}
void MEMORY_BLOCK::ReleaseBlockBuffer()
{
	if(wSize>4)
		delete []block;
	u4Bits=0;
}
bool MEMORY_BLOCK::operator==(const MEMORY_BLOCK& memblock)
{
	if(wSize!=memblock.Size())
		return false;
	const char* data=Data();
	const char* comp_data=memblock.Data();
	for(UINT i=0;i<wSize;i++){
		if(data[i]!=comp_data[i])
			return false;
	}
	return true;
}
const char* MEMORY_BLOCK::Data(WORD wiByteIndex/*=0*/) const
{
	if(wSize<=4)
		return ((char*)&u4Bits)+wiByteIndex;
	else
		return block+wiByteIndex;
}
void MEMORY_BLOCK::FromBuffer(BUFFER_IO* buffer_io,long version/*=NULL*/,long doc_type/*=0*/)
{
	WORD wSize=0;
	buffer_io->ReadWord(&wSize);
	ReleaseBlockBuffer();
	if(wSize<=4)
		buffer_io->Read(&u4Bits,wSize);
	else
	{
		char* data=new char[wSize];
		buffer_io->Read(data,wSize);
	}
}
void MEMORY_BLOCK::ToBuffer(BUFFER_IO* buffer_io,long version/*=NULL*/,long doc_type/*=0*/)
{
	buffer_io->WriteWord(Size());
	buffer_io->Write(Data(),Size());
}
void MEMORY_BLOCK::SetBitState(WORD bitpos,bool state/*=true*/)
{
	WORD iBytePos=bitpos/8;
	BYTE iBitPos=bitpos%8;
	if(iBytePos>=wSize)
	{
		char* data=new char[iBytePos+1];
		memcpy(data,Data(),wSize);
		ReleaseBlockBuffer();
		wSize=iBytePos+1;
		block=data;
	}
	BYTEFLAG flag(*Data(iBytePos));
	flag.SetBitState(iBitPos,state);
	char* addr=wSize>4 ? block : (char*)&u4Bits;
	memcpy(addr+iBytePos,&flag,1);
}
void MEMORY_BLOCK::CopyState(WORD addr_dest, const char* fromStr,WORD size)
{
	if(addr_dest+size>wSize)
	{
		char* data=new char[addr_dest+size];
		memcpy(data,Data(),wSize);
		ReleaseBlockBuffer();
		wSize=addr_dest+size;
		block=data;
	}
	memcpy(block+addr_dest,fromStr,size);
}
bool MEMORY_BLOCK::GetBitState(WORD bitpos)
{
	WORD iBytePos=bitpos/8;
	BYTE iBitPos=bitpos%8;
	if(iBytePos>=wSize)
		return false;
	BYTEFLAG flag(*Data(iBytePos));
	return flag.GetBitState(iBitPos);
}
/* �ⲿ�ִ���Ŀǰ���ã������������п����õ��ģ���ע�͵��Ա����� wjh-2015.3.7
CRelaObjGroup::MIRINFO CRelaObjGroup::TransMirInfo(long hRefMirAcs,short wRefRotAngle,long hMirAcs,short wRotAngle)
{
	MIRINFO mir;
	GEPOINT start(100,100),end_x(200,100),end_y(100,200);
	if(hRefMirAcs>3||hMirAcs>3)
	{
		mir.hMirAcs=hMirAcs;
		return mir;
	}
	else if(hRefMirAcs==1)
	{	//����X��Գ�
		start.y*=-1;
		end_x.y*=-1;
		end_y.y*=-1;
	}
	else if(hRefMirAcs==2)
	{	//����Y��Գ�
		start.x*=-1;
		end_x.x*=-1;
		end_y.x*=-1;
	}
	else if(hRefMirAcs==3)
	{	//����Z��Գ�
		start*=-1;
		end_x*=-1;
		end_y*=-1;
	}
	GEPOINT AXISZ(0,0,1);
	if(wRefRotAngle!=0)
	{
		double rotangle=wRefRotAngle*RADTODEG_COEF;
		double sina=sin(rotangle);
		double cosa=cos(rotangle);
		RotateVectorAroundVector(start,sina,cosa,AXISZ);
		RotateVectorAroundVector(end_x,sina,cosa,AXISZ);
		RotateVectorAroundVector(end_y,sina,cosa,AXISZ);
	}
	if(hMirAcs==1)
	{	//����X��Գ�
		start.y*=-1;
		end_x.y*=-1;
		end_y.y*=-1;
	}
	else if(hMirAcs==2)
	{	//����Y��Գ�
		start.x*=-1;
		end_x.x*=-1;
		end_y.x*=-1;
	}
	else if(hMirAcs==3)
	{	//����Z��Գ�
		start*=-1;
		end_x*=-1;
		end_y*=-1;
	}
	if(wRotAngle!=0)
	{
		double rotangle=wRotAngle*RADTODEG_COEF;
		double sina=sin(rotangle);
		double cosa=cos(rotangle);
		RotateVectorAroundVector(start,sina,cosa,AXISZ);
		RotateVectorAroundVector(end_x,sina,cosa,AXISZ);
		RotateVectorAroundVector(end_y,sina,cosa,AXISZ);
	}
	GEPOINT vec_x=end_x-start;
	GEPOINT vec_y=end_y-start;
	//���޶���
	// �� | ��
	//---------->X(��)
	// �� | ��
	//    V(Y��)
	if(wRefRotAngle%90==0 && wRotAngle%90==0)
	{	//���ͶԳ�
		if(fabs(vec_x.x)<EPS)	//ʸ��ΪY����,
		{
			if(start.x>0&&start.y>0)
			{	//��ת�ԳƵ�����1����
				mir.hMirAcs=2;		//��Y��Գ�
				mir.wRotAngle=-90;	//����Z�ᷴ����ת90
			}
			else if(start.x<0&&start.y<0)
			{	//��ת�ԳƵ�����4����
				mir.hMirAcs=2;		//��Y��Գ�
				mir.wRotAngle=90;	//����Z��������ת90
			}
			else if(start.x<0&&start.y>0)
			{	//��ת�ԳƵ�����2����
				mir.hMirAcs=0;		//����Գ�
				mir.wRotAngle=90;	//����Z��������ת90
			}
			else //if(start.x>0&&start.y<0)
			{	//��ת�ԳƵ�����3����
				mir.hMirAcs=0;		//����Գ�
				mir.wRotAngle=-90;	//����Z�ᷴ����ת90
			}
		}
		else
		{
			if(start.x<0&&start.y<0)
				mir.hMirAcs=3;	//Z��ԳƵ�����4����
			else if(start.x<0&&start.y>0)
				mir.hMirAcs=2;	//Y��ԳƵ�����2����
			else if(start.x>0&&start.y<0)
				mir.hMirAcs=1;	//X��ԳƵ�����2����
			else //if(start.x>0&&start.y>0)
				mir.hMirAcs=0;	//��λδ������Գ�
			mir.wRotAngle=0;	//����Ҫ��ת��ֱ�ӶԳƵ�λ
		}
	}
	else
	{
		//����Գ�(��ת)�任���X���ʶ�ߵķ�λ���߽�
		mir.wRotAngle=ftoi(Cal2dLineAng(start.x,start.y,end_x.x,end_x.y)*DEGTORAD_COEF);
		GEPOINT vec_z=vec_x^vec_y;
		if(vec_z.z>0)	//��X/Y����Գ�
		{
			mir.hMirAcs=0;
			if(mir.wRotAngle==180)
			{	//��ת180���൱��Z��Գ�
				mir.hMirAcs=3;
				mir.wRotAngle=0;
			}
		}
		else
		{	//ͳһΪY��Գƺ�����ת
			mir.hMirAcs=2;
			mir.wRotAngle-=180;	//Y��Գƺ�X���ʶ�ߵķ�λ���߽�Ϊ180��
		}
	}
	return mir;
}
*/
//DbObject
CLDSDbObject::CLDSDbObject()
{
	layer_tag[0]=layer_tag[1]=layer_tag[2]=layer_tag[3]=0;
	m_nClassTypeId = CLS_DBOBJECT;
	//strcpy(m_sClassName,"CLDSDbObject");
	is_highlight = FALSE;
	is_visible = TRUE;
	_bSyncShadowPartNo=true;
	memset(sNotes,'\0',50);
	cfgword.SetWordByNo(1);
	strcpy(layer_tag,"");
	m_nHasConstraintsCounts=0;
}
char CLDSDbObject::get_ciLegQuad()
{
	if(m_ciLegQuad>0&&m_ciLegQuad<5)
		return m_ciLegQuad;
	else if(layer_tag[0]=='L')
	{
		char ciQuad=layer_tag[2]-'0';
		return (ciQuad>0&&ciQuad<5)?ciQuad:1;
	}
	else
		return 0;
}
char CLDSDbObject::set_ciLegQuad(char ciBelongLegQuad)
{
	if(ciBelongLegQuad>0&&ciBelongLegQuad<5)
		m_ciLegQuad=ciBelongLegQuad;
	else
		m_ciLegQuad=0;
	return m_ciLegQuad;
}
//��ֲ��GlobalFunc.cpp�е�CalMirAtomLayer����
void CLDSDbObject::MirObjLayer(const char* sOrgLayer,char* sNewLayer,MIRMSG mirmsg)
{
	CXhChar16 baklayer(sNewLayer);
	if(sNewLayer!=sOrgLayer)
		strcpy(sNewLayer,sOrgLayer);

	if(mirmsg.axis_flag&1)
	{	//X��Գ�
		if(sOrgLayer[2]=='Q')
			sNewLayer[2]='H';
		else if(sOrgLayer[2]=='H')
			sNewLayer[2]='Q';
		else if(sOrgLayer[2]=='1')
			sNewLayer[2]='3';
		else if(sOrgLayer[2]=='2')
			sNewLayer[2]='4';
		else if(sOrgLayer[2]=='3')
			sNewLayer[2]='1';
		else if(sOrgLayer[2]=='4')
			sNewLayer[2]='2';
	}
	else if(mirmsg.axis_flag&2)
	{	//Y��Գ�
		if(sOrgLayer[2]=='Z')
			sNewLayer[2]='Y';
		else if(sOrgLayer[2]=='Y')
			sNewLayer[2]='Z';
		else if(sOrgLayer[2]=='1')
			sNewLayer[2]='2';
		else if(sOrgLayer[2]=='2')
			sNewLayer[2]='1';
		else if(sOrgLayer[2]=='3')
			sNewLayer[2]='4';
		else if(sOrgLayer[2]=='4')
			sNewLayer[2]='3';
	}
	else if(mirmsg.axis_flag&4)
	{	//Z��Գ�
		if(sOrgLayer[2]=='Q')
			sNewLayer[2]='H';
		else if(sOrgLayer[2]=='H')
			sNewLayer[2]='Q';
		else if(sOrgLayer[2]=='Z')
			sNewLayer[2]='Y';
		else if(sOrgLayer[2]=='Y')
			sNewLayer[2]='Z';
		else if(sOrgLayer[2]=='1')
			sNewLayer[2]='4';
		else if(sOrgLayer[2]=='2')
			sNewLayer[2]='3';
		else if(sOrgLayer[2]=='3')
			sNewLayer[2]='2';
		else if(sOrgLayer[2]=='4')
			sNewLayer[2]='1';
	}
	else if(mirmsg.axis_flag&16)
	{
		if(     sOrgLayer[2]=='Q'&&(mirmsg.mir_norm.IsEqual(1,-1,0)||mirmsg.mir_norm.IsEqual(-1,1,0)))
			sNewLayer[2]='Y';	//ǰ��45��ԳƵ�����
		else if(sOrgLayer[2]=='Y'&&(mirmsg.mir_norm.IsEqual(1,-1,0)||mirmsg.mir_norm.IsEqual(-1,1,0)))
			sNewLayer[2]='Q';	//����45��ԳƵ�ǰ��
		else if(sOrgLayer[2]=='H'&&(mirmsg.mir_norm.IsEqual(1,-1,0)||mirmsg.mir_norm.IsEqual(-1,1,0)))
			sNewLayer[2]='Z';	//����45��ԳƵ�����
		else if(sOrgLayer[2]=='Z'&&(mirmsg.mir_norm.IsEqual(1,-1,0)||mirmsg.mir_norm.IsEqual(-1,1,0)))
			sNewLayer[2]='H';	//����45��ԳƵ�����
		else if(sOrgLayer[2]=='3'&&(mirmsg.mir_norm.IsEqual(1,-1,0)||mirmsg.mir_norm.IsEqual(-1,1,0)))
			sNewLayer[2]='2';	//3����45��ԳƵ�2����
		else if(sOrgLayer[2]=='2'&&(mirmsg.mir_norm.IsEqual(1,-1,0)||mirmsg.mir_norm.IsEqual(-1,1,0)))
			sNewLayer[2]='3';	//2����45��ԳƵ�3����

		else if(sOrgLayer[2]=='Q'&&(mirmsg.mir_norm.IsEqual(1,1,0)||mirmsg.mir_norm.IsEqual(-1,-1,0)))
			sNewLayer[2]='Z';	//ǰ��135��ԳƵ�����
		else if(sOrgLayer[2]=='Z'&&(mirmsg.mir_norm.IsEqual(1,1,0)||mirmsg.mir_norm.IsEqual(-1,-1,0)))
			sNewLayer[2]='Q';	//����135��ԳƵ�ǰ��
		else if(sOrgLayer[2]=='H'&&(mirmsg.mir_norm.IsEqual(1,1,0)||mirmsg.mir_norm.IsEqual(-1,-1,0)))
			sNewLayer[2]='Y';	//����135��ԳƵ�����
		else if(sOrgLayer[2]=='Y'&&(mirmsg.mir_norm.IsEqual(1,1,0)||mirmsg.mir_norm.IsEqual(-1,-1,0)))
			sNewLayer[2]='H';	//����135��ԳƵ�����
		else if(sOrgLayer[2]=='1'&&(mirmsg.mir_norm.IsEqual(1,1,0)||mirmsg.mir_norm.IsEqual(-1,-1,0)))
			sNewLayer[2]='4';	//1����135��ԳƵ�4����
		else if(sOrgLayer[2]=='4'&&(mirmsg.mir_norm.IsEqual(1,1,0)||mirmsg.mir_norm.IsEqual(-1,-1,0)))
			sNewLayer[2]='1';	//4����135��ԳƵ�1����

		else	//��ԭԭͼ��
			strncpy(sNewLayer,baklayer,3);
	}
	if(mirmsg.axis_flag&8)	//��ת�Գ�
	{
		int iRot=(ftoi(-mirmsg.rotate_angle/90)%4);
		if(iRot<0)
			iRot+=4;
		char mir_CHAR1[5]="QYHZ";
		char mir_CHAR2[5]="2134";
		if(sNewLayer[2]=='Q')
			sNewLayer[2]=mir_CHAR1[iRot];
		else if(sNewLayer[2]=='H')
			sNewLayer[2]=mir_CHAR1[(iRot+2)%4];
		else if(sNewLayer[2]=='Z')
			sNewLayer[2]=mir_CHAR1[(iRot+3)%4];
		else if(sNewLayer[2]=='Y')
			sNewLayer[2]=mir_CHAR1[(iRot+1)%4];
		else if(sNewLayer[2]=='1')
			sNewLayer[2]=mir_CHAR2[(iRot+1)%4];
		else if(sNewLayer[2]=='2')
			sNewLayer[2]=mir_CHAR2[iRot];
		else if(sOrgLayer[2]=='3')
			sNewLayer[2]=mir_CHAR2[(iRot+2)%4];
		else if(sNewLayer[2]=='4')
			sNewLayer[2]=mir_CHAR2[(iRot+3)%4];
	}
}
//��Ϊ��������򷵻ز���ȫ������ռλ�����ĺ�
CFGWORD CLDSDbObject::UnifiedCfgword()
{
	if(IsLegObj())
		return cfgword;
	else
		return cfgword=cfgword.ToBodyWord();
}
CFGWORD CLDSDbObject::GetUnifiedCfgword() const//��Ϊ��������򷵻ز���ȫ������ռλ�����ĺ�
{
	if(IsLegObj())
		return cfgword;
	else
		return cfgword.ToBodyWord();
}
bool CLDSDbObject::ModuleTogetherWith(const CFGINFO& cfginfo) const//�ж��������Ƿ񹲴���ͬһ����ʵ��
{
	CFGWORD temp_cfgword=cfgword;
	if(!IsLegObj())
		temp_cfgword=GetUnifiedCfgword();
	return temp_cfgword.And(cfginfo.Cfgword())!=FALSE;
}
bool CLDSDbObject::ModuleTogetherWith(CLDSDbObject* pDbObj) const
{
	if(pDbObj==NULL)
		return false;
	CFGWORD temp_cfgword=cfgword;
	CFGWORD temp_cfgword2=pDbObj->cfgword;
	if(!IsLegObj())
		temp_cfgword=GetUnifiedCfgword();
	if(!pDbObj->IsLegObj())
		temp_cfgword2=pDbObj->GetUnifiedCfgword();
	return temp_cfgword.And(temp_cfgword2)!=FALSE;
}
void CLDSDbObject::CopyModuleInstanceInfo(CLDSDbObject* pDbObj)
{
	cfgword=pDbObj->cfgword;
	layer_tag[0]=pDbObj->layer(0);
	if(pDbObj->Layer(0)=='L')
		layer_tag[2]=pDbObj->layer(2);
}
bool CLDSDbObject::IsAcceptableTypeId(int type_id)
{
	if(type_id==CLS_OBJECT||type_id==CLS_DBOBJECT)
		return true;
	else if(type_id==m_nClassTypeId)
		return true;
	else
		return false;
}
void CLDSDbObject::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( version==0||
		(doc_type==1&&version>=4020004)||	//TMA(V4.2.0.4)
		(doc_type==2&&version>=1020004)||	//LMA(V1.2.0.4)
		(doc_type==3&&version>=2000004)||	//TSA(V2.0.0.4)
		(doc_type==4&&version>=1020004)||	//LDS(V1.2.0.4)
		(doc_type==5&&version>=1020004))	//TDA(V1.2.0.4)
	{
		buffer.ReadDword(&dwPermission);
		buffer.ReadString(layer_tag,4);
		buffer.ReadInteger(&_iSeg.iSeg);
		CfgwordFromBuffer(cfgword,buffer,version,doc_type);
		BelongModel()->EvolveCfgword(cfgword,version,doc_type);
		buffer.ReadString(sNotes,51);
		int n=0;
		buffer.ReadInteger(&n);
		for(int j=0;j<n;j++)
		{
			RELATIVE_OBJECT *pMirObj=relativeObjs.Add(buffer.ReadInteger());
			buffer.ReadByte(&pMirObj->mirInfo.axis_flag);
			buffer.ReadPoint(pMirObj->mirInfo.origin);
			if((pMirObj->mirInfo.axis_flag&0x08)>0)
			{
				buffer.ReadWord(&pMirObj->mirInfo.rotate_angle);
				buffer.ReadByte(&pMirObj->mirInfo.array_num);
			}
			if(pMirObj->mirInfo.axis_flag&0x10)	//����
				buffer.ReadPoint(pMirObj->mirInfo.mir_norm);
		}
	}
}
void CLDSDbObject::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteDword(dwPermission);
	buffer.WriteString(layer_tag);
	buffer.WriteInteger(_iSeg.iSeg);
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	buffer.WriteString(sNotes);
	BUFFERPOP stack(&buffer,relativeObjs.GetNodeNum());
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for(RELATIVE_OBJECT *pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pMirObj->hObj);
		buffer.WriteByte(pMirObj->mirInfo.axis_flag);
		buffer.WritePoint(pMirObj->mirInfo.origin);
		if((pMirObj->mirInfo.axis_flag&8)>0)
		{
			buffer.WriteWord(pMirObj->mirInfo.rotate_angle);
			buffer.WriteByte(pMirObj->mirInfo.array_num);
		}
		if(pMirObj->mirInfo.axis_flag&0x10)	//����
			buffer.WritePoint(pMirObj->mirInfo.mir_norm);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of relative object is wrong!");
#else
		CLDSObject::Log2File()->Log("����������¼�����ִ���!");
#endif
}

int CLDSDbObject::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	char sText[101]="";
	if(GetPropID("handle")==id)
		sprintf(sText,"0X%X",handle);
	else if(GetPropID("layer")==id)
		strcpy(sText,layer());
	else if(GetPropID("layer1")==id)
		sprintf(sText,"%c",Layer(0));
	else if(GetPropID("layer2")==id)
		sprintf(sText,"%c",Layer(1));
	else if(GetPropID("layer3")==id)
		sprintf(sText,"%c",Layer(2));
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(cfgword,valueStr);
	{
		if(layer(0)=='L')	//���ȹ���
			return cfgword.GetLegScopeStr(valueStr,50,true);
		else
			return cfgword.GetBodyScopeStr(valueStr);
	}
	else if(GetPropID("m_hArcLift")==id)
	{
		CLDSArcLift *pLift=BelongModel()->FromArcLiftHandle(ArcLiftHandle());
		if(pLift)
			_snprintf(sText,100,"%s(%d)",pLift->name,pLift->iNo);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Without Lift Arc");
#else 
			strcpy(sText,"��Ԥ��");
#endif
	}
	else if(GetPropID("sNotes")==id)
			strcpy(valueStr,sNotes);
	else if(GetPropID("m_hBlock")==id)
		strcpy(sText,GetBlockIdName());
	else if(GetPropID("m_hBlockRef")==id)
	{
		if(m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
			sprintf(sText,"0x%X",shadow.BLOCK_OBJ.m_hBlockRef);
		else 
			strcpy(sText,"0x0");
	}
	else if(GetPropID("m_hMotherObject")==id)
	{
		if(IsShadowObject())
			sprintf(sText,"0x%X",m_pMotherObj->handle);
		else 
			strcpy(sText,"0x0");
	}
	if(valueStr)
		strcpy(valueStr,sText);
	return strlen(sText);
}
long CLDSDbObject::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem=CLDSDbObject::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
	{
		/*char prop_str[500];
		strcpy(prop_str,"û�ҵ���Ϊ\"");
		strcat(prop_str,propStr);
		strcat(prop_str,"\"������ID��";
		AfxMessageBox(prop_str);*/
		return 0;
	}
}

int CLDSDbObject::GetPropStr(long id, char *propStr)
{
	if(GetPropID("basicInfo")==0)
	{
		if(propStr)
			strcpy(propStr,"basicInfo");
		return strlen("basicInfo");
	}
	else if(GetPropID("handle")==id)
	{
		if(propStr)
			strcpy(propStr,"handle");
		return strlen("handle");
	}
	else if(GetPropID("layer")==id)
	{
		if(propStr)
			strcpy(propStr,"layer");
		return strlen("layer");
	}
	else if(GetPropID("iSeg")==id)
	{
		if(propStr)
			strcpy(propStr,"iSeg");
		return strlen("iSeg");
	}
	else if(GetPropID("cfgword")==id)
	{
		if(propStr)
			strcpy(propStr,"cfgword");
		return strlen("cfgword");
	}
	else if(GetPropID("sNotes")==id)
	{
		if(propStr)
			strcpy(propStr,"sNotes");
		return strlen("sNotes");
	}
	else
		return 0;
}
BOOL CLDSDbObject::IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSDbObject *pDbObject=NULL, *pFirstDbObject = (CLDSDbObject*)selectObjs.GetFirst();
	pFirstDbObject->GetPropValueStr(idProp,valueStr);
	for(pDbObject=(CLDSDbObject*)selectObjs.GetNext();pDbObject;pDbObject=(CLDSDbObject*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pDbObject->GetPropValueStr(idProp,valueStr2);
		if(_stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CLDSDbObject::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CLDSDbObject::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}

BOOL CLDSDbObject::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSDbObject::propStatusHashtable.SetValueAt(id,dwStatus);
}
char CLDSDbObject::Layer(int i) const
{
	if(i<=0)
		return toupper(layer_tag[0]);
	else if(i>=2)
		return toupper(layer_tag[2]);
	else
		return toupper(layer_tag[i]);
}

char& CLDSDbObject::layer(int i)
{
	if(i<0)
	{
		//layer_tag[0] = toupper(layer_tag[0]);
		return layer_tag[0];
	}
	else if(i>2)
	{
		//layer_tag[2] = toupper(layer_tag[2]);
		return layer_tag[3];
	}
	else
	{
		//layer_tag[i] = toupper(layer_tag[i]);
		return layer_tag[i];
	}
}
char CLDSDbObject::get_chLayer1st()
{
	if(layer_tag[0]>='a'&&layer_tag[0]<='z')
		return layer_tag[0]-32;//32='a'-'A';
	else
		return layer_tag[0];
}
char CLDSDbObject::set_chLayer1st(char chValue)
{
	return layer_tag[0]=chValue;
}
char CLDSDbObject::get_chLayer2nd()
{
	if(layer_tag[1]>='a'&&layer_tag[1]<='z')
		return layer_tag[1]-32;//32='a'-'A';
	else
		return layer_tag[1];
}
char CLDSDbObject::set_chLayer2nd(char chValue)
{
	return layer_tag[1]=chValue;
}
char CLDSDbObject::get_chLayer3rd()
{
	if(layer_tag[2]>='a'&&layer_tag[2]<='z')
		return layer_tag[2]-32;//32='a'-'A';
	else
		return layer_tag[2];
}
char CLDSDbObject::set_chLayer3rd(char chValue)
{
	return layer_tag[2]=chValue;
}
char* CLDSDbObject::layer()
{
	//for(int i=0;i<3;i++)
	//	layer_tag[i] = toupper(layer_tag[i]);
	return layer_tag;
}
BOOL CLDSDbObject::SetLayer(const char *layer)
{
	if(strlen(layer)<4)
	{
		strcpy(layer_tag,layer);
		SetModified(TRUE,FALSE);
		return TRUE;
	}
	else
		return FALSE;
}
CHashList<CXhChar50> CONSTRAINT::hashExpressions;
CONSTRAINT::CONSTRAINT()
{
	synchronize=0;
	id=wiAcceptPropId=wiMasterPropId=0;
	idExpression=hAcceptObj=hMasterObj=0;
}
CONSTRAINT::~CONSTRAINT()
{
	if(idExpression>0)
		hashExpressions.DeleteNode(idExpression);
}
long CONSTRAINT::DetachExprStr()
{
	long expression_id=idExpression;
	idExpression=0;
	return expression_id;
}
IMPLEMENT_SET(CONSTRAINT,expression)
{
	if(value.GetLength()>0)
	{
		CXhChar50* pExpr=hashExpressions.Add(idExpression);
		idExpression=hashExpressions.GetCursorKey();
		pExpr->Copy(value);
	}
	else if(idExpression>0)
	{
		hashExpressions.DeleteNode(idExpression);
		idExpression=0;
	}
}
IMPLEMENT_GET(CONSTRAINT,expression)
{
	CXhChar50* pExpr=hashExpressions.GetValue(idExpression);
	return (pExpr==NULL)?"":*pExpr;
}
//˫��ͬ��
IMPLEMENT_SET(CONSTRAINT,dual)
{
	if(value)
		synchronize|=0x01;
	else
		synchronize&=0xfE;
}
IMPLEMENT_GET(CONSTRAINT,dual)
{
	return (synchronize&0x01)>0;
}
//�Ѵ�Master->Accept������ͬ����������ѭ��
IMPLEMENT_SET(CONSTRAINT,gone)
{
	if(value)
		synchronize|=0x02;
	else
		synchronize&=0xfD;
}
IMPLEMENT_GET(CONSTRAINT,gone)
{
	return (synchronize&0x02)>0;
}
//˫��ͬ���Ѵ�Accept->Master������ͬ����������ѭ��
IMPLEMENT_SET(CONSTRAINT,came)
{
	if(value)
		synchronize|=0x04;
	else
		synchronize&=0xfB;
}
IMPLEMENT_GET(CONSTRAINT,came)
{
	return (synchronize&0x04)>0;
}
bool CONSTRAINT::IsOriginator(long hObj)
{
	return hMasterObj==hObj||(dual&&hAcceptObj==hObj);
}
void CONSTRAINT::CopyProperty(CONSTRAINT* pCopyFrom)
{
	if(pCopyFrom==NULL)
		return;
	wiAcceptPropId=pCopyFrom->wiAcceptPropId;//�����ܿص����Ի����
	wiMasterPropId=pCopyFrom->wiMasterPropId;//���ع����Ŀ������Ի����
	hAcceptObj=pCopyFrom->hAcceptObj;	//�����ܿع�������
	hMasterObj=pCopyFrom->hMasterObj;	//���ع�������
	dual=pCopyFrom->dual;
	expression=pCopyFrom->expression;
}
void CLDSDbObject::IncrementConstraintRef()
{
	m_nHasConstraintsCounts++;
}
CONSTRAINT* CLDSDbObject::AppendConstraint(CONSTRAINT* pCopyFrom/*=NULL*/)
{
	CONSTRAINT* pConstraint=BelongModel()->AppendConstraint();
	if(pCopyFrom)
		pConstraint->CopyProperty(pCopyFrom);
	m_nHasConstraintsCounts++;
	return pConstraint;
}
bool CLDSDbObject::RemoveConstraint(long constraint_id)
{
	//CONSTRAINT* pConstraint
	if(BelongModel()->RemoveConstraint(constraint_id,handle))
	{
		m_nHasConstraintsCounts--;
		return true;
	}
	return false;
}
CONSTRAINT* CLDSDbObject::GetConstraintByPropID(WORD idDbProp)
{
	CONSTRAINT* pConstraint=NULL;
	for(pConstraint=BelongModel()->EnumConstraintFirst();pConstraint;pConstraint=BelongModel()->EnumConstraintNext())
	{
		if(pConstraint->hMasterObj==handle&&pConstraint->wiMasterPropId==idDbProp)
			break;
		if(pConstraint->hAcceptObj==handle&&pConstraint->wiAcceptPropId==idDbProp)
			break;
	}
	return pConstraint;
}
IXhTemplEnumerator<CONSTRAINT>* CLDSDbObject::GetConstraintEnumerator()
{
	CXhInternalTemplEnumerator<CONSTRAINT>* pInterEnum=(CXhInternalTemplEnumerator<CONSTRAINT>*)IXhEnumerator::NewEnumerator();
	for(CONSTRAINT* pConstraint=BelongModel()->EnumConstraintFirst();pConstraint;pConstraint=BelongModel()->EnumConstraintNext())
	{
		if(pConstraint->hMasterObj==handle||(pConstraint->dual&&pConstraint->hAcceptObj==handle))
			pInterEnum->AppendObjPtr(pConstraint);
	}
	return pInterEnum;
}
bool CLDSDbObject::ProcessConstraintFromMasterObj(CLDSDbObject* pMasterObj,CONSTRAINT* pConstraint)
{
	return false;
}
long CLDSDbObject::ReDesignConstraintSonObjs(WORD idMasterObjProp/*=0*/,CHashSet<CLDSDbObject*>* pHashUpdateObjSet/*=NULL*/)
{
	if(m_nHasConstraintsCounts<=0)
		return false;
	int processed_count=0;
	//������ص�Լ�����ƶ���
	IXhTemplEnumerator<CONSTRAINT>* pList=GetConstraintEnumerator();
	for(CONSTRAINT* pConstraint=pList->EnumFirst();pConstraint;pConstraint=pList->EnumNext())
	{
		if( (pConstraint->hMasterObj==handle&&pConstraint->wiMasterPropId==idMasterObjProp)||
			(pConstraint->hAcceptObj==handle&&pConstraint->wiAcceptPropId==idMasterObjProp&&pConstraint->dual))
		{
			if(pConstraint->hMasterObj==handle)
				pConstraint->came=true;	//��ֹ�����������
			else if(pConstraint->hAcceptObj==handle)
				pConstraint->gone=true;	//��ֹ�����������
			CLDSDbObject* pAcceptObj=NULL;
			CONSTRAINT::ACCEPTOR acceptor;
			if(pConstraint->Execute(BelongModel(),this,NULL,&acceptor))
			{
				if(pHashUpdateObjSet)
					pHashUpdateObjSet->SetValue(acceptor.pAcceptObj->handle,acceptor.pAcceptObj);
				processed_count++;
				processed_count+=acceptor.pAcceptObj->ReDesignConstraintSonObjs(acceptor.wiAcceptPropId,pHashUpdateObjSet);
			}
		}
	}
	return processed_count;
}
bool CONSTRAINT::Execute(IModel* pModel/*=NULL*/,CLDSDbObject* pMasterObj/*=NULL*/,CLDSDbObject* pAcceptObj/*=NULL*/,ACCEPTOR* lpAcceptor/*=NULL*/)
{
	if(pModel==NULL)
		pModel=CLDSPart::_console;
	WORD wiCurrMasterPropId=this->wiMasterPropId;
	WORD wiCurrAcceptPropId=this->wiAcceptPropId;
	if(this->dual&&pMasterObj!=NULL&&pMasterObj->handle==hAcceptObj)
	{
		if(this->came||idExpression!=0)
			return false;
		if(dual)
			came=true;
		if(pAcceptObj==NULL||pAcceptObj->handle!=hMasterObj)
			pAcceptObj=(CLDSDbObject*)pModel->FromHandle(hMasterObj,true);
		wiCurrMasterPropId=this->wiAcceptPropId;
		wiCurrAcceptPropId=this->wiMasterPropId;
	}
	else
	{
		if(this->dual&&this->gone)
			return false;
		if(dual)
			gone=true;
		if(pAcceptObj==NULL||pAcceptObj->handle!=hAcceptObj)
			pAcceptObj=(CLDSDbObject*)pModel->FromHandle(hAcceptObj,true);
		if(pMasterObj==NULL||pMasterObj->handle!=hMasterObj)
			pMasterObj=(CLDSDbObject*)pModel->FromHandle(hMasterObj,true);
	}
	if(pAcceptObj==NULL||pMasterObj==NULL)
		return false;
	if(pAcceptObj->ProcessConstraintFromMasterObj(pMasterObj,this))
	{
		if(lpAcceptor)
		{
			lpAcceptor->pAcceptObj=pAcceptObj;
			lpAcceptor->wiAcceptPropId=pAcceptObj->handle==hMasterObj?wiMasterPropId:wiAcceptPropId;
		}
		return true;
	}
	if(idExpression==0)
	{	//��ֵԼ��
		CLDSDbObject::PROPVAL propval;
		if(!pMasterObj->GetPropParam(wiCurrMasterPropId,&propval))
			return false;
		if(!pAcceptObj->SetPropParam(wiCurrAcceptPropId,propval))
			return false;
		if(lpAcceptor)
		{
			lpAcceptor->pAcceptObj=pAcceptObj;
			lpAcceptor->wiAcceptPropId=wiCurrAcceptPropId;
		}
		return true;
	}
	else if(idExpression>0)
	{
		CXhChar50* pExprStr=hashExpressions.GetValue(idExpression);
		CExpression expr;
		EXPRESSION_VAR* pVar=expr.varList.Append();
		CLDSDbObject::PROPVAL propval;
		if(!pMasterObj->GetPropParam(wiMasterPropId,&propval))
			return false;
		pVar->fValue=propval.ToDouble();
		pVar->variableStr[0]='M';
		propval.fVal=expr.SolveExpression(expression);
		propval.ciType=CLDSDbObject::PROPVAL::TYPE_DOUBLE;
		if(!pAcceptObj->SetPropParam(this->wiAcceptPropId,propval))
			return false;
		if(lpAcceptor)
		{
			lpAcceptor->pAcceptObj=pAcceptObj;
			lpAcceptor->wiAcceptPropId=wiCurrAcceptPropId;
		}
		return true;
	}
	return false;
}
//Part
IPartLibrary* CLDSPart::library=NULL;
int CLDSPart::CustomerSerial=0;	//�ͻ����Ʊ�ʶ���
BOOL CLDSPart::UPDATE_PARTNO_AND_SEG=false;
bool CLDSPart::INTERNAL_TEST_VERSION=false;
CLDSPart::CLDSPart()
{
	m_nClassTypeId = CLS_PART;
	LoadDefaultUCS(&ucs);
	//strcpy(m_sClassName,"CLDSPart");
	m_uStatMatCoef=1;
	m_bVirtualPart = false;
	_bLiveNeedSupplyPart=false;
	is_undo_modified=is_data_modified=is_solid_modified=1;
	//strcpy(guige,"");
	m_bDisplayHole=false;
	strcpy(sPartNo,"");
	_hPartWeldParent=0;
	m_bUserSpecColor=false;
	pSolidBody=NULL;
	m_pMotherBodySolidBuf=NULL;
	m_cFuncType=CLDSPart::FUNC_COMMON;	//������������
	_cQualityLevel=-1;//0��ʾĬ�ϲ���ʾ���������ȼ�,0xff(-1)��ʾ����ϱ��е�Ĭ�������ȼ�����
}
char CLDSPart::set_cMaterial(char cBriefMaterialMark)
{
	CLDSPart* pMotherPart=MotherPart();
	if(library!=NULL&&_material!=cBriefMaterialMark)
	{	//�任����ʱ�����ܸ���Ĭ�ϵĲ��������ȼ�
		STEELMAT* pMat=library->QuerySteelMatByBriefMark(cBriefMaterialMark);
		if(pMat)
			_cQualityLevel=pMat->cPreferQualityLevel;
	}
	_material=cBriefMaterialMark;
	SetModified(TRUE,FALSE);
	if(pMotherPart!=this)
	{
		pMotherPart->cMaterial=cBriefMaterialMark;
		pMotherPart->cQualityLevel=_cQualityLevel;
	}
	return cBriefMaterialMark;
}
char CLDSPart::get_cMaterial()
{
	CLDSPart* pMotherPart=MotherPart();
	if(pMotherPart==this)
		return _material;
	else
		return pMotherPart->cMaterial;
}
char CLDSPart::set_cQualityLevel(char qualitylevel)
{
	_cQualityLevel=qualitylevel;
	SetModified(TRUE,FALSE);
	return _cQualityLevel;
}
char CLDSPart::get_cQualityLevel()
{
	if (cbVirginQualityLevel==0xff)
	{
		if(library!=NULL)
		{
			STEELMAT* pSteel=library->QuerySteelMatByBriefMark(_material);
			return (pSteel!=NULL)?pSteel->cPreferQualityLevel:0;
		}
		else
			return 0;
	}
	else 
		return _cQualityLevel;
}
BYTE CLDSPart::get_cbVirginQualityLevel(){
	return (BYTE)_cQualityLevel;
}
BYTE CLDSPart::set_cbVirginQualityLevel(BYTE qualitylevel)
{
	_cQualityLevel=(char)qualitylevel;
	return qualitylevel;
}
bool CLDSPart::get_blDefaultQualityLevel()
{
	return (((BYTE)_cQualityLevel)==0xff);
}
bool CLDSPart::set_blDefaultQualityLevel(bool blSetTrue)
{
	if(blSetTrue)
		_cQualityLevel=-1;
	else if(((BYTE)_cQualityLevel)==0xff)
	{
		char quality=cQualityLevel;
		_cQualityLevel=quality;
	}
	return blSetTrue;
}
IMPLEMENT_GET(CLDSPart::HOLE,fHoleD)
{
	return wBoltD+fHoleMargin;
}
IMPLEMENT_SET(CLDSPart::HOLE,fHoleD)
{
	if(wBoltD<12)
	{
		wBoltD=ftoi(value-1.6);
		if(wBoltD<=15)
			wBoltD=12;
		else if(wBoltD<=17)
			wBoltD=16;
		else if(wBoltD<=19)
			wBoltD=18;
		else if(wBoltD<=23)
			wBoltD=20;
		fHoleMargin=value-wBoltD;
	}
	else
		fHoleMargin=value-wBoltD;
}
char CLDSPart::get_ciLegQuad()
{
	if(m_ciLegQuad>0&&m_ciLegQuad<5)
		return m_ciLegQuad;
	else if(layer_tag[0]=='L')
	{
		char ciQuad=layer_tag[2]-'0';
		if(ciQuad>0&&ciQuad<5)
			return ciQuad;
		if(ucs.origin.x>EPS&&ucs.origin.y>EPS)
			return 1;
		else if(ucs.origin.x<EPS&&ucs.origin.y>EPS)
			return 2;
		else if(ucs.origin.x>EPS&&ucs.origin.y<EPS)
			return 3;
		else if(ucs.origin.x<EPS&&ucs.origin.y<EPS)
			return 4;
		else
			return 0;
	}
	else
		return 0;
}
UINT CLDSPart::AddMiscellaneousHoles(IXhList<HOLE>* pListHoles)
{
	return 0;
}	//��GetHoleSet�ڵ���,����ӳ�������˨����������

UINT CLDSPart::GetHoleSet(IXhList<HOLE>* pListHoles,bool bOnlyBoltHole/*=false*/)
{
	return 0;	//����ֵΪ������
}
void CLDSPart::UpdatePartNoBySegI()
{
	if(!UPDATE_PARTNO_AND_SEG)
		return;
	if(GetClassTypeId()==CLS_BOLT)
		return;
	if(m_nClassTypeId==CLS_PARAMPLATE && ((CLDSParamPlate*)this)->m_bStdPart)
		return;
	SEGI iOldSeg;
	CXhChar16 serial,sSeparator;
	if(sPartNo.GetLength()<=0 || !ParsePartNo(sPartNo,&iOldSeg,serial,NULL,sSeparator))
		return;
	sPartNo.Copy(iSeg.ToString());
	if(sSeparator.GetLength()>0)
		sPartNo.Append(sSeparator);
	sPartNo.Append(serial);
}
bool CLDSPart::IsAcceptableTypeId(int type_id)
{
	if(type_id==CLS_PART||type_id==CLS_OBJECT||type_id==CLS_DBOBJECT)
		return true;
	else if(type_id==m_nClassTypeId)
		return true;
	else
		return false;
}
BOOL CLDSPart::CreateShadowPart3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/,
											double sample_len/*=5.0*/,int smoothness/*=36*/)
{
#ifdef __ANGLE_PART_INC_
	if(IsShadowObject())
	{
		bool bNeedUpdate=false;
		CLDSPart *pMotherPart=MotherPart(false);
		if(pSolidBody==NULL||pMotherPart->pSolidBody==NULL||IsModified()||pMotherPart->IsModified())
			bNeedUpdate=true;
		else if(pMotherPart->pSolidBody->BufferPtr()!=m_pMotherBodySolidBuf)
			bNeedUpdate=true;
		if(bNeedUpdate)
		{
			SetModified(FALSE);
			if(pSolidBody==NULL)	//��һ������Ӱ�乹��ʱ��Ҫ����ShadowDesign()����
				ShadowDesign();
			pMotherPart->Create3dSolidModel(TRUE,scale_of_user2scr,sample_len,smoothness);
			if(m_iShadowObjType==CLDSObject::SHADOW_ISOLATED||m_iShadowObjType==CLDSObject::SHADOW_EMBEDDED)
			{
				if(IsAngle())
					((CLDSLineAngle*)this)->getUCS(ucs);
				else if(IsTube())
					ucs=((CLDSLineTube*)this)->BuildUCS();
				else if(GetClassTypeId()==CLS_LINESLOT)
					ucs=((CLDSLineSlot*)this)->BuildUCS();
				else if(GetClassTypeId()==CLS_LINEFLAT)
					ucs=((CLDSLineFlat*)this)->BuildUCS();
				else if(GetClassTypeId()==CLS_BOLT)
					ucs=((CLDSBolt*)this)->GetUCS();
				f3dPoint org=ucs.origin;	//������ƫ�ƿ��ǵ�����ϵת��
				if(GetClassTypeId()==CLS_PLATE)
					ucs.origin+=ucs.axis_z*((CLDSPlate*)this)->m_fNormOffset;
				else if(GetClassTypeId()==CLS_PARAMPLATE)
				{

				} 
				CloneBodyFrom(pMotherPart->pSolidBody);
				ucs.origin=org;				//�ָ�ԭԭ������
			}
			else if(m_iShadowObjType==CLDSObject::SHADOW_EXTERBLK)
			{
				CBlockModel *pBlock=(CBlockModel*)pMotherPart->BelongModel();
				CBlockReference* pBlockRef=((CTower*)m_pModel)->BlockRef.FromHandle(shadow.BLOCK_OBJ.m_hBlockRef);
				ucs=pMotherPart->ucs;
				UCS_STRU cs=pBlock->TransToACS(pBlockRef->GetACS());
				coord_trans(ucs.origin,cs,TRUE);
				vector_trans(ucs.axis_x,cs,TRUE);
				vector_trans(ucs.axis_y,cs,TRUE);
				vector_trans(ucs.axis_z,cs,TRUE);
				CloneBodyFrom(pMotherPart->pSolidBody);
			}
			else if (m_iShadowObjType==CLDSObject::SHADOW_ISOLATED_RELA_LOCATION)
			{
				CloneBodyFrom(pMotherPart->pSolidBody);
			}
			m_pMotherBodySolidBuf=pMotherPart->pSolidBody->BufferPtr();
			//Ӱ�񹹼�����ɫ��ǿ��
			/*for(f3dPolyFace *pFace=pBody->faceList.GetFirst();pFace;pFace=pBody->faceList.GetNext())
			{
				BYTE* color=(BYTE*)&pFace->material;
				for(int i=0;i<3;i++)
				{	
					color[i]/=3;
					color[i]+=color[i];
				}
			}*/
		}
		return TRUE;
	}
#endif
	return FALSE;
}
void CLDSPart::SetModified(BOOL bTrue/*=TRUE*/,BOOL bRenderSolid/*=TRUE*/)
{
	is_data_modified=(bTrue!=0);
	if(bTrue)
	{
		if(m_pModel)
			SetDocModifiedFlag();
		is_undo_modified=TRUE;
		is_solid_modified|=(bRenderSolid!=0);
	}
	else
		is_solid_modified=bRenderSolid&&is_data_modified;
}
//���ڸ˼�0.��ȫ�Գ�һ��;1.��ʵ��������Գ�һ��;2.ʰȡ�㴦����ͬ��Գ�;3.ʰȡ�㴦��������Գ�
BYTE CLDSPart::MirSimilarGradeWith(CLDSPart* pPart,const MIRMSG& msg,f3dPoint *pPickPos/*=NULL*/)
{
	return 255;	//δʵ��
}
CLDSPart *CLDSPart::GetMirPart(MIRMSG mirmsg,const double *pPickPos/*=NULL*/,BOOL bCheckProfile/*=TRUE*/)
{
	//try{
		int cls_typeid=GetClassTypeId();
		for(RELATIVE_OBJECT *pItem=relativeObjs.GetFirst();pItem;pItem=relativeObjs.GetNext())
		{	//���ڹ��������б��в���
			CLDSPart *pPart=NULL;
			if(pItem->mirInfo.IsSameMirMsg(mirmsg))
			{
				pPart=BelongModel()->FromPartHandle(pItem->hObj,cls_typeid);
				if(pPart&&ModuleTogetherWith(pPart))
					return pPart;
				else
					break;
			}
		}
		CSuperSmartPtr<CLDSPart>pMirPart;
		f3dPoint mir_origin=GetMirPos(ucs.origin,mirmsg);
		BOOL pushed=m_pModel->PushPartStack(); //���������б�ǰӦ��ѹջ wht 12-03-05
		for(pMirPart=m_pModel->EnumPartFirst(cls_typeid);pMirPart.IsHasPtr();pMirPart=m_pModel->EnumPartNext(cls_typeid))
		{
			if(pMirPart->ucs.origin==mir_origin&&ModuleTogetherWith(pMirPart))
			{
				m_pModel->PopPartStack(pushed);
				return pMirPart;
			}
		}
		m_pModel->PopPartStack(pushed);
	/*}
	catch(CPtrException &e)
	{
		AfxMessageBox(e.sError);
	}*/
	return NULL;
}
int CLDSPart::GetPropValueStr(long id, char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	char sText[500]="";
	if(GetPropID("handle")==id)
		sprintf(sText,"0X%X",handle);
	else if(GetPropID("layer")==id)
		strcpy(sText,layer());
	else if(GetPropID("cMaterial")==id)	
	{
		char matStr[20]="";
		QuerySteelMatMark(cMaterial,matStr);
		sprintf(sText,"%s",matStr);
	}
	else if(GetPropID("m_cQualityLevel")==id)
	{
		if(isalpha(m_cQualityLevel))
			sprintf(sText,"%C",m_cQualityLevel);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Default");
#else
			strcpy(sText,"Ĭ��");
#endif
	}
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("sPartNo")==id)
		strcpy(sText,GetPartNo());
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(cfgword,valueStr);
	{
		if(layer(0)=='L')	//���ȹ���
			return cfgword.GetLegScopeStr(valueStr,50,true);
		else
			return cfgword.GetBodyScopeStr(valueStr);
	}
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",m_uStatMatCoef);
	else if(GetPropID("m_hPartWeldParent")==id)
		sprintf(sText,"0X%X",m_hPartWeldParent);
	else if(GetPropID("m_hBlock")==id)
		strcpy(sText,GetBlockIdName());
	else if(GetPropID("m_bDisplayHole")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bDisplayHole)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(m_bDisplayHole)
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bVirtualPart)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");	
#else 
		if(m_bVirtualPart)
			strcpy(sText,"��");
		else
			strcpy(sText,"��");	
#endif
	}
	else if(GetPropID("m_bUserSpecColor")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bUserSpecColor)
			sprintf(sText,"Yes");
		else
			sprintf(sText,"No");
#else 
		if(m_bUserSpecColor)
			sprintf(sText,"��");
		else
			sprintf(sText,"��");
#endif
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",crMaterial);
	else if(GetPropID("sNotes")==id)
		strcpy(sText,sNotes);
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
long CLDSPart::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem=CLDSPart::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
	{
		if(log2file)
			log2file->Log("û�ҵ���Ϊ'%s'������Id",propStr);
		return 0;
	}
}
int CLDSPart::GetPropStr(long id, char *propStr)
{
	if(GetPropID("basicInfo")==0)
	{
		if(propStr)
			strcpy(propStr,"basicInfo");
		return strlen("basicInfo");
	}
	else if(GetPropID("handle")==id)
	{
		if(propStr)
			strcpy(propStr,"handle");
		return strlen("handle");
	}
	else if(GetPropID("layer")==id)
	{
		if(propStr)
			strcpy(propStr,"layer");
		return strlen("layer");
	}
	else if(GetPropID("cMaterial")==id)
	{
		if(propStr)
			strcpy(propStr,"cMaterial");
		return strlen("cMaterial");
	}
	else if(GetPropID("m_cQualityLevel")==id)
	{
		if(propStr)
			strcpy(propStr,"m_cQualityLevel");
		return strlen("m_cQualityLevel");
	}
	else if(GetPropID("iSeg")==id)
	{
		if(propStr)
			strcpy(propStr,"iSeg");
		return strlen("iSeg");
	}
	else if(GetPropID("sPartNo")==id)
	{
		if(propStr)
			strcpy(propStr,"sPartNo");
		return strlen("sPartNo");
	}
	else if(GetPropID("cfgword")==id)
	{
		if(propStr)
			strcpy(propStr,"cfgword");
		return strlen("cfgword");
	}
	else if(GetPropID("m_uStatMatCoef")==id)
	{
		if(propStr)
			strcpy(propStr,"m_uStatMatCoef");
		return strlen("m_uStatMatCoef");
	}
	else if(GetPropID("m_hPartWeldParent")==id)
	{
		if(propStr)
			strcpy(propStr,"m_hPartWeldParent");
		return strlen("m_hPartWeldParent");
	}
	else if(GetPropID("m_bDisplayHole")==id)
	{
		if(propStr)
			strcpy(propStr,"m_bDisplayHole");
		return strlen("m_bDisplayHole");
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
		if(propStr)
			strcpy(propStr,"m_bVirtualPart");
		return strlen("m_bVirtualPart");
	}
	else if(GetPropID("sNotes")==id)
	{
		if(propStr)
			strcpy(propStr,"sNotes");
		return strlen("sNotes");
	}
	else
	{
		if(propStr)
			strcpy(propStr,"");
		return 0;
	}
}
BOOL CLDSPart::IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSPart *pPart=NULL, *pFirstPart = (CLDSPart*)selectObjs.GetFirst();
	pFirstPart->GetPropValueStr(idProp,valueStr);
	for(pPart=(CLDSPart*)selectObjs.GetNext();pPart;pPart=(CLDSPart*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pPart->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CLDSPart::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CLDSPart::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}

BOOL CLDSPart::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSPart::propStatusHashtable.SetValueAt(id,dwStatus);
}
void CLDSPart::PEER_PART::CopyFrom(PEER_PART& peerpart)
{
	ciEqualState=peerpart.ciEqualState;	//0.��ȫһ�¿ɹ��ջ���;1.
	cbDetailFlag=peerpart.cbDetailFlag;	//0.������κ�ϸ��;0x01:���hashBoltsByBase; 0x02:���hashBoltsByPeer;
	hPeerPart=peerpart.hPeerPart;		//��鹹���ľ��
	m_hBlock=peerpart.m_hBlock;
	csBase=peerpart.csBase;	//��׼�����Ա�һ�»����ʱ�ľֲ���װ�䣩����ϵ
	csPeer=peerpart.csPeer;	//��鹹���Ա�һ�»����ʱ�ľֲ���װ�䣩����ϵ
	PEER_BOLT* pPeerBolt;
	hashBoltsByBase.Empty();
	hashBoltsByPeer.Empty();
	for(pPeerBolt=peerpart.hashBoltsByBase.GetFirst();pPeerBolt;pPeerBolt=peerpart.hashBoltsByBase.GetNext())
		hashBoltsByBase.SetValue(pPeerBolt->hBaseBolt,*pPeerBolt);
	for(pPeerBolt=peerpart.hashBoltsByPeer.GetFirst();pPeerBolt;pPeerBolt=peerpart.hashBoltsByPeer.GetNext())
		hashBoltsByPeer.SetValue(pPeerBolt->hPeerBolt,*pPeerBolt);
}
char* CLDSPart::GetPartTypeName()
{
#ifdef AFX_TARG_ENU_ENGLISH
	if(GetClassTypeId()==CLS_LINEANGLE)
		return "Angle";
	else if(GetClassTypeId()==CLS_GROUPLINEANGLE)
		return "Composite angle";
	else if(GetClassTypeId()==CLS_LINETUBE)
		return "Tube";
	else if(GetClassTypeId()==CLS_LINESLOT)
		return "[ steel";
	else if(GetClassTypeId()==CLS_LINEFLAT)
		return "Flat steel";
	else if(GetClassTypeId()==CLS_PLATE)
		return "Plate";
	else if(GetClassTypeId()==CLS_PARAMPLATE)
		return "Parametric Plate";
	else if(GetClassTypeId()==CLS_ARCANGLE)
		return "Circular Angle";
	else if(GetClassTypeId()==CLS_ARCFLAT)
		return "Circular Flat steel";
	else if(GetClassTypeId()==CLS_ARCSLOT)
		return "Circular [ steel";
	else if(GetClassTypeId()==CLS_SPHERE)
		return "Sphere";
	else if(GetClassTypeId()==CLS_LINEPART)
		return "Rod";
	else if(GetClassTypeId()==CLS_POLYPART)
		return "Bend Rod";
	else if(GetClassTypeId()==CLS_BOLT)
		return "Bolt";
	else if(GetClassTypeId()==CLS_SPHERE)
		return "Sphere";
#else 
	if(GetClassTypeId()==CLS_LINEANGLE)
		return "�Ǹ�";
	else if(GetClassTypeId()==CLS_GROUPLINEANGLE)
		return "��ϽǸ�";
	else if(GetClassTypeId()==CLS_LINETUBE)
		return "�ֹ�";
	else if(GetClassTypeId()==CLS_LINESLOT)
		return "�۸�";
	else if(GetClassTypeId()==CLS_LINEFLAT)
		return "����";
	else if(GetClassTypeId()==CLS_PLATE)
		return "�ְ�";
	else if(GetClassTypeId()==CLS_PARAMPLATE)
		return "�������ְ�";
	else if(GetClassTypeId()==CLS_ARCANGLE)
		return "���νǸ�";
	else if(GetClassTypeId()==CLS_ARCFLAT)
		return "���α���";
	else if(GetClassTypeId()==CLS_ARCSLOT)
		return "���β۸�";
	else if(GetClassTypeId()==CLS_SPHERE)
		return "����";
	else if(GetClassTypeId()==CLS_LINEPART)
		return "�˼�";
	else if(GetClassTypeId()==CLS_POLYPART)
		return "����˼�";
	else if(GetClassTypeId()==CLS_BOLT)
		return "��˨";
	else if(GetClassTypeId()==CLS_SPHERE)
		return "����";
#endif
	else
		return "";
}
//bAnalysisMode=trueʱ����size.idClassType�жϣ��������GetClassTypeId�ж�
bool CLDSPart::IsAngle(BOOL bAnalysisMode/*=false*/)
{
	if(bAnalysisMode&&IsLinePart())
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)this;
		if(pLinePart->size_idClassType==CLS_LINEANGLE||pLinePart->size_idClassType==CLS_GROUPLINEANGLE)
			return true;
		else
			return false;
	}
	else
	{
		if(GetClassTypeId()==CLS_LINEANGLE||GetClassTypeId()==CLS_GROUPLINEANGLE)
			return true;
		else
			return false;
	}
}
//�򻯹��������ж� wht 11-05-19
bool CLDSPart::IsTube(BOOL bAnalysisMode/*=false*/)
{
	if(bAnalysisMode&&IsLinePart())
	{
		CLDSLinePart *pLinePart=(CLDSLinePart*)this;
		if(pLinePart->size_idClassType==CLS_LINETUBE)
			return true;
		else
			return false;
	}
	else
	{
		if(GetClassTypeId()==CLS_LINETUBE)
			return true;
		else
			return false;
	}
}

void CLDSPart::CloneBodyFrom(CSolidBody *pMotherBody)
{
	if(pSolidBody)
		pSolidBody->Empty();
	else
		pSolidBody=new CSolidBody();
	SetModified(FALSE);
	//TODO:δ���
	CLDSPart *pMotherPart=MotherPart(false);
	CHashTable<f3dPoint*> hashVertexPtr;
	CBuffer buf(pMotherBody->BufferPtr(),pMotherBody->BufferLength());
	pSolidBody->ReadFrom(&buf,buf.GetLength());
	pSolidBody->TransACS(pMotherPart->ucs,ucs);
	/*CSolidBodyBuffer solidbuf(pSolidBody->BufferPtr(),pSolidBody->BufferLength());
	//ȫ����������ת��
	UINT vertex_n=solidbuf.VertexNumber;
	BYTE vertex_record_len=solidbuf.VertexRecordLength;
	char* vertex_data_addr=solidbuf.GetBufferPtr()+solidbuf.VertexDataStartAddr;
	GEPOINT vertex;
	for(UINT i=0;i<vertex_n;i++)
	{
		memcpy(&vertex,vertex_data_addr+i*vertex_record_len,24);
		coord_trans(vertex,pMotherPart->ucs,FALSE);
		coord_trans(vertex,ucs,TRUE);
		memcpy(vertex_data_addr+i*vertex_record_len,&vertex,24);
	}
	//ȫ��ʵ�����Բ��ʸ������ת��
	UINT edge_n=solidbuf.EdgeNumber;
	char* edge_data_addr=solidbuf.GetBufferPtr()+solidbuf.EdgeDataStartAddr;
	CRawSolidEdge rawedge;
	WORD edgebuf_len;
	for(i=0;i<edge_n;i++)
	{
		memcpy(&edgebuf_len,edge_data_addr,2);
		edge_data_addr+=2;
		rawedge.InitBuffer(edge_data_addr,edgebuf_len,true);
		edge_data_addr+=edgebuf_len;
		if(rawedge.EdgeType==CRawSolidEdge::ARCLINE||rawedge.EdgeType==CRawSolidEdge::ELLIPSE)
		{
			vertex=rawedge.Center;
			coord_trans(vertex,pMotherPart->ucs,FALSE);
			coord_trans(vertex,ucs,TRUE);
			rawedge.Center=vertex;
			vertex=rawedge.WorkNorm;
			vector_trans(vertex,pMotherPart->ucs,FALSE);
			vector_trans(vertex,ucs,TRUE);
			rawedge.WorkNorm=vertex;
			if(rawedge.EdgeType==CRawSolidEdge::ELLIPSE)
			{
				vertex=rawedge.ColumnNorm;
				vector_trans(vertex,pMotherPart->ucs,FALSE);
				vector_trans(vertex,ucs,TRUE);
				rawedge.ColumnNorm=vertex;
			}
		}
	}
	//ȫ��ʵ������ָ���淨�߲���ת��
	UINT face_n=solidbuf.RawFaceNumber;
	WORD facebuf_len;
	CRawSolidFace rawface;
	char* rawface_data_addr=solidbuf.GetBufferPtr()+solidbuf.RawFaceDataStartAddr;
	for(i=0;i<face_n;i++)
	{
		memcpy(&facebuf_len,rawface_data_addr,2);
		rawface_data_addr+=2;
		rawface.InitBuffer(rawface_data_addr,facebuf_len);
		rawface_data_addr+=facebuf_len;
		vertex = rawface.WorkNorm;
		vector_trans(vertex,pMotherPart->ucs,FALSE);
		vector_trans(vertex,ucs,TRUE);
		rawface.WorkNorm=vertex;
	}*/
}
#ifdef __LDS_CONTEXT_
bool CLDSPart::CloneToBomPart(BOMPART* pBomPart)
{	//��������
	pBomPart->sPartNo.Copy(GetPartNo());
	pBomPart->iSeg=iSeg;
	if(IsLinePart())
	{
		if(toupper(layer_tag[1])=='Z')
			pBomPart->siSubType=BOMPART::SUB_TYPE_ROD_Z;
		else if(toupper(layer_tag[1])=='F')
			pBomPart->siSubType=BOMPART::SUB_TYPE_ROD_F;
	}
	pBomPart->cMaterial=cMaterial;
	pBomPart->cQualityLevel=this->m_cQualityLevel;
	pBomPart->wide = GetWidth();
	pBomPart->thick = GetThick();
	GetSpecification(pBomPart->sSpec,FALSE);
	pBomPart->length=GetLength();
	pBomPart->fPieceWeight = GetWeight();
	pBomPart->siZhiWan=0;//pBomAngle->bHuoQuStart||pBomAngle->bHuoQuEnd;
	if(IsAngle())	//���ںϷ����Ӹ������򺸽�����ΪTRUE
		pBomPart->bWeldPart=((CLDSLineAngle*)this)->IsWeldPart();
	else 
		pBomPart->bWeldPart=(BelongModel()->FromPartHandle(m_hPartWeldParent)!=NULL);
	pBomPart->bLegPart=IsLegObj();
	if(strlen(sNotes)>0)
		strcpy(pBomPart->sNotes,sNotes);
	//��˨��
	pBomPart->nM16Ls=GetLsCount(16);
	pBomPart->nM20Ls=GetLsCount(20);
	pBomPart->nM24Ls=GetLsCount(24);
	pBomPart->nMXLs=GetLsCount()-pBomPart->nM16Ls-pBomPart->nM20Ls-pBomPart->nM24Ls;
	pBomPart->nMSumLs = GetLsCount();
	return true;
}
#endif
HOLE_WALL::HOLE_WALL()
{
	is_visible=true;
	hBolt=0;
	pBolt=NULL;
	pSolidBody=NULL;
	for(int i=0;i<4;i++)
		items[i].hRelaPart=0;
}
HOLE_WALL::~HOLE_WALL()
{
	if(pSolidBody)
		delete pSolidBody;
}
void HOLE_WALL::SetKey(XHUINT64 key)
{
	DUALKEY dk=key;
	hBolt=dk.lowId;
	this->items[0].hRelaPart=dk.highId;
}
BOOL HOLE_WALL::UpdateSolid()
{
	vStart=vEnd=GEPOINT(0,0,0);
	for(int i=0;i<4;i++)
	{
		if(items[i].hRelaPart<=0)
			continue;
		CLDSPart* pPart=pBolt->BelongModel()->FromPartHandle(items[i].hRelaPart);
		pPart->UpdateHoleWall(this);
	}
	Create3dSolidModel();
	return TRUE;
}
BOOL HOLE_WALL::Create3dSolidModel(double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(pBolt==NULL)
		return FALSE;
	GECS holeucs;
	holeucs.origin=vStart;
	holeucs.axis_z=vEnd-vStart;//pBolt->get_norm();
	normalize(holeucs.axis_z);
	holeucs.axis_x=pBolt->ucs.axis_x;
	if(holeucs.axis_x.IsZero())
		holeucs.axis_x=btc::CalLocalCS_X_AXIS(holeucs.axis_z);
	holeucs.axis_y=holeucs.axis_z^holeucs.axis_x;
	if(fabs(holeucs.axis_x*holeucs.axis_z)>EPS)
	{
		normalize(holeucs.axis_y);
		holeucs.axis_x=holeucs.axis_y^holeucs.axis_z;
	}
	int i,slices=12;//CalArcResolution(radius,2*Pi,scale_of_user2scr,sample_len,smoothness);
	if(slices<3)
		return FALSE;
	COLORREF crLsHole=CLDSPart::GetColor(CLS_BOLT,ftoi(pBolt->get_d()),' ');	//������˨ָ���õ���˨����ɫ
	DYN_ARRAY<f3dPoint> scan_point_arr(slices);
	double radius=0.5*(pBolt->get_d()+pBolt->hole_d_increment);
	CLDSPart::GetCircleSimuPolyVertex(radius,scan_point_arr,slices);
	double inner_coef=(radius-0.25)/radius;
	fBody body;
	for(int index=0;index<4;index++)
	{	//�ֱ����ÿ�������ϵ���˨��
		if(items[index].hRelaPart<=0)
			continue;
		f3dPoint ptS=items[index].ptS;
		f3dPoint ptE=items[index].ptE;
		f3dPoint vec=(ptE-ptS).normalized();
		if(vec*holeucs.axis_z<0)
		{
			f3dPoint pt=ptS;
			ptS=ptE;
			ptE=pt;
		}
		DYN_ARRAY<f3dPoint*> up_cir_vertex(slices);
		DYN_ARRAY<f3dPoint*> down_cir_vertex(slices);
		DYN_ARRAY<f3dPoint*> up_hole_wall(slices);
		DYN_ARRAY<f3dPoint*> down_hole_wall(slices);
		for(i=0;i<slices;i++)
		{
			down_cir_vertex[i]=body.vertex.append(scan_point_arr[i]);
			down_hole_wall[i]=body.vertex.append(scan_point_arr[i]);
			down_hole_wall[i]->x*=inner_coef;
			down_hole_wall[i]->y*=inner_coef;
			down_hole_wall[i]->z+=0.5; 
			holeucs.origin=ptS;
			coord_trans(*down_cir_vertex[i],holeucs,TRUE,TRUE);
			coord_trans(*down_hole_wall[i],holeucs,TRUE,TRUE);

			up_cir_vertex[i]=body.vertex.append(scan_point_arr[i]);
			up_hole_wall[i]=body.vertex.append(scan_point_arr[i]);
			up_hole_wall[i]->x*=inner_coef;
			up_hole_wall[i]->y*=inner_coef;
			up_hole_wall[i]->z-=0.5; 
			holeucs.origin=ptE;
			coord_trans(*up_cir_vertex[i],holeucs,TRUE,TRUE);
			coord_trans(*up_hole_wall[i],holeucs,TRUE,TRUE);
		}
		for(i=0;i<slices;i++)
		{
			f3dPolyFace *pInnerFace = body.faceList.append();
			pInnerFace->material = crLsHole;
			pInnerFace->outer_edge.append(up_hole_wall[i],up_hole_wall[(i+1)%slices]);
			pInnerFace->outer_edge.append(up_hole_wall[(i+1)%slices],down_hole_wall[(i+1)%slices]);
			pInnerFace->outer_edge.append(down_hole_wall[(i+1)%slices],down_hole_wall[i]);
			pInnerFace->outer_edge.append(down_hole_wall[i],up_hole_wall[i]);
			//draw up  hole wall
			f3dPolyFace *pUpFilletFace = body.faceList.append();
			pUpFilletFace->material = crLsHole;
			pUpFilletFace->outer_edge.append(up_cir_vertex[i],up_cir_vertex[(i+1)%slices]);
			pUpFilletFace->outer_edge.append(up_cir_vertex[(i+1)%slices],up_hole_wall[(i+1)%slices]);
			pUpFilletFace->outer_edge.append(up_hole_wall[(i+1)%slices],up_hole_wall[i]);
			pUpFilletFace->outer_edge.append(up_hole_wall[i],up_cir_vertex[i]);
			//draw down  hole wall
			f3dPolyFace *pDownFilletFace = body.faceList.append();
			pDownFilletFace->material = crLsHole;
			pDownFilletFace->outer_edge.append(down_hole_wall[i],down_hole_wall[(i+1)%slices]);
			pDownFilletFace->outer_edge.append(down_hole_wall[(i+1)%slices],down_cir_vertex[(i+1)%slices]);
			pDownFilletFace->outer_edge.append(down_cir_vertex[(i+1)%slices],down_cir_vertex[i]);
			pDownFilletFace->outer_edge.append(down_cir_vertex[i],down_hole_wall[i]);
		}
	}
	if(pSolidBody==NULL)
		pSolidBody=new CSolidBody();
	else
		pSolidBody->Empty();
	pSolidBody->ConvertFrom(&body);
	return TRUE;	
}
CSolidPart HOLE_WALL::GetSolidPartObject()
{
	CSolidPart solidPart;
	//solidPart.m_hPart = hBolt;
	solidPart.hiberid.masterId=hBolt;
	solidPart.hiberid.SetHiberarchy(1,items[0].hRelaPart);	//�趨�������������ױ���������
	solidPart.m_idClassType = CLS_HOLEWALL;
	solidPart.acs = pBolt->ucs;
	solidPart.m_iLineType = 0;
	solidPart.pBody = pSolidBody;
	solidPart.dfSphereRadius=0;
	return solidPart;
}
//�˺���Ŀ���Ǽ������º����Ĳ����������⣬��δ���� wjh-2011.7.18
BOOL CLDSPart::Create3dSolidModel(SOLIDMODEL_DISPLAY_PARA dispPara)
{
	return Create3dSolidModel(dispPara.bDisplayHole,dispPara.scale_of_user2scr,dispPara.sample_len,dispPara.smoothness);
}
BOOL CLDSPart::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(pSolidBody!=NULL&& !IsModified())
		return FALSE;
	else
	{
		if(pSolidBody == NULL)
			pSolidBody = new CSolidBody;
		else
			pSolidBody->Empty();
	}
	SetModified(FALSE);
	return TRUE;
}
CLDSPart::~CLDSPart()
{
	if(pSolidBody)
	{
		delete pSolidBody;
		pSolidBody=NULL;
	}
}

CSolidPart CLDSPart::GetSolidPartObject()
{
	CSolidPart solidPart;
	if(IsLinePart())
	{
		CLDSLinePart* pLinePart=(CLDSLinePart*)this;
		CLDSNode *pStart=NULL,*pEnd=NULL;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLinePart)->group_father_jg_h>0x20)
			solidPart.m_bDisplaySingle=FALSE;	//����ʾ����
		else 
#endif
		{
			if(pLinePart->pStart&&pLinePart->pEnd)
			{
				pStart=pLinePart->pStart;
				pEnd=pLinePart->pEnd;
				solidPart.line.startPt = pLinePart->pStart->ActivePosition();
				solidPart.line.endPt = pLinePart->pEnd->ActivePosition();
				solidPart.m_iLineType = 1;
			}
			else
				solidPart.m_iLineType = 0;
		}
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(IsArcPart())
	{
		CLDSArcPart* pArcPart=(CLDSArcPart*)this;
		solidPart.arc=pArcPart->baseline;
		solidPart.m_iLineType = 2;
	}
#endif
	else
		solidPart.m_iLineType = 0;
	solidPart.m_hPart = handle;
	solidPart.m_idClassType = GetMotherClassTypeId();
	solidPart.acs = ucs;
	solidPart.pBody = pSolidBody;
	return solidPart;
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
bool CLDSPart::IsEqual(CLDSPart *pPart, BOOL bCompareBoltInfo,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
						   int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{
	if(pPart->GetClassTypeId()!=m_nClassTypeId)
	{
		if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(error_info,"Part type id different");
#else 
			strcpy(error_info,"�������Ͳ�ͬ");
#endif
		return false;	//�������Ͳ�һ��
	}
	if(pPart==this)
		return true;	//pPartΪ��ǰ�������ñȽ�
	if(pPart->MotherPart()==MotherPart())
		return true;	//Ӱ��ĸ����һ��ʱ���ñȽ�
	if(pPart->GetClassTypeId()==CLS_LINEANGLE)
		return (bool)((CLDSLineAngle*)this)->IsEqual((CLDSLineAngle*)pPart,epsilon,error_info,pErrorInfo,pChkStyleFlag);
	else if(pPart->GetClassTypeId()==CLS_LINETUBE)
		return (bool)((CLDSLineTube*)this)->IsEqual((CLDSLineTube*)pPart,epsilon,error_info,pErrorInfo,pChkStyleFlag);
	else if(pPart->GetClassTypeId()==CLS_PLATE)
		return (bool)((CLDSPlate*)this)->IsEqual((CLDSPlate*)pPart,epsilon,error_info,pErrorInfo,pChkStyleFlag);
	else if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		return (bool)((CLDSParamPlate*)this)->IsEqual((CLDSParamPlate*)pPart,epsilon,error_info,pErrorInfo,pChkStyleFlag);
	else if(pPart->GetClassTypeId()==CLS_LINEFLAT)
		return (bool)((CLDSLineFlat*)this)->IsEqual((CLDSLineFlat*)pPart,epsilon,error_info,pErrorInfo,pChkStyleFlag);
	else if(pPart->GetClassTypeId()==CLS_LINESLOT)
		return (bool)((CLDSLineSlot*)this)->IsEqual((CLDSLineSlot*)pPart,epsilon,error_info,pErrorInfo,pChkStyleFlag);
	else if(pPart->GetClassTypeId()==CLS_ARCANGLE)
		return (bool)((CLDSArcAngle*)this)->IsEqual((CLDSArcAngle*)pPart,epsilon,error_info,pErrorInfo,pChkStyleFlag);
	else if(pPart->GetClassTypeId()==CLS_ARCFLAT)
		return (bool)((CLDSArcFlat*)this)->IsEqual((CLDSArcFlat*)pPart,epsilon,error_info,pErrorInfo,pChkStyleFlag);
	else if(pPart->GetClassTypeId()==CLS_ARCSLOT)
		return (bool)((CLDSArcSlot*)this)->IsEqual((CLDSArcSlot*)pPart,epsilon,error_info,pErrorInfo,pChkStyleFlag);
	else if(pPart->GetClassTypeId()==CLS_SPHERE)
		return (bool)((CLDSSphere*)this)->IsEqual((CLDSSphere*)pPart,epsilon,error_info,pErrorInfo,pChkStyleFlag);
	else if(pPart->GetClassTypeId()==CLS_POLYPART)
		return (bool)((CLDSPolyPart*)this)->IsEqual((CLDSPolyPart*)pPart,epsilon,error_info,pErrorInfo,pChkStyleFlag);
	else
		return FALSE;
}
#endif

void CLDSPart::ResetRelativeObjs()
{
	for(RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		CLDSPart *pMirPart=BelongModel()->FromPartHandle(pRelaObj->hObj);
		if(pMirPart==NULL)	//ɾ�������õĹ�������
			relativeObjs.DeleteCursor();
	}
	relativeObjs.Clean();
}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
int CLDSPart::GetLsCount(short d)
{
	CLDSPart* pMotherPart=MotherPart();
	if(pMotherPart!=this)
		return pMotherPart->GetLsCount(d);
	if(GetLsRefList()==NULL)
		return 0;
	else
		return GetLsRefList()->GetLsCount(d);
}
CLsRef* CLDSPart::AppendLsRef(CLsRef &ls_ref,BOOL bNearLsCheck/*=TRUE*/)
{
	//if(bNearLsCheck)	//��鸽����˨
	//{
	//	CLsRef *pLsRef=pLsRefList->FindLsByPos(ls_ref->ucs.origin,ls_ref->get_norm(),ls_ref->get_d());
	//	if(pLsRef&&(*pLsRef)->get_d()==ls_ref->get_d())
	//		return pLsRef;
	//}
	//SetModified();
	//return pLsRefList->append(ls_ref);
	return NULL;
}
BOOL CLDSPart::DeleteLsRef(THANDLE hBolt)
{
	if(GetLsRefList()==NULL)
		return FALSE;
	BOOL blDeleted=GetLsRefList()->DeleteNode(hBolt);
	if(blDeleted)
		SetModified();
	return blDeleted;
}
BOOL CLDSPart::IsHasFootNail()
{
	if(GetLsRefList()==NULL)
		return FALSE;
	if(GetClassTypeId()==CLS_LINESLOT&&((CLDSLineSlot*)this)->IsBaseSlot())
		return FALSE;
	else if(GetClassTypeId()==CLS_LINETUBE)
		return ((CLDSLineTube*)this)->GetFootNailCount()>0;
	for(CLsRef* pLsRef=GetLsRefList()->GetFirst();pLsRef;pLsRef=GetLsRefList()->GetNext())
	{	//�жϹ����Ƿ��нŶ���Ӧ����get_oldL()��ֵ��old_L������¼ԭ��˨����Ч���� wxc-2019.6.13
		if (!pLsRef->GetLsPtr()->IsFootNail())
			continue;
		//�������нŶ��������׽Ŷ�����Ҫ���ǲ�������Ļ��������ģ���
		//������������������Ŷ����޳�б�Ĵ��������ʱʶ��Ϊ�����Ŷ��� wjh-2019.8.14
		if(this->chLayer2nd=='Z'||pLsRef->GetLsPtr()->get_oldL()<EPS)
			return TRUE;
	}
	return FALSE;
}
//���ݹ�������˨�׵��Ⱥ�˳������˨���->��������ʶ(��1Ϊ����)ӳ�䣬������˨������ wjh-2016.1.05
long CLDSPart::DispatchHoleIndexId(CHashList<long>* hashHoleIndexIdByBoltHandle)
{
	CLsRefList* pLsRefList=GetLsRefList();
	if(pLsRefList==NULL||hashHoleIndexIdByBoltHandle==NULL)
		return 0;
	long holeId=1;
	for(CLsRef* pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext(),holeId++)
		hashHoleIndexIdByBoltHandle->SetValue((*pLsRef)->handle,holeId);
	return hashHoleIndexIdByBoltHandle->GetNodeNum();
}

BOOL CLDSPart::ResetSonBoltRayNo()
{
#ifdef __DRAWING_CONTEXT_
	RAYNO_RECORD *pRayNo=NULL;
	ATOM_LIST<RAYNO_RECORD>rayNoList;	//���ߺż�¼�б�
	CLsRef *pLsRef;
	CLDSBolt *pBolt;
	CLsRefList *pLsRefList=NULL;
	GEPOINT rayVec;
	if(GetClassTypeId()==CLS_LINEANGLE)
	{	//������˨���ߺ�ʱ����Ҫ���Ƕ̽Ǹ֣��������׵�����˨���������࣬
		//�������¼����������˨�������ߺ�ʱ����32������ wht 15-04-21
		if(((CLDSLineAngle*)this)->pStart==NULL||((CLDSLineAngle*)this)->pEnd==NULL)
			return FALSE;	
		pLsRefList=((CLDSLineAngle*)this)->GetLsRefList();
		rayVec=((CLDSLineAngle*)this)->End()-((CLDSLineAngle*)this)->Start();
		normalize(rayVec);
	}
	//else if(GetClassTypeId()==CLS_PLATE)
	//	pLsRefList=((CLDSPlate*)this)->GetLsRefList();
	else if(GetClassTypeId()==CLS_PARAMPLATE)
	{
		int iParamType=((CLDSParamPlate*)this)->m_iParamType;
		if(iParamType!=TYPE_XEND&&iParamType!=TYPE_UEND&&iParamType!=TYPE_ROLLEND)
			return FALSE;
		pLsRefList=((CLDSParamPlate*)this)->GetLsRefList();
		rayVec=ucs.axis_x;
	}
	if(pLsRefList!=NULL)
	{
		f3dPoint wing_x_vec,wing_y_vec;
		if(GetClassTypeId()==CLS_LINEANGLE)
		{
			wing_x_vec=((CLDSLineAngle*)this)->GetWingVecX();
			wing_y_vec=((CLDSLineAngle*)this)->GetWingVecY();
		}
		for(pLsRef=pLsRefList->GetFirst();pLsRef;pLsRef=pLsRefList->GetNext())
		{
			pBolt=pLsRef->GetLsPtr();
			//if(pBolt->des_base_pos.hPart!=handle)
			//	continue;	//�����Ե�ǰ�Ǹ�Ϊ��׼��λ����˨
			double yCoord=0;
			f3dPoint prjBoltPos;	//��˨�ڽǸ�֫�ϵ�ͶӰλ��
			if(GetClassTypeId()==CLS_LINEANGLE)
			{
				if(fabs(pBolt->ucs.axis_z*((CLDSLineAngle*)this)->get_norm_x_wing())>fabs(pBolt->ucs.axis_z*((CLDSLineAngle*)this)->get_norm_y_wing()))
				{	//X֫�ϵ���˨��Yֵȡ��
					if(Int3dlf(prjBoltPos,pBolt->ucs.origin,pBolt->ucs.axis_z,((CLDSLineAngle*)this)->Start(),((CLDSLineAngle*)this)->get_norm_x_wing())>0)
						yCoord=-ftoi(wing_x_vec*(prjBoltPos-((CLDSLineAngle*)this)->Start()));
				}
				else
				{	//Y֫�ϵ���˨��Yֵȡ��
					if(Int3dlf(prjBoltPos,pBolt->ucs.origin,pBolt->ucs.axis_z,((CLDSLineAngle*)this)->Start(),((CLDSLineAngle*)this)->get_norm_y_wing())>0)
						yCoord=ftoi(wing_y_vec*(prjBoltPos-((CLDSLineAngle*)this)->Start()));
				}
			}
			else if(GetClassTypeId()==CLS_PLATE)
			{
				if(((CLDSPlate*)this)->SpreadLsPos(pBolt,prjBoltPos))
					yCoord=prjBoltPos.y;
			}
			else if(GetClassTypeId()==CLS_PARAMPLATE)
			{
				if(Int3dlf(prjBoltPos,pBolt->ucs.origin,pBolt->ucs.axis_z,ucs.origin,ucs.axis_z)>0)
				{
					coord_trans(prjBoltPos,ucs,FALSE);
					yCoord=prjBoltPos.y;
				}
			}
			if(yCoord!=0)
			{
				for(pRayNo=rayNoList.GetFirst();pRayNo;pRayNo=rayNoList.GetNext())
				{
					if(fabs(pRayNo->yCoord-yCoord)<0.5)	//0.5�Ǹ������������ԭ������ľ������ϵ��
					{
						pBolt->SetRelaPartRayNo(handle,pRayNo->dwRayNo,rayVec);
						break;
					}
				}
				if(pRayNo==NULL)
				{
					pRayNo=rayNoList.append();
					pRayNo->yCoord=yCoord;
					pRayNo->dwRayNo=GetSingleWord(rayNoList.GetNodeNum());
					pBolt->SetRelaPartRayNo(handle,pRayNo->dwRayNo,rayVec);	//��˨֧��ͬʱ�洢������ߺ� wjh 2011.8.11
					//pBolt->dwRayNo=pRayNo->dwRayNo;	//�˴�Ӧ������˨���ߺŸ�ֵ wht 11-07-28
				}
			}
		}
		return TRUE;
	}
	else
#endif
		return FALSE;
}
#endif
bool CLDSPart::ReplaceReferenceHandle(CHandleReplacer& replacer,int idClassType/*=0*/)
{
	CSuperSmartPtr<CLDSPart>pPart=this;
	bool replaced=replacer.VerifyAndReplace(&handle,true);
	if(!pPart->IsLinePart()&&!replacer.SelectPerfectPartHandle(pPart->ucs.origin))
		return false;	//�Ǹ˼������Ҳ��ڹ����߶�����
	replaced=replacer.VerifyAndReplace(&handle,true);
	replaced|=replacer.VerifyAndReplace(&pPart->_hPartWeldParent);
	if(pPart->IsLinePart()&&!replacer.IsCollectiveTransition()&&(idClassType==0||idClassType==CLS_NODE))
	{
		if(!replacer.IsExcludeReplaceObjPointer())
		{
			int push=BelongModel()->PushNodeStack();
			if(pPart.LinePartPointer()->pStart&&pPart.LinePartPointer()->pStart->handle==replacer.hOld)
			{
				replaced=true;
				pPart.LinePartPointer()->pStart=BelongModel()->FromNodeHandle(replacer.hNew);
			}
			if(pPart.LinePartPointer()->pEnd&&pPart.LinePartPointer()->pEnd->handle==replacer.hOld)
			{
				replaced=true;
				pPart.LinePartPointer()->pEnd=BelongModel()->FromNodeHandle(replacer.hNew);
			}
			BelongModel()->PopNodeStack(push);
		}
	}
	if(idClassType==0||idClassType!=CLS_NODE)
	{
		for(RELATIVE_OBJECT *pRelaObj=pPart->relativeObjs.GetFirst();pRelaObj;pRelaObj=pPart->relativeObjs.GetNext())
			replaced|=replacer.VerifyAndReplace(&pRelaObj->hObj);
	}
	if(idClassType==0||idClassType!=CLS_NODE)
	{
		if(pPart->IsLinePart())
		{
			replacer.SelectPerfectPartHandle(pPart.LinePartPointer()->Start());
			replaced|=replacer.VerifyAndReplace(&pPart.LinePartPointer()->desStartOdd.m_hRefPart1);
			replaced|=replacer.VerifyAndReplace(&pPart.LinePartPointer()->desStartOdd.m_hRefPart2);
			replacer.SelectPerfectPartHandle(pPart.LinePartPointer()->End());
			replaced|=replacer.VerifyAndReplace(&pPart.LinePartPointer()->desEndOdd.m_hRefPart1);
			replaced|=replacer.VerifyAndReplace(&pPart.LinePartPointer()->desEndOdd.m_hRefPart2);
		}
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if(pPart->GetClassTypeId()==CLS_LINEANGLE||pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		if(idClassType==0||idClassType!=CLS_NODE)
		{
			replaced|=replacer.VerifyAndReplace(&pPart.LineAnglePointer()->des_norm_x.hViceJg);
			replaced|=replacer.VerifyAndReplace(&pPart.LineAnglePointer()->des_norm_y.hViceJg);
			if(pPart.LineAnglePointer()->des_norm_x.bSpecific)
			{
				replaced=true;
				pPart.LineAnglePointer()->des_norm_x.spec_norm.ReplaceReferenceHandle(replacer);
			}
			if(pPart.LineAnglePointer()->des_norm_y.bSpecific)
			{
				replaced=true;
				pPart.LineAnglePointer()->des_norm_y.spec_norm.ReplaceReferenceHandle(replacer);
			}
			replaced|=replacer.VerifyAndReplace(&pPart.LineAnglePointer()->desStartOdd.m_hRefPart1);
			replaced|=replacer.VerifyAndReplace(&pPart.LineAnglePointer()->desStartOdd.m_hRefPart2);
			replaced|=replacer.VerifyAndReplace(&pPart.LineAnglePointer()->desEndOdd.m_hRefPart1);
			replaced|=replacer.VerifyAndReplace(&pPart.LineAnglePointer()->desEndOdd.m_hRefPart2);
			replaced|=pPart.LineAnglePointer()->desStartPos.ReplaceReferenceHandle(replacer,pPart.LineAnglePointer()->Start());
			replaced|=pPart.LineAnglePointer()->desStartPos.datumPoint.ReplaceReferenceHandle(replacer,pPart.LineAnglePointer()->Start(),idClassType);
			replaced|=pPart.LineAnglePointer()->desEndPos.ReplaceReferenceHandle(replacer,pPart.LineAnglePointer()->End());
			replaced|=pPart.LineAnglePointer()->desEndPos.datumPoint.ReplaceReferenceHandle(replacer,pPart.LineAnglePointer()->End(),idClassType);
			if(replacer.pTag && replacer.IsReverseWingXAndWingY())
			{	//�������㶨λ��ʽ
				ANGLEREPLACER_TAG::ANGLE_STATE* pJgState=replacer.pTag->hashAngles.GetValue(pPart->handle);
				if(pJgState&&pJgState->replaceStartDependRod)
				{
					if(pPart.LineAnglePointer()->desStartPos.IsInDatumJgWingX())
						pPart.LineAnglePointer()->desStartPos.spatialOperationStyle+=1;
					else if(pPart.LineAnglePointer()->desStartPos.IsInDatumJgWingY())
						pPart.LineAnglePointer()->desStartPos.spatialOperationStyle-=1;
				}
				if(pJgState&&pJgState->replaceEndDependRod)
				{
					if(pPart.LineAnglePointer()->desEndPos.IsInDatumJgWingX())
						pPart.LineAnglePointer()->desEndPos.spatialOperationStyle+=1;
					else if(pPart.LineAnglePointer()->desEndPos.IsInDatumJgWingY())
						pPart.LineAnglePointer()->desEndPos.spatialOperationStyle-=1;
				}
			}
			if(IsLinePart())
			{	//���е�Ϊʰȡ���жϸ��Ӷ��������Σ������ڴ����ϽǸ�ʱ�ᵼ�¸��Ǹ��д��� wjh-2014.5.28
				CLDSLinePart* pRod=(CLDSLinePart*)this;
				GEPOINT middle=0.5*(pRod->Start()+pRod->End());
				replacer.SelectPerfectPartHandle(middle);
			}
			replaced|=replacer.VerifyAndReplace(&pPart.LineAnglePointer()->group_father_jg_h);
			if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			{
				for(int i=0;i<4;i++)
				{
					replaced|=replacer.VerifyAndReplace(&pPart.LineAnglePointer()->son_jg_h[i],handle==replacer.hOld);
				}
			}
		}
	}
	else if(pPart->GetClassTypeId()==CLS_LINETUBE)
	{
		if(idClassType==0||idClassType!=CLS_NODE)
		{
			replaced|=replacer.VerifyAndReplace(&pPart.LineTubePointer()->hWorkNormRefPart);
			replacer.hNew=replacer.SelectPerfectPartHandle(pPart.LineTubePointer()->Start());
			replaced|=replacer.VerifyAndReplace(&pPart.LineTubePointer()->m_tJointStart.hLinkObj);
			replaced|=replacer.VerifyAndReplace(&pPart.LineTubePointer()->m_tJointStart.hViceLinkObj);
			replacer.hNew=replacer.SelectPerfectPartHandle(pPart.LineTubePointer()->End());
			replaced|=replacer.VerifyAndReplace(&pPart.LineTubePointer()->m_tJointEnd.hLinkObj);
			replaced|=replacer.VerifyAndReplace(&pPart.LineTubePointer()->m_tJointEnd.hViceLinkObj);
			replaced|=pPart.LineTubePointer()->desStartPos.ReplaceReferenceHandle(replacer,pPart.LineTubePointer()->Start());
			replaced|=pPart.LineTubePointer()->desEndPos.ReplaceReferenceHandle(replacer,pPart.LineTubePointer()->End());
			replaced|=replacer.VerifyAndReplace(&pPart.LineTubePointer()->hTransTube);
		}
	}
	else if(pPart->GetClassTypeId()==CLS_PLATE)
	{
		replaced|=pPart.PlatePointer()->designInfo.ReplaceReferenceHandle(replacer,idClassType);
		for(PROFILE_VERTEX* pVertex=pPart.PlatePointer()->vertex_list.GetFirst();pVertex;
			pVertex=pPart.PlatePointer()->vertex_list.GetNext())
		{
			if( pVertex->m_cPosDesignType>=1&&pVertex->m_cPosDesignType<=4&&
				pVertex->m_hRod==replacer.hOld&&replacer.IsReverseStartAndEnd())
			{	//����֫������߲� wjh-2016.5.31
				if(pVertex->m_cPosDesignType%2==1)
					pVertex->m_cPosDesignType+=1;
				else
					pVertex->m_cPosDesignType-=1;
			}
			if(pVertex->m_cPosDesignType>0)
				replaced|=replacer.VerifyAndReplace(&pVertex->m_hRod);
		}
	}
	else if(pPart->GetClassTypeId()==CLS_SPHERE)
	{
		if(idClassType==0||idClassType==CLS_NODE)
			replaced|=replacer.VerifyAndReplace(&pPart.SpherePointer()->hDatumNode);
	}
#endif
	if(replaced)
		SetModified();
	return replaced;
}
long CLDSPart::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	CSuperSmartPtr<CLDSPart>pPart = this;
	long nInitCount = pListHandlers->Count;
	pListHandlers->Append(HANDLE_ADDR(&handle));
	pListHandlers->Append(HANDLE_ADDR(&_hPartWeldParent));
	//UINT xarrId[2] = { 0 };
	//CUniqueHandleAddrAdder uniaddr(pListHandlers, xarrId, 8);
	for (RELATIVE_OBJECT *pRelaObj = pPart->relativeObjs.GetFirst(); pRelaObj; pRelaObj = pPart->relativeObjs.GetNext())
		pListHandlers->Append(HANDLE_ADDR(&pRelaObj->hObj));
	if (pPart->IsLinePart())
	{
		pListHandlers->Append(HANDLE_ADDR(&pPart.pRod->desStartOdd.m_hRefPart1));
		pListHandlers->Append(HANDLE_ADDR(&pPart.pRod->desStartOdd.m_hRefPart2));
		pListHandlers->Append(HANDLE_ADDR(&pPart.pRod->desEndOdd.m_hRefPart1));
		pListHandlers->Append(HANDLE_ADDR(&pPart.pRod->desEndOdd.m_hRefPart2));
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if (pPart->GetClassTypeId() == CLS_LINEANGLE || pPart->GetClassTypeId() == CLS_GROUPLINEANGLE)
	{
		pListHandlers->Append(HANDLE_ADDR(&pPart.pAngle->des_norm_x.hViceJg));
		pListHandlers->Append(HANDLE_ADDR(&pPart.pAngle->des_norm_y.hViceJg));
		if (pPart.LineAnglePointer()->des_norm_x.bSpecific)
			pPart.pAngle->des_norm_x.spec_norm.StatObjHandleReference(pListHandlers);
		if (pPart.LineAnglePointer()->des_norm_y.bSpecific)
			pPart.pAngle->des_norm_y.spec_norm.StatObjHandleReference(pListHandlers);
		pPart.pAngle->desStartPos.StatObjHandleReference(pListHandlers);
		//pPart.pAngle->desStartPos.datumPoint.StatObjHandleReference(replacer, pPart.LineAnglePointer()->Start(), idClassType);
		pPart.pAngle->desEndPos.StatObjHandleReference(pListHandlers);
		//pPart.pAngle->desEndPos.datumPoint.StatObjHandleReference(replacer, pPart.LineAnglePointer()->End(), idClassType);
		pListHandlers->Append(HANDLE_ADDR(&pPart.pAngle->group_father_jg_h));
		if (pPart->GetClassTypeId() == CLS_GROUPLINEANGLE)
		{
			for (int i = 0; i < 4; i++)
				pListHandlers->Append(HANDLE_ADDR(&pPart.pAngle->son_jg_h[i]));
		}
	}
	else if (pPart->GetClassTypeId() == CLS_LINETUBE)
	{
		pListHandlers->Append(HANDLE_ADDR(&pPart.pTube->hWorkNormRefPart));
		pListHandlers->Append(HANDLE_ADDR(&pPart.pTube->m_tJointStart.hLinkObj));
		pListHandlers->Append(HANDLE_ADDR(&pPart.pTube->m_tJointStart.hViceLinkObj));
		pListHandlers->Append(HANDLE_ADDR(&pPart.pTube->m_tJointEnd.hLinkObj));
		pListHandlers->Append(HANDLE_ADDR(&pPart.pTube->m_tJointEnd.hViceLinkObj));
		pListHandlers->Append(HANDLE_ADDR(&pPart.pTube->hTransTube));
		pPart.pTube->desStartPos.StatObjHandleReference(pListHandlers);
		pPart.pTube->desEndPos.StatObjHandleReference(pListHandlers);
	}
	else if (pPart->GetClassTypeId() == CLS_PLATE)
	{
		pPart.pPlate->designInfo.StatObjHandleReference(pListHandlers);
		for (PROFILE_VERTEX* pVertex = pPart.PlatePointer()->vertex_list.GetFirst(); pVertex;
			pVertex = pPart.PlatePointer()->vertex_list.GetNext())
		{
			if (pVertex->m_cPosDesignType > 0)
				pListHandlers->Append(HANDLE_ADDR(&pVertex->m_hRod));
		}
	}
	else if (pPart->GetClassTypeId() == CLS_SPHERE)
	{
		pListHandlers->Append(HANDLE_ADDR(&pPart.pSphere->hDatumNode));
	}
#endif
	return pListHandlers->Count- nInitCount;
}

void CDesignThickPara::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	if(pModel==NULL)
		return;
	//
}
void CLDSPart::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	if(pModel==NULL)
	{
		if(m_pModel!=NULL)
			pModel=m_pModel;
	}
	if(pModel==NULL)
		return;
	m_pModel->ValidateObjRefH(&_hPartWeldParent);
}
void CLDSBolt::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	if(pModel==NULL)
	{
		if(m_pModel!=NULL)
			pModel=m_pModel;
	}
	if(pModel==NULL)
		return;
	des_work_norm.CleanInvalidObjRef(pModel);
#ifdef __DRAWING_CONTEXT_
	for(int i=0;i<4;i++)
		m_pModel->ValidateObjRefH(&relaRayArr[i].hRelaPart);
#endif
}
void CMultiOffsetPos::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	if(pModel==NULL)
		return;
	pModel->ValidateObjRefH(&datum_jg_h);
	pModel->ValidateObjRefH(&huoqu_jg_h);
	pModel->ValidateObjRefH(&hDatumLine);
	if(spatialOperationStyle==16||spatialOperationStyle==17)
	{
		datumLineStart.CleanInvalidObjRef(pModel);
		datumLineEnd.CleanInvalidObjRef(pModel);
	}
	datumPoint.CleanInvalidObjRef(pModel);
	//hDatumLine  //��׼�ߣ�����spatiaOperationStyle=16,17ʱ��Ч��hDatumLine=0ʱ��ʾ�����׼��
}
void CTubeEndPosPara::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	if(pModel==NULL)
		return;
	pModel->ValidateObjRefH(&hDatumEndPart);
	pModel->ValidateObjRefH(&hDatumPart);
	pModel->ValidateObjRefH(&hDatumStartPart);
}
void CLDSNode::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	if(pModel==NULL)
	{
		if(m_pModel!=NULL)
			pModel=m_pModel;
	}
	if(pModel==NULL)
		return;
	m_pModel->ValidateObjRefH(&_hFatherPart);
	m_pModel->ValidateObjRefH(&m_hArcLift);
	for(int i=0;i<4;i++)
	{
		if(arrRelationNode[i]>0x20)
			m_pModel->ValidateObjRefH(&arrRelationNode[i]);
		if(i<2&&arrRelationPole[i]>0x20)
			m_pModel->ValidateObjRefH(&arrRelationPole[i]);
	}
}
void CLDSParamPlate::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	if(pModel==NULL)
	{
		if(m_pModel!=NULL)
			pModel=m_pModel;
	}
	if(pModel==NULL)
		return;
	if(GetClassTypeId()!=CLS_PARAMPLATE)
		return;
	THANDLE handle;
	if(m_iParamType==TYPE_WATERSHIELDEDPLATE)
	{
		if(GetDesignItemValue('B',&handle))        //��׼�ڵ��� 
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('C',&handle))
			pModel->ValidateObjRefH(&handle);       //��׼�ֹܾ��
	}
	else if (m_iParamType==TYPE_CIRRIBPLATE||m_iParamType==TYPE_TUBERIBPLATE)//�����߰�, �ֹ��߰�
	{
		if(GetDesignItemValue(KEY2C('C'),&handle)) //C:��λ��׼�ֹ�(����)���(��λ����)
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('E'),&handle)) //E:��һ�������׼�ְ���
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('F'),&handle)) //F:�ڶ��������׼�ְ������򺸽ӹ����ֹܾ��(�����ֹ���ָ�뻷����к���ĸֹܣ���������Ĳ����˨���Ӹֹܣ���˲����ܳ�������)
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('I',&handle))		   //I:��һ�����ӷ���ֹܾ��(ָ���д˸ֹܷ����������ͼ���������˨���ӻ�����������)
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('J',&handle))		   //J:�ڶ������ӷ���ֹܾ��(ָ���д˸ֹܷ����������ͼ���������˨���ӻ�����������)
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('P',&handle))		   //P:��λ��׼�ڵ���
			pModel->ValidateObjRefH(&handle);
	}
	else if (m_iParamType==TYPE_CIRCOVERPLATE) //������
	{
		if(GetDesignItemValue(KEY2C('E'),&handle))	//��λ��׼�ڵ�
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('F'),&handle))	//��һ�鶨λ��׼�ְ�
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('G'),&handle))	//�ڶ��鶨λ��׼�ְ�
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('C'),&handle))	//�ڶ��鶨λ��׼�ְ�
			pModel->ValidateObjRefH(&handle);
	}
	else if(m_iParamType==TYPE_CIRRIBPLATE)   //������
	{
		if(GetDesignItemValue(KEY2C('A'),&handle))	//��λ��׼�ڵ�
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('B'),&handle))	//��һ�鶨λ��׼�ְ�
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('C'),&handle))	//�ڶ��鶨λ��׼�ְ�
			pModel->ValidateObjRefH(&handle);
	}
	else if(m_iParamType==TYPE_SHOEPLATE)      //����ѥ��
	{
		if(GetDesignItemValue(KEY2C("LX"),&handle))	//���ӽǸ�
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C("LH"),&handle))	//���ӽǸ�
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('C',&handle))	        //���ӵװ�
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('D',&handle))	        //�����Ǹ�
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('G',&handle))         //����ѥ��
			pModel->ValidateObjRefH(&handle);
	}
	else if (m_iParamType==TYPE_TUBERIBPLATE)//�ֹ��߰�
	{
		if(GetDesignItemValue('S',&handle))		    //���ᶨ���׼�ֹ�
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('P',&handle))	        //��׼�ڵ�
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('B',&handle))		    //��һ�鶨λ��׼�ְ�
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('C',&handle))         //�ڶ��鶨λ��׼�ְ�
			pModel->ValidateObjRefH(&handle);
	}
	else if (m_iParamType==TYPE_BASEPLATE)//���ŵװ�
	{
		if(GetDesignItemValue('A',&handle))	//��λ��׼�ڵ�
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('L'),&handle)) // hBaseAngle ��׼�Ǹ�
			pModel->ValidateObjRefH(&handle);
	}
	else if(m_iParamType==TYPE_ELBOWPLATE)
	{
		if(GetDesignItemValue('L',&handle))
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('K',&handle))
			pModel->ValidateObjRefH(&handle);
	}
	else if(m_iParamType==TYPE_RIBPLATE)//��ͨ���߰�
	{
		if(GetDesignItemValue('A',&handle))	    //��׼����
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('B',&handle))		//��׼�ڵ�
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('C',&handle))		//��������1
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('D',&handle))		//��������2
			pModel->ValidateObjRefH(&handle);
	}
	workNorm.CleanInvalidObjRef(pModel);
}
void CLDSPlane::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	if(pModel==NULL)
	{
		if(m_pModel!=NULL)
			pModel=m_pModel;
	}
	if(pModel==NULL)
		return;
	for (int i=0;i<3;i++)
		pModel->ValidateObjRefH(&hPointArr[i].handle,true);
}

void CLDSArcLift::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	if(pModel==NULL)
	{
		if(m_pModel!=NULL)
			pModel=m_pModel;
	}
	if(pModel==NULL)
		return;
	//pModel->ValidateObjRefH();û�ҵ�
}
IMPLEMENT_GET(CLDSPart,m_bLiveNeedSupplyPart){
	CLDSPart* pMotherPart=MotherPart();
	if(pMotherPart==this)
		return _bLiveNeedSupplyPart;
	else
		return pMotherPart->_bLiveNeedSupplyPart;
}
IMPLEMENT_SET(CLDSPart,m_bLiveNeedSupplyPart){
	CLDSPart* pMotherPart=MotherPart();
	_bLiveNeedSupplyPart=value;
	SetModified(TRUE,FALSE);
	if(pMotherPart!=this)
		pMotherPart->m_bLiveNeedSupplyPart=value;
}
IMPLEMENT_GET(CLDSPart,m_hPartWeldParent){
	return _hPartWeldParent;
}
IMPLEMENT_SET(CLDSPart,m_hPartWeldParent){
	if(_hPartWeldParent!=value)
		is_data_modified=true;
	_hPartWeldParent=value;
}
bool CLDSPart::IsLaboratoryFuncEnabled()
{
	return INTERNAL_TEST_VERSION||AgentDogType()==3;
}

const char* CLDSPart::GetPartNo()
{
	if(!_bSyncShadowPartNo)
		return sPartNo;
	CLDSPart* pMotherPart=MotherPart();
	if(pMotherPart==this)
		return sPartNo;
	else //if(pMotherPart->iSeg==iSeg)
		return pMotherPart->GetPartNo();
	/*else
	{
		CXhChar16 sSerialNo;
		if(ParsePartNo(pMotherPart->GetPartNo(),&&segi,sSerialNo))
		{
			sPartNo.Copy(iSeg.ToString());
			sPartNo.Append(sSerialNo);
		}
	}*/
}
void CLDSPart::SetPartNo(const char *part_no,bool bIncStdPart/*=false*/)
{
	if(!_bSyncShadowPartNo)
	{
		sPartNo.Copy(part_no);
		SetModified(TRUE,FALSE);
		return;
	}
	CLDSPart* pMotherPart=MotherPart();
	sPartNo.Copy(part_no);
	if(pMotherPart!=this)//if(pMotherPart->iSeg==iSeg)
		pMotherPart->SetPartNo(part_no,bIncStdPart);
	SetModified();
}
bool CLDSPart::ModifyPartNoSegI(SEGI newSegI)	//���¹�������еĶκŲ���
{
	BOOL bStdPart=FALSE;
	if(GetClassTypeId()==CLS_PARAMPLATE)
		bStdPart=((CLDSParamPlate*)this)->m_bStdPart;
	CXhChar16 sSegNo = _iSeg.ToString();
	iSeg=newSegI;
	if(bStdPart||newSegI.iSeg==0)	//���ܸ��ݶκŸ��±�׼�����ļ��� wht 15-07-13
		return false;	//�¶κ�Ϊ0ʱ����ԭ�м��� wht 16-04-07
	if(strstr(sPartNo,sSegNo)==(char*)sPartNo)
	{
		CXhChar16 serialStr("%s",&sPartNo[sSegNo.GetLength()]);
		sPartNo.Printf("%s%s",(char*)newSegI.ToString(),(char*)serialStr);
		return true;
	}
	return false;
}

//ancestor=trueʱ����ʼ�漶���������򷵻�ֱ�Ӹ���������Գ�ʱ��
CLDSPart* CLDSPart::MotherPart(bool ancestor/*=true*/)
{
	CLDSObject *pMotherObj=MotherObject(ancestor);
	if(pMotherObj==NULL||pMotherObj->GetClassTypeId()!=m_nClassTypeId)
#ifdef AFX_TARG_ENU_ENGLISH
		throw "The part type of projective mother part is wrong��";
#else 
		throw "Ӱ�乹��ĸ���������ʹ���";
#endif
	else 
		return (CLDSPart*)pMotherObj;
}

void CLDSPart::Offset(const double* offset)
{
	ucs.origin+=GEPOINT(offset);
	SetModified();
}
bool CLDSPart::IsBelongTo(int iClassTypeId)const
{
	if(iClassTypeId==m_nClassTypeId||(IsPart()&&iClassTypeId==CLS_PART))
		return true;
	else
		return false;
}
CLDSPart* CLDSPart::NewCopy(bool bByShadowMode/*=false*/)
{
	if(bByShadowMode)
	{
		CParaCS::PARAMETRIC_INFO param;
		param.m_cDefStyle=CParaCS::CS_MIRTRANS;
		CParaCS *pAssembleCS=BelongModel()->AppendAcs();
		pAssembleCS->SetParamInfo(param);
		CLDSPart* pShadowPart=BelongModel()->AppendPart(m_nClassTypeId);
		pShadowPart->SetLayer(layer_tag);
		//if(hRelationPart!=0&&pObj->GetClassTypeId()==CLS_BOLT)
		//	pShadowPart->SetShadowPara(pPart,hRelationPart,CLDSObject::SHADOW_ISOLATED_RELA_LOCATION);
		//else
			pShadowPart->SetShadowPara(this,pAssembleCS->id);
		return pShadowPart;
	}
	else
		return NULL;
}
bool CLDSPart::ShadowDesign()
{
	if(!IsShadowObject())
		return false;
	SetModified();
	CLDSPart *pMotherPart=MotherPart(false);
	if(m_iShadowObjType==CLDSObject::SHADOW_ISOLATED)
	{
		CParaCS *pAcs=m_pModel->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
		if(pAcs==NULL)
		{
			if(pMotherPart)	//���������ȴ�ͬλ��ͬ�ȵ����ӹ������� wjh-2015.7.23
				ucs=pMotherPart->ucs;
			return false;
		}
		pAcs->SetModifiedState();
		if(pAcs->GetDefStyle()==CParaCS::CS_PARAM)
			pAcs->UpdateACS(BelongModel());
		else
			pAcs->UpdateACS(pMotherPart);
		ucs =UCS_STRU(pAcs->GetACS());
		return true;
	}
	else if(m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
	{
		ucs=pMotherPart->ucs;
		CBlockParameter blockPara;
		if(!GetBlockParameter(blockPara))
			return false;
		UCS_STRU cs=blockPara.m_pBlock->TransToACS(blockPara.m_pBlockRef->GetACS());
		coord_trans(ucs.origin,cs,TRUE,TRUE);
		vector_trans(ucs.axis_x,cs,TRUE,TRUE);
		vector_trans(ucs.axis_y,cs,TRUE,TRUE);
		vector_trans(ucs.axis_z,cs,TRUE,TRUE);
	}
	return false;
}

BOOL CLDSLinePart::bDisplayLineVecMark=TRUE;
//////////////////////////////////////////////////////////////
// CLDSCable ��
CLDSCable::CLDSCable()
{
}
BOOL CLDSCable::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	return TRUE;
}
//////////////////////////////////////////////////////////////
// CLDSDelegateRod ��
CLDSDelegateRod::CLDSDelegateRod()
{
	//strcpy(m_sClassName,"");
	m_nClassTypeId=CLS_DELEGATEROD;
}
//////////////////////////////////////////////////////////////
// CLDSLinePart ��
bool SIZESPEC::IsEqual(const SIZESPEC& size)
{
	if(size.idClassType!=idClassType)
		return false;
	if(size.idClassType==CLS_LINEANGLE&&size.cSubClassType!=cSubClassType)
		return false;
	if((size.idClassType==CLS_CABLE||size.idClassType==CLS_LINESLOT)&&
		stricmp(size.spec,spec)!=0)
		return false;
	else if(size.wide!=wide||size.thick!=thick)
		return false;
	return true;
}
CLDSLinePart::CLDSLinePart()
{
	m_pOriginalPart=NULL;
	m_nClassTypeId = CLS_LINEPART;
	//strcpy(m_sClassName,"CLDSLinePart");
	pStart = NULL;
	pEnd = NULL;
	if(library)
		desStartOdd.m_fCollideDist=desEndOdd.m_fCollideDist=library->GetVertexDist();
	else
		desStartOdd.m_fCollideDist=desEndOdd.m_fCollideDist=5;
	m_bLockEnd=m_bLockStart=m_bLockStartPos=m_bLockEndPos=FALSE;
	dwPermission=dwStartPermission=dwEndPermission=0x80000000;		//PERM_TOWER_ADMIN��������Ա���;
	_material=CSteelMatLibrary::Q235BriefMark();//'S';
	crMaterial=GetColor(CLS_LINEPART,3,'S');	//������ɫ
	size.idClassType=CLS_LINEANGLE;
	size.cSubClassType='L';
	size.wide=80;
	size.thick=7;
	size.height=0;
	size.wideUpSide=0;
	m_hArcLift=0;
	m_bSpecAngleWingNorm=false;
	des_wing_norm.hRefPoleArr[0]=des_wing_norm.hRefPoleArr[1]=0;
	des_wing_norm.wLayoutStyle=0;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	start_oddment=end_oddment=0;
	pLsRefList = new CLsRefList();
	pLsRefList->SetModifiedFlagAddress(m_pModified);
	huoqu_handle=0;
	bHuoQuStart=bHuoQuEnd=FALSE;
#endif
#if defined(__TSA_)||defined(__LDS_)||defined(__TSA_FILE_)||defined(__LDS_FILE_)
	m_fPreTension=0;
	m_bAutoCalHoleOutCoef=TRUE;
	m_bNeedSelMat=FALSE;
	m_bNeedSelSize= TRUE;
	iInitialSizeIndex=iSizeIndex=-1;
	pCtrlElem=NULL;
	bear_load_coef=0.025;
	hole_out=2.0;
	start_force_type=end_force_type=ECCENTRIC_FORCE;
	start_joint_type = JOINT_AUTOJUSTIFY;
	end_joint_type = JOINT_AUTOJUSTIFY;
	sel_mat_method = AUX_AUTODESIGN;
	//�˴�������memset,��Ϊ�������麯��,�����л������麯��ָ��
	//memset(&CalLenCoef,0,sizeof(POLE_CALLEN_COEF));
	CalLenCoef.minR.coef=CalLenCoef.paraR.coef=1.0;
	CalLenCoef.hHorizPole=CalLenCoef.hRefPole=0;
	CalLenCoef.iTypeNo=CalLenCoef.iSelRFlag=0;
	m_uStatMatNo = 0;
	m_fWeightRaiseCoef=1.2;
	m_iElemType=0;	//�Զ��жϵ�Ԫ����
	strcpy(connectStart.grade,"6.8");
	strcpy(connectEnd.grade,"6.8");
	connectMid.d = connectStart.d = connectEnd.d = 20;
	connectMid.rows = connectStart.rows = connectEnd.rows = 1;
	connectMid.uiLegacyN = connectStart.uiLegacyN = connectEnd.uiLegacyN = 2;
	memset(&userdef,0,sizeof(USERDEF_DATA));
	m_cbRodFlag=0;
#endif
}
CLDSLinePart::ENDBOLTS::ENDBOLTS()
{
	memset(this,0,sizeof(ENDBOLTS));
}
CLDSLinePart::ENDBOLTS::ENDBOLTS(WORD d,WORD N,const char* grade)
{
	Init(d,N,grade);
}
void CLDSLinePart::ENDBOLTS::Init(WORD d,WORD N,const char* gradestr)
{
	memset(this,0,sizeof(ENDBOLTS));
	if(d==16)
		n16=(BYTE)N;
	else if(d==20)
		n20=(BYTE)N;
	else if(d==24)
		n24=(BYTE)N;
	StrCopy(grade,gradestr,5);
	//Avf=Pi*d*d*0.25;
	//Ahf=;		//��˨��Ч�м����,�ױڳ�ѹ���
}
IMPLEMENT_GET(CLDSLinePart::ENDBOLTS,szBoltStr)
{
	CXhChar50 boltstr;
	if(n16>0)
		boltstr.Printf("M16*%d",n16);
	if(n20>0)
		boltstr.Append(CXhChar16("M20*%d",n20),'+');
	if(n24>0)
		boltstr.Append(CXhChar16("M24*%d",n24),'+');
	if(nX>0)
		boltstr.Append(CXhChar16("MX*%d",nX),'+');
	return boltstr;
}
bool CLDSLinePart::IntelliUpdateConnBoltsBySummBolts()	//������˨���������ܸ���(��֫)������
{
	WORD* pwnSummBoltCount=((WORD*)&connectStart.uiLegacyN)+1;
	if(*pwnSummBoltCount!=0)
	{
		WORD odd=0;
		if(this->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)this;
			if(pGroupAngle->wSectStyle<2)	//�Խǻ�T����ϽǸ�
			{
				odd=(*pwnSummBoltCount)%2;
				this->connectStart.wnConnBoltN=(*pwnSummBoltCount)/2+odd;
			}
			else
			{
				odd=(*pwnSummBoltCount)%4;
				this->connectStart.wnConnBoltN=(*pwnSummBoltCount)/4+odd;
			}
			if(pGroupAngle->wSectStyle!=1)
			{	//T����ϽǸ�һ����Ϊ�ǵ�֫���ӣ�������Ϊ��˫֫����
				odd=connectStart.wnConnBoltN%2;
				this->connectStart.wnConnBoltN=connectStart.wnConnBoltN/2+odd;
			}
		}
		else if(GetClassTypeId()==CLS_LINEANGLE&&connectStart.m_iConnectWing==0)
		{
			odd=(*pwnSummBoltCount)%2;
			this->connectStart.wnConnBoltN=(*pwnSummBoltCount)/2+odd;
		}
		else
			this->connectStart.wnConnBoltN=(*pwnSummBoltCount);
	}
	pwnSummBoltCount=((WORD*)&connectEnd.uiLegacyN)+1;
	if(*pwnSummBoltCount!=0)
	{
		WORD odd=0;
		if(this->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)this;
			if(pGroupAngle->wSectStyle<2)	//�Խǻ�T����ϽǸ�
			{
				odd=(*pwnSummBoltCount)%2;
				this->connectEnd.wnConnBoltN=(*pwnSummBoltCount)/2+odd;
			}
			else
			{
				odd=(*pwnSummBoltCount)%4;
				this->connectEnd.wnConnBoltN=(*pwnSummBoltCount)/4+odd;
			}
			if(pGroupAngle->wSectStyle!=1)
			{	//T����ϽǸ�һ����Ϊ�ǵ�֫���ӣ�������Ϊ��˫֫����
				odd=connectEnd.wnConnBoltN%2;
				this->connectEnd.wnConnBoltN=connectEnd.wnConnBoltN/2+odd;
			}
		}
		else if(GetClassTypeId()==CLS_LINEANGLE&&connectEnd.m_iConnectWing==0)
		{
			odd=(*pwnSummBoltCount)%2;
			this->connectEnd.wnConnBoltN=(*pwnSummBoltCount)/2+odd;
		}
		else
			this->connectEnd.wnConnBoltN=(*pwnSummBoltCount);
	}
	return true;
}
WORD CLDSLinePart::BoltsNumOn1Wing(BOOL bStart)
{
	CConnectInfo *pConnect=&connectStart;
	if(!bStart)
		pConnect=&connectEnd;
	return pConnect->wnConnBoltN;
}
WORD CLDSLinePart::SetBoltsNumOn1Wing(int n,BOOL bStart)
{
	CConnectInfo *pConnect=&connectStart;
	if(!bStart)
		pConnect=&connectEnd;
	/*if(GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		if(size_cSubClassType=='X')
			n*=4;
		else //if(size_cSubClassType=='D'||size_cSubClassType=='T')
			n*=2;
	}*/
	pConnect->wnConnBoltN=(WORD)n;
	return n;
}
BOOL CLDSLinePart::IsCfgWordCompatible(CFGWORD filter)	//�ж϶˽ڵ���˼�������ĺ��Ƿ�һ��
{
	CFGWORD basic=filter;
	basic&=UnifiedCfgword();
	if(pStart)
		basic&=pStart->UnifiedCfgword();
	if(pEnd)
		basic&=pEnd->UnifiedCfgword();
	CFGWORD lineword=cfgword;
	lineword&=filter;
	return basic.IsEqual(lineword);
}
bool CLDSLinePart::IsAcceptableTypeId(int type_id)
{
	if(type_id==CLS_LINEPART||type_id==CLS_PART||type_id==CLS_DBOBJECT||type_id==CLS_OBJECT)
		return true;
	else if(type_id==m_nClassTypeId)
		return true;
	else
		return false;
}
CLDSLinePart::~CLDSLinePart()
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if(pLsRefList!=NULL)
	{
		delete pLsRefList;
		pLsRefList=NULL;
	}
#endif
}
bool CLDSLinePart::get_blGroupSonAngle()
{
	if (this->m_nClassTypeId != CLS_LINEANGLE)
		return false;
	return ((CLDSLineAngle*)this)->group_father_jg_h > 0x20;
}
char CLDSLinePart::get_ciLegQuad()
{
	if(m_ciLegQuad>0&&m_ciLegQuad<5)
		return m_ciLegQuad;
	else if(layer_tag[0]=='L')
	{
		char ciQuad=layer_tag[2]-'0';
		if(ciQuad>0&&ciQuad<5)
			return ciQuad;
		GEPOINT xStartPoint=pStart!=NULL?pStart->xPreliftPos:xPosStart;
		GEPOINT xEndPoint  =pEnd!=NULL?pEnd->xPreliftPos:xPosEnd;
		if(xStartPoint.x>=0&&xStartPoint.y>=0&&xEndPoint.x>=0&&xEndPoint.y>=0)
			return 1;
		else if(xStartPoint.x<=0&&xStartPoint.y>=0&&xEndPoint.x<=0&&xEndPoint.y>=0)
			return 2;
		else if(xStartPoint.x>=0&&xStartPoint.y<=0&&xEndPoint.x>=0&&xEndPoint.y<=0)
			return 3;
		else if(xStartPoint.x<=0&&xStartPoint.y<=0&&xEndPoint.x<=0&&xEndPoint.y<=0)
			return 4;
		else
			return 0;
	}
	else
		return 0;
}
bool CLDSLinePart::IsSyncMirPropWith(CLDSDbObject* pObj,const char* propKeyStr)
{	//�˼����ýǸ����͵�Ĭ��ͬ���趨
	if(pObj==NULL||propKeyStr==NULL||!pObj->IsLinePart())
		return false;
	CLDSLinePart* pRod=(CLDSLinePart*)pObj;
	MEMORY_BLOCK cl=mpcl;
	if(cl.IsZero())
		cl=CLDSLineAngle::MPCL;
	MEMORY_BLOCK mir_cl=pRod->mpcl;
	if(mir_cl.IsZero())
		mir_cl=CLDSLineAngle::MPCL;
	PROPLIST_ITEM* pItem=CLDSLineAngle::propMirSync.GetPropItem(propKeyStr);
	if(pItem!=NULL)
		return cl.GetBitState((WORD)pItem->id-1)&&mir_cl.GetBitState((WORD)pItem->id-1);
	else
	{
		Log2File()->Log("%sͬ���Գ�������ȱʧ",propKeyStr);
		return false;
	}
}
bool CLDSLinePart::SyncMirPropTo(CLDSDbObject* pDestObj,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	if(pDestObj==NULL)
		return false;
	CLDSLinePart* pRod=(CLDSLinePart*)pDestObj;
	if(propKeyStr==NULL||strcmp(propKeyStr,"synchronize_scope")==0)
	{
		MEMORY_BLOCK cl=mpcl;
		if(cl.IsZero())
			cl=CLDSLineAngle::MPCL;
		PROPLIST_ITEM* pItem=CLDSLineAngle::propMirSync.GetPropItem("synchronize_scope");
		if(pItem!=NULL && cl.GetBitState((WORD)pItem->id-1))
			pDestObj->mpcl=mpcl;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cMaterial")==0)&&IsSyncMirPropWith(pDestObj,"cMaterial"))
	{	//����
		pRod->cMaterial=cMaterial;
		pRod->cbVirginQualityLevel=cbVirginQualityLevel;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"specification")==0)&&IsSyncMirPropWith(pDestObj,"specification"))
	{	//���
		pRod->size_wide=size_wide;
		pRod->size_thick=size_thick;
		pRod->size_idClassType=size_idClassType;
		pRod->size_cSubClassType=size_cSubClassType;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"iSeg")==0)&&IsSyncMirPropWith(pDestObj,"iSeg"))
	{	//�ֶκ�
		pRod->iSeg=iSeg;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cfgword")==0)&&IsSyncMirPropWith(pDestObj,"cfgword"))
	{	//��ĺ�
		pRod->cfgword=cfgword;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"crMaterial")==0)&&IsSyncMirPropWith(pDestObj,"crMaterial"))
	{	//������ɫ
		pRod->crMaterial=crMaterial;
	}
#ifdef __PART_DESIGN_INC_
	if((propKeyStr==NULL||strcmp(propKeyStr,"DesignInfo")==0))//&&IsSyncMirPropWith(pDestObj,"DesignInfo"))
	{
		pRod->CalLenCoef.iTypeNo=CalLenCoef.iTypeNo;
		pRod->CalLenCoef.minR.coef=CalLenCoef.minR.coef;
		pRod->CalLenCoef.paraR.coef=CalLenCoef.paraR.coef;
		pRod->m_bAutoCalHoleOutCoef=m_bAutoCalHoleOutCoef;
		pRod->hole_out=hole_out;
		pRod->sel_mat_method=sel_mat_method;
		pRod->start_force_type=start_force_type;
		pRod->start_joint_type=start_joint_type;
		pRod->end_force_type=end_force_type;
		pRod->end_joint_type=end_joint_type;
		pRod->m_iElemType=m_iElemType;
		pRod->m_bNeedSelSize=m_bNeedSelSize;
		pRod->m_fWeightRaiseCoef=m_fWeightRaiseCoef;
	}
#endif
	if((propKeyStr==NULL||strcmp(propKeyStr,"linkInfo")==0)&&IsSyncMirPropWith(pDestObj,"linkInfo"))
	{	//������˨��Ϣ
		pRod->connectStart=connectStart;
		pRod->connectEnd=connectEnd;
	}
	pDestObj->SetModified();
	return true;
}
bool CLDSLinePart::SyncMirPropTo(CLDSDbObject* pDestObj,MIRMSG mirmsg,const char* propKeyStr/*=NULL*/,DWORD dwPropFilterFlag/*=0xffffffff*/)
{
	if(pDestObj==NULL)
		return false;
	CLDSLinePart* pRod=(CLDSLinePart*)pDestObj;
	if(propKeyStr==NULL||strcmp(propKeyStr,"synchronize_scope")==0)
	{
		MEMORY_BLOCK cl=mpcl;
		if(cl.IsZero())
			cl=CLDSLineAngle::MPCL;
		PROPLIST_ITEM* pItem=CLDSLineAngle::propMirSync.GetPropItem("synchronize_scope");
		if(pItem!=NULL && cl.GetBitState((WORD)pItem->id-1))
			pDestObj->mpcl=mpcl;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"layer")==0)&&IsSyncMirPropWith(pDestObj,"layer"))
	{	//ͼ����ͬ���Գ�
		MirObjLayer(layer(),pRod->layer(),mirmsg);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cMaterial")==0)&&IsSyncMirPropWith(pDestObj,"cMaterial"))
	{	//����
		pRod->cMaterial=cMaterial;
		pRod->cbVirginQualityLevel=cbVirginQualityLevel;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"specification")==0)&&IsSyncMirPropWith(pDestObj,"specification"))
	{	//���
		pRod->size_wide=size_wide;
		pRod->size_thick=size_thick;
		pRod->size_idClassType=size_idClassType;
		pRod->size_cSubClassType=size_cSubClassType;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"iSeg")==0)&&IsSyncMirPropWith(pDestObj,"iSeg"))
	{	//�ֶκ�
		pRod->iSeg=iSeg;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"sPartNo")==0)&&IsSyncMirPropWith(pDestObj,"sPartNo"))
	{	//�������
		pRod->SetPartNo(sPartNo);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cfgword")==0)&&IsSyncMirPropWith(pDestObj,"cfgword"))
	{	//��ĺ�
		pRod->cfgword=cfgword;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"m_bDisplayHole")==0)&&IsSyncMirPropWith(pDestObj,"m_bDisplayHole"))
	{	//��ʾ��
		pRod->m_bDisplayHole=m_bDisplayHole;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"crMaterial")==0)&&IsSyncMirPropWith(pDestObj,"crMaterial"))
	{	//������ɫ
		pRod->crMaterial=crMaterial;
	}
	bool reverseStartEnd=false;
	if(pStart && pRod->pEnd)
	{
		f3dPoint pos=pStart->Position(false);
		f3dPoint mirpos=pRod->pEnd->Position(false);
		f3dPoint destpos=mirmsg.MirPoint(pos);
		if(mirpos.IsEqual(destpos))
			reverseStartEnd=true;	//�ԳƽǸ���Ҫ�ߵ�ʼĩ��
	}
	else if(pEnd && pRod->pStart)
	{
		f3dPoint pos=pEnd->Position(false);
		f3dPoint mirpos=pRod->pStart->Position(false);
		f3dPoint destpos=mirmsg.MirPoint(pos);
		if(mirpos.IsEqual(destpos))
			reverseStartEnd=true;	//�ԳƽǸ���Ҫ�ߵ�ʼĩ��
	}
#ifdef __PART_DESIGN_INC_
	if((propKeyStr==NULL||strcmp(propKeyStr,"DesignInfo")==0))//&&IsSyncMirPropWith(pDestObj,"DesignInfo"))
	{
		pRod->CalLenCoef.iTypeNo=CalLenCoef.iTypeNo;
		pRod->CalLenCoef.minR.coef=CalLenCoef.minR.coef;
		CLDSNode* pNode,*pMirNode=NULL;
		pNode=CalLenCoef.minR.hStartNode>0x20?BelongModel()->FromNodeHandle(CalLenCoef.minR.hStartNode):NULL;
		pMirNode=pNode!=NULL?pNode->GetMirNode(mirmsg):NULL;
		if(pMirNode)
			pRod->CalLenCoef.minR.hStartNode=pMirNode->handle;
		else
			pRod->CalLenCoef.minR.hStartNode=0;
		pNode=CalLenCoef.minR.hEndNode>0x20?BelongModel()->FromNodeHandle(CalLenCoef.minR.hEndNode):NULL;
		pMirNode=pNode!=NULL?pNode->GetMirNode(mirmsg):NULL;
		if(pMirNode)
			pRod->CalLenCoef.minR.hEndNode=pMirNode->handle;
		else
			pRod->CalLenCoef.minR.hEndNode=0;

		pRod->CalLenCoef.paraR.coef=CalLenCoef.paraR.coef;
		pNode=CalLenCoef.paraR.hStartNode>0x20?BelongModel()->FromNodeHandle(CalLenCoef.paraR.hStartNode):NULL;
		pMirNode=pNode!=NULL?pNode->GetMirNode(mirmsg):NULL;
		if(pMirNode)
			pRod->CalLenCoef.paraR.hStartNode=pMirNode->handle;
		else
			pRod->CalLenCoef.paraR.hStartNode=0;
		pNode=CalLenCoef.paraR.hEndNode>0x20?BelongModel()->FromNodeHandle(CalLenCoef.paraR.hEndNode):NULL;
		pMirNode=pNode!=NULL?pNode->GetMirNode(mirmsg):NULL;
		if(pMirNode)
			pRod->CalLenCoef.paraR.hEndNode=pMirNode->handle;
		else
			pRod->CalLenCoef.paraR.hEndNode=0;
		MirPartHandle(BelongModel(),&pRod->CalLenCoef.hHorizPole,CalLenCoef.hHorizPole,mirmsg);
		MirPartHandle(BelongModel(),&pRod->CalLenCoef.hRefPole,CalLenCoef.hRefPole,mirmsg);
		pRod->m_bAutoCalHoleOutCoef=m_bAutoCalHoleOutCoef;
		pRod->hole_out=hole_out;
		pRod->sel_mat_method=sel_mat_method;
		if(!reverseStartEnd)
		{
			pRod->start_force_type=start_force_type;
			pRod->start_joint_type=start_joint_type;
			pRod->end_force_type=end_force_type;
			pRod->end_joint_type=end_joint_type;
		}
		else
		{
			pRod->start_force_type=end_force_type;
			pRod->start_joint_type=end_joint_type;
			pRod->end_force_type=start_force_type;
			pRod->end_joint_type=start_joint_type;
		}
		pRod->m_iElemType=m_iElemType;
		pRod->m_bNeedSelSize=m_bNeedSelSize;
		pRod->m_fWeightRaiseCoef=m_fWeightRaiseCoef;
	}
#endif
	
	if((propKeyStr==NULL||strcmp(propKeyStr,"linkInfo")==0)&&IsSyncMirPropWith(pDestObj,"linkInfo"))
	{	//������˨��Ϣ
		if(!reverseStartEnd)
		{
			pRod->connectStart=connectStart;
			pRod->connectEnd=connectEnd;
		}
		else
		{
			pRod->connectStart=connectEnd;
			pRod->connectEnd=connectStart;
		}
	}
	pDestObj->SetModified();
	return true;
}
bool CLDSLinePart::SyncMirProp(const char* propKeyStr/*=NULL*/,BYTE ciMirAll0X1Y2Z3/*=0*/,DWORD dwPropFilterFlag/*=0xffffffff*/)//propKeyStr��ʾͬ���Գ�ȫ������
{
	MIRMSG xSpecMirInfo(ciMirAll0X1Y2Z3);
	for(RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
	{
		if(ciMirAll0X1Y2Z3!=0&&!pRelaObj->mirInfo.IsSameMirMsg(xSpecMirInfo))
			continue;
		BOOL pushed=relativeObjs.push_stack();
		CLDSLinePart* pRod=BelongModel()->FromRodHandle(pRelaObj->hObj);
		SyncMirPropTo(pRod,pRelaObj->mirInfo,propKeyStr,dwPropFilterFlag);
		relativeObjs.pop_stack(pushed);
	}
	return true;
}

int CLDSLinePart::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	char sText[100]="";
	if(GetPropID("handle")==id)
		sprintf(sText,"0X%X",handle);
	else if(GetPropID("layer")==id)
		strcpy(sText,layer());
	else if(GetPropID("cMaterial")==id)	
	{
		char matStr[20]="";
		QuerySteelMatMark(cMaterial,matStr);
		strcpy(sText,matStr);
	}
	else if(GetPropID("m_cQualityLevel")==id)
	{
		if(isalpha(m_cQualityLevel))
			sprintf(sText,"%C",m_cQualityLevel);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Default");
#else
			strcpy(sText,"Ĭ��");
#endif
	}
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("sPartNo")==id)
		strcpy(sText,GetPartNo());
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(cfgword,valueStr);
	{
		if(layer(0)=='L')	//���ȹ���
			return cfgword.GetLegScopeStr(valueStr,50,true);
		else
			return cfgword.GetBodyScopeStr(valueStr);
	}
	else if(GetPropID("m_hBlock")==id)
		strcpy(sText,GetBlockIdName());
	else if(GetPropID("relativeObjs")==id)
	{
		for(RELATIVE_OBJECT *pRelaObj=relativeObjs.GetFirst();pRelaObj;pRelaObj=relativeObjs.GetNext())
		{
			if(strlen(sText)==0)
				_snprintf(sText,99,"0X%X",pRelaObj->hObj);
			else
				_snprintf(sText,99,"%s,0X%X",sText,pRelaObj->hObj);
		}
	}
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",m_uStatMatCoef);
	else if(GetPropID("m_hPartWeldParent")==id)
		sprintf(sText,"0X%X",m_hPartWeldParent);
	else if(GetPropID("m_bDisplayHole")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bDisplayHole)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(m_bDisplayHole)
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("m_bVirtualPart")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bVirtualPart)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(m_bVirtualPart)
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("m_bUserSpecColor")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bUserSpecColor)
			sprintf(sText,"Yes");
		else
			sprintf(sText,"No");
#else 
		if(m_bUserSpecColor)
			sprintf(sText,"��");
		else
			sprintf(sText,"��");
#endif
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",crMaterial);
	else if(GetPropID("sNotes")==id)
		strcpy(sText,sNotes);
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(GetPropID("oddStart")==id)
	{
		sprintf(sText,"%f",startOdd());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("oddEnd")==id)
	{
		sprintf(sText,"%f",endOdd());
		SimplifiedNumString(sText);
	}
#endif
	else if(GetPropID("hStart")==id)
	{
		if(pStart)
			sprintf(sText,"0X%X",pStart->handle);
		else
			strcpy(sText,"0X0");
	}
	else if(GetPropID("hEnd")==id)
	{
		if(pEnd)
			sprintf(sText,"0X%X",pEnd->handle);
		else
			strcpy(sText,"0X0");
	}
#if defined(__TSA_)||defined(__LDS_)
	else if(GetPropID("m_uStatMatNo")==id)
		sprintf(sText,"%d",m_uStatMatNo);
	else if(GetPropID("m_fWeightRaiseCoef")==id)
	{
		sprintf(sText,"%f",m_fWeightRaiseCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_iElemType")==id)	//�Զ��жϵ�Ԫ����
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(size_idClassType==CLS_CABLE)
			strcpy(sText,"3.Wire Cable");
		else if(m_iElemType==0)
			strcpy(sText,"0.Default");
		else if(m_iElemType==1)
			strcpy(sText,"1.������");
		else if(m_iElemType==2)
			strcpy(sText,"2.Classical Beam");
		else
			strcpy(sText,"1.������");
#else 
		if(size_idClassType==CLS_CABLE)
			strcpy(sText,"3.����");
		else if(m_iElemType==0)
			strcpy(sText,"0.�Զ��ж�");
		else if(m_iElemType==1)
			strcpy(sText,"1.������");
		else if(m_iElemType==2)
			strcpy(sText,"2.������");
		else
			strcpy(sText,"1.������");
#endif
	}
	else if(GetPropID("sel_mat_method")==id)	//�Զ��жϵ�Ԫ����
	{
		if(!IsAuxPole())
			strcpy(sText,"");
		else if(sel_mat_method==AUX_AUTODESIGN)
			strcpy(sText,"�Զ����");
		else if(sel_mat_method==AUX_BY_START)
			strcpy(sText,"����ʼ��");
		else if(sel_mat_method==AUX_BY_END)
			strcpy(sText,"�����ն�");
		else if(sel_mat_method==AUX_ONLY_PRIMARY)
			strcpy(sText,"����б��");
		else
			strcpy(sText,"");
	}
	else if(GetPropID("size.idClassType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(size_idClassType==CLS_LINEANGLE)
			strcpy(sText, "Angle With Equal Leg");
		else if(size_idClassType==CLS_GROUPLINEANGLE)
		{
			if(size_cSubClassType=='T')
				strcpy(sText, "T-Type combine");
			else if(size_cSubClassType=='D')
				strcpy(sText, "Diagonal combine");
			else if(size_cSubClassType=='X')
				strcpy(sText, "Cross Combine");
		}
		else if(size_idClassType==CLS_LINETUBE)
			strcpy(sText, "Tube");
		else if(size_idClassType==CLS_LINEFLAT)
			strcpy(sText, "Flat Angle");
		else if(size_idClassType==CLS_LINESLOT)
			strcpy(sText, "[ steel");
		else if(size_idClassType==CLS_CABLE)
			strcpy(sText, "Wire Cable");
		else if(size_idClassType==0)
			strcpy(sText, "User appoint");
#else 
		if(size_idClassType==CLS_LINEANGLE)
			strcpy(sText, "��֫�Ǹ�");
		else if(size_idClassType==CLS_GROUPLINEANGLE)
		{
			if(size_cSubClassType=='T')
				strcpy(sText, "T�����");
			else if(size_cSubClassType=='D')
				strcpy(sText, "�Խ����");
			else if(size_cSubClassType=='X')
				strcpy(sText, "ʮ�����");
		}
		else if(size_idClassType==CLS_LINETUBE)
			strcpy(sText, "�ֹ�");
		else if(size_idClassType==CLS_LINEFLAT)
			strcpy(sText, "���");
		else if(size_idClassType==CLS_LINESLOT)
			strcpy(sText, "�۸�");
		else if(size_idClassType==CLS_CABLE)
			strcpy(sText, "����");
		else if(size_idClassType==0)
			strcpy(sText, "�û�����");
#endif
	}
	else if(GetPropID("m_sWireType")==id)
		strcpy(sText,GetWireType());
	else if(GetPropID("m_sWireSpec")==id)
		strcpy(sText,GetSpec());
	else if(GetPropID("m_fWireArea")==id)	//���½����
	{
		if(library)
			sprintf(sText,"%f",GetWireArea());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fUnitWeight")==id)	//��λ������
	{
		if(library)
			sprintf(sText,"%f",GetWireUnitWeight());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fForcePermit")==id)	//�������
	{
		if(library)
			sprintf(sText,"%f",GetWireForcePermit()/1000);	//N->kN
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fPreTension")==id)//����ê�̶˰�װԤ����
	{
		sprintf(sText,"%f",m_fPreTension/1000);	//N->kN
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fElasticCoef")==id)	//����ϵ��(ģ��)
	{
		sprintf(sText,"%f",GetWireElasticCoef());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fExpansionCoef")==id)//�¶�������ϵ��
	{
		sprintf(sText,"%f",GetWireExpansionCoef());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("specification")==id)
	{
		if(GetClassTypeId()==CLS_LINEPART&&size_idClassType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Cross-section Constant");
#else 
			strcpy(sText,"���泣��");
#endif
		else
		{
			char s_wing_wide[200],s_wing_thick[200];
			sprintf(s_wing_wide,"%f",size_wide);
			sprintf(s_wing_thick,"%f",size_thick);
			SimplifiedNumString(s_wing_wide);
			SimplifiedNumString(s_wing_thick);
			sprintf(sText,"%sX%s", s_wing_wide,s_wing_thick);
		}
	}
	else if(GetPropID("userdef.size")==id)
	{
		GetSpecification(sText,FALSE);
	}
	else if(GetPropID("userdef.area")==id)
	{
		sprintf(sText,"%f",userdef.area);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("userdef.E")==id)
	{
		sprintf(sText,"%f",userdef.E);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("userdef.fc")==id)
	{
		sprintf(sText,"%f",userdef.fc);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("userdef.ft")==id)
	{
		sprintf(sText,"%f",userdef.ft);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("userdef.pre_stress")==id)
	{
		sprintf(sText,"%f",userdef.pre_stress);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("userdef.unit_weight")==id)
	{
		sprintf(sText,"%f",userdef.unit_weight);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("length")==id)
	{
		sprintf(sText,"%f",GetLength());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("lengthSingleLine")==id)
	{
		if(pStart!=NULL&&pEnd!=NULL)
		{
			double len=DISTANCE(pStart->Position(false),pEnd->Position(false));
			sprintf(sText,"%f",len);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("m_fWeightRaiseCoef")==id)
	{
		sprintf(sText,"%f",m_fWeightRaiseCoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("startPos.x")==id)
	{
		if(pStart)
		{
			sprintf(sText,"%f",pStart->Position(false).x);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("startPos.y")==id)
	{
		if(pStart)
		{
			sprintf(sText,"%f",pStart->Position(false).y);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("startPos.z")==id)
	{
		if(pStart)
		{
			sprintf(sText,"%f",pStart->Position(false).z);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("endPos.x")==id)
	{
		if(pEnd)
		{
			sprintf(sText,"%f",pEnd->Position(false).x);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("endPos.y")==id)
	{
		if(pEnd)
		{
			sprintf(sText,"%f",pEnd->Position(false).y);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("endPos.z")==id)
	{
		if(pEnd)
		{
			sprintf(sText,"%f",pEnd->Position(false).z);
			SimplifiedNumString(sText);
		}
	}
	else if(GetPropID("m_iElemType")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_iElemType==0)
			strcpy(sText,"0.Default");
		else if(m_iElemType==1)
			strcpy(sText,"1.������");
		else if(m_iElemType==2)
			strcpy(sText,"2.Classical Beam");
		else if(m_iElemType==3)
			strcpy(sText,"3.������Ԫ");
#else 
		if(m_iElemType==0)
			strcpy(sText,"0.�Զ��ж�");
		else if(m_iElemType==1)
			strcpy(sText,"1.������");
		else if(m_iElemType==2)
			strcpy(sText,"2.������");
		else if(m_iElemType==3)
			strcpy(sText,"3.������Ԫ");
#endif
	}
	else if(GetPropID("m_bNeedSelMat")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bNeedSelMat)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(m_bNeedSelMat)
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("m_bNeedSelSize")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bNeedSelSize)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(m_bNeedSelSize)
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	/*else if(GetPropID("CalLenCoef.iSelRFlag")==id)
	{
		if(CalLenCoef.iSelRFlag==id)
	}*/
	else if(GetPropID("CalLenCoef.iTypeNo")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(CalLenCoef.iTypeNo==0)
			strcpy(sText,"0.Judge Intelligent");
		else if(CalLenCoef.iTypeNo==1)
			strcpy(sText,"1.V tilted material(crossing foot material connect)");
		else if(CalLenCoef.iTypeNo==2)
			strcpy(sText,"2.V tilted material(V-plane connect)");
		else if(CalLenCoef.iTypeNo==3)
			strcpy(sText,"3.V tilted material 3 Section Horizon Pole");
		else if(CalLenCoef.iTypeNo==4)
			strcpy(sText,"4.V tilted material 4 Section Horizon Pole");
		else if(CalLenCoef.iTypeNo==6)
			strcpy(sText,"6.Specified Cross Rod Calculate Length");
		else //if(CalLenCoef.iTypeNo==5)
			strcpy(sText,"5.Specified Rod Calculate Length");
#else 
		if(CalLenCoef.iTypeNo==0)
			strcpy(sText,"0.�����ж�");
		else if(CalLenCoef.iTypeNo==1)
			strcpy(sText,"1.Vб��(��Ų�����)");
		else if(CalLenCoef.iTypeNo==2)
			strcpy(sText,"2.Vб��(V������)");
		else if(CalLenCoef.iTypeNo==3)
			strcpy(sText,"3.Vб��3�ڼ��������");
		else if(CalLenCoef.iTypeNo==4)
			strcpy(sText,"4.Vб��4�ڼ��������");
		else if(CalLenCoef.iTypeNo==6)
			strcpy(sText,"6.ָ������ļ��㳤��");
		else //if(CalLenCoef.iTypeNo==5)
			strcpy(sText,"5.ָ���˼����㳤��");
#endif
	}
	else if(GetPropID("CalLenCoef.pHorizPole")==id)
	{
		if(CalLenCoef.hHorizPole>0x20)
			sprintf(sText,"0X%X",CalLenCoef.hHorizPole);
		else
			strcpy(sText,"0X0");
	}
	else if(GetPropID("CalLenCoef.pRefPole")==id)
	{
		if(CalLenCoef.hRefPole>0x20)
			sprintf(sText,"0X%X",CalLenCoef.hRefPole);
		else
			strcpy(sText,"0X0");
	}
	else if(GetPropID("CalLenCoef.spec_callenMinR")==id)
	{
		sprintf(sText,"%f",CalSpecLamdaLenMinR());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("CalLenCoef.minR.hStartNode")==id)
	{
		sprintf(sText,"0X%X",CalLenCoef.minR.hStartNode);
	}
	else if(GetPropID("CalLenCoef.minR.hEndNode")==id)
	{
		sprintf(sText,"0X%X",CalLenCoef.minR.hEndNode);
	}
	else if(GetPropID("CalLenCoef.minR.coef")==id)
	{
		sprintf(sText,"%f",CalLenCoef.minR.coef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("CalLenCoef.spec_callenParaR")==id)
	{
		sprintf(sText,"%f",CalSpecLamdaLenParaR());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("CalLenCoef.paraR.hStartNode")==id)
	{
		sprintf(sText,"0X%X",CalLenCoef.paraR.hStartNode);
	}
	else if(GetPropID("CalLenCoef.paraR.hEndNode")==id)
	{
		sprintf(sText,"0X%X",CalLenCoef.paraR.hEndNode);
	}
	else if(GetPropID("CalLenCoef.paraR.coef")==id)
	{
		sprintf(sText,"%f",CalLenCoef.paraR.coef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("start_joint_type")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(start_joint_type==JOINT_HINGE)
			strcpy(sText,"Articulate");
		else if(start_joint_type==JOINT_RIGID)
			strcpy(sText,"Rigid Connect");
		else //if(start_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"Default");
#else 
		if(start_joint_type==JOINT_HINGE)
			strcpy(sText,"�½�");
		else if(start_joint_type==JOINT_RIGID)
			strcpy(sText,"�ս�");
		else //if(start_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"�Զ��ж�");
#endif
	}
	else if(GetPropID("start_force_type")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(start_force_type==CENTRIC_FORCE)
			strcpy(sText,"Center Force");
		else //if(start_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"Eccentric Force");
#else 
		if(start_force_type==CENTRIC_FORCE)
			strcpy(sText,"��������");
		else //if(start_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"ƫ������");
#endif
	}
	else if(GetPropID("connectStart.grade")==id)
		strcpy(sText,connectStart.grade);
	else if(GetPropID("connectStart.d")==id)
		sprintf(sText,"M%d",connectStart.d);
	else if(GetPropID("connectStart.N")==id)
		sprintf(sText,"%d",connectStart.wnConnBoltN);
	else if(GetPropID("connectStart.summBoltN")==id)
		sprintf(sText,"%d",connectStart.wnSummBoltN);
	else if(GetPropID("connectStart.bearloadcoef")==id)
	{
		sprintf(sText,"%f",connectStart.bearloadcoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("connectStart.maxTension")==id)
	{
		sprintf(sText,"%.1f",connectStart.maxTension*0.001);
	}
	else if(GetPropID("connectStart.maxCompression")==id)
	{
		sprintf(sText,"%.1f",connectStart.maxCompression*0.001);
	}

	else if(GetPropID("end_joint_type")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(end_joint_type==JOINT_HINGE)
			strcpy(sText,"Articulate");
		else if(end_joint_type==JOINT_RIGID)
			strcpy(sText,"Rigid Connect");
		else //if(end_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"Default");
#else 
		if(end_joint_type==JOINT_HINGE)
			strcpy(sText,"�½�");
		else if(end_joint_type==JOINT_RIGID)
			strcpy(sText,"�ս�");
		else //if(end_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"�Զ��ж�");
#endif
	}
	else if(GetPropID("end_force_type")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(end_force_type==CENTRIC_FORCE)
			strcpy(sText,"Center Force");
		else //if(end_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"Eccentric Force");
#else 
		if(end_force_type==CENTRIC_FORCE)
			strcpy(sText,"��������");
		else //if(end_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"ƫ������");
#endif
	}
	else if(GetPropID("connectEnd.grade")==id)
		strcpy(sText,connectEnd.grade);
	else if(GetPropID("connectEnd.d")==id)
		sprintf(sText,"M%d",connectEnd.d);
	else if(GetPropID("connectEnd.N")==id)
		sprintf(sText,"%d",connectEnd.wnConnBoltN);
	else if(GetPropID("connectEnd.summBoltN")==id)
		sprintf(sText,"%d",connectEnd.wnSummBoltN);
	else if(GetPropID("connectEnd.bearloadcoef")==id)
	{
		sprintf(sText,"%f",connectEnd.bearloadcoef);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("connectEnd.maxTension")==id)
	{
		sprintf(sText,"%.1f",connectEnd.maxTension*0.001);
	}
	else if(GetPropID("connectEnd.maxCompression")==id)
	{
		sprintf(sText,"%.1f",connectEnd.maxCompression*0.001);
	}

	else if(GetPropID("m_bAutoCalHoleOutCoef")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bAutoCalHoleOutCoef)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(m_bAutoCalHoleOutCoef)
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("hole_out")==id)
	{
		sprintf(sText,"%f",hole_out);
		SimplifiedNumString(sText);
	}
#endif
#ifdef AFX_TARG_ENU_ENGLISH
	if(strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("0X%X Rod's property##d string value out of length",handle,id);
#else
	if(strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("�˼�0X%X����#%d�ַ���������������",handle,id);
#endif
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
long CLDSLinePart::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem=CLDSLinePart::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
	{
		/*CString prompt="û�ҵ���Ϊ\"";
		prompt+=propStr;
		prompt+="\"������ID��";
		AfxMessageBox(prompt);*/
		return 0;
	}
}

BOOL CLDSLinePart::IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSLinePart *pLinePart=NULL, *pFirstLinePart = (CLDSLinePart*)selectObjs.GetFirst();
	pFirstLinePart->GetPropValueStr(idProp,valueStr);
	for(pLinePart=(CLDSLinePart*)selectObjs.GetNext();pLinePart;pLinePart=(CLDSLinePart*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pLinePart->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CLDSLinePart::GetPropStatus(long id)
{	
	DWORD dwStatus;
	if(CLDSLinePart::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}

BOOL CLDSLinePart::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSLinePart::propStatusHashtable.SetValueAt(id,dwStatus);
}

void CLDSLinePart::SetStart(f3dPoint start,bool bArcLift/*=true*/)
{
	if(IsStartPosLocked())	//�ն�λ���ѱ�����
		return;
	CLDSArcLift* pLift=NULL;
	if(m_hArcLift>0x20)
		pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
	if(!bArcLift||pLift==NULL)
		m_xStartBeforeLift=start;
	else
		m_xStartBeforeLift=pLift->ReverseLiftPosTrans(start);
	baseline.SetStart(start);
	SetModified(TRUE);
}
f3dPoint CLDSLinePart::Start(bool bArcLift/*=true*/) const
{
	if( !xStartNode.bActiveHandle&&xStartNode.pNode!=NULL&&size.idClassType==CLS_LINEPART)
		return xStartNode.pNode->Position(bArcLift);
#ifdef __COMPLEX_PART_INC_
	else if(!bArcLift)
		return m_xStartBeforeLift;
#endif
	else
		return baseline.Start();
}

void CLDSLinePart::SetEnd(f3dPoint end,bool bArcLift/*=true*/)
{
	if(IsEndPosLocked())	//ʼ��λ���ѱ�����
		return;
	CLDSArcLift* pLift=NULL;
	if(m_hArcLift>0x20)
		pLift=BelongModel()->FromArcLiftHandle(m_hArcLift);
	if(!bArcLift||pLift==NULL)
		m_xEndBeforeLift=end;
	else
		m_xEndBeforeLift=pLift->ReverseLiftPosTrans(end);
	baseline.SetEnd(end);
	SetModified(TRUE);
}
f3dPoint CLDSLinePart::End(bool bArcLift/*=true*/) const
{	
	if( !xEndNode.bActiveHandle&&xEndNode.pNode&&size.idClassType==CLS_LINEPART)
		return xEndNode.pNode->Position(bArcLift);
#ifdef __COMPLEX_PART_INC_
	else if(!bArcLift)
		return m_xEndBeforeLift;
#endif
	else
		return baseline.End();
}

//0.��ȫ�Գ�һ��;1.���Ǹ�֫������λ�öԳ�һ��;2.ʰȡ�㴦����ͬ��Գ�;3.ʰȡ�㴦��������Գ�
BYTE CLDSLinePart::MirSimilarGradeWith(CLDSPart *pPart,const MIRMSG& mirmsg,const double *pPickPos/*=NULL*/)
{
	BOOL bSingleLineCoord=FALSE;
	if(pStart&&pEnd)
		bSingleLineCoord=TRUE;
	if(pPart==NULL || GetClassTypeId()!=pPart->GetClassTypeId())
		return 255;	//���Ͳ�ͬ
	CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
	f3dLine line,mirLine;
	double INTER_EPS_COS=0.99999,internal_eps=0.00001;	//��Щ������ߵ�Ҳ���Ƿǳ��ϸ�Գƣ�������һЩ����ģ�� wjh-2019.11.23
	if(bSingleLineCoord)
	{
		line.startPt=pStart->Position(false);
		line.endPt=pEnd->Position(false);
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			return 255;
		mirLine.startPt=pLinePart->pStart->Position(false);
		mirLine.endPt=pLinePart->pEnd->Position(false);
	}
	else
	{
		line.startPt=Start();
		line.endPt=End();
		mirLine.startPt=pLinePart->Start();
		mirLine.endPt=pLinePart->End();
		internal_eps=0.5;	//ʵ�����꾫ȷ��0.5mm���㹻��
	}
	f3dPoint line_vec=line.endPt-line.startPt;
	f3dPoint mir_line_vec=mirLine.endPt-mirLine.startPt;
	normalize(line_vec);
	normalize(mir_line_vec);
	line_vec=GetMirVector(line_vec,mirmsg);
	double justify=line_vec*mir_line_vec;
	f3dPoint mirStartPoint=GetMirPos(line.startPt,mirmsg);
	f3dPoint mirEndPoint=GetMirPos(line.endPt,mirmsg);
	if(bSingleLineCoord&&(fabs(justify)<INTER_EPS_COS||mirLine.PtInLine(mirStartPoint,internal_eps)==0))
		return 255;	//ʸ�����򲻶Գ�
	else if(!bSingleLineCoord&&(fabs(justify)<EPS_COS2||mirLine.PtInLine(mirStartPoint,EPS2)==0))
		return 255;	//ʸ�����򲻶Գ�
	BYTE similarGrade=255;
	if(pPickPos)	//ָ���Գ�ʰȡ��
	{
		GEPOINT mirPickPos,perp,pickPos(pPickPos);
		double sectlen_start, sectlen_end, mirsectlen_start, mirsectlen_end, dist = 0;
		for (int i = 0; i < 2; i++)
		{
			SnapPerp(&pickPos,line,pickPos);
			mirPickPos=GetMirPos(pickPos,mirmsg);
			SnapPerp(&perp,mirStartPoint,mirEndPoint,mirPickPos);
			sectlen_start=(mirStartPoint-perp)*line_vec;
			sectlen_end=(mirEndPoint-perp)*line_vec;
			if (sectlen_start*sectlen_end > 0 &&min(fabs(sectlen_start),fabs(sectlen_end)) > 1000)
			{	//sectlen_start��sectlen_endͬ������С���ȴ���1��ʱ��pickPosȡֵ������
				//���磺�ᵣ�����߶̽Ǹ�,pickposһ��Ĭ��ȡֵ��0��0��0��,���Ǹֻ���ƽ��ʱ��Ŵ�����
				//��ʱpickPos��Ҫ���¸�ֵ��ȡ�˼��е�Ϊʰȡ�㣩�ٴμ��� wht 19-09-18
				pickPos = (line.startPt + line.endPt)*0.5;
				continue;
			}
			else
				break;
		}
		SnapPerp(&perp,mirLine,mirPickPos,&dist);
		if(dist>internal_eps)
			return 255;
		if(justify>0)
		{	//�ԳƷ�����ͬ
			mirsectlen_start=(mirLine.startPt-perp)*line_vec;
			mirsectlen_end  =(mirLine.endPt  -perp)*line_vec;
		}
		else
		{	//�ԳƷ����෴
			mirsectlen_start=((mirLine.endPt  -perp)*line_vec);
			mirsectlen_end  =((mirLine.startPt-perp)*line_vec);
		}
		if(mirStartPoint.IsEqual(mirPickPos)&&(mirsectlen_start>internal_eps||mirsectlen_end<-internal_eps))
			return 255;	//ʼ�˵㵱ǰ���Ӷˣ��Գ����뱻��֤�˼���ʰȡ�㴦���غ�
		else if(mirEndPoint.IsEqual(mirPickPos)&&(mirsectlen_start>internal_eps||mirsectlen_end<-internal_eps))
			return 255;	//�ն˵㵱ǰ���Ӷˣ��Գ����뱻��֤�˼���ʰȡ�㴦���غ�
		else if(fabs(sectlen_end-mirsectlen_start)<internal_eps||fabs(sectlen_start-mirsectlen_end)<internal_eps)
			return 3;	//�˵㷴��Խ�
		else if(sectlen_end<mirsectlen_start||sectlen_start>mirsectlen_end)
			return 255;	//���κν���
		else	//�н�������ʰȡ�㲻�Ƕ˵�ʱ������ͬ��Գƴ���
			similarGrade=2;
	}
	//����ʼ�ն˱ȶ�
	f3dPoint mirStartPos=GetMirPos(line.startPt,mirmsg);
	f3dPoint mirEndPos=GetMirPos(line.endPt,mirmsg);
	if( (mirStartPos.IsEqual(mirLine.startPt,internal_eps)&&mirEndPos.IsEqual(mirLine.endPt,internal_eps))||
		(mirStartPos.IsEqual(mirLine.endPt,internal_eps)&&mirEndPos.IsEqual(mirLine.startPt,internal_eps)))
	{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		if(GetClassTypeId()!=CLS_LINEANGLE)
			similarGrade=0;	//����˼�������֫���������
		else
		{
			similarGrade=1;	//λ������Գ�
			//����ϽǸ��ӽǸ�Ӧ���ݷ��߶Գ��ж�
			CLDSLineAngle* pLineAngle=(CLDSLineAngle*)this;
			CLDSLineAngle* pMirLineAngle=(CLDSLineAngle*)pPart;
			f3dPoint norm_x_wing=pLineAngle->get_norm_x_wing();
			f3dPoint norm_y_wing=pLineAngle->get_norm_y_wing();
			f3dPoint mir_norm_x_wing=GetMirVector(norm_x_wing,mirmsg);
			f3dPoint mir_norm_y_wing=GetMirVector(norm_y_wing,mirmsg);
			if( (pMirLineAngle->get_norm_x_wing()*mir_norm_x_wing>EPS_COS2&&pMirLineAngle->get_norm_y_wing()*mir_norm_y_wing>EPS_COS2)||
				(pMirLineAngle->get_norm_x_wing()*mir_norm_y_wing>EPS_COS2&&pMirLineAngle->get_norm_y_wing()*mir_norm_x_wing>EPS_COS2))
				similarGrade=0;		//����ȫ�ԳƵ�����ӽǸ�ѧϰ
			else if(GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLineAngle)->group_father_jg_h>0x20)
				similarGrade=255;	//��ϽǸ��ӽǸ�Ҫô��ȫ�Գ�,Ҫô���Գ�,������ֻλ�öԳ����
			else if(!bSingleLineCoord)
				similarGrade=255;	//ʵ��ǸֶԳ�Ҳ������ֻλ�öԳƵ����
		}
		return similarGrade;
#else
		return 0;
#endif
	}
	else if(!pPickPos)	//δָ���Գ�ʰȡ��ʱ���˼�������ȫ�ԳƲ���Ϊ���жԳƹ��� wjh-2015.6.07
		return 255;
	else if(GetClassTypeId()!=CLS_LINEANGLE)
		return similarGrade;	//����˼�������֫���������
	else
	{
		//����ϽǸ��ӽǸ�Ӧ���ݷ��߶Գ��ж�
		CLDSLineAngle* pLineAngle=(CLDSLineAngle*)this;
		CLDSLineAngle* pMirLineAngle=(CLDSLineAngle*)pPart;
		f3dPoint norm_x_wing=pLineAngle->get_norm_x_wing();
		f3dPoint norm_y_wing=pLineAngle->get_norm_y_wing();
		f3dPoint mir_norm_x_wing=GetMirVector(norm_x_wing,mirmsg);
		f3dPoint mir_norm_y_wing=GetMirVector(norm_y_wing,mirmsg);
		if( (pMirLineAngle->get_norm_x_wing()*mir_norm_x_wing>EPS_COS2&&pMirLineAngle->get_norm_y_wing()*mir_norm_y_wing>EPS_COS2)||
			(pMirLineAngle->get_norm_x_wing()*mir_norm_y_wing>EPS_COS2&&pMirLineAngle->get_norm_y_wing()*mir_norm_x_wing>EPS_COS2))
			similarGrade=1;		//֫����Գ�
		else if(GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLineAngle)->group_father_jg_h>0x20)
			similarGrade=255;	//��ϽǸ��ӽǸ�Ҫô��ȫ�Գ�,Ҫô���Գ�,������ֻλ�öԳ����
		else if(!bSingleLineCoord)
			similarGrade=255;	//ʵ��ǸֶԳ�Ҳ������ֻλ�öԳƵ����
		return similarGrade;
	}
}

CLDSLinePart *CLDSLinePart::GetMirRod(MIRMSG mirmsg,double epsilong/*=EPS*/)
{
	CLDSLinePart *pMirRod=NULL;
	if(pStart==NULL||pEnd==NULL)	//ֻ��ʼ�չ����ڵ㶼����ʱ�Ų��õ�������
		return NULL;
	f3dPoint mir_start=mirmsg.MirPoint(pStart->Position(false));
	f3dPoint mir_end=mirmsg.MirPoint(pEnd->Position(false));
	if(mir_start.IsEqual(pEnd->Position(false),epsilong)&&mir_end.IsEqual(pStart->Position(false),epsilong))
		return this;	//�˼�ʼ�ն˽ڵ㻥��Գ�,�ԳƵĸ˼������Լ�
	CLDSLinePart *pLastMirLinePart=NULL;
	for(RELATIVE_OBJECT *pItem=relativeObjs.GetFirst();pItem;pItem=relativeObjs.GetNext())
	{	//�����ڹ��������б��в��ҶԳƶ�С�ڵ���3�Ĺ�����������
		pMirRod=NULL;
		if(pItem->mirInfo.IsSameMirMsg(mirmsg))
		{	//�жϸ��ݹ��������б��ҵ��ĶԳƸ˼��Ƿ�Ϸ�(��Ҫʱ������һ�˶Գ�ʱ���ܳ���) wht 09-12-06
			pMirRod=(CLDSLinePart*)BelongModel()->FromPartHandle(pItem->hObj,GetClassTypeId());
			if(pMirRod==NULL||pMirRod->pStart==NULL||pMirRod->pEnd==NULL)
				continue;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			if( pMirRod->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pMirRod)->group_father_jg_h>0x20)
				continue;	//������ϽǸֵ��ӽǸ�
			if( pMirRod->GetClassTypeId()!=this->m_nClassTypeId)
				continue;	//�������Ͳ�һ�£���ֹ��ϽǸֵ��ӽǸ�����ϽǸּ�����Գƹ��� wjh-2019.1.16
#endif
			if( (mir_start.IsEqual(pMirRod->pStart->Position(false),epsilong)&&mir_end.IsEqual(pMirRod->pEnd->Position(false),epsilong))||
				(mir_start.IsEqual(pMirRod->pEnd->Position(false),epsilong)&&mir_end.IsEqual(pMirRod->pStart->Position(false),epsilong)))
				return pMirRod;
		}
	}

	CLdsPartListStack stack(m_pModel);	//ԭ������forѭ�����˳�ʱ���������ջ��wjh-2017.1.22
	for(pMirRod=m_pModel->EnumRodFirst();pMirRod;pMirRod=m_pModel->EnumRodNext())
	{
		if(!pMirRod->cfgword.And(cfgword))
			continue;
		if(pMirRod==NULL||pMirRod->pStart==NULL||pMirRod->pEnd==NULL)
			continue;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		if( pMirRod->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pMirRod)->group_father_jg_h>0x20)
			continue;	//������ϽǸֵ��ӽǸ�
		if( pMirRod->GetClassTypeId()!=this->m_nClassTypeId)
			continue;	//�������Ͳ�һ�£���ֹ��ϽǸֵ��ӽǸ�����ϽǸּ�����Գƹ��� wjh-2019.1.16
#endif
		if( (mir_start.IsEqual(pMirRod->pStart->Position(false),epsilong)&&mir_end.IsEqual(pMirRod->pEnd->Position(false),epsilong))||
			(mir_start.IsEqual(pMirRod->pEnd->Position(false),epsilong)&&mir_end.IsEqual(pMirRod->pStart->Position(false),epsilong)))
			return pMirRod;
	}
	return NULL;
}
CLDSPart *CLDSLinePart::GetMirPart(MIRMSG mirmsg,const double *pPickPos/*=NULL*/,BOOL bCheckProfile/*=TRUE*/)
{
	CLDSLinePart *pMirLinePart=NULL;
	//0.��ȫ�Գ�һ��;1.��֫������Գ�һ��;2.ʰȡ�㴦����ͬ��Գ�;3.ʰȡ�㴦��������Գ�
	BYTE similarGrade=255;
	BOOL bSingleLineCoord=TRUE;
	if(pStart==NULL||pEnd==NULL)	//ֻ��ʼ�չ����ڵ㶼����ʱ�Ų��õ�������
		bSingleLineCoord=FALSE;
	f3dPoint mir_start=GetMirPos(Start(),mirmsg);
	f3dPoint mir_end=GetMirPos(End(),mirmsg);
	CSmartPtr<CLDSNode>pStartMirNode,pEndMirNode;
	if(pStart!=NULL)
		pStartMirNode = pStart->GetMirNode(mirmsg);
	if(pEnd!=NULL)
		pEndMirNode = pEnd->GetMirNode(mirmsg);
	CLDSLineAngle* pAngleSelf=NULL;
	if(pStartMirNode.IsHasPtr()&&pEndMirNode.IsHasPtr()&&(
		(pStartMirNode.IsEqualPtr(pEnd)&&pEndMirNode.IsEqualPtr(pStart))||
		(pStartMirNode.IsEqualPtr(pStart)&&pEndMirNode.IsEqualPtr(pEnd))))
	{
		if(GetClassTypeId()==CLS_LINEANGLE)
			pAngleSelf=(CLDSLineAngle*)this;
		if(pAngleSelf==NULL || pAngleSelf->group_father_jg_h<0x20)
			return this;	//�˼�ʼ�ն˽ڵ㻥��Գ�,�ԳƵĸ˼������Լ�
		//else	��ϽǸֵ��ӽǸּ�Ҳ�п��ܴ��ڶԳƹ�ϵ
	}
	else if((pStart==NULL||pEnd==NULL)&&MirSimilarGradeWith(this,mirmsg,pPickPos)==0)
		return this;	//����̽Ǹ��ԶԳ� wht 16-10-31
	CLDSLinePart *pLastMirLinePart=NULL;
	for(RELATIVE_OBJECT *pItem=relativeObjs.GetFirst();pItem;pItem=relativeObjs.GetNext())
	{	//�����ڹ��������б��в��ҶԳƶ�С�ڵ���3�Ĺ�����������
		pMirLinePart=NULL;
		if(pItem->mirInfo.IsSameMirMsg(mirmsg))
		{	//�жϸ��ݹ��������б��ҵ��ĶԳƸ˼��Ƿ�Ϸ�(��Ҫʱ������һ�˶Գ�ʱ���ܳ���) wht 09-12-06
			pMirLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(pItem->hObj,GetClassTypeId());
			BYTE similar=MirSimilarGradeWith(pMirLinePart,mirmsg,pPickPos);
			if(similar<similarGrade)
			{
				pLastMirLinePart=pMirLinePart;
				similarGrade=similar;
			}
			if(similar>1)
				continue;	//��ȫ���Գ�
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			//��ϽǸ��ӽǸֱ��뺬ʵ��������ȫһ�� wjh-2011.8.9
			if( pMirLinePart&&GetClassTypeId()==CLS_LINEANGLE&&//pMirLinePart->m_bVirtualPart&&
				((CLDSLineAngle*)pMirLinePart)->group_father_jg_h>0x20&&similar>0)
				continue;	//ͨ�����������б�õ��ĶԳƹ���Ϊ�����ӽǸ�ʱӦͨ���ԳƲ��ҶԳƹ��� wht 11-08-04
#endif
			return pMirLinePart;
		}
	}

	CLdsPartListStack stack(m_pModel);	//ԭ������forѭ�����˳�ʱ���������ջ��wjh-2017.1.22
	int cls_typeid=GetClassTypeId();
	for(pMirLinePart=(CLDSLinePart*)m_pModel->EnumPartFirst(cls_typeid);pMirLinePart;pMirLinePart=(CLDSLinePart*)m_pModel->EnumPartNext(cls_typeid))
	{
		if(!ModuleTogetherWith(pMirLinePart))
			continue;
		//pAngleSelf!=NULL��ʾ��ϽǸ��ӽǸ��ԶԳƵ�����������λ�ý��жԳ�,
		//��:�����ᵣ�ҵ���ϽǸֵĽڵ��Գ�ʱ�ӽǸֶԳƸ˼�Ϊ�Լ� wht 15-08-11
		if(pMirLinePart==this&&pAngleSelf==NULL)
			continue;	//�ԳƸ˼�Ϊ�Լ�����������ϱߴ����
		BYTE similar=MirSimilarGradeWith(pMirLinePart,mirmsg,pPickPos);
		if(similar<similarGrade)
		{
			similarGrade=similar;
			if(similar==0)	//���ҵ���ȫ�Գ�һ�µĸ˼�,�Ͳ��ڼ�������
				return pMirLinePart;
			pLastMirLinePart=pMirLinePart;
		}
	}
	return pLastMirLinePart;
	/*f3dPoint line_vec;
	if(pStart&&pEnd)
		line_vec=pEnd->pos-pStart->pos;
	else
		line_vec=End()-Start();
	normalize(line_vec);*/
	/*if(scope_start0_end1_elseall!=1)
	{	//�Ƚ�ʼ��
		if(pStart)
		{	//ԭʼ�˼�ʼ�ڵ㲻Ϊ��
			if(pStartMirNode.IsNULL())
				continue;	//Ϊ��
			else if(!pStartMirNode.IsEqualPtr(pMirLinePart->pStart)&&!pStartMirNode.IsEqualPtr(pMirLinePart->pEnd))
				continue;	//��Ϊ�յ���һ��
		}
		else
		{	//ԭʼ�˼�ʼ�ڵ㼴Ϊ��,�Ƚ���������Ƿ�һ��
			if(mir_start!=pMirLinePart->Start()&&mir_start!=pMirLinePart->End())
				continue;
		}
	}
	if(scope_start0_end1_elseall!=0)
	{	//�Ƚ��ն�
		if(pEnd)
		{	//ԭʼ�˼�ʼ�ڵ㲻Ϊ��
			if(pEndMirNode.IsNULL())
				continue;	//Ϊ��
			else if(!pEndMirNode.IsEqualPtr(pMirLinePart->pEnd)&&!pEndMirNode.IsEqualPtr(pMirLinePart->pStart))
				continue;	//Ϊ�յ���һ��
		}
		else
		{	//ԭʼ�˼�ʼ�ڵ㼴Ϊ��,�Ƚ���������Ƿ�һ��
			if(mir_end!=pMirLinePart->End()&&mir_end!=pMirLinePart->Start())
				continue;
		}
	}
	if(scope_start0_end1_elseall==0||scope_start0_end1_elseall==1)
	{	//ֻ�Ƚ�ʼ�˻��ն�
		f3dPoint new_line_vec,mir_line_vec;
		//�ԳƸ˼�ʼ�˶�ӦԴ�˼�ʼ�˻��ն˶�Ӧ�ն� �˼����췽��Ӧ��Դ�˼��ԳƷ�����ͬ,��֮�෴ wht 09-12-22
		if(pStart&&pEnd&&pMirLinePart->pStart&&pMirLinePart->pEnd)
		{
			if(pStartMirNode.IsEqualPtr(pMirLinePart->pStart)||pEndMirNode.IsEqualPtr(pMirLinePart->pEnd))
				new_line_vec=pMirLinePart->pEnd->pos-pMirLinePart->pStart->pos;
			else 
				new_line_vec=pMirLinePart->pStart->pos-pMirLinePart->pEnd->pos;
		}
		else
		{
			if(mir_start==pMirLinePart->Start()||mir_end==pMirLinePart->End())
				new_line_vec=pMirLinePart->End()-pMirLinePart->Start();
			else
				new_line_vec=pMirLinePart->Start()-pMirLinePart->End();
		}
		normalize(new_line_vec);
		//Ӧ�ý���ԭ�˼�������õĶԳƷ��򱣴浽��ʱ�����У���Ӧ���浽ԭ�˼�������
		mir_line_vec=GetMirVector(line_vec,mirmsg);
		if(mir_line_vec*new_line_vec<EPS_COS)
			continue;
	}*/
}

void CLDSLinePart::GetMirLinePartsByPos(MIRMSG mirmsg,LINEPARTSET &mirRodSet)
{
	if(pStart==NULL||pEnd==NULL)
		return;
	BOOL pushed=m_pModel->PushPartStack(); //���������б�ǰӦ��ѹջ wht 12-03-05
	int cls_typeid=GetClassTypeId();
	CLDSLinePart *pMirLinePart=NULL;
	for(pMirLinePart=(CLDSLinePart*)m_pModel->EnumPartFirst(cls_typeid);pMirLinePart;pMirLinePart=(CLDSLinePart*)m_pModel->EnumPartNext(cls_typeid))
	{
		if(!pMirLinePart->cfgword.And(cfgword))
			continue;
		if(pMirLinePart==this)
			continue;	//�ԳƸ˼�Ϊ�Լ�����������ϱߴ����
		if(pMirLinePart->pStart==NULL||pMirLinePart->pEnd==NULL)
			continue;
		f3dPoint line_vec=pMirLinePart->pEnd->Position(false)-pMirLinePart->pStart->Position(false);
		normalize(line_vec);
		int n=3;
		if(mirmsg.axis_flag&0x08)
			n=7;	//������ת�ԳƼ�XYZ��ת�ԳƸ˼�
		for(int i=1;i<=n;i++)
		{
			MIRMSG msg;
			f3dPoint mir_start=pStart->Position(false),mir_end=pEnd->Position(false);
			if(i>3)
			{	//��ת�ԳƼ�XYZ��ת�Գ�
				if(i>4)
				{	//�ȼ������XYZ�Գ��Ժ�����꣬Ȼ���ٽ�����ת�Գ�
					msg=mirmsg.GetSubMirItem(i-4);
					mir_start=GetMirPos(mir_start,msg);
					mir_end=GetMirPos(mir_end,msg);
				}
				msg.axis_flag=0x08;
				msg.rotate_angle=90;
			}
			else //XYZ�Գ�
				msg=mirmsg.GetSubMirItem(i);
			mir_start=GetMirPos(mir_start,msg);
			mir_end=GetMirPos(mir_end,msg);
			f3dPoint mir_line_vec=mir_end-mir_start;
			normalize(mir_line_vec);
			double justify=line_vec*mir_line_vec;
			if(fabs(justify)<EPS_COS)
				continue;	//ʸ�����򲻶Գ�
			//����ʼ�ն˱ȶ�
			if( (mir_start.IsEqual(pMirLinePart->pStart->Position(false),EPS)&&mir_end.IsEqual(pMirLinePart->pEnd->Position(false),EPS))||
				(mir_start.IsEqual(pMirLinePart->pEnd->Position(false),EPS)&&mir_end.IsEqual(pMirLinePart->pStart->Position(false),EPS)))
			{
				mirRodSet.append(pMirLinePart);
				break;
			}
		}
	}
	m_pModel->PopPartStack(pushed);
}

double CLDSLinePart::GetDatumLength()
{
	return DISTANCE(Start(),End());
	/*f3dPoint start,end;
	if(IsAngle())
	{	//�Ǹ�
		start=((CLDSLineAngle*)this)->GetStartDatumPosEx();
		end=((CLDSLineAngle*)this)->GetEndDatumPosEx();
	}
	else if(IsTube()||CLS_LINEFLAT==m_nClassTypeId||CLS_LINESLOT==m_nClassTypeId)
	{
		f3dPoint workNorm;
		CTubeEndPosPara *pDesStartPos=NULL,*pDesEndPos=NULL;
		if(IsTube())
		{
			CLDSLineTube *pTube=(CLDSLineTube*)this;
			pDesStartPos=&(pTube->desStartPos);
			pDesEndPos=&(pTube->desEndPos);
			workNorm=pTube->WorkPlaneNorm();
		}
		else if(CLS_LINEFLAT==m_nClassTypeId)
		{
			CLDSLineTube *pFlat=(CLDSLineTube*)this;
			pDesStartPos=&(pFlat->desStartPos);
			pDesEndPos=&(pFlat->desEndPos);
			workNorm=pFlat->WorkPlaneNorm();
		}
		else if(CLS_LINESLOT==m_nClassTypeId)
		{
			CLDSLineTube *pSlot=(CLDSLineTube*)this;
			pDesStartPos=&(pSlot->desStartPos);
			pDesEndPos=&(pSlot->desEndPos);
			workNorm=pSlot->WorkPlaneNorm();
		}
		if(pDesStartPos&&pDesEndPos)
		{
			start=baseline.Start();
			if(fabs(pDesStartPos->fNormOffset)>0)
			{	
				if(pDesStartPos->endPosType==0		//�˵㼴�ڵ�
					||pDesStartPos->endPosType==1	//����ڽڵ㸸�ֹܸ˼���
					||pDesStartPos->endPosType==3)	//�˼����߽���
					start -= workNorm*pDesStartPos->fNormOffset;
			}
			end=baseline.End();
			if(fabs(pDesEndPos->fNormOffset)>0)
			{	
				if(pDesEndPos->endPosType==0		//�˵㼴�ڵ�
					||pDesEndPos->endPosType==1		//����ڽڵ㸸�ֹܸ˼���
					||pDesEndPos->endPosType==3)	//�˼����߽���
					end -= workNorm*pDesEndPos->fNormOffset;
			}
		}
	}
	f3dPoint vec=end-start;
	return vec.mod();*/
}
//�ԳƸ˼���񡢲��ʡ���׼����
bool CLDSLinePart::IsEqualWith(CLDSLinePart *pLinePart,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
							   int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{	
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHK_MIRROD_BASICINFO)
	{	
		bool bRet=true;
		//�Ƚϸ˼�����
		if(pLinePart->GetClassTypeId()!=GetClassTypeId())
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"The rod type is different between 0x%X and 0x%X��",handle,pLinePart->handle);
#else 
				sprintf(error_info,"0x%X��0x%X�˼����Ͳ�ͬ��",handle,pLinePart->handle);
#endif
			bRet=false;
		}
		//�Ƚϸ˼����
		else if(fabs(pLinePart->GetWidth()-GetWidth())>eps||fabs(pLinePart->GetThick()-GetThick())>eps)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"The rod specification is different between 0x%X and 0x%X��",handle,pLinePart->handle);
#else 
				sprintf(error_info,"0x%X��0x%X�˼����ͬ��",handle,pLinePart->handle);
#endif
			bRet=false;
		}
		//�Ƚϸ˼�����
		else if(pLinePart->cMaterial!=cMaterial)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"The rod material is different between 0x%X and 0x%X��",handle,pLinePart->handle);
#else 
				sprintf(error_info,"0x%X��0x%X�˼����ʲ�ͬ��",handle,pLinePart->handle);
#endif
			bRet= false;
		}
		if(!bRet)
		{
			if(pErrorInfo!=NULL)
				*pErrorInfo=CHK_MIRROD_BASICINFO;
			return false;
		}
	}
	if(pChkStyleFlag==NULL || (*pChkStyleFlag)&CHK_MIRROD_LENGTHINFO)
	{
		bool bRet=true;
		//�Ƚϸ˼���׼���ȣ���ʱû��ʵ��
		if(fabs(pLinePart->GetDatumLength()-GetDatumLength())>epsilon)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"The rod length is different between 0x%X and 0x%X��",handle,pLinePart->handle);
#else 
				sprintf(error_info,"0x%X��0x%X�˼���׼���Ȳ�ͬ��",handle,pLinePart->handle);
#endif
			bRet= false;
		}
		if(!bRet)
		{
			if(pErrorInfo!=NULL)
				*pErrorInfo=CHK_MIRROD_LENGTHINFO;
			return false;
		}
	}
	return true;
}

CLDSGeObject::CLDSGeObject()
{
	m_nClassTypeId = CLS_GEOBJECT;
	//strcpy(m_sClassName,"CLDSGeObject");
}
bool CLDSGeObject::IsAcceptableTypeId(int type_id)
{
	if(type_id==CLS_OBJECT||type_id==CLS_GEOBJECT)
		return true;
	else if(type_id==m_nClassTypeId)
		return true;
	else
		return false;
}
long HANDLEEX::set_hPoint(long _hPoint)
{
	if(handle&0x80000000)
		handle=0x80000000|_hPoint;
	else
		handle=0x7fffffff&_hPoint;
	return 0x7fffffff&_hPoint;
}
long HANDLEEX::get_hPoint()
{
	return 0x7fffffff&handle;
}
bool HANDLEEX::set_bSimpleNodePoint(bool _bSimpleNodePoint)
{
	if(_bSimpleNodePoint)
		handle|=0x80000000;
	else
		handle&=0x7fffffff;
	return _bSimpleNodePoint;
}
bool HANDLEEX::get_bSimpleNodePoint()
{
	return (handle&0x80000000)!=0;
}
bool HANDLEPOINT::set_bSimpleNodePoint(bool _bSimpleNodePoint)
{
	if(_bSimpleNodePoint)
		ciHidType=1;
	else if(ciHidType==1)
		ciHidType=0;
	return _bSimpleNodePoint;
}
bool HANDLEPOINT::get_bSimpleNodePoint()
{
	return ciHidType==1;
}
bool HANDLEPOINT::set_bRodTipStart(bool _bRodTipPoint)
{
	if(_bRodTipPoint)
		ciHidType='S';
	else if(ciHidType=='S')
		ciHidType='E';
	return _bRodTipPoint;
}
bool HANDLEPOINT::get_bRodTipStart()
{
	return ciHidType=='S';
}
bool HANDLEPOINT::set_bRodTipEnd(bool _bRodTipPoint)
{
	if(_bRodTipPoint)
		ciHidType='E';
	else if(ciHidType=='E')
		ciHidType='S';
	return _bRodTipPoint;
}
bool HANDLEPOINT::get_bRodTipEnd()
{
	return ciHidType=='E';
}
HANDLEPOINT::HANDLEPOINT(long _hPoint/*=0*/,char _ciHidType/*=0*/){
	memset(this,0,4);
	ciHidType=_ciHidType;
	handle=_hPoint;
}
bool CLDSLine::IsSameWith(const double* pxLineOrg,const double* pvLineVec)
{
	return false;
}

/*f3dPoint CLDSPoint::Position()
{
	if(IsShadowObject())
	{
		if(m_iShadowObjType==1)
		{
			CTower *pTower=(CTower*)m_pModel;
			CParaCS *pAcs=pTower->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
			if(pAcs)
				pos=pAcs->m_xMirInfo.MirPoint(pos);
		}
		else if(m_iShadowObjType==2||m_iShadowObjType==3)
		{
			CBlockParameter blockPara;
			if(GetBlockParameter(blockPara))
			{
				UCS_STRU cs=blockPara.m_pBlock->TransToACS(blockPara.m_pBlockRef->GetACS());
				coord_trans(pos,cs,TRUE);
			}
		}
	}
	return pos;
}*/
f3dPoint CLDSPoint::SetRsltPositionOnly(const double* pxRsltPos)
{
	pos = GEPOINT(pxRsltPos);
	return pos;
}
f3dPoint CLDSPoint::SetPosition(const f3dPoint& position)
{
	pos.Set(position.x,position.y,position.z);
	if(datum_pos_style!=9)
		datum_pos=pos;
	return pos;
}
f3dPoint CLDSPoint::SetPosition(double x,double y,double z)
{
	pos.x=x;
	pos.y=y;
	pos.z=z;
	if(datum_pos_style!=9)
		datum_pos=pos;
	return pos;
}
f3dPoint CLDSPoint::SetPositionX(double x)
{
	pos.x=x;
	if(datum_pos_style!=9)
		datum_pos.x=pos.x;
	return pos;
}
f3dPoint CLDSPoint::SetPositionY(double y)
{
	pos.y=y;
	if(datum_pos_style!=9)
		datum_pos.y=pos.y;
	return pos;
}
f3dPoint CLDSPoint::SetPositionZ(double z)
{
	pos.z=z;
	if(datum_pos_style!=9)
		datum_pos.z=pos.z;
	return pos;
}
bool ReplaceReferenceHandleOfNormOffset(char* norm_offset,CHandleReplacer& replacer)
{	//�滻����ƫ�����ַ����еľ��
	CDesignThickPara paraThick(norm_offset);
	bool replaced=paraThick.ReplaceReferenceHandle(replacer);
	strcpy(norm_offset,paraThick.key_str);
	return replaced;
}
bool CLDSPoint::ReplaceRodEndNode(long hRod,long hOldEndNode,long hNewEndNode)
{
	if(datum_pos_style==2&&des_para.hPart==hRod&&des_para.hNode==hOldEndNode)
	{
		des_para.hNode=hNewEndNode;
		return true;
	}
	return false;
}
bool CLDSPoint::ReplaceReferenceHandle(CHandleReplacer& replacer,double* pickpos_coord,int idClassType/*=0*/)
{
	bool replaced=false;
	if(replacer.hOld==0)
		handle+=replacer.hNew;
	else if(handle==replacer.hOld&&replacer.IsReplaceObjSelfHandle())
		handle=replacer.hNew;
	if(!replacer.SelectPerfectPartHandle(pickpos_coord))
		return false;
	long hDesPart=des_para.hPart;
	if(idClassType==0||idClassType==CLS_NODE)
		replaced|=replacer.VerifyAndReplace(&des_para.hNode);
	if(datum_pos_style==1)
	{
		if(idClassType==0||idClassType==CLS_LINEANGLE)
		{
			replaced|=replacer.VerifyAndReplace(&des_para.RODEND.hRod);
			replaced|=ReplaceReferenceHandleOfNormOffset(des_para.RODEND.norm_offset,replacer);
			if(hDesPart==replacer.hOld&&replacer.IsReverseWingXAndWingY()&&des_para.RODEND.offset_wing<2)
				des_para.RODEND.offset_wing=1-des_para.RODEND.offset_wing;
		}
	}
	else if(datum_pos_style==2)
	{
		if(idClassType==0||idClassType==CLS_LINEANGLE||idClassType==CLS_LINETUBE
			||idClassType==CLS_LINEFLAT||idClassType==CLS_LINESLOT)
		{
			replaced|=replacer.VerifyAndReplace(&des_para.RODNODE.hRod);
			if(hDesPart==replacer.hOld&&replacer.IsReverseWingXAndWingY()&&des_para.RODNODE.offset_wing<2)
				des_para.RODNODE.offset_wing=1-des_para.RODNODE.offset_wing;
		}
		if(idClassType==0||idClassType==CLS_NODE)
			replaced|=replacer.VerifyAndReplace(&des_para.RODNODE.hNode);
		replaced|=ReplaceReferenceHandleOfNormOffset(des_para.RODNODE.norm_offset,replacer);
	}
	else if(datum_pos_style==3)	//�Ǹ����߽���
	{
		if(idClassType==0||idClassType==CLS_LINEANGLE||idClassType==CLS_LINETUBE
			||idClassType==CLS_LINEFLAT||idClassType==CLS_LINESLOT)
		{
			replaced|=replacer.VerifyAndReplace(&des_para.AXIS_INTERS.hDatum1);
			replaced|=replacer.VerifyAndReplace(&des_para.AXIS_INTERS.hDatum2);
			replaced|=ReplaceReferenceHandleOfNormOffset(des_para.AXIS_INTERS.norm_offset,replacer);
		}
	}
	else if(datum_pos_style==4)	//��׼�߿ռ佻��
	{
		if(idClassType==0||idClassType==CLS_GELINE)
		{
			replaced|=replacer.VerifyAndReplace(&des_para.AXIS_INTERS.hDatum1);
			replaced|=replacer.VerifyAndReplace(&des_para.AXIS_INTERS.hDatum2);
			replaced|=ReplaceReferenceHandleOfNormOffset(des_para.AXIS_INTERS.norm_offset,replacer);
		}
	}
	else if(datum_pos_style==5)	//��׼�����������겻���
	{
		if(idClassType==0||idClassType==CLS_GELINE)
			replaced|=replacer.VerifyAndReplace(&des_para.DATUMLINE.hDatumLine);
	}
	else if(datum_pos_style==6)	//��׼����ָ�������
	{
		if(idClassType==0||idClassType==CLS_GELINE)
			replaced|=replacer.VerifyAndReplace(&des_para.DATUMPLANE.hDatumPlane);
	}
	else if(datum_pos_style==7)	//�Ǹ�����������
	{
		if(idClassType==0||idClassType==CLS_LINEANGLE)
		{
			replaced|=replacer.VerifyAndReplace(&des_para.RODEND.hRod);
			replaced|=ReplaceReferenceHandleOfNormOffset(des_para.RODEND.norm_offset,replacer);
		}
	}
	else if(datum_pos_style==8)	//��ڵ�ͬλ
	{
		if(idClassType==0||idClassType==CLS_NODE)
			replaced|=replacer.VerifyAndReplace(&des_para.hNode);
	}
	else if(datum_pos_style==9)	//ָ�������ϵ��������
	{
		if(idClassType==0||idClassType!=CLS_NODE)
			replaced|=replacer.VerifyAndReplace(&des_para.hPart);
	}
	else if(datum_pos_style==10)
	{
		if(idClassType!=CLS_NODE&&replacer.VerifyAndReplace(&des_para.SCALE_NODE.hLinePart))//==replacer.hOld)
			replaced=true;
		else if(replacer.VerifyAndReplace(&des_para.SCALE_NODE.hStartNode))//==replacer.hOld)
			replaced=true;
		else if(replacer.VerifyAndReplace(&des_para.SCALE_NODE.hEndNode))//==replacer.hOld)
			replaced=true;
	}
	else if(datum_pos_style==11)
	{
		if(idClassType!=CLS_NODE&&replacer.VerifyAndReplace(&des_para.COLUMN_INTERS.hLineTube))//==replacer.hOld)
			replaced=true;
	}
	//else
	if(replaced)
		SetModified();
	return replaced;
}
class CUniqueHandleAddrAdder {
	IXhList<HANDLE_ADDR>* m_pListHandlers;
	UINT m_uCount, m_uMaxCount;
	UINT* m_pAttachAddrArray;
	bool SafeAdd(UINT* pVarAddr, char ciAddrType, WORD wnStrBuffLen=0)
	{
		UINT id = (UINT)pVarAddr;
		if (*pVarAddr == 0)
			return false;
		for (UINT i = 0; i < m_uCount; i++)
		{
			if (m_pAttachAddrArray[i] == id)
				return false;	//��ַ�ظ������������
		}
		if (m_uCount < m_uMaxCount)
		{
			m_pAttachAddrArray[m_uCount] = id;
			if(ciAddrType==0)
				m_pListHandlers->Append(HANDLE_ADDR((long*)pVarAddr));
			else
				m_pListHandlers->Append(HANDLE_ADDR((char*)pVarAddr,wnStrBuffLen));
			m_uCount++;
		}
		else
			return false;
		return true;
	}
public:
	CUniqueHandleAddrAdder(IXhList<HANDLE_ADDR>* pListHandlers, UINT* pAttachIdArray = NULL, UINT uMaxCount = 0)
	{
		m_pListHandlers = pListHandlers;
		m_pAttachAddrArray = pAttachIdArray;
		m_uCount = 0;
		m_uMaxCount = uMaxCount;
	}
	bool SafeAdd(char* pszStrVarAddr)
	{
		return SafeAdd((UINT*)pszStrVarAddr, 1);
	}
	bool SafeAdd(long* pHandlAddr)
	{
		return SafeAdd((UINT*)pHandlAddr, 0);
	}
	bool SafeAdd(UINT* pHandlAddr)
	{
		return SafeAdd(pHandlAddr, 0);
	}
	UINT get_uAddCount() { return m_uCount; }
	__declspec(property(get = get_uAddCount)) UINT uAddCount;
};
long CLDSPoint::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	UINT xarrId[8] = { 0 };
	CUniqueHandleAddrAdder uniaddr(pListHandlers,xarrId,8);
	uniaddr.SafeAdd(&handle);
	uniaddr.SafeAdd(&des_para.hPart);
	uniaddr.SafeAdd(&des_para.hNode);
	if (datum_pos_style == 1)
	{
		uniaddr.SafeAdd(&des_para.RODEND.hRod);
		uniaddr.SafeAdd(des_para.RODEND.norm_offset);
	}
	else if (datum_pos_style == 2)
	{
		uniaddr.SafeAdd(&des_para.RODNODE.hRod);
		uniaddr.SafeAdd(&des_para.RODNODE.hNode);
		uniaddr.SafeAdd(des_para.RODNODE.norm_offset);
	}
	else if (datum_pos_style == 3|| datum_pos_style == 4)	//�Ǹ�����(��ռ��׼��)����
	{
		uniaddr.SafeAdd(&des_para.AXIS_INTERS.hDatum1);
		uniaddr.SafeAdd(&des_para.AXIS_INTERS.hDatum2);
		uniaddr.SafeAdd(des_para.AXIS_INTERS.norm_offset);
	}
	else if (datum_pos_style == 5)	//��׼�����������겻���
		uniaddr.SafeAdd(&des_para.DATUMLINE.hDatumLine);
	else if (datum_pos_style == 6)	//��׼����ָ�������
	{
		uniaddr.SafeAdd(&des_para.DATUMPLANE.hDatumPlane);
	}
	else if (datum_pos_style == 7)	//�Ǹ�����������
	{
		uniaddr.SafeAdd(&des_para.RODEND.hRod);
		uniaddr.SafeAdd(des_para.RODEND.norm_offset);
	}
	else if (datum_pos_style == 8)	//��ڵ�ͬλ
	{
		uniaddr.SafeAdd(&des_para.hNode);
		uniaddr.SafeAdd(des_para.RODEND.norm_offset);
	}
	else if (datum_pos_style == 9)	//ָ�������ϵ��������
	{
		uniaddr.SafeAdd(&des_para.hPart);
	}
	else if (datum_pos_style == 10)
	{
		uniaddr.SafeAdd(&des_para.SCALE_NODE.hLinePart);
		uniaddr.SafeAdd(&des_para.SCALE_NODE.hStartNode);
		uniaddr.SafeAdd(&des_para.SCALE_NODE.hEndNode);
	}
	else if (datum_pos_style == 11)
	{
		uniaddr.SafeAdd(&des_para.COLUMN_INTERS.hLineTube);
	}
	return uniaddr.uAddCount;
}
void CLDSPoint::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	if(pModel==NULL)
	{
		if(m_pModel!=NULL)
			pModel=m_pModel;
		else
			pModel=m_pTower;
	}
	if(pModel==NULL)
		return;
	pModel->ValidateObjRefH(&des_para.hNode);
	if(datum_pos_style==1)
	{
		pModel->ValidateObjRefH(&des_para.RODEND.hRod);
		//replaced|=ReplaceReferenceHandleOfNormOffset(des_para.RODEND.norm_offset,replacer);
	}
	else if(datum_pos_style==2)
	{
		pModel->ValidateObjRefH(&des_para.RODNODE.hRod);
		pModel->ValidateObjRefH(&des_para.RODNODE.hNode);
		//replaced|=ReplaceReferenceHandleOfNormOffset(des_para.RODNODE.norm_offset,replacer);
	}
	else if(datum_pos_style==3||datum_pos_style==4)	//�Ǹ����߽���/��׼�߿ռ佻��
	{
		pModel->ValidateObjRefH(&des_para.AXIS_INTERS.hDatum1);
		pModel->ValidateObjRefH(&des_para.AXIS_INTERS.hDatum2);
		//replaced|=ReplaceReferenceHandleOfNormOffset(des_para.AXIS_INTERS.norm_offset,replacer);
	}
	else if(datum_pos_style==5)	//��׼�����������겻���
	{
		pModel->ValidateObjRefH(&des_para.DATUMLINE.hDatumLine);
	}
	else if(datum_pos_style==6)	//��׼����ָ�������
	{
		pModel->ValidateObjRefH(&des_para.DATUMPLANE.hDatumPlane);
	}
	else if(datum_pos_style==7)	//�Ǹ�����������
	{
		pModel->ValidateObjRefH(&des_para.RODEND.hRod);
		//replaced|=ReplaceReferenceHandleOfNormOffset(des_para.RODEND.norm_offset,replacer);
	}
	else if(datum_pos_style==8)	//��ڵ�ͬλ
	{
		pModel->ValidateObjRefH(&des_para.hNode);
	}
	else if(datum_pos_style==9)	//ָ�������ϵ��������
	{
		pModel->ValidateObjRefH(&des_para.hPart);
	}
	else if(datum_pos_style==10)
	{
		pModel->ValidateObjRefH(&des_para.SCALE_NODE.hLinePart);
		pModel->ValidateObjRefH(&des_para.SCALE_NODE.hStartNode);
		pModel->ValidateObjRefH(&des_para.SCALE_NODE.hEndNode);
	}
	else if(datum_pos_style==11)
	{
		pModel->ValidateObjRefH(&des_para.COLUMN_INTERS.hLineTube);
	}
}
void CLDSPoint::DES_PARA::tagRODEND::SetWingOffset(double wing_offset/*=0*/,short offset_g_style/*=4*/)
{
	wing_offset_dist=wing_offset;
	wing_offset_style=offset_g_style;
}
void CLDSPoint::DES_PARA::tagCOLUMN_INTERS::SetAxisVector(double vx,double vy,double vz)
{
	cPointAndVectorFlag&=0XFE;	//��ձ�ʶλ 0xFE=0xFF^0x01
	AxisVector.axis_vec[0]=vx;
	AxisVector.axis_vec[1]=vy;
	AxisVector.axis_vec[2]=vz;
}
void CLDSPoint::DES_PARA::tagCOLUMN_INTERS::SetSectCenter(double cx,double cy,double cz)
{
	cPointAndVectorFlag&=0XFD;	//��ձ�ʶλ 0xFE=0xFF^0x02
	SectCenter.sect_center[0]=cx;
	SectCenter.sect_center[1]=cy;
	SectCenter.sect_center[2]=cz;
}
void CLDSPoint::DES_PARA::tagCOLUMN_INTERS::SetSectNorm(double vx,double vy,double vz)
{
	cPointAndVectorFlag&=0XFB;	//��ձ�ʶλ 0xFE=0xFF^0x04
	SectNorm.sect_norm[0]=vx;
	SectNorm.sect_norm[1]=vy;
	SectNorm.sect_norm[2]=vz;
}
void CLDSPoint::DES_PARA::tagCOLUMN_INTERS::SetRayVector(double vx,double vy,double vz)
{
	cPointAndVectorFlag&=0XF7;	//��ձ�ʶλ 0xFE=0xFF^0x08
	RayVector.ray_vec[0]=vx;
	RayVector.ray_vec[1]=vy;
	RayVector.ray_vec[2]=vz;
}
#ifdef __ALFA_TEST_VERSION_
extern BOOL gbLegacyMode/*=FALSE*/;
#endif
bool CLDSPoint::OffsetPosParam(double offset,const double* vOffsetDirection)
{
	GEPOINT vDirection(vOffsetDirection);
	pos+=vDirection*offset;
	if(m_pModel==NULL)
		return false;
	if(vDirection.IsZero())
		return false;
	else if(datum_pos_style==1)	//�Ǹ���㶨λ��
	{
		CLDSLinePart* pLinePart=(CLDSLinePart*)m_pModel->FromPartHandle(des_para.RODEND.hRod,CLS_LINEPART);
		if(pLinePart==NULL||!pLinePart->IsAngle())
			return false; //throw "���㶨λ����ȱ�ٻ�׼�Ǹ���Ϣ";
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pLinePart;
		if(des_para.RODEND.offset_wing==0 && fabs(pAngle->vxWingNorm*vDirection)>EPS_COS)
		{
			CParaThick parathick(des_para.RODEND.norm_offset);
			parathick.AddThick(ftoi(offset));
			StrCopy(des_para.RODEND.norm_offset,parathick.ToThickStr(true),51);
			return true;
		}
		else if(des_para.RODEND.offset_wing==1 && fabs(pAngle->vyWingNorm*vDirection)>EPS_COS)
		{
			CParaThick parathick(des_para.RODEND.norm_offset);
			parathick.AddThick(ftoi(offset));
			StrCopy(des_para.RODEND.norm_offset,parathick.ToThickStr(true),51);
			return true;
		}
	}
	else if(datum_pos_style==2)	//�˼��Ͻڵ㶨λ��
	{
		f3dPoint offset_vec;
		SmartRodPtr pBaseRod=m_pModel->FromRodHandle(des_para.RODNODE.hRod);
		if(pBaseRod.IsNULL())
			return FALSE;	//throw "���㶨λ����ȱ�ٸ˼���Ϣ";
		CLDSLineAngle delegateAngle;
		SmartRodPtr pLinePart;
		if(pBaseRod->IsAngle()&&des_para.RODNODE.offset_wing==0 && fabs(pBaseRod.pAngle->vxWingNorm*vDirection)>EPS_COS)
		{
			CParaThick parathick(des_para.RODNODE.norm_offset);
			parathick.AddThick(ftoi(offset));
			StrCopy(des_para.RODNODE.norm_offset,parathick.ToThickStr(true),51);
			return true;
		}
		else if(des_para.RODEND.offset_wing==1 && fabs(pBaseRod.pAngle->vyWingNorm*vDirection)>EPS_COS)
		{
			CParaThick parathick(des_para.RODNODE.norm_offset);
			parathick.AddThick(ftoi(offset));
			StrCopy(des_para.RODNODE.norm_offset,parathick.ToThickStr(true),51);
			return true;
		}
	}
	return false;
}
IMPLEMENT_GET(CLDSPoint,m_hNode){
	if(datum_pos_style==2)
		return des_para.RODNODE.hNode;
	else if(datum_pos_style==8)
		return des_para.hNode;	//��ڵ�ͬλ
	else //if(datum_pos_style==0)
		return 0;
}
IMPLEMENT_GET(CLDSPoint,m_hDatumRod){
	if(datum_pos_style==1||datum_pos_style==7)
		return des_para.RODEND.hRod;
	else if(datum_pos_style==2)
		return des_para.RODNODE.hRod;
	else //if(datum_pos_style==0)
		return 0;
}
bool CParaPointLifeObj::m_bLivingState=false;
long CParaPointLifeObj::CallAngleIds[2]={0,0};
CHashList<bool> CParaPointLifeObj::hashAngleLockStates;
CParaPointLifeObj::CParaPointLifeObj()
{
	m_bEnabled=!m_bLivingState;
	m_bLivingState=true;
}
CParaPointLifeObj::~CParaPointLifeObj()
{
	if(m_bEnabled)
	{
		m_bLivingState=false;
		CallAngleIds[0]=CallAngleIds[1]=0;
		hashAngleLockStates.Empty();
	}
}
bool CParaPointLifeObj::LockCall(long hAngle)	//ָ���Ǹ�δ����ʱ����������true,���򷵻�false wjh-2018.1.5
{
	if(hAngle<=0)
		return false;
	if(CallAngleIds[0]==hAngle||CallAngleIds[1]==hAngle)
		return false;
	else if(CallAngleIds[0]>0&&CallAngleIds[1]>0&&hashAngleLockStates.GetValue(hAngle)!=NULL)
		return false;
	if(m_bLivingState)
	{
		if(CallAngleIds[0]==0)
			CallAngleIds[0]=hAngle;
		else if(CallAngleIds[1]==0)
			CallAngleIds[1]=hAngle;
		else
			hashAngleLockStates.SetValue(hAngle,true);
	}
	return true;
}
BOOL CLDSPoint::UpdatePos(IModel *pTower,CLDSArcLift* pLift,bool bReverseLift/*=false*/)
{
	if(pTower==NULL)
	{
		if(m_pModel!=NULL)
			pTower=m_pModel;
		else
			pTower=m_pTower;
	}
	if(pTower==NULL)
		return FALSE;
	if(datum_pos_style==0)
	{
		if(pLift)
			pos=pLift->LiftPosTrans(datum_pos);
		else
			pos=datum_pos;
		return TRUE;
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(datum_pos_style==1)	//�Ǹ���㶨λ��
	{
		f3dPoint offset_vec;
		CLDSLinePart* pLinePart=(CLDSLinePart*)pTower->FromPartHandle(des_para.RODEND.hRod,CLS_LINEPART);
		if(pLinePart==NULL)
			return FALSE; //throw "���㶨λ����ȱ�ٻ�׼�Ǹ���Ϣ";
		if(pLinePart->IsAngle())
		{
			CSmartPtr<CLDSLineAngle> pJg=(CLDSLineAngle*)pLinePart;
			if(des_para.RODEND.wing_offset_style==0)
			{//��׼׼��
				if(des_para.RODEND.direction==0)
				{
					if(des_para.RODEND.offset_wing==0)
						des_para.RODEND.wing_offset_dist=pJg->GetNodeWingXG(pJg->pStart);
					else
						des_para.RODEND.wing_offset_dist=pJg->GetNodeWingYG(pJg->pStart);
				}
				else
				{
					if(des_para.RODEND.offset_wing==0)
						des_para.RODEND.wing_offset_dist=pJg->GetNodeWingXG(pJg->pEnd);
					else
						des_para.RODEND.wing_offset_dist=pJg->GetNodeWingYG(pJg->pEnd);
				}
			}
			else if(des_para.RODEND.wing_offset_style!=4)
			{
				if(des_para.RODEND.offset_wing==0)
					des_para.RODEND.wing_offset_dist=pJg->GetWingXG(des_para.RODEND.wing_offset_style);
				else
					des_para.RODEND.wing_offset_dist=pJg->GetWingYG(des_para.RODEND.wing_offset_style);
			}
			if(des_para.RODEND.direction==0)		//ʼ->��
			{
				offset_vec = pJg->End()-pJg->Start();
				normalize(offset_vec);
				pos = pJg->Start()+offset_vec*des_para.RODEND.len_offset_dist;
				if(des_para.RODEND.bIncOddEffect)
					pos-=offset_vec*pJg->startOdd();
			}
			else
			{
				offset_vec = pJg->Start()-pJg->End();
				normalize(offset_vec);
				pos = pJg->End()+offset_vec*des_para.RODEND.len_offset_dist;
				if(des_para.RODEND.bIncOddEffect)
					pos-=offset_vec*pJg->endOdd();
			}
			CDesignThickPara thick(des_para.RODEND.norm_offset);
			double norm_offset_dist=thick.Thick(pTower);
			if(des_para.RODEND.offset_wing==0)	//X֫
			{
				pos += pJg->CalWingOffsetVec('X',des_para.RODEND.wing_offset_dist);//*pJg->GetWingVecX();
				pos += pJg->get_norm_x_wing()*norm_offset_dist;
			}
			else							//Y֫
			{
				pos += pJg->CalWingOffsetVec('Y',des_para.RODEND.wing_offset_dist);//*pJg->GetWingVecY();
				pos += pJg->get_norm_y_wing()*norm_offset_dist;
			}
		}
		else 
		{	//���Ǹ���������˼�
			if(des_para.RODEND.direction==0)		//ʼ->��
			{
				offset_vec = pLinePart->End()-pLinePart->Start();
				normalize(offset_vec);
				pos = pLinePart->Start()+offset_vec*des_para.RODEND.len_offset_dist;
				if(des_para.RODEND.bIncOddEffect)
					pos-=offset_vec*pLinePart->startOdd();
			}
			else
			{
				offset_vec = pLinePart->Start()-pLinePart->End();
				normalize(offset_vec);
				pos = pLinePart->End()+offset_vec*des_para.RODEND.len_offset_dist;
				if(des_para.RODEND.bIncOddEffect)
					pos-=offset_vec*pLinePart->endOdd();
			}
			if (pLinePart->GetClassTypeId() == CLS_LINESLOT)
			{	//�������λ�ڲ۸���֫�����  wxc-2019.7.26
				if (des_para.RODEND.offset_wing == 1)			//X+֫
					pos += pLinePart->ucs.axis_x*pLinePart->size_height*0.5;
				else if (des_para.RODEND.offset_wing == 2)	//X-֫
					pos -= pLinePart->ucs.axis_x*pLinePart->size_height*0.5;
				if (des_para.RODEND.offset_wing > 0 && des_para.RODEND.wing_offset_dist > 0)
					pos += pLinePart->ucs.axis_y*des_para.RODEND.wing_offset_dist;
			}
		}
	}
	else if(datum_pos_style==2)	//�˼��Ͻڵ㶨λ��
	{
		f3dPoint offset_vec;
		SmartPartPtr pBasePart=pTower->FromPartHandle(des_para.RODNODE.hRod);
		CSmartPtr<CLDSNode>pNode=pTower->FromNodeHandle(des_para.RODNODE.hNode);
		if(pNode.IsNULL())
			return FALSE;	//throw "���㶨λ����ȱ�ٽڵ���Ϣ";
		if(pBasePart.IsNULL())
			return FALSE;	//throw "���㶨λ����ȱ�ٸ˼���Ϣ";
		CLDSLineAngle* pBackOnAngle=NULL;
		if(pBasePart->IsAngle()&&(pBasePart.pRod->pStart==NULL||pBasePart.pRod->pEnd==NULL))
		{
			if( pBasePart.pAngle->desStartPos.spatialOperationStyle==0&&pBasePart.pAngle->desStartPos.datumPoint.m_hDatumRod>0x20)
				pBackOnAngle=(CLDSLineAngle*)pTower->FromPartHandle(pBasePart.pAngle->desStartPos.datumPoint.m_hDatumRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
			if( pBackOnAngle==NULL&&
				pBasePart.pAngle->desEndPos.spatialOperationStyle==0&&pBasePart.pAngle->desEndPos.datumPoint.m_hDatumRod>0x20)
				pBackOnAngle=(CLDSLineAngle*)pTower->FromPartHandle(pBasePart.pAngle->desEndPos.datumPoint.m_hDatumRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		}
		CLDSLineAngle delegateAngle;
		SmartRodPtr pLinePart;
		if(pBasePart->IsLinePart())
			pLinePart=pBasePart.LinePartPointer();
		else
		{
			delegateAngle.Fabricate(pBasePart,pNode->Position());
			pLinePart=&delegateAngle;
		}
		if(pLinePart->GetClassTypeId()==CLS_LINEANGLE||pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			if(des_para.RODNODE.wing_offset_style==0)
			{//��׼׼��
				if(des_para.RODNODE.offset_wing==0)
					des_para.RODNODE.wing_offset_dist=pLinePart.LineAnglePointer()->GetNodeWingXG(pNode);
				else
					des_para.RODNODE.wing_offset_dist=pLinePart.LineAnglePointer()->GetNodeWingYG(pNode);
			}
			else if(des_para.RODNODE.wing_offset_style!=4)
			{
				if(des_para.RODNODE.offset_wing==0)
					des_para.RODNODE.wing_offset_dist=pLinePart.LineAnglePointer()->GetWingXG(des_para.RODNODE.wing_offset_style);
				else
					des_para.RODNODE.wing_offset_dist=pLinePart.LineAnglePointer()->GetWingYG(des_para.RODNODE.wing_offset_style);
			}
			if(des_para.RODNODE.direction==0)		//ʼ->��
				offset_vec = pLinePart->End()-pLinePart->Start();
			else
				offset_vec = pLinePart->Start()-pLinePart->End();
			normalize(offset_vec);
			CDesignThickPara thick(des_para.RODNODE.norm_offset);
			double norm_offset_dist=thick.Thick(pTower);
			f3dPoint face_norm(des_para.RODNODE.vPlaneNormal);
			double dfErrorIntersPlaneNormalDetect = fabs(offset_vec*face_norm);
			if (des_para.RODNODE.bFaceOffset&&dfErrorIntersPlaneNormalDetect < 0.2)
				logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "��׼��(�˼�0x%x@�ڵ�0x%x)ƽ���淨�����ݴ���", pLinePart->handle, des_para.RODNODE.hNode);
			if(des_para.RODNODE.bFaceOffset&&!face_norm.IsZero()&& dfErrorIntersPlaneNormalDetect >0.2)
			{
				f3dLine axisLine;
				if(des_para.RODNODE.offset_wing==0)	//X֫
				{
					axisLine.startPt=pLinePart.LineAnglePointer()->Start()+
						pLinePart.LineAnglePointer()->CalWingOffsetVec('X',des_para.RODNODE.wing_offset_dist);//GetWingVecX();
					axisLine.endPt=pLinePart.LineAnglePointer()->End()+
						pLinePart.LineAnglePointer()->CalWingOffsetVec('X',des_para.RODNODE.wing_offset_dist);//GetWingVecX();
				}
				else
				{
					axisLine.startPt=pLinePart.LineAnglePointer()->Start()+
						pLinePart.LineAnglePointer()->CalWingOffsetVec('Y',des_para.RODNODE.wing_offset_dist);//GetWingVecY();
					axisLine.endPt=pLinePart.LineAnglePointer()->End()+
						pLinePart.LineAnglePointer()->CalWingOffsetVec('Y',des_para.RODNODE.wing_offset_dist);//GetWingVecY();
				}
				f3dPoint pt(pos);
				Int3dlf(pt,axisLine,pNode->Position(true),face_norm);
				pos=pt;
				if(des_para.RODNODE.offset_wing==0)	//X֫
					pos +=pLinePart.LineAnglePointer()->get_norm_x_wing()*norm_offset_dist;
				else							//Y֫
					pos +=pLinePart.LineAnglePointer()->get_norm_y_wing()*norm_offset_dist;
				//ƽ��ʱ������ƫ�� 
				pos += offset_vec*des_para.RODNODE.len_offset_dist;
			}
			else
			{
				if(pBackOnAngle)
				{
					if(pNode==pBackOnAngle->pStart&&des_para.RODNODE.hRod==pBackOnAngle->handle)
						pos=pBackOnAngle->Start();
					else if(pNode==pBackOnAngle->pEnd&&des_para.RODNODE.hRod==pBackOnAngle->handle)
						pos=pBackOnAngle->End();
					else
					{
						bool bClearLock=CParaPointLifeObj::LockCall(pBackOnAngle->handle);
						pos=pBackOnAngle->GetDatumPosBer(pNode,0,bClearLock);
						f3dPoint perp=pos;
						SnapPerp(&perp,pLinePart->Start(),pLinePart->End(),pos);
						pos=perp;
					}
				}
				else
				{
					//ֻ�е��ڵ�ǵ�ǰ�˼��˽ڵ�ʱ�ſ��ǵ���GetDatumPosBer��������ܻ����ڹ̶�����ͷ����ʱ�漰CLDSPoint::UpdatePos->CLDSLineAngle::GetDatumPosBer������ѭ����wjh-2018.1.5
					if(pNode==pLinePart->pStart&&des_para.RODNODE.hRod==pLinePart->handle)
						pos=pLinePart->Start();
					else if(pNode==pLinePart->pEnd&&des_para.RODNODE.hRod==pLinePart->handle)
						pos=pLinePart->End();
					else
					{
						bool bClearLock=CParaPointLifeObj::LockCall(pLinePart->handle);
						pos = pLinePart.LineAnglePointer()->GetDatumPosBer(pNode,0,bClearLock);
					}
				}
				pos+=offset_vec*des_para.RODNODE.len_offset_dist;
				if(des_para.RODNODE.offset_wing==0)	//X֫
				{
					pos += pLinePart.LineAnglePointer()->CalWingOffsetVec('X',des_para.RODNODE.wing_offset_dist);//GetWingVecX();
					pos +=pLinePart.LineAnglePointer()->get_norm_x_wing()*norm_offset_dist;
				}
				else							//Y֫
				{
					pos += pLinePart.LineAnglePointer()->CalWingOffsetVec('Y',des_para.RODNODE.wing_offset_dist);//GetWingVecY();
					pos +=pLinePart.LineAnglePointer()->get_norm_y_wing()*norm_offset_dist;
				}
			}
		}
		else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
		{
			if(des_para.RODNODE.direction==0)		//ʼ->��
				offset_vec=(pLinePart->End()-pLinePart->Start()).normalized();
			else
				offset_vec=(pLinePart->Start()-pLinePart->End()).normalized();
			f3dPoint perp=pos;
			if(pNode->m_cPosCalType==4)	//�����Ӧȡ��������˼���ʵ�ʽ��㣨�Ǹ�Ϊ���߽��㣬�ֹ�Ϊ���߽��㣩
				pNode->GetIntersZhun(&perp);
			else
				SnapPerp(&perp,pLinePart->Start(),pLinePart->End(),pNode->Position(true));
			pos=perp;
			pos+=offset_vec*des_para.RODNODE.len_offset_dist;
			//
			if(des_para.RODNODE.offset_wing==1)			//X+֫
				pos+=pLinePart->ucs.axis_x*pLinePart->size_height*0.5;
			else if(des_para.RODNODE.offset_wing==2)	//X-֫
				pos-=pLinePart->ucs.axis_x*pLinePart->size_height*0.5;
			if(des_para.RODNODE.offset_wing>0&&des_para.RODNODE.wing_offset_dist>0)
				pos+=pLinePart->ucs.axis_y*des_para.RODNODE.wing_offset_dist;
		}
		else if(pLinePart->GetClassTypeId()==CLS_LINETUBE||pLinePart->GetClassTypeId()==CLS_LINEFLAT)
		{
			if(des_para.RODNODE.direction==0)		//ʼ->��
				offset_vec = pLinePart->End()-pLinePart->Start();
			else
				offset_vec = pLinePart->Start()-pLinePart->End();
			normalize(offset_vec);
			f3dPoint perp=pos;
			if(pNode->m_cPosCalType==4)	//�����Ӧȡ��������˼���ʵ�ʽ��㣨�Ǹ�Ϊ���߽��㣬�ֹ�Ϊ���߽��㣩
				pNode->GetIntersZhun(&perp);
			else
				SnapPerp(&perp,pLinePart->Start(),pLinePart->End(),pNode->Position(true));
			pos=perp;
			pos+=offset_vec*des_para.RODNODE.len_offset_dist;
		}
		else
			pos=pNode->Position(true);
	} 
	else if(datum_pos_style==3)	//�˼�����(�Ǹ�����)����
	{
		typedef	CLDSLinePart* RODPTR;
		SmartPartPtr datumPartPtr[2]={NULL};
		RODPTR rodPtrArr[2]={NULL};
		datumPartPtr[0]=pTower->FromPartHandle(des_para.AXIS_INTERS.hDatum1);
		datumPartPtr[1]=pTower->FromPartHandle(des_para.AXIS_INTERS.hDatum2);
		if(  datumPartPtr[0].IsNULL()||datumPartPtr[1].IsNULL()||
			(!datumPartPtr[0]->IsLinePart()&&!datumPartPtr[1]->IsLinePart())||
			(!datumPartPtr[0]->IsLinePart()&&!datumPartPtr[0]->IsArcPart())||
			(!datumPartPtr[1]->IsLinePart()&&!datumPartPtr[1]->IsArcPart()))
			return FALSE;
		rodPtrArr[0]=datumPartPtr[0]->IsLinePart()?datumPartPtr[0].pRod:NULL;
		rodPtrArr[1]=datumPartPtr[1]->IsLinePart()?datumPartPtr[1].pRod:NULL;
		
		//�˴�����ʹ�ýڵ�λ�ü����淨�ߣ�Ԥ������㲻׼ȷ wht 13-04-18
		f3dPoint norm,wing_vec,vec1,vec2,inters;
		if(rodPtrArr[0]!=NULL)
			vec1 = rodPtrArr[0]->End()-rodPtrArr[0]->Start();
		else
		{
			Int3dlf(inters,f3dLine(rodPtrArr[1]->Start(),rodPtrArr[1]->End()),datumPartPtr[0].pArcRod->center,datumPartPtr[0].pArcRod->worknorm);
			vec1=datumPartPtr[0].pArcRod->GetArcRayNorm(inters);
			pos=datumPartPtr[0].pArcRod->center+vec1*datumPartPtr[0].pArcRod->radius;
			return TRUE;
		}
		if(rodPtrArr[1]!=NULL)
			vec2 = rodPtrArr[1]->End()-rodPtrArr[1]->Start();
		else
		{
			Int3dlf(inters,f3dLine(rodPtrArr[0]->Start(),rodPtrArr[0]->End()),datumPartPtr[1].pArcRod->center,datumPartPtr[1].pArcRod->worknorm);
			vec2=datumPartPtr[1].pArcRod->GetArcRayNorm(inters);
			pos=datumPartPtr[0].pArcRod->center+vec2*datumPartPtr[1].pArcRod->radius;
			return TRUE;
		}
		norm = vec1^vec2;
		normalize(norm);
		f3dPoint justify(des_para.AXIS_INTERS.near_vector[0],des_para.AXIS_INTERS.near_vector[1],des_para.AXIS_INTERS.near_vector[2]);
		if(norm*justify<0)	//�������Ԥ�����Ʒ����෴����Ҫ����
			norm*=-1.0;

		f3dLine zhun_line[2];
		short wing_offset_style[2]={des_para.AXIS_INTERS.wing_offset_style1,des_para.AXIS_INTERS.wing_offset_style2};
		long wing_offset_dist[2]={des_para.AXIS_INTERS.wing_offset_dist1,des_para.AXIS_INTERS.wing_offset_dist2};
		int i,baseAngleI=0;
		GEPOINT vWorkWingNorm=norm;
		CLDSLineAngle *pCrossJg=datumPartPtr[0]->IsAngle()?datumPartPtr[0].pAngle:NULL;
		if(datumPartPtr[1]->IsAngle()&&datumPartPtr[1]->GetWidth()>datumPartPtr[0]->GetWidth())
		{
			baseAngleI=1;
			pCrossJg=datumPartPtr[1].pAngle;	//��֫��ϴ�ĽǸ�Ϊ���߽����ͶӰ��׼
		}
		if(pCrossJg&&fabs(pCrossJg->vxWingNorm*norm)>fabs(pCrossJg->vyWingNorm*norm))
			vWorkWingNorm=pCrossJg->vxWingNorm;
		else if(pCrossJg)
			vWorkWingNorm=pCrossJg->vyWingNorm;
		for(i=0;i<2;i++)
		{
			if(datumPartPtr[i]->IsAngle())
			{
				pCrossJg=datumPartPtr[i].pAngle;
				if(fabs(norm*pCrossJg->get_norm_x_wing())>fabs(norm*pCrossJg->get_norm_y_wing()))
				{
					if(wing_offset_style[i]!=4)
						wing_offset_dist[i]=pCrossJg->GetWingXG(wing_offset_style[i]);
					wing_vec = pCrossJg->GetWingVecX();
				}
				else
				{
					if(wing_offset_style[i]!=4)
						wing_offset_dist[i]=pCrossJg->GetWingYG(wing_offset_style[i]);
					wing_vec = pCrossJg->GetWingVecY();
				}
				zhun_line[i].startPt = pCrossJg->Start()+wing_vec*wing_offset_dist[i];
				zhun_line[i].endPt = pCrossJg->End()+wing_vec*wing_offset_dist[i];
			}
			else 
			{
				if(datumPartPtr[i]->IsLinePart())
				{
					zhun_line[i].startPt = rodPtrArr[i]->Start();
					zhun_line[i].endPt = rodPtrArr[i]->End();
					project_point(zhun_line[i].startPt,rodPtrArr[i]->Start(),norm);
					project_point(zhun_line[i].endPt,rodPtrArr[i]->Start(),norm);
				}
				else
				{
					//TODO:
				}
			}
		}
		f3dPoint pt(pos);
		f3dPoint line0End=zhun_line[0].endPt;
#ifndef __ALFA_TEST_VERSION_
		BOOL gbLegacyMode=FALSE;
#endif
		if(rodPtrArr[0]->IsAngle()&&rodPtrArr[1]->IsAngle()&&!gbLegacyMode&&pTower->RepositionAlgor()!=1)
		{	//ֻ�м������Ǹ����߽���ʱ����Ҫ������һ��Ϊ��׼(�����ھɰ�����Կ��Ǳ����Ե�һ��Ϊ����)��������ͶӰ���� wjh-2016.3.26
			project_point(zhun_line[1-baseAngleI].startPt,zhun_line[0].startPt,vWorkWingNorm);
			project_point(zhun_line[1-baseAngleI].endPt,zhun_line[0].startPt,vWorkWingNorm);
		}
		Int3dll(zhun_line[0],zhun_line[1],pt);
		if(datumPartPtr[0]->IsAngle()&&datumPartPtr[1]->IsAngle()&&!gbLegacyMode&&pTower->RepositionAlgor()!=1)
		{	//Ϊ���ټ�����ȡ�����������1������ʰȡ��Ϊƽ����㡡wjh-2016.4.22
			GEPOINT vec_inters2start=pt-zhun_line[0].startPt;
			GEPOINT vec_end2start=zhun_line[0].endPt-zhun_line[0].startPt;
			GEPOINT vec_end2starti=line0End-zhun_line[0].startPt;
			double scale=(fabs(vec_inters2start.x)+fabs(vec_inters2start.y)+fabs(vec_inters2start.z))/
				(fabs(vec_end2start.x)+fabs(vec_end2start.y)+fabs(vec_end2start.z));
			pt+=((vec_end2starti*vWorkWingNorm)*scale)*vWorkWingNorm;
		}
		pos=pt;
		CDesignThickPara thick(des_para.AXIS_INTERS.norm_offset);
		double norm_offset_dist=thick.Thick(pTower);
		pos+=norm*norm_offset_dist;
	}
#endif
	else if(datum_pos_style==4)	//��׼�߿ռ佻��
	{
		CSmartPtr<CLDSLine>pLineFir=pTower->FromLineHandle(des_para.AXIS_INTERS.hDatum1);
		CSmartPtr<CLDSLine>pLineSec=pTower->FromLineHandle(des_para.AXIS_INTERS.hDatum2);
		if(pLineFir==NULL||pLineSec==NULL)
			return FALSE;	//throw "���㶨λ����ȱ�ٻ�׼����Ϣ";
		pLineFir->UpdateLine(pTower);
		pLineSec->UpdateLine(pTower);
		f3dLine L1(pLineFir->start_pos,pLineFir->end_pos),L2(pLineSec->start_pos,pLineSec->end_pos);
		f3dPoint line_vec1=L1.endPt-L1.startPt;
		f3dPoint line_vec2=L2.endPt-L2.startPt;
		if(des_para.AXIS_INTERS.cProjectPlane<=0||des_para.AXIS_INTERS.cProjectPlane>3)
		{	//���θĶ�����,�˸����� -wjh 2013.8.3
			f3dPoint work_norm=line_vec1^line_vec2;
			normalize(work_norm);
			project_point(L1.endPt,L1.startPt,work_norm);
			project_point(L2.startPt,L1.startPt,work_norm);
			project_point(L2.endPt,L1.startPt,work_norm);
			f3dPoint pt(pos);
			int retCode=Int3dll(L1,L2,pt);
			pos=pt;
			if(retCode==-1||retCode==0)
			{
				//AfxMessageBox("����׼��ƽ���غϻ�����!");
				return FALSE;
			}
		}
		else
		{
			f3dPoint face_norm,inters;
			if(des_para.AXIS_INTERS.cProjectPlane==1)	//X-Zƽ��
				face_norm=line_vec2^f3dPoint(0,1,0);
			else if(des_para.AXIS_INTERS.cProjectPlane==2)	//Y-Zƽ��
				face_norm=line_vec2^f3dPoint(1,0,0);
			else if(des_para.AXIS_INTERS.cProjectPlane==3)	//X-Yƽ��
				face_norm=line_vec2^f3dPoint(0,0,1);
			int retCode=Int3dlf(inters,L1,L2.startPt,face_norm);
			if(retCode==-1||retCode==0)
				return FALSE;//AfxMessageBox("ͶӰ�����׼��ƽ�л��غ�!");
		}
	}
	else if(datum_pos_style==5)	//��׼�����������겻���
	{
		CSmartPtr<CLDSLine>pLineFir=pTower->FromLineHandle(des_para.DATUMLINE.hDatumLine);
		if(pLineFir==NULL)
			return FALSE;	//throw "���㶨λ����ȱ�ٻ�׼����Ϣ";
		pLineFir->UpdateLine(pTower);
		f3dPoint line_vec=pLineFir->end_pos-pLineFir->start_pos;
		normalize(line_vec);
		if(pLift)
		{
			if(!bReverseLift)
				pos=pLift->LiftPosTrans(datum_pos);
			else
				pos=pLift->ReverseLiftPosTrans(datum_pos);
			f3dPoint face_norm;
			if(des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0&&fabs(line_vec.x)>EPS)
				face_norm.Set(1,0,0);
			else if(des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1&&fabs(line_vec.y)>EPS)
				face_norm.Set(0,1,0);
			else if(des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==2&&fabs(line_vec.z)>EPS)
				face_norm.Set(0,0,1);
			else
			{
				//AfxMessageBox("��׼�����׼����ƽ��!");
				return FALSE;
			}
			f3dPoint pt(pos);
			if(!bReverseLift)
				face_norm=pLift->LiftVecTrans(face_norm,pt);
			else
				face_norm=pLift->ReverseLiftVecTrans(face_norm,pt);
			Int3dlf(pt,pLineFir->start_pos,line_vec,pt,face_norm);
			pt=pos;
		}
		else
		{
			pos=datum_pos;
			if(des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0&&fabs(line_vec.x)>EPS)
			{
				pos.y=pLineFir->start_pos.y+(pos.x-pLineFir->start_pos.x)*(line_vec.y/line_vec.x);
				pos.z=pLineFir->start_pos.z+(pos.x-pLineFir->start_pos.x)*(line_vec.z/line_vec.x);
			}
			else if(des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1&&fabs(line_vec.y)>EPS)
			{
				pos.x=pLineFir->start_pos.x+(pos.y-pLineFir->start_pos.y)*(line_vec.x/line_vec.y);
				pos.z=pLineFir->start_pos.z+(pos.y-pLineFir->start_pos.y)*(line_vec.z/line_vec.y);
			}
			else if(des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==2&&fabs(line_vec.z)>EPS)
			{
				pos.x=pLineFir->start_pos.x+(pos.z-pLineFir->start_pos.z)*(line_vec.x/line_vec.z);
				pos.y=pLineFir->start_pos.y+(pos.z-pLineFir->start_pos.z)*(line_vec.y/line_vec.z);
			}
			else
			{
				//AfxMessageBox("��׼�����׼����ƽ��!");
				return FALSE;
			}
		}
	}
	else if(datum_pos_style==6)	//��׼����ָ�������
	{
		CSmartPtr<CLDSPlane>pPlane=pTower->FromPlaneHandle(des_para.DATUMPLANE.hDatumPlane);
		if(pPlane==NULL)
			return FALSE;	//throw "���㶨λ����ȱ�ٻ�׼����Ϣ";
		if(!pPlane->UpdatePlane(pTower))
		{
			//AfxMessageBox("��׼�涨������!");
			return FALSE;
		}
		f3dPoint temporary;
		if(pLift)	//Բ��Ԥ����׼���ʼλ��
		{
			if(!bReverseLift)
				temporary=pos=pLift->LiftPosTrans(datum_pos);
			else
				temporary=pos=pLift->ReverseLiftPosTrans(datum_pos);
		}
		else
			temporary=pos=datum_pos;
		//������նԳ�
		if(des_para.DATUMPLANE.cMirTransType=='X')	//X��������ԳƱ任
			temporary.y=-temporary.y;
		else if(des_para.DATUMPLANE.cMirTransType=='Y')	//Y��������ԳƱ任
			temporary.x=-temporary.x;
		else if(des_para.DATUMPLANE.cMirTransType=='Z')	//Z��������ԳƱ任
		{
			temporary.x=-temporary.x;
			temporary.y=-temporary.y;
		}
		f3dPoint line_vec;
		if(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0&&fabs(pPlane->GetWorkNorm().x)>EPS)
			line_vec.Set(1,0,0);
		else if(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1&&fabs(pPlane->GetWorkNorm().y)>EPS)
			line_vec.Set(0,1,0);
		else if(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==2&&fabs(pPlane->GetWorkNorm().z)>EPS)
			line_vec.Set(0,0,1);
		else
		{
			Log2File()->Log("0X%X��׼�����׼����ƽ�У�Ҳ�����ǻ�׼�淨��Ϊ��ֵ!",pPlane->handle);
			return FALSE;
		}
		if(pLift)
		{
			f3dPoint pt(temporary);
			if(!bReverseLift)
				line_vec=pLift->LiftVecTrans(line_vec,pt);
			else
				line_vec=pLift->ReverseLiftVecTrans(line_vec,pt);
			temporary=pt;
		}
		if(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0&&fabs(pPlane->GetWorkNorm().x)>EPS)
		{
			//Int3dlf(pos,pos,line_vec,pPlane->GetBasePos(),pPlane->GetWorkNorm());
			f3dPoint pt(temporary);
			Int3dlf(pt,temporary,line_vec,pPlane->GetBasePos(),pPlane->GetWorkNorm());
			temporary=pt;
		}
		else if(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1&&fabs(pPlane->GetWorkNorm().y)>EPS)
		{
			//Int3dlf(pos,pos,line_vec,pPlane->GetBasePos(),pPlane->GetWorkNorm());
			f3dPoint pt(temporary);
			Int3dlf(pt,temporary,line_vec,pPlane->GetBasePos(),pPlane->GetWorkNorm());
			temporary=pt;
		}
		else if(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==2&&fabs(pPlane->GetWorkNorm().z)>EPS)
		{
			//Int3dlf(pos,pos,line_vec,pPlane->GetBasePos(),pPlane->GetWorkNorm());
			f3dPoint pt(temporary);
			Int3dlf(pt,temporary,line_vec,pPlane->GetBasePos(),pPlane->GetWorkNorm());
			temporary=pt;
		}
		else
		{
			Log2File()->Log("0X%X��׼�����׼����ƽ�У�Ҳ�����ǻ�׼�淨��Ϊ��ֵ!");
			return FALSE;
		}
		if(des_para.DATUMPLANE.cMirTransType=='X')	//X��������ԳƱ任
			temporary.y=-temporary.y;
		else if(des_para.DATUMPLANE.cMirTransType=='Y')	//Y��������ԳƱ任
			temporary.x=-temporary.x;
		else if(des_para.DATUMPLANE.cMirTransType=='Z')	//Z��������ԳƱ任
		{
			temporary.x=-temporary.x;
			temporary.y=-temporary.y;
		}
		pos=temporary;
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(datum_pos_style==7)	//�Ǹ�����������
	{
		f3dPoint offset_vec;
		CSmartPtr<CLDSLineAngle> pJg=(CLDSLineAngle*)pTower->FromPartHandle(des_para.RODEND.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pJg.IsNULL())
			return FALSE;	//throw "���㶨λ����ȱ�ٻ�׼�Ǹ���Ϣ";
		//if(pLift)
		//	pos=pLift->LiftPosTrans(datum_pos);
		//else
		//	pos=datum_pos;
		if(des_para.RODEND.direction==0)		//ʼ->��
		{
			offset_vec = pJg->End()-pJg->Start();
			normalize(offset_vec);
			//Ĭ�ϼ�������ͷӰ��
			pos = pJg->Start()+offset_vec*(des_para.RODEND.len_offset_dist-pJg->startOdd());
		}
		else
		{
			offset_vec = pJg->Start()-pJg->End();
			normalize(offset_vec);
			//Ĭ�ϼ�������ͷӰ��
			pos = pJg->End()+offset_vec*(des_para.RODEND.len_offset_dist-pJg->endOdd());
		}
		f3dPoint norm_x_wing=pJg->get_norm_x_wing();
		f3dPoint norm_y_wing=pJg->get_norm_y_wing();
		f3dPoint bi_vec=norm_x_wing+norm_y_wing;
		normalize(bi_vec);
		CDesignThickPara thick(des_para.RODEND.norm_offset);
		double norm_offset_dist=thick.Thick(pTower);
		if(pJg->IsReservedHeJiao())	//����һ��֫��ȵ�����Ͻ�ʱ��㴦��ֱ�ǲ��ü���ȱ���
			pos+=(norm_offset_dist*1.4142135623731)*bi_vec;//sqrt(2)=1.4142135623731
		else
		{
			double cosa=bi_vec*norm_x_wing;
			pos+=(norm_offset_dist/cosa)*bi_vec;
		}
	}
	else if(datum_pos_style==10)
	{	//���ڵ��ı����ȷֵ� wht 11-01-23
		CSuperSmartPtr<CLDSLinePart> pLinePart;
		CLDSNode *pStartNode=pTower->FromNodeHandle(des_para.SCALE_NODE.hStartNode);
		CLDSNode *pEndNode=pTower->FromNodeHandle(des_para.SCALE_NODE.hEndNode);
		pLinePart=(CLDSLinePart*)pTower->FromPartHandle(des_para.SCALE_NODE.hLinePart,CLS_LINEPART);
		if(pStartNode==NULL||pEndNode==NULL||pLinePart.IsNULL())
			return FALSE;
		f3dPoint start,end,len_vec;
		len_vec=pLinePart->End()-pLinePart->Start();
		normalize(len_vec);
		if(pLinePart->IsAngle())
		{
			bool bClearLock=CParaPointLifeObj::LockCall(pLinePart->handle);
			start=pLinePart.LineAnglePointer()->GetDatumPosBer(pStartNode,0,bClearLock);
			end=pLinePart.LineAnglePointer()->GetDatumPosBer(pEndNode,0,bClearLock);
		}
		else 
		{
			SnapPerp(&start,pLinePart->Start(),pLinePart->End(),pStartNode->Position(true));
			SnapPerp(&end,pLinePart->Start(),pLinePart->End(),pEndNode->Position(true));
		}
		if(pStartNode==pLinePart->pStart)
			start=start-len_vec*pLinePart->startOdd();
		else if(pStartNode==pLinePart->pEnd)
			start=start+len_vec*pLinePart->endOdd();
		if(pEndNode==pLinePart->pStart)
			end=end-len_vec*pLinePart->startOdd();
		else if(pEndNode==pLinePart->pEnd)
			end=end+len_vec*pLinePart->endOdd();
		f3dPoint verfiy_vec=end-start;
		normalize(verfiy_vec);
		if(verfiy_vec*len_vec<0)
			len_vec*=-1.0;
		start+=len_vec*des_para.SCALE_NODE.start_offset_dist;
		end-=len_vec*des_para.SCALE_NODE.end_offset_dist;
		double sum_len=DISTANCE(start,end);
		pos=start+len_vec*sum_len*des_para.SCALE_NODE.offset_scale;
		if(fabs(des_para.SCALE_NODE.offset_dist)>0)
			pos+=len_vec*des_para.SCALE_NODE.offset_dist;
	}
#endif
	else if(datum_pos_style==8)	//��ڵ�ͬλ
	{
		CLDSNode *pNode = pTower->FromNodeHandle(des_para.hNode);
		if(pNode)
		{
			if(pLift!=NULL&&!bReverseLift)
				pos = pNode->Position(pLift!=NULL);
			else if(pLift!=NULL&&bReverseLift)
				pos = pNode->xPreliftPos;
			else
				pos = pNode->xOriginalPos;
		}
		else
		{
			//AfxMessageBox("�Ҳ���ָ���Ļ�׼�ڵ�!");
			return FALSE;
		}
	}
	else if(datum_pos_style==9)	//ָ�������ϵ��������
	{
		SmartPartPtr pSmartPart=pTower->FromPartHandle(des_para.hPart);
		if(pSmartPart.blValid)
		{
			pos=datum_pos;
			if(pSmartPart->IsAngle())//�Ǹֵ�װ������ϵ���洢���ļ����Ҫʵʱ���� wjh-2019.5.18
				pSmartPart.pAngle->getUCS(pSmartPart->ucs);
			coord_trans(pos,pSmartPart->ucs,TRUE);
		}
		else
			return FALSE;	//�Ҳ���ָ������
	}
#ifdef __COMPLEX_PART_INC_
	else if(datum_pos_style==11)
	{	//���������߽���
		CLDSLineTube* pTube=NULL;
		if(des_para.COLUMN_INTERS.hLineTube>0)
			pTube=(CLDSLineTube*)pTower->FromPartHandle(des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
		f3dPoint axis_vec,sect_norm;
		f3dPoint sect_center(des_para.COLUMN_INTERS.SectCenter.sect_center[0],des_para.COLUMN_INTERS.SectCenter.sect_center[1],des_para.COLUMN_INTERS.SectCenter.sect_center[2]);
		//1.ȷ�����֣�Բ����Բ�ĵ㼰�����߷���
		if(pTube)
		{
			f3dPoint axis_pick=pTube->Start();
			axis_vec=pTube->End()-pTube->Start();
			normalize(axis_vec);
			des_para.COLUMN_INTERS.column_r=pTube->GetWidth()*0.5;
			sect_norm.Set(des_para.COLUMN_INTERS.SectNorm.sect_norm[0],des_para.COLUMN_INTERS.SectNorm.sect_norm[1],des_para.COLUMN_INTERS.SectNorm.sect_norm[2]);
			if(sect_norm.IsZero())
				sect_norm=axis_vec;
			if(des_para.COLUMN_INTERS.cPointAndVectorFlag&0x02)
			{	//�������������Բ�ĵ�����
				CLDSNode* pNode=pTower->FromNodeHandle(abs(des_para.COLUMN_INTERS.SectCenterParam.hNode));
				if(pNode && des_para.COLUMN_INTERS.SectCenterParam.hNode!=0)
				{
					f3dPoint datumStartPos=pTube->GetDatumPos(pNode);
					if(des_para.COLUMN_INTERS.SectCenterParam.hNode>0)
						des_para.COLUMN_INTERS.SectCenter.sect_center=sect_center=datumStartPos+axis_vec*des_para.COLUMN_INTERS.SectCenterParam.len_offset;
					else
						des_para.COLUMN_INTERS.SectCenter.sect_center=sect_center=datumStartPos-axis_vec*des_para.COLUMN_INTERS.SectCenterParam.len_offset;
				}
			}
			if(Int3dlf(sect_center,axis_pick,axis_vec,sect_center,sect_norm)<=0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("�ڼ��������������λ��ʱ���������������ʧ�ܣ�");
#else 
				logerr.Log("�ڼ��������������λ��ʱ���������������ʧ�ܣ�");
#endif
				return FALSE;
			}
		}
		else
		{
			axis_vec.Set(des_para.COLUMN_INTERS.AxisVector.axis_vec[0],des_para.COLUMN_INTERS.AxisVector.axis_vec[1],des_para.COLUMN_INTERS.AxisVector.axis_vec[2]);
			if(sect_norm.IsZero())
				sect_norm=axis_vec;
		}
		//2.�淶��������������淨��
		normalize(sect_norm);
		//3.ȷ������������ת�����
		f3dPoint ray_vec(des_para.COLUMN_INTERS.RayVector.ray_vec[0],des_para.COLUMN_INTERS.RayVector.ray_vec[1],des_para.COLUMN_INTERS.RayVector.ray_vec[2]);
		if(des_para.COLUMN_INTERS.ray_vec_style==0||pTube==NULL)//ray_vecΪֱ�Ӷ�������߾��Է���
		{
			if(!project_vector(ray_vec,sect_norm))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("�ڽ������������ͶӰʱ������ʧ��");
#else 
				logerr.Log("�ڽ������������ͶӰʱ������ʧ��");
#endif
				return FALSE;
			}
		}
		else if(des_para.COLUMN_INTERS.ray_vec_style>0)
		{	// ָ��������ʼ�ߣ�������ray_angleȷ�����߷���
			double rot_ang=des_para.COLUMN_INTERS.ray_angle*RADTODEG_COEF;
			if(des_para.COLUMN_INTERS.ray_vec_style==4)
			{	//�����ڸֹ���б���ϵ�ˮƽ�۸ֵ����Ŷ� wjh-2015.10.10
				GEPOINT horiRayVec=sect_norm^f3dPoint(0,0,1);
				if(horiRayVec*ray_vec<0)
					horiRayVec*=-1.0;
				if(normalize(horiRayVec))
					des_para.COLUMN_INTERS.RayVector.ray_vec=ray_vec=horiRayVec;
			}
			else
			{
				if(des_para.COLUMN_INTERS.ray_vec_style>1)
				{	//�Ըֹܻ�׼�߻򺸵���Ϊ������ʼ�ߣ�����תȷ�����߷���
					pTube->BuildUCS();
					ray_vec=pTube->ucs.axis_x;
					if(pTube&&des_para.COLUMN_INTERS.ray_vec_style==3)	//�ֹܺ�����Ϊ������ʼ��
						rot_ang+=pTube->CalWeldRoadAngle();
				}
				//else if(pTube&&ray_vec_style==2)	//�ֹܻ�׼��Ϊ������ʼ��
				RotateVectorAroundVector(ray_vec,rot_ang,axis_vec);
				//��б����Ӱ��
				if(fabs(sect_norm*axis_vec)<EPS_COS)
				{
					f3dPoint ray_pick=sect_center+ray_vec;
					if(Int3dlf(ray_pick,ray_pick,axis_vec,sect_center,sect_norm)<=0)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						logerr.Log("�ڼ��������������λ��ʱ������ʰȡ���������ʧ�ܣ�");
#else 
						logerr.Log("�ڼ��������������λ��ʱ������ʰȡ���������ʧ�ܣ�");
#endif
						return FALSE;
					}
					ray_vec=ray_pick-sect_center;
				}
			}
		}
		normalize(ray_vec);
		if(des_para.COLUMN_INTERS.ray_vec_style==2||des_para.COLUMN_INTERS.ray_vec_style==3)
			des_para.COLUMN_INTERS.RayVector.ray_vec=ray_vec;
		//4.�������������������λ��
		f3dPoint ray_vert_vec=ray_vec;
		if(fabs(des_para.COLUMN_INTERS.ray_para_offset)>EPS)	//��Ҫ�����߽���ƽ���ƶ�
			RotateVectorAroundVector(ray_vert_vec,0.5*Pi,sect_norm);
		if(!des_para.COLUMN_INTERS.bSpecRo)	//����뾶���������߽������������ƫ��ȷ��
		{	//4.1�������������߽���
			f3dPoint inters,inters1,inters2;
			f3dPoint line_pick=sect_center;
			if(fabs(des_para.COLUMN_INTERS.ray_para_offset)>EPS)	//��Ҫ�����߽���ƽ���ƶ�
				line_pick+=ray_vert_vec*des_para.COLUMN_INTERS.ray_para_offset;
			int inters_n=Int3dlc(&inters1,&inters2,line_pick,ray_vec,
				sect_center,sect_center+axis_vec,des_para.COLUMN_INTERS.column_r,des_para.COLUMN_INTERS.column_r);
			if(inters_n<=0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("�ڼ��������������λ��ʱ��������Բ������ʧ��");
#else 
				logerr.Log("�ڼ��������������λ��ʱ��������Բ������ʧ��");
#endif
				return FALSE;
			}
			if(inters_n==2 && ((inters1-sect_center)*ray_vec<0))
				inters=inters2;
			else
				inters=inters1;
			//4.2�������������߽���������߷���ƫ��ȷ�����ն�λ��
			pos=inters+des_para.COLUMN_INTERS.ray_offset_r*ray_vec;
		}
		else
		{	//4.3����ָ���ĵ�����Բ�ĵ��ͶӰ�������ֵ�������ն�λ��
			pos=sect_center+ray_vec*des_para.COLUMN_INTERS.ray_offset_r;
			if(fabs(des_para.COLUMN_INTERS.ray_para_offset)>EPS)	//��Ҫ�����߽���ƽ���ƶ�
				pos+=ray_vert_vec*des_para.COLUMN_INTERS.ray_para_offset;
		}
	}
#endif
	return TRUE;
}
int CLDSPoint::MirToPoint(IModel *pModel,CLDSPoint &mirPoint,MIRMSG& mirinfo)
{
	mirPoint.datum_pos_style=datum_pos_style;
	strcpy(mirPoint.description,description);
	mirPoint.pos=GetMirPos(pos,mirinfo);
	f3dPoint line_vec,mir_line_vec,line_vec_tm;
	MIRMSG msg=mirinfo;
	int errorstatus=0;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	if(datum_pos_style==0)	//ֱ��ָ��
		mirPoint.datum_pos=GetMirPos(datum_pos,mirinfo);
	else if(datum_pos_style==1||datum_pos_style==7)
	{	//�Ǹ��������Ϊ��׼��Ǹְ������
		CLDSLinePart *pRod=pModel->FromRodHandle(des_para.RODEND.hRod);
		CLDSLinePart *pMirRod=NULL;
		if(pRod)	//���ڴ��ںö��ļ��������ķǶԳƴ���������ʱ��˨�����˼��Գ�û�취��ѭ�ϸ�Գ�ԭ�� wjh-2015.11.26
			pMirRod=(CLDSLinePart*)pRod->GetMirPart(mirinfo,pos);
		//�ڸߵ��Ƚṹ�������ĵ���ͶӰ(ƽ��)�к��п����Ҳ����ԳƲģ����ǲ���ֱ�Ӵֱ��������Ӧ�ñ���ԭֵ
		long hOldRod=mirPoint.des_para.RODEND.hRod;
		mirPoint.des_para.RODEND=des_para.RODEND;
		mirPoint.des_para.RODEND.hRod=hOldRod;//0;
		if (pMirRod)
		{
			mirPoint.des_para.RODEND.hRod = pMirRod->handle;
			GEPOINT line_vec,mir_line_vec;
			if (pRod->pStart&&pRod->pEnd&&pMirRod->pStart&&pMirRod->pEnd)
			{	//�˼���ʼ�˵�λ��������ȷλ�ô��ڽϴ�ƫ��ʱ��������жϴ��������ѡ�õ������� wjh-2020.2.11
				line_vec=pRod->pEnd->xOriginalPos-pRod->pStart->xOriginalPos;
				mir_line_vec=pMirRod->pEnd->xOriginalPos-pMirRod->pStart->xOriginalPos;
			}
			else
			{
				line_vec = pRod->xPosEnd - pRod->xPosStart;
				mir_line_vec = pMirRod->xPosEnd - pMirRod->xPosStart;
			}
			f3dPoint line_vec_tm = GetMirVector(line_vec, msg);
			if (line_vec_tm*mir_line_vec < 0)
				mirPoint.des_para.RODEND.direction = 1 - mirPoint.des_para.RODEND.direction;
			if (datum_pos_style == 1 && pRod->IsAngle() && pMirRod->IsAngle() && mirPoint.des_para.RODEND.wing_offset_dist > 0)
			{	//�Ǹ�������㶨λ
				f3dPoint mir_wing_norm = mirinfo.MirVector(((CLDSLineAngle*)pRod)->get_norm_x_wing());
				if (fabs(mir_wing_norm*((CLDSLineAngle*)pMirRod)->get_norm_x_wing()) <
					fabs(mir_wing_norm*((CLDSLineAngle*)pMirRod)->get_norm_y_wing()))	//��֫�ߵ��Գ�
					mirPoint.des_para.RODEND.offset_wing = 1 - mirPoint.des_para.RODEND.offset_wing;
			}
			if (datum_pos_style == 1 && pRod->IsAngle() && des_para.RODEND.wing_offset_style == 4 && des_para.RODEND.wing_offset_dist != 0)
			{	//�Զ�ʶ����Զ���֫ƫ����Ϊ��׼׼������,�����жԳ�
				JGZJ zj;
				CLDSLineAngle* pAngle = (CLDSLineAngle*)pRod;
				if (des_para.RODEND.offset_wing == 0)
					zj = pAngle->GetZhunJu('X');
				else
					zj = pAngle->GetZhunJu('Y');
				if (des_para.RODEND.wing_offset_dist == zj.g)
					mirPoint.des_para.RODEND.wing_offset_style = 0;
				else if (des_para.RODEND.wing_offset_dist == zj.g1&&zj.g1 > 0)
					mirPoint.des_para.RODEND.wing_offset_style = 1;
				else if (des_para.RODEND.wing_offset_dist == zj.g2&&zj.g2 > 0)
					mirPoint.des_para.RODEND.wing_offset_style = 2;
				else if (des_para.RODEND.wing_offset_dist == zj.g3&&zj.g3 > 0)
					mirPoint.des_para.RODEND.wing_offset_style = 3;
			}
		}
		else
			mirPoint.des_para.RODEND.hRod = 0;// pMirRod->handle;
		CDesignThickPara thick(des_para.RODEND.norm_offset);
		CDesignThickPara mir=thick.MirThickPara(pModel,mirinfo);
		strcpy(mirPoint.des_para.RODEND.norm_offset,mir.key_str);
	}
	else if(datum_pos_style==2)
	{	//�˼��Ͻڵ�Ϊ��׼
		//�ڸߵ��Ƚṹ�������ĵ���ͶӰ(ƽ��)�к��п����Ҳ����ԳƲģ����ǲ���ֱ�Ӵֱ��������Ӧ�ñ���ԭֵ wjh-2015.10.09
		long hOldDatumRod=mirPoint.des_para.RODNODE.hRod;
		mirPoint.des_para.RODNODE=des_para.RODNODE;
		mirPoint.des_para.RODNODE.hRod=hOldDatumRod;//0;
		CLDSNode* pNode=pModel->FromNodeHandle(des_para.RODNODE.hNode);
		CLDSNode* pMirNode=pNode!=NULL?pNode->GetMirNode(mirinfo):NULL;
		if(pMirNode)
			mirPoint.des_para.RODNODE.hNode=pMirNode->handle;
		else
		{
			mirPoint.des_para.RODNODE.hNode=0;
			Log2File()->Log("δ�ҵ���������(0x%X)��λ�ڵ�0x%X��ƥ��Գƽڵ�",handle,des_para.RODNODE.hNode);
			errorstatus=-1;	//��ʧ��׼�˼�
		}
		CLDSLinePart *pRod=pModel->FromRodHandle(des_para.RODNODE.hRod);
		CLDSLinePart *pMirRod=NULL;
		if(pRod)
			pMirRod=(CLDSLinePart*)pRod->GetMirPart(mirinfo,pos);
		if(pMirRod)
		{
			mirPoint.des_para.RODNODE.hRod=pMirRod->handle;
			f3dPoint line_vec=pRod->End()-pRod->Start();
			f3dPoint mir_line_vec=pMirRod->End()-pMirRod->Start();
			f3dPoint line_vec_tm=GetMirVector(line_vec,msg);
			if(line_vec_tm*mir_line_vec<0)
				mirPoint.des_para.RODNODE.direction=1-mirPoint.des_para.RODNODE.direction;
			if(pRod->IsAngle()&&pMirRod->IsAngle())//&&mirPoint.des_para.RODNODE.wing_offset_dist>0)
			{
				f3dPoint mir_wing_norm=mirinfo.MirVector(((CLDSLineAngle*)pRod)->get_norm_x_wing());
				if( fabs(mir_wing_norm*((CLDSLineAngle*)pMirRod)->get_norm_x_wing()) <
					fabs(mir_wing_norm*((CLDSLineAngle*)pMirRod)->get_norm_y_wing()))	//��֫�ߵ��Գ�
					mirPoint.des_para.RODNODE.offset_wing=1-mirPoint.des_para.RODNODE.offset_wing;
			}
		}
		if(pRod!=NULL&&pRod->IsAngle()&&des_para.RODNODE.wing_offset_style==4&&des_para.RODNODE.wing_offset_dist!=0)
		{	//�Զ�ʶ����Զ���֫ƫ����Ϊ��׼׼������,�����жԳ�
			JGZJ zj;
			CLDSLineAngle* pAngle=(CLDSLineAngle*)pRod;
			if(des_para.RODEND.offset_wing==0)
				zj=pAngle->GetZhunJu('X');
			else
				zj=pAngle->GetZhunJu('Y');
			if(des_para.RODNODE.wing_offset_dist==zj.g)
				mirPoint.des_para.RODNODE.wing_offset_style=0;
			else if(des_para.RODNODE.wing_offset_dist==zj.g1&&zj.g1>0)
				mirPoint.des_para.RODNODE.wing_offset_style=1;
			else if(des_para.RODNODE.wing_offset_dist==zj.g2&&zj.g2>0)
				mirPoint.des_para.RODNODE.wing_offset_style=2;
			else if(des_para.RODNODE.wing_offset_dist==zj.g3&&zj.g3>0)
				mirPoint.des_para.RODNODE.wing_offset_style=3;
		}
	}
	else if(datum_pos_style==3)
	{	//�Ǹ����߽���
		mirPoint.des_para.AXIS_INTERS=des_para.AXIS_INTERS;
		CLDSLinePart *pRod=pModel->FromRodHandle(des_para.AXIS_INTERS.hDatum1);
		CLDSLinePart *pMirRod=NULL;
		if(pRod)
			pMirRod=(CLDSLinePart*)pRod->GetMirPart(mirinfo,pos);
		if(pMirRod)
			mirPoint.des_para.AXIS_INTERS.hDatum1=pMirRod->handle;
		if((pRod=pModel->FromRodHandle(des_para.AXIS_INTERS.hDatum2))!=NULL)
		{
			pMirRod=(CLDSLinePart*)pRod->GetMirPart(mirinfo,pos);
			if(pMirRod)
				mirPoint.des_para.AXIS_INTERS.hDatum2=pMirRod->handle;
		}
		f3dPoint nearVector;
		nearVector.x=des_para.AXIS_INTERS.near_vector[0];
		nearVector.y=des_para.AXIS_INTERS.near_vector[1];
		nearVector.z=des_para.AXIS_INTERS.near_vector[2];
		nearVector=mirinfo.MirVector(nearVector);
		mirPoint.des_para.AXIS_INTERS.near_vector[0]=nearVector.x;
		mirPoint.des_para.AXIS_INTERS.near_vector[1]=nearVector.y;
		mirPoint.des_para.AXIS_INTERS.near_vector[2]=nearVector.z;
	}
	else if(mirPoint.datum_pos_style==6)	//��׼����ָ����������ڵ�
	{
		if(msg.axis_flag==0x01||msg.axis_flag==0x02||msg.axis_flag==0x04)
		{
			mirPoint.datum_pos=GetMirPos(datum_pos,msg);
			mirPoint.datum_pos_style=datum_pos_style;
			mirPoint.des_para.DATUMPLANE.hDatumPlane=des_para.DATUMPLANE.hDatumPlane;
			mirPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=des_para.DATUMPLANE.fix_axis_style_x0_y1_z2;
			//�Գƻ�׼�涨λ�Ķ�λ������Ϣ
			if(mirinfo.axis_flag==0x01)	//X��Գ�
			{
				if(des_para.DATUMPLANE.cMirTransType==0)
					mirPoint.des_para.DATUMPLANE.cMirTransType='X';
				else if(des_para.DATUMPLANE.cMirTransType=='X')
					mirPoint.des_para.DATUMPLANE.cMirTransType=0;
				else if(des_para.DATUMPLANE.cMirTransType=='Y')
					mirPoint.des_para.DATUMPLANE.cMirTransType='Z';
				else if(des_para.DATUMPLANE.cMirTransType=='Z')
					mirPoint.des_para.DATUMPLANE.cMirTransType='Y';
			}
			else if(mirinfo.axis_flag==0x02)	//Y��Գ�
			{
				if(des_para.DATUMPLANE.cMirTransType==0)
					mirPoint.des_para.DATUMPLANE.cMirTransType='Y';
				else if(des_para.DATUMPLANE.cMirTransType=='Y')
					mirPoint.des_para.DATUMPLANE.cMirTransType=0;
				else if(des_para.DATUMPLANE.cMirTransType=='X')
					mirPoint.des_para.DATUMPLANE.cMirTransType='Z';
				else if(des_para.DATUMPLANE.cMirTransType=='Z')
					mirPoint.des_para.DATUMPLANE.cMirTransType='X';
			}
			else if(mirinfo.axis_flag==0x04)	//Z��Գ�
			{
				if(des_para.DATUMPLANE.cMirTransType==0)
					mirPoint.des_para.DATUMPLANE.cMirTransType='Z';
				else if(des_para.DATUMPLANE.cMirTransType=='Z')
					mirPoint.des_para.DATUMPLANE.cMirTransType=0;
				else if(des_para.DATUMPLANE.cMirTransType=='X')
					mirPoint.des_para.DATUMPLANE.cMirTransType='Y';
				else if(des_para.DATUMPLANE.cMirTransType=='Y')
					mirPoint.des_para.DATUMPLANE.cMirTransType='X';
			}
		}
	}
	else if(mirPoint.datum_pos_style==8)	//�ڵ������ͬλ
	{
		MirPartHandle(pModel,&mirPoint.des_para.hNode,des_para.hNode,msg);
	}
	else if(mirPoint.datum_pos_style==9)	//��׼�����ϵ��������
	{	
		mirPoint.datum_pos=datum_pos;
		MirPartHandle(pModel,&mirPoint.des_para.hPart,des_para.hPart,msg);
		if(pModel)
		{
			CLDSPart *pDatumPart=pModel->FromPartHandle(des_para.hPart);
			CLDSPart *pMirDatumPart=pModel->FromPartHandle(mirPoint.des_para.hPart);
			if(pDatumPart==NULL||pMirDatumPart==NULL)
				return -1;	//��ʧ��׼�˼�
			f3dPoint datumpos=datum_pos;
			coord_trans(datumpos,pDatumPart->ucs,TRUE);
			mirPoint.datum_pos=GetMirPos(datumpos,msg);
			coord_trans(mirPoint.datum_pos,pMirDatumPart->ucs,FALSE);
		}
	}
	else if(mirPoint.datum_pos_style==10)	//���ڵ��ı����ȷֵ� wht 12-03-19
	{	
		MirPartHandle(pModel,&mirPoint.des_para.SCALE_NODE.hLinePart,des_para.SCALE_NODE.hLinePart,msg);
		MirPartHandle(pModel,&mirPoint.des_para.SCALE_NODE.hStartNode,des_para.SCALE_NODE.hStartNode,msg);
		MirPartHandle(pModel,&mirPoint.des_para.SCALE_NODE.hEndNode,des_para.SCALE_NODE.hEndNode,msg);
		mirPoint.des_para.SCALE_NODE.start_offset_dist=des_para.SCALE_NODE.start_offset_dist;
		mirPoint.des_para.SCALE_NODE.end_offset_dist=des_para.SCALE_NODE.end_offset_dist;
		mirPoint.des_para.SCALE_NODE.offset_dist=des_para.SCALE_NODE.offset_dist;
		mirPoint.des_para.SCALE_NODE.offset_scale=des_para.SCALE_NODE.offset_scale;
	}
#endif
	return errorstatus;
}
CHashStrList<PROPLIST_ITEM>CLDSPoint::propHashtable;	//�����ַ�����ϣ��
CXhChar100 CLDSPoint::HELPLINK_URL;
CHashTable<DWORD>CLDSPoint::propStatusHashtable;	//����״̬��ϣ��
CHashSet<PROPLIST_ITEM*>CLDSPoint::hashPropItemById;
void CLDSPoint::InitPropHashtable()						//��ʼ�����Թ�ϣ��
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(50);

#ifdef AFX_TARG_ENU_ENGLISH
	AddPropItem("datumPoint",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("description",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("pos",PROPLIST_ITEM(id++,"Datum Point Coordinate"));
	CLDSPoint::AddPropItem("pos.x",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("pos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("pos.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("datum_pos_style",PROPLIST_ITEM(id++,"Datum Point Locate Mode"));
	//��ڵ�ͬλ
	CLDSPoint::AddPropItem("des_para.hNode",PROPLIST_ITEM(id++,"Datum Node Handle"));
	//ָ�������ϵ��������
	CLDSPoint::AddPropItem("des_para.hPart",PROPLIST_ITEM(id++,"Datum Part Handle"));
	CLDSPoint::AddPropItem("datum_pos",PROPLIST_ITEM(id++,"Relative Coordinates"));
	CLDSPoint::AddPropItem("datum_pos.x",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("datum_pos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("datum_pos.z",PROPLIST_ITEM(id++,"Z"));
	//���߽���
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.hDatum1",PROPLIST_ITEM(id++,"Datum Line 1 Handle"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.hDatum2",PROPLIST_ITEM(id++,"Datum Line 2 Handle"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_style1",PROPLIST_ITEM(id++,"Leg Offset Type 1"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_style2",PROPLIST_ITEM(id++,"Leg Offset Type 2"));//0.g;1.g1;2.g2;3.g3;4.�Զ���
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_dist1",PROPLIST_ITEM(id++,"Leg Offset 1"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_dist2",PROPLIST_ITEM(id++,"Leg Offset 2"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector",PROPLIST_ITEM(id++,"Near Normal"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector[0]",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector[1]",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector[2]",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.norm_offset",PROPLIST_ITEM(id++,"Normal Offset"));	//����ƫ����;
	//�Ǹ����
	CLDSPoint::AddPropItem("des_para.RODEND.hRod",PROPLIST_ITEM(id++,"Datum Rod Handle"));				//�Ǹ־��
	CLDSPoint::AddPropItem("des_para.RODEND.direction",PROPLIST_ITEM(id++,"Vertical Offset Direction"));			//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	CLDSPoint::AddPropItem("des_para.RODEND.offset_wing",PROPLIST_ITEM(id++,"Offset Leg"));		//0.X֫;1.Y֫
	CLDSPoint::AddPropItem("des_para.RODEND.wing_offset_style",PROPLIST_ITEM(id++,"Leg Offset Type "));	//0.g;1.g1;2.g2;3.g3;4.�Զ���
	CLDSPoint::AddPropItem("des_para.RODEND.wing_offset_dist",PROPLIST_ITEM(id++,"Leg Offset "));
	CLDSPoint::AddPropItem("des_para.RODEND.len_offset_dist",PROPLIST_ITEM(id++,"Vertical Direction Offset"));	//֫��ƫ��ֵ;����ƫ��ֵ;
	CLDSPoint::AddPropItem("des_para.RODEND.bIncOddEffect",PROPLIST_ITEM(id++,"Odd Length Affect"));		//����ƫ��ֵ��������ͷӰ��
	CLDSPoint::AddPropItem("des_para.RODEND.norm_offset",PROPLIST_ITEM(id++,"Normal Offset"));		//����ƫ����
	//�˼��Ͻڵ�Ϊ��׼
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.hLinePart",PROPLIST_ITEM(id++,"Datum Line Part Handle"));	//�Ǹ־��
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.hNode",PROPLIST_ITEM(id++,"Datum Node Handle"));		//��׼�ڵ���
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.bFaceOffset",PROPLIST_ITEM(id++,"Offset Flatly"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.direction",PROPLIST_ITEM(id++,"Vertical Offset Direction"));	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.offset_wing",PROPLIST_ITEM(id++,"Offset Leg"));	//0.X֫;1.Y֫
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.wing_offset_style",PROPLIST_ITEM(id++,"Leg Offset Type "));//0.g;1.g1;2.g2;3.g3;4.�Զ���
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.wing_offset_dist",PROPLIST_ITEM(id++,"Leg Offset "));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.len_offset_dist",PROPLIST_ITEM(id++,"Vertical Direction Offset"));	//֫��ƫ��ֵ;����ƫ��ֵ;
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.norm_offset",PROPLIST_ITEM(id++,"Normal Offset"));		//����ƫ����;
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset",PROPLIST_ITEM(id++,"Offset Plane Normal "));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset[0]",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset[1]",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset[2]",PROPLIST_ITEM(id++,"Z"));
	//��׼�����������겻���
	CLDSPoint::AddPropItem("des_para.DATUMLINE.hDatumLine",PROPLIST_ITEM(id++,"Datum Line Handle"));
	CLDSPoint::AddPropItem("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2",PROPLIST_ITEM(id++,"Specify Axial"));
	//��׼�����������겻���
	CLDSPoint::AddPropItem("des_para.DATUMPLANE.hDatumPlane",PROPLIST_ITEM(id++,"Datum Plane Handle"));
	CLDSPoint::AddPropItem("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2",PROPLIST_ITEM(id++,"Specify Axial"));
	//���ڵ������ȷֵ�
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.hLinePart",PROPLIST_ITEM(id++,"Datum Rod Handle"));			//��׼�˼����
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.hStartNode",PROPLIST_ITEM(id++,"Start Datum Node Handle"));		//ʼ�˽ڵ���
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.hEndNode",PROPLIST_ITEM(id++,"End Datum Node Handle"));			//�ն˽ڵ���
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.start_offset_dist",PROPLIST_ITEM(id++,"Start Offset"));	//ʼ��ƫ����
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.end_offset_dist",PROPLIST_ITEM(id++,"End Offset"));	//�ն�ƫ����
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.offset_scale",PROPLIST_ITEM(id++,"Offset Proportion"));		//ƫ�Ʊ���
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.offset_dist",PROPLIST_ITEM(id++,"Offset Distance"));		//�ڵȷֻ����ϵ�ƫ��ֵ
	//���涨λ��
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.column",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.column_r",PROPLIST_ITEM(id++,"Column Radius"));		//����뾶
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.hLineTube",PROPLIST_ITEM(id++,"Datum Tube Handle"));		//�ֹܾ��
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec",PROPLIST_ITEM(id++,"Column Axis Positive Direction"));		//��������������
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec.x",PROPLIST_ITEM(id++,"X"));		
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_plane",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center",PROPLIST_ITEM(id++,"Section Center"));	//����Բ�ĵ�
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center.x",PROPLIST_ITEM(id++,"X"));	
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm",PROPLIST_ITEM(id++,"Section Normal"));		//���淨��NULL��ʾ����Ĵ�ֱ����
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm_std_style",PROPLIST_ITEM(id++,"Specify Direction"));//���߷�������
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm.x",PROPLIST_ITEM(id++,"X"));	
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec_style",PROPLIST_ITEM(id++,"Projective Direction"));	//���߷����巽ʽ
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec",PROPLIST_ITEM(id++,""));		//���߷���	
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec_std_style",PROPLIST_ITEM(id++,"Specify Direction"));	//���߷�������
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec.x",PROPLIST_ITEM(id++,"X"));		
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_angle",PROPLIST_ITEM(id++,"Rotation Angle"));		//ƫ�ƽǶ�
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_para_offset",PROPLIST_ITEM(id++,"Ray Line Offset"));//����ƽ���ƶ�����
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.bSpecRo",PROPLIST_ITEM(id++,"Offset Datum Point"));		//�����뾶���巽ʽ
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_offset_r",PROPLIST_ITEM(id++,"Offset|Ray Line Radius"));	//�����뾶
#else 
	AddPropItem("datumPoint",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("description",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("pos",PROPLIST_ITEM(id++,"��������"));
	CLDSPoint::AddPropItem("pos.x",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("pos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("pos.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("datum_pos_style",PROPLIST_ITEM(id++,"���㶨λ��ʽ"));
	//��ڵ�ͬλ
	CLDSPoint::AddPropItem("des_para.hNode",PROPLIST_ITEM(id++,"��׼�ڵ���"));
	//ָ�������ϵ��������
	CLDSPoint::AddPropItem("des_para.hPart",PROPLIST_ITEM(id++,"��׼�������"));
	CLDSPoint::AddPropItem("datum_pos",PROPLIST_ITEM(id++,"�������"));
	CLDSPoint::AddPropItem("datum_pos.x",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("datum_pos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("datum_pos.z",PROPLIST_ITEM(id++,"Z"));
	//���߽���
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.hDatum1",PROPLIST_ITEM(id++,"��׼��1���"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.hDatum2",PROPLIST_ITEM(id++,"��׼��2���"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_style1",PROPLIST_ITEM(id++,"֫��ƫ������1"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_style2",PROPLIST_ITEM(id++,"֫��ƫ������2"));//0.g;1.g1;2.g2;3.g3;4.�Զ���
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_dist1",PROPLIST_ITEM(id++,"֫��ƫ��ֵ1"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_dist2",PROPLIST_ITEM(id++,"֫��ƫ��ֵ2"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector",PROPLIST_ITEM(id++,"���Ʒ���"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector[0]",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector[1]",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector[2]",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.norm_offset",PROPLIST_ITEM(id++,"����ƫ����"));	//����ƫ����;
	//�Ǹ����
	CLDSPoint::AddPropItem("des_para.RODEND.hRod",PROPLIST_ITEM(id++,"��׼�˼����"));				//�Ǹ־��
	CLDSPoint::AddPropItem("des_para.RODEND.direction",PROPLIST_ITEM(id++,"����ƫ�Ʒ���"));			//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	CLDSPoint::AddPropItem("des_para.RODEND.offset_wing",PROPLIST_ITEM(id++,"ƫ��֫"));		//0.X֫;1.Y֫
	CLDSPoint::AddPropItem("des_para.RODEND.wing_offset_style",PROPLIST_ITEM(id++,"֫��ƫ������"));	//0.g;1.g1;2.g2;3.g3;4.�Զ���
	CLDSPoint::AddPropItem("des_para.RODEND.wing_offset_dist",PROPLIST_ITEM(id++,"֫��ƫ��ֵ"));
	CLDSPoint::AddPropItem("des_para.RODEND.len_offset_dist",PROPLIST_ITEM(id++,"����ƫ��ֵ"));	//֫��ƫ��ֵ;����ƫ��ֵ;
	CLDSPoint::AddPropItem("des_para.RODEND.bIncOddEffect",PROPLIST_ITEM(id++,"����ƫ�Ƽ�������ͷӰ��"));		//����ƫ��ֵ��������ͷӰ��
	CLDSPoint::AddPropItem("des_para.RODEND.norm_offset",PROPLIST_ITEM(id++,"����ƫ����"));		//����ƫ����
	//�˼��Ͻڵ�Ϊ��׼
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.hLinePart",PROPLIST_ITEM(id++,"��׼�˼����"));	//�Ǹ־��
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.hNode",PROPLIST_ITEM(id++,"��׼�ڵ���"));		//��׼�ڵ���
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.bFaceOffset",PROPLIST_ITEM(id++,"ƽ��"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.direction",PROPLIST_ITEM(id++,"����ƫ�Ʒ���"));	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.offset_wing",PROPLIST_ITEM(id++,"ƫ��֫"));	//0.X֫;1.Y֫
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.wing_offset_style",PROPLIST_ITEM(id++,"֫��ƫ������"));//0.g;1.g1;2.g2;3.g3;4.�Զ���
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.wing_offset_dist",PROPLIST_ITEM(id++,"֫��ƫ��ֵ"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.len_offset_dist",PROPLIST_ITEM(id++,"����ƫ��ֵ"));	//֫��ƫ��ֵ;����ƫ��ֵ;
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.norm_offset",PROPLIST_ITEM(id++,"����ƫ����"));		//����ƫ����;
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset",PROPLIST_ITEM(id++,"ƽ���淨��"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset[0]",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset[1]",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset[2]",PROPLIST_ITEM(id++,"Z"));
	//��׼�����������겻���
	CLDSPoint::AddPropItem("des_para.DATUMLINE.hDatumLine",PROPLIST_ITEM(id++,"��׼�߾��"));
	CLDSPoint::AddPropItem("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2",PROPLIST_ITEM(id++,"ָ������"));
	//��׼�����������겻���
	CLDSPoint::AddPropItem("des_para.DATUMPLANE.hDatumPlane",PROPLIST_ITEM(id++,"��׼����"));
	CLDSPoint::AddPropItem("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2",PROPLIST_ITEM(id++,"ָ������"));
	//���ڵ������ȷֵ�
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.hLinePart",PROPLIST_ITEM(id++,"��׼�˼����"));			//��׼�˼����
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.hStartNode",PROPLIST_ITEM(id++,"ʼ�˻�׼�ڵ���"));		//ʼ�˽ڵ���
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.hEndNode",PROPLIST_ITEM(id++,"�ն˻�׼�ڵ���"));			//�ն˽ڵ���
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.start_offset_dist",PROPLIST_ITEM(id++,"ʼ��ƫ����"));	//ʼ��ƫ����
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.end_offset_dist",PROPLIST_ITEM(id++,"�ն�ƫ����"));	//�ն�ƫ����
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.offset_scale",PROPLIST_ITEM(id++,"ƫ�Ʊ���"));		//ƫ�Ʊ���
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.offset_dist",PROPLIST_ITEM(id++,"ƫ�ƾ���"));		//�ڵȷֻ����ϵ�ƫ��ֵ
	//���涨λ��
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.column",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.column_r",PROPLIST_ITEM(id++,"����뾶"));		//����뾶
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.hLineTube",PROPLIST_ITEM(id++,"��׼�ֹܾ��"));		//�ֹܾ��
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec",PROPLIST_ITEM(id++,"��������������"));		//��������������
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec.x",PROPLIST_ITEM(id++,"X"));		
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_plane",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center",PROPLIST_ITEM(id++,"����Բ��"));	//����Բ�ĵ�
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center.x",PROPLIST_ITEM(id++,"X"));	
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm",PROPLIST_ITEM(id++,"���淨��"));		//���淨��NULL��ʾ����Ĵ�ֱ����
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm_std_style",PROPLIST_ITEM(id++,"ָ������"));//���߷�������
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm.x",PROPLIST_ITEM(id++,"X"));	
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec_style",PROPLIST_ITEM(id++,"���߷���"));	//���߷����巽ʽ
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec",PROPLIST_ITEM(id++,""));		//���߷���	
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec_std_style",PROPLIST_ITEM(id++,"ָ������"));	//���߷�������
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec.x",PROPLIST_ITEM(id++,"X"));		
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_angle",PROPLIST_ITEM(id++,"��ת�Ƕ�"));		//ƫ�ƽǶ�
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_para_offset",PROPLIST_ITEM(id++,"����ƽ�ƾ���"));//����ƽ���ƶ�����
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.bSpecRo",PROPLIST_ITEM(id++,"ƫ�ƻ�׼��"));		//�����뾶���巽ʽ
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_offset_r",PROPLIST_ITEM(id++,"ƫ����|�������߰뾶"));	//�����뾶
#endif
}
long CLDSPoint::GetPropID(char* propStr)			//�õ�����ID
{
	PROPLIST_ITEM* pItem=propHashtable.GetValue(propStr);
	if(pItem)
		return pItem->id;
	else
		return 0;
}
int CLDSPoint::GetPropStatus(long id)					//�õ�����״̬
{
	DWORD dwStatus;
	if(CLDSPoint::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}
BOOL CLDSPoint::SetPropStatus(long id,BOOL bHide)		//��������״̬
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSPoint::propStatusHashtable.SetValueAt(id,dwStatus);
}
int CLDSPoint::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)	//��������ID�õ�����ֵ
{
	CXhChar100 sText;//[100]="";
	if(GetPropID("pos.x")==id)
	{
		sprintf(sText,"%f",pos.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("pos.y")==id)
	{
		sprintf(sText,"%f",pos.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("pos.z")==id)
	{
		sprintf(sText,"%f",pos.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("datum_pos_style")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(datum_pos_style==1)
			strcpy(sText,"1.Angle Ridge Vertex Locating Point");
		else if(datum_pos_style==2)
			strcpy(sText,"2.Rod Node Locating Poin");
		else if(datum_pos_style==3)
			strcpy(sText,"3.Angle Center Line Intersection Point");
		else if(datum_pos_style==4)
			strcpy(sText,"4.Baseline Space Intersection Point");
		else if(datum_pos_style==5)
			strcpy(sText,"5.Specified Component Point Of Datum Line");
		else if(datum_pos_style==6)
			strcpy(sText,"6.Specified Component Point Of Datum Plane");
		else if(datum_pos_style==7)
			strcpy(sText,"7.Outer Angle's Ridge Vertex Along Angle's Normal Offset");
		else if(datum_pos_style==8)
			strcpy(sText,"8.With The Node In The Same Position");
		else if(datum_pos_style==9)
			strcpy(sText,"9.Datum Rod Relative Coordinate");
		else //if(datum_pos_style==0)
			strcpy(sText,"0.Manual Input");
#else 
		if(datum_pos_style==1)
			strcpy(sText,"1.�Ǹ���㶨λ��");
		else if(datum_pos_style==2)
			strcpy(sText,"2.�˼��ڵ㶨λ��");
		else if(datum_pos_style==3)
			strcpy(sText,"3.�Ǹ����߽���");
		else if(datum_pos_style==4)
			strcpy(sText,"4.��׼�߿ռ佻��");
		else if(datum_pos_style==5)
			strcpy(sText,"5.��׼����ָ�����������");
		else if(datum_pos_style==6)
			strcpy(sText,"6.��׼����ָ�����������");
		else if(datum_pos_style==7)
			strcpy(sText,"7.�ؽǸ���֫����ƫ�Ƶİ������");
		else if(datum_pos_style==8)
			strcpy(sText,"8.��ڵ�ͬλ");
		else if(datum_pos_style==9)
			strcpy(sText,"9.��׼�����ϵ��������");
		else //if(datum_pos_style==0)
			strcpy(sText,"0.�ֶ�����");
#endif
	}
	else if(GetPropID("des_para.RODEND.hRod")==id)	//�Ǹ־��
		sprintf(sText,"0X%X",des_para.RODEND.hRod);
	else if(GetPropID("des_para.RODEND.direction")==id)	
	{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.RODEND.direction==0)
			strcpy(sText,"0.Start->End");
		else 
			strcpy(sText,"1.End->Start");
#else 
		if(des_para.RODEND.direction==0)
			strcpy(sText,"0.ʼ->��");
		else 
			strcpy(sText,"1.��->ʼ");
#endif
	}
	else if(GetPropID("des_para.RODEND.offset_wing")==id)//0.X֫;1.Y֫
	{	//0.X֫;1.Y֫
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.RODEND.offset_wing==0)
			strcpy(sText,"0.X Leg");
		else 
			strcpy(sText,"1.Y Leg");
#else 
		if(des_para.RODEND.offset_wing==0)
			strcpy(sText,"0.X֫");
		else 
			strcpy(sText,"1.Y֫");
#endif
	}
	else if(GetPropID("des_para.RODEND.wing_offset_style")==id)	
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(des_para.RODEND.wing_offset_style==0)
			strcpy(sText,"g");
		else if(des_para.RODEND.wing_offset_style==1)
			strcpy(sText,"g1");
		else if(des_para.RODEND.wing_offset_style==2)
			strcpy(sText,"g2");
		else if(des_para.RODEND.wing_offset_style==3)
			strcpy(sText,"g3");
		else 
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User Specify");
#else 
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("des_para.RODEND.wing_offset_dist")==id)
	{
		sprintf(sText,"%f",des_para.RODEND.wing_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.RODEND.len_offset_dist")==id)	
	{	//����ƫ��ֵ;
		sprintf(sText,"%f",des_para.RODEND.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.RODEND.bIncOddEffect")==id)		
	{	//����ƫ��ֵ��������ͷӰ��
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.RODEND.bIncOddEffect)
			strcpy(sText,"Yes");
		else 
			strcpy(sText,"No");
#else 
		if(des_para.RODEND.bIncOddEffect)
			strcpy(sText,"��");
		else 
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("des_para.RODEND.norm_offset")==id)//����ƫ����
	{
		CDesignThickPara thick(des_para.RODEND.norm_offset);
		sprintf(sText,"%d",thick.Thick(NULL));
	}
	else if(GetPropID("des_para.LINEPARTNODE.hLinePart")==id)	//�Ǹ־��
		sprintf(sText,"0X%X",des_para.RODNODE.hRod);
	else if(GetPropID("des_para.LINEPARTNODE.hNode")==id)	//��׼�ڵ���
		sprintf(sText,"0X%X",des_para.RODNODE.hNode);
	else if(GetPropID("des_para.LINEPARTNODE.bFaceOffset")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.RODNODE.bFaceOffset)
			strcpy(sText,"Yes");
		else 
			strcpy(sText,"No");
#else 
		if(des_para.RODNODE.bFaceOffset)
			strcpy(sText,"��");
		else 
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("des_para.LINEPARTNODE.direction")==id)	
	{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.RODNODE.direction==0)
			strcpy(sText,"0.Start->End");
		else 
			strcpy(sText,"1.End->Start");
#else 
		if(des_para.RODNODE.direction==0)
			strcpy(sText,"0.ʼ->��");
		else 
			strcpy(sText,"1.��->ʼ");
#endif
	}
	else if(GetPropID("des_para.LINEPARTNODE.offset_wing")==id)	
	{	//0.X֫;1.Y֫
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.RODNODE.offset_wing==0)
			strcpy(sText,"0.X Leg");
		else 
			strcpy(sText,"1.Y Leg");
#else 
		if(des_para.RODNODE.offset_wing==0)
			strcpy(sText,"0.X֫");
		else 
			strcpy(sText,"1.Y֫");
#endif
	}
	else if(GetPropID("des_para.LINEPARTNODE.wing_offset_style")==id)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(des_para.RODNODE.wing_offset_style==0)
			strcpy(sText,"g");
		else if(des_para.RODNODE.wing_offset_style==1)
			strcpy(sText,"g1");
		else if(des_para.RODNODE.wing_offset_style==2)
			strcpy(sText,"g2");
		else if(des_para.RODNODE.wing_offset_style==3)
			strcpy(sText,"g3");
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User Specify");
#else 
			strcpy(sText,"�Զ���");
#endif
	}	
	else if(GetPropID("des_para.LINEPARTNODE.wing_offset_dist")==id)
	{
		sprintf(sText,"%f",des_para.RODNODE.wing_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.LINEPARTNODE.len_offset_dist")==id)	
	{	//����ƫ��ֵ
		sprintf(sText,"%f",des_para.RODNODE.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.LINEPARTNODE.norm_offset")==id)	//����ƫ����
	{
		CDesignThickPara thick(des_para.RODNODE.norm_offset);
		sprintf(sText,"%d",thick.Thick(NULL));
	}
	else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset")==id)
		sprintf(sText,"%s",CStdVector::GetVecTypeString(des_para.RODNODE.vPlaneNormal));
	else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[0]")==id)
	{
		sprintf(sText,"%f",des_para.RODNODE.vPlaneNormal.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[1]")==id)
	{
		sprintf(sText,"%f",des_para.RODNODE.vPlaneNormal.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.LINEPARTNODE.face_norm_offset[2]")==id)
	{
		sprintf(sText,"%f",des_para.RODNODE.vPlaneNormal.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.AXIS_INTERS.hDatum1")==id)
		sprintf(sText,"0X%X",des_para.AXIS_INTERS.hDatum1);
	else if(GetPropID("des_para.AXIS_INTERS.hDatum2")==id)
		sprintf(sText,"0X%X",des_para.AXIS_INTERS.hDatum2);
	else if(GetPropID("des_para.AXIS_INTERS.wing_offset_style1")==id)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(des_para.AXIS_INTERS.wing_offset_style1==0)
			strcpy(sText,"g");
		else if(des_para.AXIS_INTERS.wing_offset_style1==1)
			strcpy(sText,"g1");
		else if(des_para.AXIS_INTERS.wing_offset_style1==2)
			strcpy(sText,"g2");
		else if(des_para.AXIS_INTERS.wing_offset_style1==3)
			strcpy(sText,"g3");
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User Specify");
#else 
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("des_para.AXIS_INTERS.wing_offset_style2")==id)
	{	//0.g;1.g1;2.g2;3.g3;4.�Զ���
		if(des_para.AXIS_INTERS.wing_offset_style2==0)
			strcpy(sText,"g");
		else if(des_para.AXIS_INTERS.wing_offset_style2==1)
			strcpy(sText,"g1");
		else if(des_para.AXIS_INTERS.wing_offset_style2==2)
			strcpy(sText,"g2");
		else if(des_para.AXIS_INTERS.wing_offset_style2==3)
			strcpy(sText,"g3");
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"User Specify");
#else 
			strcpy(sText,"�Զ���");
#endif
	}
	else if(GetPropID("des_para.AXIS_INTERS.wing_offset_dist1")==id)
	{
		sprintf(sText,"%d",des_para.AXIS_INTERS.wing_offset_dist1);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.AXIS_INTERS.wing_offset_dist2")==id)
	{
		sprintf(sText,"%d",des_para.AXIS_INTERS.wing_offset_dist2);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.AXIS_INTERS.near_vector")==id)
		sprintf(sText,"%s",CStdVector::GetVecTypeString(f3dPoint(des_para.AXIS_INTERS.near_vector)));
	else if(GetPropID("des_para.AXIS_INTERS.near_vector[0]")==id)
	{
		sprintf(sText,"%f",des_para.AXIS_INTERS.near_vector[0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.AXIS_INTERS.near_vector[1]")==id)
	{
		sprintf(sText,"%f",des_para.AXIS_INTERS.near_vector[1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.AXIS_INTERS.near_vector[2]")==id)
	{
		sprintf(sText,"%f",des_para.AXIS_INTERS.near_vector[2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.AXIS_INTERS.norm_offset")==id)	//����ƫ����;
	{
		CDesignThickPara thick(des_para.AXIS_INTERS.norm_offset);
		sprintf(sText,"%d",thick.Thick(NULL));
	}
	else if(GetPropID("des_para.AXIS_INTERS.hDatum1")==id)
		sprintf(sText,"0X%X",des_para.AXIS_INTERS.hDatum1);
	else if(GetPropID("des_para.AXIS_INTERS.hDatum2")==id)
		sprintf(sText,"0X%X",des_para.AXIS_INTERS.hDatum2);
	else if(GetPropID("des_para.DATUMLINE.hDatumLine")==id)
		sprintf(sText,"0X%X",des_para.DATUMLINE.hDatumLine);
	else if(GetPropID("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			sprintf(sText,"0.X axis");
		else if(des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			sprintf(sText,"1.Y axis");
		else 
			sprintf(sText,"2.Z axis");
#else 
		if(des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			sprintf(sText,"0.X��");
		else if(des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			sprintf(sText,"1.Y��");
		else 
			sprintf(sText,"2.Z��");
#endif
	}
	else if(GetPropID("des_para.DATUMPLANE.hDatumPlane")==id)
		sprintf(sText,"0X%X",des_para.DATUMPLANE.hDatumPlane);
	else if(GetPropID("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
			sprintf(sText,"0.X axis");
		else if(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
			sprintf(sText,"1.Y axis");
		else 
			sprintf(sText,"2.Z axis");
#else 
		if(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==0)
			sprintf(sText,"0.X��");
		else if(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
			sprintf(sText,"1.Y��");
		else 
			sprintf(sText,"2.Z��");
#endif
	}
	else if(GetPropID("des_para.RODEND.hRod")==id)	//�Ǹ־��
		sprintf(sText,"0X%X",des_para.RODEND.hRod);
	else if(GetPropID("des_para.RODEND.direction")==id)	
	{	//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.RODEND.direction==0)
			strcpy(sText,"0.Start->End");
		else 
			strcpy(sText,"1.End->Start");
#else 
		if(des_para.RODEND.direction==0)
			strcpy(sText,"0.ʼ->��");
		else 
			strcpy(sText,"1.��->ʼ");
#endif
	}
	else if(GetPropID("des_para.RODEND.len_offset_dist")==id)	
	{	//����ƫ��ֵ;
		sprintf(sText,"%f",des_para.RODEND.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.RODEND.norm_offset")==id)//����ƫ����
	{
		CDesignThickPara thick(des_para.RODEND.norm_offset);
		sprintf(sText,"%d",thick.Thick(NULL));
	}
	else if(GetPropID("des_para.hNode")==id)
		sprintf(sText,"0X%X",des_para.hNode);
	else if(GetPropID("des_para.hPart")==id)
		sprintf(sText,"0X%X",des_para.hPart);
	else if(GetPropID("datum_pos.x")==id)
	{
		sprintf(sText,"%f",datum_pos.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("datum_pos.y")==id)
	{
		sprintf(sText,"%f",datum_pos.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("datum_pos.z")==id)
	{
		sprintf(sText,"%f",datum_pos.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.hLinePart")==id)			//��׼�˼����
		sprintf(sText,"0X%0X",des_para.SCALE_NODE.hLinePart);
	else if(GetPropID("des_para.SCALE_NODE.hStartNode")==id)		//ʼ�˽ڵ���
		sprintf(sText,"0X%0X",des_para.SCALE_NODE.hStartNode);
	else if(GetPropID("des_para.SCALE_NODE.hEndNode")==id)			//�ն˽ڵ���
		sprintf(sText,"0X%0X",des_para.SCALE_NODE.hEndNode);
	else if(GetPropID("des_para.SCALE_NODE.start_offset_dist")==id)	//ʼ��ƫ����
	{
		sprintf(sText,"%f",des_para.SCALE_NODE.start_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.end_offset_dist")==id)	//�ն�ƫ����
	{
		sprintf(sText,"%f",des_para.SCALE_NODE.end_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.offset_scale")==id)		//ƫ�Ʊ���
	{
		sprintf(sText,"%f",des_para.SCALE_NODE.offset_scale);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.offset_dist")==id)		//�ڵȷֻ����ϵ�ƫ��ֵ
	{
		sprintf(sText,"%f",des_para.SCALE_NODE.offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.hLineTube")==id)		//�ֹܾ��
		sprintf(sText,"0X%X",des_para.COLUMN_INTERS.hLineTube);
	else if(GetPropID("des_para.COLUMN_INTERS.axis_vec")==id)
	{
		f3dPoint axis_vec(des_para.COLUMN_INTERS.AxisVector.axis_vec[0],des_para.COLUMN_INTERS.AxisVector.axis_vec[1],des_para.COLUMN_INTERS.AxisVector.axis_vec[2]);
		sText.ConvertFromPoint(axis_vec);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.axis_vec.x")==id)		//��������������
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.AxisVector.axis_vec[0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.axis_vec.y")==id)
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.AxisVector.axis_vec[1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.axis_vec.z")==id)
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.AxisVector.axis_vec[2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_center")==id)
	{
		f3dPoint center(des_para.COLUMN_INTERS.SectCenter.sect_center[0],des_para.COLUMN_INTERS.SectCenter.sect_center[1],des_para.COLUMN_INTERS.SectCenter.sect_center[2]);
		sText.ConvertFromPoint(center);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_center.x")==id)	//����Բ�ĵ�
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.SectCenter.sect_center[0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_center.y")==id)
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.SectCenter.sect_center[1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_center.z")==id)
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.SectCenter.sect_center[2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_norm")==id)
	{
		f3dPoint sect_norm(des_para.COLUMN_INTERS.SectNorm.sect_norm[0],des_para.COLUMN_INTERS.SectNorm.sect_norm[1],des_para.COLUMN_INTERS.SectNorm.sect_norm[2]);
		sText.ConvertFromPoint(sect_norm);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_norm_std_style")==id)
	{
		f3dPoint norm(des_para.COLUMN_INTERS.SectNorm.sect_norm[0],des_para.COLUMN_INTERS.SectNorm.sect_norm[1],des_para.COLUMN_INTERS.SectNorm.sect_norm[2]);
		sprintf(sText,"%s",CStdVector::GetVecTypeString(norm));
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_norm.x")==id)	//���淨��NULL��ʾ����Ĵ�ֱ����
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.SectNorm.sect_norm[0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_norm.y")==id)
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.SectNorm.sect_norm[1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.sect_norm.z")==id)
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.SectNorm.sect_norm[2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_vec_style")==id)	//���߷����巽ʽ
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.COLUMN_INTERS.ray_vec_style==0)
			strcpy(sText,"0.Specify Directly");
		else if(des_para.COLUMN_INTERS.ray_vec_style==1)
			strcpy(sText,"1.Specify Datum Edge");
		else if(des_para.COLUMN_INTERS.hLineTube>0x20 && des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"2.Base On Tube Edge");
		else if(des_para.COLUMN_INTERS.hLineTube>0x20 && des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"3.Base On Tube Weld Line");
#else 
		if(des_para.COLUMN_INTERS.ray_vec_style==0)
			strcpy(sText,"0.ֱ��ָ��");
		else if(des_para.COLUMN_INTERS.ray_vec_style==1)
			strcpy(sText,"1.ָ����׼��");
		else if(des_para.COLUMN_INTERS.hLineTube>0x20 && des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"2.�Ըֹܻ�׼��Ϊ��׼");
		else if(des_para.COLUMN_INTERS.hLineTube>0x20 && des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"3.�Ըֹܺ�����Ϊ��׼");
#endif
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_vec_std_style")==id)
	{
		f3dPoint rec_vector(des_para.COLUMN_INTERS.RayVector.ray_vec[0],des_para.COLUMN_INTERS.RayVector.ray_vec[1],des_para.COLUMN_INTERS.RayVector.ray_vec[2]);
		sprintf(sText,"%s",CStdVector::GetVecTypeString(rec_vector));
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_vec.x")==id)		//���߷���
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.RayVector.ray_vec[0]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_vec.y")==id)
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.RayVector.ray_vec[1]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_vec.z")==id)
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.RayVector.ray_vec[2]);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_angle")==id)		//ƫ�ƽǶ�
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.ray_angle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.column_r")==id)		//����뾶
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.column_r);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_para_offset")==id)	//����ƽ���ƶ�����
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.ray_para_offset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.bSpecRo")==id)			//�����뾶���巽ʽ
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(!des_para.COLUMN_INTERS.bSpecRo)
			strcpy(sText,"0.Ray Line And Column Intersection");
		else
			strcpy(sText,"1.Column Center");
#else 
		if(!des_para.COLUMN_INTERS.bSpecRo)
			strcpy(sText,"0.���������潻��");
		else
			strcpy(sText,"1.Բ������");
#endif
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_offset_r")==id)	//�����뾶
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.ray_offset_r);
		SimplifiedNumString(sText);
	}
	else 
		strcpy(sText,"");
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}

IModel* CLDSVector::m_pTower=NULL;
CLDSVector::CLDSVector()
{
	hVicePart=0;				//�������߻�׼�Ǹ�
	hCrossPart=0;
	norm_wing=direction=0;	//0.X֫;1.Y֫	//����0.����;1.���� 
	norm_style=0;			//0.ָ��;1.�Ǹ�֫����;2�����淨��
	rot_ang=0;
	hAgentBlockRef=0;
}
void CLDSVector::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	buffer.ReadWord(&norm_style);
	buffer.ReadWord(&direction);
	buffer.ReadInteger(&hVicePart);
	if(norm_style==2||CTower::IsPrevDeflateVersion(version,doc_type))
		buffer.ReadInteger(&hCrossPart);
	else
		hCrossPart=0;
	buffer.ReadWord(&norm_wing);
	buffer.ReadPoint(nearVector);
	if(norm_style==0||norm_style==4)
		buffer.ReadPoint(vector);
	else if(CTower::IsPrevDeflateVersion(version,doc_type))	//�ļ�����֮ǰ�İ汾
		buffer.ReadPoint(vector);
	if( norm_style==5&&(version==0||doc_type==5||
		(doc_type==1&&version>=4000105)||(doc_type==2&&version>=1000105)||
		(doc_type==3&&version>=1070305)||(doc_type==4&&version>=1000305)))
		buffer.ReadDouble(&rot_ang);
}
void CLDSVector::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	buffer.WriteWord(norm_style);
	buffer.WriteWord(direction);
	buffer.WriteInteger(hVicePart);
	if(norm_style==2)	//�˼������淨�߶�λ
		buffer.WriteInteger(hCrossPart);
	buffer.WriteWord(norm_wing);
	buffer.WritePoint(nearVector);
	if(norm_style==0||norm_style==4)	//ָ�����߻��׼�����ϵ�ָ������
		buffer.WritePoint(vector);
	if(norm_style==5)
		buffer.WriteDouble(rot_ang);
}
void CLDSVector::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<������Ʋ���>\n");
	if (norm_style==0)//0.ָ��;1.�Ǹ�֫����;2.�˼������淨��3.������������4.��׼�����ϵ���Է���5.��׼�˼��ϵ���������
		fprintf(fp,"<��λ��ʽ norm_style=\"0.ָ��\"/>\n");
	else if (norm_style==1)
		fprintf(fp,"<��λ��ʽ norm_style=\"1.�Ǹ�֫����\"/>\n");
	else if (norm_style==2)
		fprintf(fp,"<��λ��ʽ norm_style=\"2.�˼������淨��\"/>\n");
	else if (norm_style==3)
		fprintf(fp,"<��λ��ʽ norm_style=\"3.������������\"/>\n");
	else if (norm_style==4)
		fprintf(fp,"<��λ��ʽ norm_style=\"4.��׼�����ϵ���Է���\"/>\n");
	else if (norm_style==5)
		fprintf(fp,"<��λ��ʽ norm_style=\"5.��׼�˼��ϵ���������\"/>\n");
	if (direction==0)
		fprintf(fp,"<���� direction=\"0.����\"/>\n");
	else if (direction==1)
		fprintf(fp,"<���� direction=\"1.����\"/>\n");
	fprintf(fp,"<�������߻�׼�˼� hVicePart=\"OX%X\"/>\n",hVicePart);
	if(norm_style==2)	//�˼������淨�߶�λ
		fprintf(fp,"<����˼� hCrossPart=\"OX%X\"/>\n",hCrossPart);
	if (norm_wing==0)
		fprintf(fp,"<��׼֫ norm_wing=\"0.X֫\"/>\n");
	else if (norm_wing==1)				
		fprintf(fp,"<��׼֫ norm_wing=\"1.Y֫\"/>\n");
	sprintf(sText,"%f",nearVector.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",nearVector.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",nearVector.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<���Ƶķ���ʸ�� nearVector=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	if(norm_style==0||norm_style==4)	//ָ�����߻��׼�����ϵ�ָ������
	{
		sprintf(sText,"%f",vector.x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",vector.y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",vector.z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<ʵ��������ķ���ʸ�� vector=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	}
	if(norm_style==5)
	{
		sprintf(sText,"%f",rot_ang);
		SimplifiedNumString(sText);
		fprintf(fp,"<�Ƹֹ���ʱ����תָ���ĽǶ� rot_ang=\"%s\"/>\n",sText);
	}
	fprintf(fp,"</������Ʋ���>\n");
}
void CLDSVector::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(CLDSObject::_console==NULL||m_pTower==NULL)	//�ѵ��ù�)
		return;
	CLDSPart* pPart;
	if(norm_style>0)	//�Ǹ�֫���ߵ�
	{
		pPart=m_pTower->FromPartHandle(hVicePart);
		if(pPart!=NULL)
			pPart->AddToConsoleObjBuf(bAddRelaObj&&bAddSubLevelRelaObj);
	}
	if(norm_style==2)	//�˼������淨��
	{
		pPart=m_pTower->FromPartHandle(hCrossPart);
		if(pPart!=NULL)
			pPart->AddToConsoleObjBuf(bAddRelaObj&&bAddSubLevelRelaObj);
	}
}

BOOL CLDSVector::AssertValid(IModel *pTower,char *error_info/*=NULL*/)
{
	if(norm_style==1&&pTower->FromPartHandle(hVicePart,CLS_LINEANGLE,CLS_GROUPLINEANGLE)==NULL)
	{
		if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(error_info,"The handle of specified angle is illegal��");
#else 
			strcpy(error_info,"ָ���Ǹ־���Ƿ���");
#endif
		return FALSE;
	}
	else if(norm_style==2)
	{
		if(pTower->FromPartHandle(hVicePart,CLS_LINEPART)==NULL)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified rod is illegal��");
#else 
				strcpy(error_info,"ָ����׼�˼�����Ƿ���");
#endif
			return FALSE;
		}
		if(pTower->FromPartHandle(hCrossPart,CLS_LINEPART)==NULL)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified cross-bracing rod is illegal��");
#else 
				strcpy(error_info,"ָ������˼�����Ƿ���");
#endif
			return FALSE;
		}
	}
	else if(norm_style==5)
	{	//�˼��ϵ���������
		CLDSLinePart *pLinePart=(CLDSLinePart*)pTower->FromPartHandle(hVicePart,CLS_LINEPART);
		if(pLinePart==NULL)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified rod is illegal��");
#else 
				strcpy(error_info,"ָ����׼�˼�����Ƿ���");
#endif
			return FALSE;
		}
		if(!pLinePart->IsAngle()&&!pLinePart->IsTube()&&pLinePart->GetClassTypeId()!=CLS_LINESLOT&&direction>2)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The specified datum rod isn't angle, tube or [ steel handle��");
#else 
				strcpy(error_info,"ָ���Ļ�׼�˼����ǽǸ֡��ֹܻ�۸־����");
#endif
			return FALSE;
		}
	}
	else if(norm_style!=0)
	{	
		if(pTower->FromPartHandle(hVicePart)==NULL)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified part is illegal��");
#else 
				strcpy(error_info,"ָ����׼��������Ƿ���");
#endif
			return FALSE;
		}
	}
	return TRUE;
}
bool CLDSVector::ReplaceReferenceHandle(CHandleReplacer& replacer)
{
	bool replaced=false;
	replaced|=replacer.VerifyAndReplace(&hCrossPart);
	if(hVicePart==replacer.hOld)
	{
		if(replacer.IsReverseWingXAndWingY()&&norm_style==1&&norm_wing<2)
			norm_wing=1-norm_wing;	//1.�Ǹ�֫����
		else if(replacer.IsReverseWingXAndWingY()&&norm_style==5&&direction>1)
		{	//��׼�˼��ϵ���������(hVicePartָ���ǽǸ�ʱIsReverseWingXAndWingY()!=true) wjh-2016.6.14
			if(direction==2||direction==3)
				direction+=2;
			else if(direction==4||direction==5)
				direction-=2;
			else//direction>5��Ϊ5
				direction=3;
		}
		if(replacer.IsReverseStartAndEnd()&&norm_style==5&&direction<=1)
			direction=1-direction;	//��תʼĩ��
	}
	replaced|=replacer.VerifyAndReplace(&hVicePart);
	//if(replaced)
	//	SetModified();
	return replaced;
}
long CLDSVector::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	if(hCrossPart>0)
		pListHandlers->Append(HANDLE_ADDR(&hCrossPart));
	if(hVicePart>0)
		pListHandlers->Append(HANDLE_ADDR(&hVicePart));
	return 2;
}
void CLDSVector::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	if(pModel==NULL)
		pModel=m_pTower;
	if(pModel==NULL)
		return;
	pModel->ValidateObjRefH(&hCrossPart);
	pModel->ValidateObjRefH(&hVicePart);
}
BOOL CLDSVector::UpdateVector(IModel *pTower)
{
	if(norm_style==0)	//ָ������ʸ��
		return normalize(vector);
	if(pTower==NULL)
		pTower=m_pTower;
	if(pTower==NULL)
		return FALSE;
	CSuperSmartPtr<CLDSPart> pPart,pDatumPart,pLinePart,pCrossLinePart;
	pPart=pTower->FromPartHandle(hVicePart);
	if(pPart.IsHasPtr())
	{
		pDatumPart=pPart;
		if(pPart->IsLinePart())
			pLinePart=pPart;
	}
	pPart=pTower->FromPartHandle(hCrossPart);
	if(pPart.IsHasPtr()&&pPart->IsLinePart())
		pCrossLinePart=pPart;
	vector.Set();	//��ָ������ʱ�ڴ˴���ʼ��vector wht 11-05-24
	//try{
		if(norm_style==2&&pLinePart.IsHasPtr()&&pCrossLinePart.IsHasPtr())
		{	//�˼������淨��
			f3dPoint vec=pLinePart.LinePartPointer()->End()-pLinePart.LinePartPointer()->Start();
			f3dPoint cross_vec=pCrossLinePart.LinePartPointer()->End()-pCrossLinePart.LinePartPointer()->Start();
			normalize(vec);
			normalize(cross_vec);
			if(fabs(vec*cross_vec)>EPS_COS)	//���˼��ӽ����ߣ���ʱ���淨��Ӧ���˼�����
			{
				cross_vec=pCrossLinePart.LinePartPointer()->Start()-pLinePart.LinePartPointer()->Start();
				if(cross_vec.IsZero())
					cross_vec=pCrossLinePart.LinePartPointer()->Start()-pLinePart.LinePartPointer()->End();
				normalize(cross_vec);
				vector=vec^cross_vec;
			}
			else
				vector=vec^cross_vec;
			normalize(vector);
			if(!nearVector.IsZero()&&nearVector*vector<0)
				vector*=-1.0;
		}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		else if(norm_style==1&&pLinePart.IsHasPtr()&&(pLinePart->GetClassTypeId()==CLS_LINEANGLE||pLinePart->GetClassTypeId()==CLS_GROUPLINEANGLE))
		{	//�Ǹ�֫����
			if(norm_wing==0&&direction==0)
				vector=pLinePart.LineAnglePointer()->get_norm_x_wing();
			else if(norm_wing==0&&direction==1)
				vector=-pLinePart.LineAnglePointer()->get_norm_x_wing();
			else if(norm_wing==1&&direction==0)
				vector=pLinePart.LineAnglePointer()->get_norm_y_wing();
			else if(norm_wing==1&&direction==1)
				vector=-pLinePart.LineAnglePointer()->get_norm_y_wing();
			else
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("The parametric direction of angle's normal specify unreasonable vector direction{%d}",direction);
#else 
				logerr.Log("�Ǹ�֫�������͵Ĳ�������ָ���˲������ʸ������{%d}",direction);
#endif
		}
		else if(norm_style==3&&pDatumPart.IsHasPtr())
		{	//������������
			if(direction==0)	//����
				vector=pDatumPart->ucs.axis_z;
			else
				vector=-pDatumPart->ucs.axis_z;
			if(pDatumPart->GetClassTypeId()==CLS_PARAMPLATE&&norm_wing==10)
			{	//ʮ�ֲ���������崹ֱ�İ��ϵ���˨
				if(direction==0)	//����
					vector=pDatumPart->ucs.axis_y;
				else
					vector=-pDatumPart->ucs.axis_y;
			}
			else if(pDatumPart->GetClassTypeId()==CLS_PLATE&&reside_plane>0&&reside_plane<pDatumPart.PlatePointer()->face_N)
			{	//����������ϵĹ��߿�������˨ wjh-2015.2.7
				if(direction==0)	//����
					vector=pDatumPart.PlatePointer()->huoqufacenorm[reside_plane-1];
				else
					vector=-pDatumPart.PlatePointer()->huoqufacenorm[reside_plane-1];
			}
			normalize(vector);
		}
		else if(norm_style==4&&pDatumPart.IsHasPtr())
		{	//��׼��������ϵ�µ���Է���
			if(!nearVector.IsZero())
			{
				vector=nearVector;
				vector_trans(vector,pDatumPart->ucs,TRUE);
				normalize(vector);
			}
		}
		else if(norm_style==5&&pLinePart.IsHasPtr())
		{	//��׼�˼��ϵ���������
			if(direction<2)
			{
				if(direction==0)
					vector=pLinePart.LinePartPointer()->End()-pLinePart.LinePartPointer()->Start();
				else if(direction==1)
					vector=pLinePart.LinePartPointer()->Start()-pLinePart.LinePartPointer()->End();
				normalize(vector);
			}
			else if(pLinePart->IsAngle())
			{	//��׼�˼��ǽǸ�
				if(direction==2)
					vector= pLinePart.LineAnglePointer()->get_norm_x_wing();
				else if(direction==3)
					vector=-pLinePart.LineAnglePointer()->get_norm_x_wing();
				else if(direction==4)
					vector= pLinePart.LineAnglePointer()->get_norm_y_wing();
				else
					vector=-pLinePart.LineAnglePointer()->get_norm_y_wing();
			}
			else if(pLinePart->IsTube())
			{	//��׼�˼��Ǹֹ�
				if(direction==2||direction==3)
				{	
					double rot_radang=rot_ang*RADTODEG_COEF;
					f3dPoint axis_vec=pLinePart.LineTubePointer()->End()-pLinePart.LineTubePointer()->Start();
					vector=pLinePart.LineTubePointer()->WorkPlaneNorm();
					if(direction==2)
					{	//�Ի�׼��ƽ�淨���Ƹֹ���ʱ����תrot_ang�ǶȺ�ķ���
						if(rot_radang!=0)
							RotateVectorAroundVector(vector,rot_radang,axis_vec);
					}
					else if(direction==3)
					{	//�Ժ�����ƽ�淨���Ƹֹ���ʱ����תrot_ang�ǶȺ�ķ���
						double weld_ang=pLinePart.LineTubePointer()->CalWeldRoadAngle();
						if(weld_ang+rot_radang!=0)
							RotateVectorAroundVector(vector,weld_ang+rot_radang,axis_vec);
					}
				}
				//�����ֹ��������� 4.�ֹ�ʼ���и��淨�߷�������.�ֹ��ն��и��淨�� wht 12-09-05
				else if(direction==4)
					vector=pLinePart.LineTubePointer()->m_tJointStart.cutPlaneNorm;
				else 
					vector=pLinePart.LineTubePointer()->m_tJointEnd.cutPlaneNorm;
			}
			else if(pLinePart->GetClassTypeId()==CLS_LINEFLAT)
			{	//��׼�˼��ǲ۸� 2.������������+��3.�������߸���-
				pLinePart.pFlat->BuildUCS();
				if(direction==2)
					vector= pLinePart.pFlat->WorkPlaneNorm();
				else if(direction==3)
					vector=-pLinePart.pFlat->WorkPlaneNorm();
			}
			else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
			{	//��׼�˼��ǲ۸� 2.�۸�֫���߷���(X�᷽��)+��3.�۸�֫���߷���(X�᷽��)-
				//				 4.�۸ֵ��淨�߷���(Y�᷽��)+��5.�۸ֵ��淨�߷���(Y�᷽��)- wht 12-09-26
				pLinePart.LineSlotPointer()->BuildUCS();
				if(direction==2)
					vector= pLinePart->ucs.axis_x;
				else if(direction==3)
					vector=-pLinePart->ucs.axis_x;
				else if(direction==4)
					vector= pLinePart.pSlot->WorkPlaneNorm();
				else
					vector=-pLinePart.pSlot->WorkPlaneNorm();
			}
		}
#endif
		else
			return FALSE;
		vector.Fillet();
		return TRUE;
	/*}
	catch(CPtrException e)
	{
		//AfxMessageBox(e.sError);
	}*/
	return FALSE;
};
//<DEVELOP_PROCESS_MARK nameId="CLDSVector::ReverseDirection">
//��ת���ⷽ��,���ڸֹ��ϻ�׼��ת����ʱ����false
bool CLDSVector::ReverseDirection(bool bUpdateVector/*=true*/)
{
	if(direction<0)
		return false;
	if(norm_style==2||norm_style==4)
	{
		vector*=-1.0;
		if(nearVector.IsZero())
			nearVector=vector;
		else
			nearVector*=-1.0;
		return !nearVector.IsZero();
	}
	else if(norm_style==5&&direction>1)
	{
		CSuperSmartPtr<CLDSPart>pDatumPart=m_pTower->FromPartHandle(hVicePart);
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
			return false;
	}
	vector*=-1.0;
	if(direction%2==0)	//ż����1
		direction+=1;
	else				//������1
		direction-=1;
	if(bUpdateVector)
		UpdateVector(NULL);
	return true;
}
//</DEVELOP_PROCESS_MARK>
void CLDSVector::MirToVector(IModel *pModel,CLDSVector &mirVector,MIRMSG& mirinfo)
{
	if(pModel==NULL)
	{
		CLDSObject::Log2File()->Log("δ���뵱ǰ����ģ��,�޷����в���������ĶԳ�",norm_style);
		return;
	}
	UpdateVector(pModel);		//srcVectorδ����ǰ���жԳƣ����ܵ��µ�ǰ֫�жϴ��� wht 16-09-09
	mirVector.vector=GetMirVector(vector,mirinfo);
	//��������Է���ʱnearVector��ʾ�ֲ�����ϵ�·����ܷ��ţ�Ӧ�ں�����������ж�wjh-2016.9.19
	mirVector.nearVector=norm_style!=4 ? GetMirVector(nearVector,mirinfo) : nearVector;
	mirVector.norm_style=norm_style;
	if(norm_style==1)	//�Ǹ�֫����
	{
		CLDSLineAngle* pAngle=(CLDSLineAngle*)pModel->FromPartHandle(hVicePart,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		CLDSLineAngle* pMirAngle=NULL;
		if(pAngle!=NULL)
			pMirAngle=(CLDSLineAngle*)pAngle->GetMirPart(mirinfo);
#ifdef AFX_TARG_ENU_ENGLISH
		else
			CLDSObject::Log2File()->Log("The leg-normal reference angle handle '0X%X' in parametric vector is invalid, current symmetric operation failed",hVicePart);
		if(pAngle!=NULL && pMirAngle==NULL)
			CLDSObject::Log2File()->Log("The leg-normal reference angle handle '0X%X' in parametric vector is invalid, current symmetric operation failed",hVicePart);
#else
		else
			CLDSObject::Log2File()->Log("�����������е�֫���߲��սǸ־��'0X%X'���Ϸ�,�޷����в���������ĶԳ�",hVicePart);
		if(pAngle!=NULL && pMirAngle==NULL)
			CLDSObject::Log2File()->Log("�����������е�֫���߲��սǸ־��'0X%X'δ����Ӧ�ĶԳƽǸ�,�޷����в���������ĶԳ�",hVicePart);
#endif
		if(pMirAngle)
		{
			mirVector.hVicePart=pMirAngle->handle;
			f3dPoint datum_wing_norm;
			if(norm_wing==0)	//X֫����
				datum_wing_norm=pAngle->get_norm_x_wing();
			else
				datum_wing_norm=pAngle->get_norm_y_wing();
			if(direction==1)	//����
				datum_wing_norm*=-1;
			f3dPoint dest_wing_norm=mirinfo.MirVector(datum_wing_norm);
			double ddx=dest_wing_norm*pMirAngle->get_norm_x_wing();
			double ddy=dest_wing_norm*pMirAngle->get_norm_y_wing();
			if(fabs(ddx)>fabs(ddy))
			{	//X֫Ϊ��׼֫
				mirVector.norm_wing=0;
				mirVector.direction= ddx>0?0:1;
			}
			else
			{	//Y֫Ϊ��׼֫
				mirVector.norm_wing=1;
				mirVector.direction= ddy>0?0:1;
			}
		}
	}
	else if(norm_style==2)//�˼������淨��
	{
		MirPartHandle(pModel,&mirVector.hVicePart,hVicePart,mirinfo);
		MirPartHandle(pModel,&mirVector.hCrossPart,hCrossPart,mirinfo);
	}
	else if(norm_style==3)//������������
	{
		GEPOINT datum_wing_norm,dest_wing_norm;
		CLDSPart* pDatumPart=pModel->FromPartHandle(hVicePart);
		CLDSPart* pMirPart=NULL;
		if(pDatumPart!=NULL)
		{
			pMirPart=pDatumPart->GetMirPart(mirinfo);
			datum_wing_norm=pDatumPart->ucs.axis_z;
			if(direction==1)	//����
				datum_wing_norm*=-1;
			dest_wing_norm=mirinfo.MirVector(datum_wing_norm);
			if(dest_wing_norm*mirVector.vector<0)
				direction=1-direction;	//����
		}
	}
	else if(norm_style==4)//��׼�����ϵ���Է���
	{
		GEPOINT datum_wing_norm,dest_wing_norm,dest_wing_norm2;
		CLDSPart* pDatumPart=pModel->FromPartHandle(hVicePart);
		if(pDatumPart==NULL)
			return;
		CLDSPart* pMirPart=pDatumPart->GetMirPart(mirinfo);
		datum_wing_norm=nearVector;
		vector_trans(datum_wing_norm,pDatumPart->ucs,TRUE,TRUE);
		dest_wing_norm=mirinfo.MirVector(datum_wing_norm);
		dest_wing_norm2=mirVector.nearVector;
		if(pMirPart)
			vector_trans(dest_wing_norm2,pMirPart->ucs,TRUE,TRUE);
		if(dest_wing_norm*dest_wing_norm<0)
			mirVector.nearVector*=-1;	//����
	}
	else
	{
		CLDSPart* pDatumPart=pModel->FromPartHandle(hVicePart);
		if(pDatumPart==NULL)
			return;
		CLDSPart* pMirPart=pDatumPart->GetMirPart(mirinfo);
		if(norm_style==5&&pDatumPart->IsAngle()||direction<2)
		{
			GEPOINT mirvec=mirVector.vector;
			mirVector.UpdateVector(pModel?pModel:m_pTower);
			if(mirvec*mirVector.vector<0)
				direction%2==0?direction+=1:direction-=1;	//ż����1, ������1
		}
		else if(norm_style!=0)
		{
			CLDSObject::Log2File()->Log("�ݲ�֧�ַ�ʽ%d���͵Ĳ���������ĶԳ�",norm_style);
			return;
		}
	}
}
char *CLDSVector::GetDirectionDes()
{
	if(norm_style==1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(direction==0)
			return "0.Outward";
		else
			return "1.Inward";
#else 
		if(direction==0)
			return "0.����";
		else
			return "1.����";
#endif
	}
	else if(norm_style==5)
	{
		CSuperSmartPtr<CLDSPart>pDatumPart=m_pTower->FromPartHandle(hVicePart);
#ifdef AFX_TARG_ENU_ENGLISH
		if(direction==0)
			return "0.Start->End";
		else if(direction==1)
			return "1.End->Start";
#else 
		if(direction==0)
			return "0.ʼ->��";
		else if(direction==1)
			return "1.��->ʼ";
#endif
		else
		{
			if(pDatumPart.IsHasPtr()&&pDatumPart->IsAngle())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(direction==2)
					return "2.X Leg Normal+";
				else if(direction==3)
					return "3.X Leg Normal-";
				else if(direction==4)
					return "4.Y Leg Normal+";
				else 
					return "5.Y Leg Normal-";
#else 
				if(direction==2)
					return "2.X֫����+";
				else if(direction==3)
					return "3.X֫����-";
				else if(direction==4)
					return "4.Y֫����+";
				else 
					return "5.Y֫����-";
#endif
			}
			else if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(direction==2)
					return "2.Rotate along datum plane";
				else if(direction==3)
					return "3.Rotate along weld road plane";
				else if(direction==4)
					return "4.Start cutting plane normal";
				else
					return "5.End cutting plane normal";
#else 
				if(direction==2)
					return "2.�Ի�׼����ת";
				else if(direction==3)
					return "3.�Ժ�������ת";
				else if(direction==4)
					return "4.ʼ���и�ƽ�淨��";
				else
					return "5.�ն��и�ƽ�淨��";
#endif
			}
			else if(pDatumPart.IsHasPtr()&&pDatumPart->GetClassTypeId()==CLS_LINESLOT)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				if(direction==2)
					return "2.[ steel Leg Normal+";
				else if(direction==3)
					return "3.[ steel Leg Normal-";
				else if(direction==4)
					return "4.[ steel Side Normal+";
				else 
					return "5.[ steel Side Normal-";
#else 
				if(direction==2)
					return "2.�۸�֫����+";
				else if(direction==3)
					return "3.�۸�֫����-";
				else if(direction==4)
					return "4.�۸ֲ෨��+";
				else 
					return "5.�۸ֲ෨��-";
#endif
			}
		}
	}
	else 
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(direction==0)
			return "0.Forward Direction";
		else
			return "1.Backward Direction";
#else 
		if(direction==0)
			return "0.����";
		else
			return "1.����";
#endif
	}
	return "";
}

char *CLDSVector::GetDirectionCmbItems()
{
#ifdef AFX_TARG_ENU_ENGLISH
	if(norm_style==1)
		return "0.Outward|1.Inward";
	else if(norm_style==5)
	{
		CSuperSmartPtr<CLDSPart>pDatumPart=m_pTower->FromPartHandle(hVicePart);
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsAngle())
			return "0.Start->End|1.End->Start|2.X Leg Normal+|3.X Leg Normal-|4.Y Leg Normal+|5.Y Leg Normal-";
		else if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
			return "0.Start->End|1.End->Start|2.Rotate Along Datum Plane|3.Rotate Along Weld Plane|4.Start Cutting Plane Normal|5.End Cutting Plane Normal";
		else if(pDatumPart.IsHasPtr()&&pDatumPart->GetClassTypeId()==CLS_LINESLOT)
			return "0.Start->End|1.End->Start|2.[ steel Leg Normal+|3.[ steel Leg Normal-|4.[ steel Side Normal+|5.U-steel Side Normal-";
		else 
			return "0.Start->End|1.End->Start";
	}
	else 
		return "0.Forward Direction|1.Backward Direction";
#else 
	if(norm_style==1)
		return "0.����|1.����";
	else if(norm_style==5)
	{
		CSuperSmartPtr<CLDSPart>pDatumPart=m_pTower->FromPartHandle(hVicePart);
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsAngle())
			return "0.ʼ->��|1.��->ʼ|2.X֫����+|3.X֫����-|4.Y֫����+|5.Y֫����-";
		else if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
			return "0.ʼ->��|1.��->ʼ|2.�Ի�׼����ת|3.�Ժ�������ת|4.ʼ���и�ƽ�淨��|5.�ն��и�ƽ�淨��";
		else if(pDatumPart.IsHasPtr()&&pDatumPart->GetClassTypeId()==CLS_LINESLOT)
			return "0.ʼ->��|1.��->ʼ|2.�۸�֫����+|3.�۸�֫����-|4.�۸ֲ෨��+|5.�۸ֲ෨��-";
		else 
			return "0.ʼ->��|1.��->ʼ";
	}
	else 
		return "0.����|1.����";
#endif
}

char *CLDSVector::GetDirectionHelpStr()
{
#ifdef AFX_TARG_ENU_ENGLISH
	if(norm_style==1)
		return "0.Outward 1.Inward";
	else if(norm_style==5)
	{
		CSuperSmartPtr<CLDSPart>pDatumPart=m_pTower->FromPartHandle(hVicePart);
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsAngle())
			return "0.Rod Start->End Direction,1.Rod End->Start Direction,2.Angle X-leg  normal,3.Angle X-leg normal reverse direction,4.Angle Y-leg  normal,5.Angle Y-leg normal reverse direction";
		else if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
			return "0.Rod Start->End Direction\n 1.Rod End->Start Direction\n 2.The specified rotate angle that tube's datum pane around datum axis\n 3.The specified rotate angle that tube's weld road pane around datum axis\n 4,Tube Start Cutting Plane Normal\n 5,Tube End Cutting Plane Normal\n";
		else if(pDatumPart.IsHasPtr()&&pDatumPart->GetClassTypeId()==CLS_LINESLOT)
			return "0.Rod Start->End Direction,1.Rod End->Start Direction,2.[ steel Normal,3.[ steel Normal Backward Direction,4.[ steel Leg Normal,5.[ steel  Leg Normal Backward Direction";
		else 
			return "0.Rod Start->End Direction,1.Rod End->Start Direction";
	}
	else 
		return "Forward direction is the plus work normal  of datum part��reverse direction is the minus work normal  of datum part";
#else 
	if(norm_style==1)
		return "0.���� 1.����";
	else if(norm_style==5)
	{
		CSuperSmartPtr<CLDSPart>pDatumPart=m_pTower->FromPartHandle(hVicePart);
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsAngle())
			return "0.�˼�ʼ->�շ���,1.�˼���->ʼ����,2.�Ǹ�X֫���߷���,3.�Ǹ�X֫���߸�����,4.�Ǹ�Y֫���߷���,5.�Ǹ�Y֫���߸�����";
		else if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
			return "0.�˼�ʼ->�շ���\n 1.�˼���->ʼ����\n 2.�Ըֹܻ�׼��ƽ����������תָ���Ƕȡ�\n 3.�Ըֹܺ�����ƽ����������תָ���Ƕȡ�\n 4.�ֹ�ʼ���и�ƽ�淨�߷���\n 5.�ֹ��ն��и�ƽ�淨�߷���\n";
		else if(pDatumPart.IsHasPtr()&&pDatumPart->GetClassTypeId()==CLS_LINESLOT)
			return "0.�˼�ʼ->�շ���,1.�˼���->ʼ����,2.�۸ַ��߷���,3.�۸ַ��߸�����,4.�۸�֫���߷���,5.�۸�֫���߸�����";
		else 
			return "0.�˼�ʼ->�շ���,1.�˼���->ʼ����";
	}
	else 
		return "������ָ��׼���������������߷��򣻷�����ָ��׼�����ĸ��������߷���";
#endif
}

char *CLDSVector::GetNormStyleDes()
{
#ifdef AFX_TARG_ENU_ENGLISH
	if(norm_style==1)
		return "1.Single Angle Leg Normal";
	else if(norm_style==2)
		return "2.Two Rod Cross-bracing Normal";
	else if(norm_style==3)
		return "3.Part Work Normal";
	else if(norm_style==4)
		return "4.Part Relative Direction";
	else if(norm_style==5)
		return "5.Rod Characteristic Direction";
	else if(norm_style==6)
		return "6.Determined By Another Leg.";
	else //if(norm_style==0)
		return "0.Specify Normal";
#else 
	if(norm_style==1)
		return "1.���Ǹ�֫����";
	else if(norm_style==2)
		return "2.���˼������淨��";
	else if(norm_style==3)
		return "3.������������";
	else if(norm_style==4)
		return "4.�����ϵ���Է���";
	else if(norm_style==5)
		return "5.�˼��ϵ���������";
	else if(norm_style==6)
		return "6.����һ֫ȷ��";
	else //if(norm_style==0)
		return "0.ָ������";
#endif

}
char *CLDSVector::GetNormStyleCmbItems()
{
#ifdef AFX_TARG_ENU_ENGLISH
	return "0.Specify Normal|1.Single Angle Leg Normal|2.Two Rod Cross-bracing Normal|3.Part Work Normal|4.Part Relative Direction|5.Rod Characteristic Direction";
#else 
	return "0.ָ������|1.���Ǹ�֫����|2.���˼������淨��|3.������������|4.�����ϵ���Է���|5.�˼��ϵ���������";
#endif
}

CHashStrList<PROPLIST_ITEM>CLDSVector::propHashtable;	//�����ַ�����ϣ��
CXhChar100 CLDSVector::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*>CLDSVector::hashPropItemById;
CHashTable<DWORD>CLDSVector::propStatusHashtable;	//����״̬��ϣ��
void CLDSVector::InitPropHashtable()						//��ʼ�����Թ�ϣ��
{
	int id = 1;
	propHashtable.SetHashTableGrowSize(HASHTABLESIZE);
	propStatusHashtable.CreateHashTable(50);

#ifdef AFX_TARG_ENU_ENGLISH
	CLDSVector::AddPropItem("datumVector",PROPLIST_ITEM(id++,""));
	CLDSVector::AddPropItem("description",PROPLIST_ITEM(id++,""));
	CLDSVector::AddPropItem("vector",PROPLIST_ITEM(id++,"Bending Normal "));
	CLDSVector::AddPropItem("vector.x",PROPLIST_ITEM(id++,"Bending Normal X Coordinate"));
	CLDSVector::AddPropItem("vector.y",PROPLIST_ITEM(id++,"Bending Normal Y Coordinate"));
	CLDSVector::AddPropItem("vector.z",PROPLIST_ITEM(id++,"Bending Normal Z Coordinate"));
	CLDSVector::AddPropItem("norm_style",PROPLIST_ITEM(id++,"Normal Locate Mode"));
	CLDSVector::AddPropItem("hVicePart",PROPLIST_ITEM(id++,"Datum Rod"));
	CLDSVector::AddPropItem("hCrossPart",PROPLIST_ITEM(id++,"Cross-bracing Reference Rod"));
	CLDSVector::AddPropItem("norm_wing",PROPLIST_ITEM(id++,"Datum Leg"));
	CLDSVector::AddPropItem("direction",PROPLIST_ITEM(id++,"Direction"));
	CLDSVector::AddPropItem("rot_ang",PROPLIST_ITEM(id++,"Rotation Angle"));
	CLDSVector::AddPropItem("VectorType",PROPLIST_ITEM(id++,"Specify Direction"));
	CLDSVector::AddPropItem("nearVectorType",PROPLIST_ITEM(id++,"Near Direction"));
	CLDSVector::AddPropItem("nearVector.x",PROPLIST_ITEM(id++,"X Component"));
	CLDSVector::AddPropItem("nearVector.y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSVector::AddPropItem("nearVector.z",PROPLIST_ITEM(id++,"Z Component"));
#else 
	CLDSVector::AddPropItem("datumVector",PROPLIST_ITEM(id++,""));
	CLDSVector::AddPropItem("description",PROPLIST_ITEM(id++,""));
	CLDSVector::AddPropItem("vector",PROPLIST_ITEM(id++,"��������"));
	CLDSVector::AddPropItem("vector.x",PROPLIST_ITEM(id++,"��������X�᷽������"));
	CLDSVector::AddPropItem("vector.y",PROPLIST_ITEM(id++,"��������Y�᷽������"));
	CLDSVector::AddPropItem("vector.z",PROPLIST_ITEM(id++,"��������Z�᷽������"));
	CLDSVector::AddPropItem("norm_style",PROPLIST_ITEM(id++,"���߶�λ��ʽ"));
	CLDSVector::AddPropItem("hVicePart",PROPLIST_ITEM(id++,"��׼�˼�"));
	CLDSVector::AddPropItem("hCrossPart",PROPLIST_ITEM(id++,"������ո˼�"));
	CLDSVector::AddPropItem("norm_wing",PROPLIST_ITEM(id++,"��׼֫"));
	CLDSVector::AddPropItem("direction",PROPLIST_ITEM(id++,"����"));
	CLDSVector::AddPropItem("rot_ang",PROPLIST_ITEM(id++,"��ת�Ƕ�"));
	CLDSVector::AddPropItem("VectorType",PROPLIST_ITEM(id++,"ָ������"));
	CLDSVector::AddPropItem("nearVectorType",PROPLIST_ITEM(id++,"���Ʒ���"));
	CLDSVector::AddPropItem("nearVector.x",PROPLIST_ITEM(id++,"X����"));
	CLDSVector::AddPropItem("nearVector.y",PROPLIST_ITEM(id++,"Y����"));
	CLDSVector::AddPropItem("nearVector.z",PROPLIST_ITEM(id++,"Z����"));
#endif
}
int CLDSVector::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)	//��������ID�õ�����ֵ
{
	char sText[100]="";
	if (GetPropID("vector")==id)
		strcpy(sText,"");
	else if(GetPropID("vector.x")==id)
	{
		sprintf(sText,"%f",vector.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("vector.y")==id)
	{
		sprintf(sText,"%f",vector.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("vector.z")==id)
	{
		sprintf(sText,"%f",vector.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("norm_style")==id)
		strcpy(sText,GetNormStyleDes());	//��������
	else if(GetPropID("hVicePart")==id)
		sprintf(sText,"0X%X",hVicePart);
	else if(GetPropID("hCrossPart")==id)
		sprintf(sText,"0X%X",hCrossPart);
	else if(GetPropID("norm_wing")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(norm_wing==0)
			strcpy(sText,"0.X Leg Normal");
		else
			strcpy(sText,"1.Y Leg Normal");
#else 
		if(norm_wing==0)
			strcpy(sText,"0.X֫����");
		else
			strcpy(sText,"1.Y֫����");
#endif
	}
	else if(GetPropID("direction")==id)
		strcpy(sText,GetDirectionDes());
	else if(GetPropID("rot_ang")==id)
	{
		sprintf(sText,"%f",rot_ang);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("VectorType")==id)
		sprintf(sText,"%s",CStdVector::GetVecTypeString(vector));
	else if(GetPropID("nearVectorType")==id)
		sprintf(sText,"%s",CStdVector::GetVecTypeString(nearVector));
	else if(GetPropID("nearVector.x")==id)
	{
		sprintf(sText,"%lf",nearVector.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("nearVector.y")==id)
	{
		sprintf(sText,"%lf",nearVector.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("nearVector.z")==id)
	{
		sprintf(sText,"%lf",nearVector.z);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
int CLDSVector::GetPropStatus(long id)					//�õ�����״̬
{
	DWORD dwStatus;
	if(CLDSPoint::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}
BOOL CLDSVector::SetPropStatus(long id,BOOL bHide)		//��������״̬
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSPoint::propStatusHashtable.SetValueAt(id,dwStatus);
}
long CLDSVector::GetPropID(char* propStr)			//�õ�����ID
{
	PROPLIST_ITEM *pItem=CLDSVector::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
		return 0;
}

BOOL CLDSPoint::AssertValid(IModel *pTower,char *error_info/*=NULL*/)
{
	if(datum_pos_style==1)		
	{	//�Ǹ���㶨λ
		if(pTower->FromPartHandle(des_para.RODEND.hRod,CLS_LINEPART)==NULL)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified rod is illegal!");
#else 
				strcpy(error_info,"ָ���˼�����Ƿ�!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==2)
	{	//�˼��ڵ㶨λ
		if(pTower->FromPartHandle(des_para.RODNODE.hRod,CLS_LINEPART)==NULL)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified rod is illegal!");
#else 
				strcpy(error_info,"ָ���˼�����Ƿ�!");
#endif
			return FALSE;
		}
		if(pTower->FromNodeHandle(des_para.RODNODE.hNode)==NULL)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified datum node is illegal!");
#else 
				strcpy(error_info,"ָ����׼�ڵ����Ƿ�!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==3)
	{	//���߽���
		if(!pTower->FromPartHandle(des_para.AXIS_INTERS.hDatum1,CLS_LINEPART) || 
		   !pTower->FromPartHandle(des_para.AXIS_INTERS.hDatum2,CLS_LINEPART))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified rod is illegal!");
#else 
				strcpy(error_info,"ָ���˼�����Ƿ�!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==4)	
	{	//��׼�߿ռ佻��
		if(!pTower->FromLineHandle(des_para.AXIS_INTERS.hDatum1) || 
		   !pTower->FromLineHandle(des_para.AXIS_INTERS.hDatum2))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified datum line is illegal!");
#else 
				strcpy(error_info,"ָ����׼�߾���Ƿ�!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==5)	
	{	//��׼�����������겻���
		if(!pTower->FromLineHandle(des_para.DATUMLINE.hDatumLine))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified datum line is illegal!");
#else 
				strcpy(error_info,"ָ����׼�߾���Ƿ�!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==6)	
	{	//��׼����ָ�������
		if(!pTower->FromPlaneHandle(des_para.DATUMPLANE.hDatumPlane))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified datum plane is illegal!");
#else 
				strcpy(error_info,"ָ����׼�����Ƿ�!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==7)	
	{	//�Ǹ�����������
		if(!pTower->FromPartHandle(des_para.RODEND.hRod,CLS_LINEANGLE))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified angle is illegal!");
#else 
				strcpy(error_info,"ָ���Ǹ־���Ƿ�!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==8)	
	{	//��ڵ�ͬλ
		if(!pTower->FromNodeHandle(des_para.hNode))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified node is illegal!");
#else 
				strcpy(error_info,"ָ���ڵ����Ƿ�!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==9)	
	{	//ָ�������ϵ��������
		if(!pTower->FromPartHandle(des_para.hPart))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified part is illegal!");
#else 
				strcpy(error_info,"ָ����������Ƿ�!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==10) 
	{	//���ڵ��ı����ȷֵ�
		if(!pTower->FromPartHandle(des_para.SCALE_NODE.hLinePart,CLS_LINEPART))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified rod is illegal!");
#else 
				strcpy(error_info,"ָ���˼�����Ƿ�!");
#endif
			return FALSE;
		}
		if(!pTower->FromNodeHandle(des_para.SCALE_NODE.hStartNode)
			||!pTower->FromNodeHandle(des_para.SCALE_NODE.hEndNode))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified node is illegal!");
#else 
				strcpy(error_info,"ָ���ڵ����Ƿ�!");
#endif
			return FALSE;
		}
	}
	return TRUE;
}

CLDSArcLift::CLDSArcLift()
{
	iNo=0;
	m_nClassTypeId=CLS_ARCLIFT;
	//strcpy(m_sClassName,"CLDSArcLift");
	strcpy(name,"");
	m_ciLiftType=0;				//Ĭ��Բ��Ԥ��
	m_cRotAxis='Y';				//Ԥ��ʱ��ת��'X':X��;'Y':Y��
	m_xRotOrg.Set(0,0,0);		//Ԥ����ת����
	m_xDatumLiftPos.Set(0,0,0);	//��׼Ԥ�����ʼλ��
	m_fLiftHeight=0;			//��׼Ԥ�����Ԥ��ֵ,mm
	m_pModel=NULL;
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN��������Ա���;
}
bool CLDSArcLift::IsInRightSide(const f3dPoint& pickPos)
{
	bool bInRightSide=true;
	if(m_cRotAxis=='X')
	{
		if(pickPos.y>0)
			bInRightSide=false;
	}
	else //if(m_cRotAxis=='Y')
	{
		if(pickPos.x<0)
			bInRightSide=false;
	}
	return bInRightSide;
}
f3dPoint CLDSArcLift::LiftPosTrans(const f3dPoint& initPos)
{
	double deita;
	bool bInRightSide=true;
	f3dLine axis(m_xRotOrg,m_xRotOrg);
	if(m_cRotAxis=='X')
	{
		axis.startPt.x-=100;
		deita=fabs(m_xDatumLiftPos.y-m_xRotOrg.y);
		if(initPos.y>0)
			bInRightSide=false;
		if(initPos.y*m_xRotOrg.y<0)
			axis.startPt.y=axis.endPt.y=-m_xRotOrg.y;
	}
	else //if(m_cRotAxis=='Y')
	{
		axis.startPt.y-=100;
		deita=fabs(m_xDatumLiftPos.x-m_xRotOrg.x);
		if(initPos.x<0)
			bInRightSide=false;
		if(initPos.x*m_xRotOrg.x<0)
			axis.startPt.x=axis.endPt.x=-m_xRotOrg.x;
	}
	if(fabs(deita)<EPS)
		return initPos;
	if(m_ciLiftType!=1)
	{	//Բ��Ԥ��
		double tana=m_fLiftHeight/deita;
		double rot_ang=atan(tana);
		if(!bInRightSide)
			rot_ang=-rot_ang;
		f3dPoint result=initPos;
		rotate_point_around_axis(result,rot_ang,axis);
		return result;
	}
	else
	{	//��ֱԤ��
		double curr_deita=0;
		if(m_cRotAxis=='X')
		{
			if(initPos.y*m_xRotOrg.y>0)
				curr_deita=initPos.y-m_xRotOrg.y;
			else
				curr_deita=initPos.y+m_xRotOrg.y;
			deita=fabs(m_xDatumLiftPos.y-m_xRotOrg.y);
			if(initPos.y>0)
				bInRightSide=false;
			else
				curr_deita*=-1;
			double liftcoord_z=m_fLiftHeight*curr_deita/deita;
			return f3dPoint(initPos.x,initPos.y,initPos.z-liftcoord_z);
		}
		else //if(m_cRotAxis=='Y')
		{
			if(initPos.x*m_xRotOrg.x>0)
				curr_deita=initPos.x-m_xRotOrg.x;
			else
				curr_deita=initPos.x+m_xRotOrg.x;
			deita=fabs(m_xDatumLiftPos.x-m_xRotOrg.x);
			if(initPos.x<0)
			{
				curr_deita*=-1;
				bInRightSide=false;
			}
			double liftcoord_z=m_fLiftHeight*curr_deita/deita;
			return f3dPoint(initPos.x,initPos.y,initPos.z-liftcoord_z);
		}
	}
}
//��������Ԥ����ԭ���㣨��Ҫ���ڸ���Ԥ����ָ���Ǹ�Ԥ��ǰ������꣩wjh-2015.5.19
f3dPoint CLDSArcLift::ReverseLiftPosTrans(const f3dPoint& arcPos)
{
	double deita;
	bool bInRightSide=true;
	f3dLine axis(m_xRotOrg,m_xRotOrg);
	if(m_cRotAxis=='X')
	{
		axis.startPt.x-=100;
		deita=fabs(m_xDatumLiftPos.y-m_xRotOrg.y);
		if(arcPos.y>0)
			bInRightSide=false;
		if(arcPos.y*m_xRotOrg.y<0)
			axis.startPt.y=axis.endPt.y=-m_xRotOrg.y;
	}
	else //if(m_cRotAxis=='Y')
	{
		axis.startPt.y-=100;
		deita=fabs(m_xDatumLiftPos.x-m_xRotOrg.x);
		if(arcPos.x<0)
			bInRightSide=false;
		if(arcPos.x*m_xRotOrg.x<0)
			axis.startPt.x=axis.endPt.x=-m_xRotOrg.x;
	}
	if(fabs(deita)<EPS)
		return arcPos;
	if(m_ciLiftType!=1)
	{	//Բ��Ԥ��
		double tana=m_fLiftHeight/deita;
		double rot_ang=atan(tana);
		if(!bInRightSide)
			rot_ang=-rot_ang;
		f3dPoint result=arcPos;
		rotate_point_around_axis(result,-rot_ang,axis);
		return result;
	}
	else
	{	//��ֱԤ��
		double curr_deita=0;
		if(m_cRotAxis=='X')
		{
			if(arcPos.y*m_xRotOrg.y>0)
				curr_deita=arcPos.y-m_xRotOrg.y;
			else
				curr_deita=arcPos.y+m_xRotOrg.y;
			deita=fabs(m_xDatumLiftPos.y-m_xRotOrg.y);
			if(arcPos.y>0)
				bInRightSide=false;
			else
				curr_deita*=-1;
			double liftcoord_z=m_fLiftHeight*curr_deita/deita;
			return f3dPoint(arcPos.x,arcPos.y,arcPos.z+liftcoord_z);
		}
		else //if(m_cRotAxis=='Y')
		{
			if(arcPos.x*m_xRotOrg.x>0)
				curr_deita=arcPos.x-m_xRotOrg.x;
			else
				curr_deita=arcPos.x+m_xRotOrg.x;
			deita=fabs(m_xDatumLiftPos.x-m_xRotOrg.x);
			if(arcPos.x<0)
			{
				curr_deita*=-1;
				bInRightSide=false;
			}
			double liftcoord_z=m_fLiftHeight*curr_deita/deita;
			return f3dPoint(arcPos.x,arcPos.y,arcPos.z+liftcoord_z);
		}
	}
}
f3dPoint CLDSArcLift::LiftVecTrans(const f3dPoint& initVec,const f3dPoint& pickPos)
{
	bool bInRightSide=IsInRightSide(pickPos);
	return LiftVecTrans(initVec,bInRightSide);
}
f3dPoint CLDSArcLift::LiftVecTrans(const f3dPoint& initVec,bool bInRightSide/*=true*/)
{
	double deita;
	f3dPoint axis_vec;
	if(m_cRotAxis=='X')
	{
		axis_vec.Set(1,0,0);
		deita=fabs(m_xDatumLiftPos.y-m_xRotOrg.y);
	}
	else //if(m_cRotAxis=='Y')
	{
		axis_vec.Set(0,1,0);
		deita=fabs(m_xDatumLiftPos.x-m_xRotOrg.x);
	}
	if(fabs(deita)<EPS)
		return initVec;
	double tana=m_fLiftHeight/deita;
	double rot_ang=atan(tana);
	if(!bInRightSide)
		rot_ang=-rot_ang;
	f3dPoint destVec=initVec;
	RotateVectorAroundVector(destVec,rot_ang,axis_vec);
	return destVec;
}
f3dPoint CLDSArcLift::ReverseLiftVecTrans(const f3dPoint& initVec,const f3dPoint& pickPos)
{
	bool bInRightSide=IsInRightSide(pickPos);
	return ReverseLiftVecTrans(initVec,bInRightSide);
}
f3dPoint CLDSArcLift::ReverseLiftVecTrans(const f3dPoint& initVec,bool bInRightSide/*=true*/)
{
	double deita;
	f3dPoint axis_vec;
	if(m_cRotAxis=='X')
	{
		axis_vec.Set(1,0,0);
		deita=fabs(m_xDatumLiftPos.y-m_xRotOrg.y);
	}
	else //if(m_cRotAxis=='Y')
	{
		axis_vec.Set(0,1,0);
		deita=fabs(m_xDatumLiftPos.x-m_xRotOrg.x);
	}
	if(fabs(deita)<EPS)
		return initVec;
	double tana=m_fLiftHeight/deita;
	double rot_ang=atan(tana);
	if(bInRightSide)
		rot_ang=-rot_ang;
	//TODO:δ����ֱԤ����ʸ�������Ӱ�� wjh-2018.5.15
	f3dPoint destVec=initVec;
	RotateVectorAroundVector(destVec,rot_ang,axis_vec);
	return destVec;
}
void CLDSArcLift::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( version==0||
		(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030401))	//TDA(V1,3,4,1)	���ֿ������õ�V1.3.4.0����ʱ����δ����dwPermission���� wjh-2017.5.18
		buffer.ReadDword(&dwPermission);
	buffer.ReadInteger(&iNo);			//Ԥ�����
	buffer.ReadString(name,50);
	if( version==0||
		(doc_type==1&&version>=5010700)||	//TMA(V5.1.7.0)
		(doc_type==2&&version>=2000800)||	//LMA(V2.0.8.0)
		(doc_type==4&&version>=1030701)||	//LDS(V1.3.7.1)
		(doc_type==5&&version>=1030701))	//TDA(V1.3.7.1)
		buffer.ReadByte(&m_ciLiftType);
	buffer.ReadByte(&m_cRotAxis);		//Ԥ��ʱ��ת��'X':X��;'Y':Y��
	buffer.ReadPoint(m_xRotOrg);		//Ԥ����ת����
	buffer.ReadPoint(m_xDatumLiftPos);	//��׼Ԥ�����ʼλ��
	buffer.ReadDouble(&m_fLiftHeight);	//��׼Ԥ�����Ԥ��ֵ,mm
}
void CLDSArcLift::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if( version==0||
		(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030401))	//TDA(V1,3,4,1)
		buffer.WriteDword(dwPermission);
	buffer.WriteInteger(iNo);			//Ԥ�����
	buffer.WriteString(name);
	if( version==0||
		(doc_type==1&&version>=5010700)||	//TMA(V5.1.7.0)
		(doc_type==2&&version>=2000800)||	//LMA(V2.0.8.0)
		(doc_type==4&&version>=1030701)||	//LDS(V1.3.7.1)
		(doc_type==5&&version>=1030701))	//TDA(V1.3.7.1)
		buffer.WriteByte(m_ciLiftType);
	buffer.WriteByte(m_cRotAxis);		//Ԥ��ʱ��ת��'X':X��;'Y':Y��
	buffer.WritePoint(m_xRotOrg);		//Ԥ����ת����
	buffer.WritePoint(m_xDatumLiftPos);	//��׼Ԥ�����ʼλ��
	buffer.WriteDouble(m_fLiftHeight);	//��׼Ԥ�����Ԥ��ֵ,mm
}
void CLDSArcLift::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[500]="";
	char sPosX[50]="",sPosY[50]="",sPosZ[50]="";
	fprintf(fp,"<Բ��Ԥ����Ϣ>\n");
	fprintf(fp,"<Ԥ����� iNo=\"%d\"/>\n",iNo);
	fprintf(fp,"<Ԥ������ name=\"%s\"/>\n",name);
	fprintf(fp,"<��ת�� RotAxis=\"%c\"/>\n",m_cRotAxis);
	sprintf(sPosX,"%f",m_xDatumLiftPos.x);
	SimplifiedNumString(sPosX);
	sprintf(sPosY,"%f",m_xDatumLiftPos.y);
	SimplifiedNumString(sPosY);
	sprintf(sPosZ,"%f",m_xDatumLiftPos.z);
	SimplifiedNumString(sPosZ);
	fprintf(fp,"<Ԥ�����ʼλ�� DatumListPos=\"(%s,%s,%s)\"/>\n",sPosX,sPosY,sPosZ);
	sprintf(sText,"%f",m_fLiftHeight);
	SimplifiedNumString(sText);
	fprintf(fp,"<Ԥ�����Ԥ��ֵ LiftHeight=\"%s\"/>\n",sText);
	fprintf(fp,"</Բ��Ԥ����Ϣ>\n");
}
void POINT_PARA::ZeroClear()
{
	memset(this,0,sizeof(POINT_PARA));
}
void POINT_PARA::ToBuffer(BUFFER_IO* pIO)
{
	bool bHasArcLiftBeforePos=false;
	//if(	version==0||(doc_type==1&&version>=4020001)||((doc_type==2||doc_type==4||doc_type==5)&&version>=1020001))
		bHasArcLiftBeforePos=true;	//�Ѿ߱��Զ�Ԥ������

	int version=0,doc_type=1;
	pIO->WriteByte(ciLocationType);
	if(ciLocationType==0)//&&bHasArcLiftBeforePos)
		pIO->WritePoint(xDatumPos);
	if(ciLocationType==1||ciLocationType==7)		//�Ǹ���㶨λ
	{
		pIO->WriteWord(RODEND.direction);
		pIO->WriteInteger(RODEND.bIncOddEffect);
		pIO->WriteInteger(RODEND.hRod);
		pIO->WriteWord(RODEND.offset_wing);
		pIO->WriteDouble(RODEND.len_offset_dist);
		pIO->WriteWord(RODEND.wing_offset_style);
		pIO->WriteDouble(RODEND.wing_offset_dist);
		pIO->WriteString(RODEND.norm_offset);
	}
	else if(ciLocationType==2)	//�Ǹֽڵ㶨λ
	{
		pIO->WriteWord(RODNODE.direction);
		pIO->WriteInteger(RODNODE.hRod);
		pIO->WriteInteger(RODNODE.hNode);
		pIO->WriteWord(RODNODE.bFaceOffset);
		pIO->WriteWord(RODNODE.offset_wing);
		pIO->WriteDouble(RODNODE.len_offset_dist);
		pIO->WriteWord(RODNODE.wing_offset_style);
		pIO->WriteDouble(RODNODE.wing_offset_dist);
		pIO->WritePoint(RODNODE.vPlaneNormal);
		pIO->WriteString(RODNODE.norm_offset);
	}
	else if(ciLocationType==3||ciLocationType==4)	//��׼�߿ռ佻��
	{
		pIO->WriteInteger(AXIS_INTERS.hDatum1);
		pIO->WriteInteger(AXIS_INTERS.hDatum2);
		pIO->WriteInteger(AXIS_INTERS.wing_offset_dist1);
		pIO->WriteInteger(AXIS_INTERS.wing_offset_dist2);
		pIO->WriteWord(AXIS_INTERS.wing_offset_style1);
		pIO->WriteWord(AXIS_INTERS.wing_offset_style2);
		if(	version==0||
			(doc_type==1&&version>=4020009)||
			(doc_type==2&&version>=1020009)||
			(doc_type==3&&version>=2000009)||
			(doc_type==4&&version>=1020009)||
			(doc_type==5&&version>=1020009))
		pIO->WriteByte(AXIS_INTERS.cProjectPlane);
		pIO->WriteDouble(AXIS_INTERS.near_vector[0]);
		pIO->WriteDouble(AXIS_INTERS.near_vector[1]);
		pIO->WriteDouble(AXIS_INTERS.near_vector[2]);
		pIO->WriteString(AXIS_INTERS.norm_offset);
	}
	else if(ciLocationType==5)	//��׼�����������겻���
	{
		pIO->WriteInteger(DATUMLINE.hDatumLine);
		if( version==0||
			(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
			(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
			(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
			(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
			pIO->WriteByte(DATUMLINE.cMirTransType);
		pIO->WriteWord(DATUMLINE.fix_axis_val_style_x0_y1_z2);
		if(bHasArcLiftBeforePos)
			pIO->WritePoint(xDatumPos);
	}
	else if(ciLocationType==6)	//��׼����ָ�������
	{
		pIO->WriteInteger(DATUMPLANE.hDatumPlane);
		pIO->WriteByte(DATUMPLANE.cMirTransType);
		pIO->WriteWord(DATUMPLANE.fix_axis_style_x0_y1_z2);
		if(bHasArcLiftBeforePos)
			pIO->WritePoint(xDatumPos);
	}
	else if(ciLocationType==8)	//��ڵ�ͬλ
		pIO->WriteInteger(hNode);
	else if(ciLocationType==9)	//ָ�������ϵ��������
	{
		pIO->WriteInteger(hPart);
		pIO->WritePoint(xDatumPos);
	}
	else if(ciLocationType==10)//���ڵ��ı����ȷֵ� wht 11-01-24
	{
		pIO->WriteInteger(SCALE_NODE.hLinePart);
		pIO->WriteInteger(SCALE_NODE.hStartNode);
		pIO->WriteInteger(SCALE_NODE.hEndNode);
		pIO->WriteDouble(SCALE_NODE.start_offset_dist);
		pIO->WriteDouble(SCALE_NODE.end_offset_dist);
		pIO->WriteDouble(SCALE_NODE.offset_scale);
		pIO->WriteDouble(SCALE_NODE.offset_dist);
	}
	else if(ciLocationType==11)
	{
		pIO->WriteInteger(COLUMN_INTERS.hLineTube);		//>0ʱ�Ըøֹܶ��������axis_vec,axis_pick,column_r��
		pIO->WriteByte(COLUMN_INTERS.cPointAndVectorFlag);
		pIO->WritePoint(COLUMN_INTERS.AxisVector.axis_vec);		//���������������
		pIO->WritePoint(COLUMN_INTERS.SectCenter.sect_center);		//�������������Ľ��㣬��Բ���棨Բ����Բ����Բ�ĵ�
		if(  version==0||
			(doc_type==1&&version>=5000300)||	//TMA V5.0.3.0
			(doc_type==2&&version>=2000300)||	//LMA V2.0.3.0
			(doc_type==3&&version>=2010300)||	//TSA V2.1.3.0
			(doc_type==4&&version>=1030300)||	//LDS V1.3.3.0
			(doc_type==5&&version>=1030300))	//TDA V1.3.3.0
		{
			pIO->WriteInteger(COLUMN_INTERS.SectCenterParam.hNode);		//����Բ�ĵ�ƫ�ƻ�׼�ڵ�
			pIO->WriteDouble(COLUMN_INTERS.SectCenterParam.len_offset);	//����Բ���Ի�׼�ڵ��ƫ����
		}
		pIO->WritePoint(COLUMN_INTERS.SectNorm.sect_norm);		//���淨��Ϊ��ʱ��ʾΪ����Ĵ�ֱ����
		pIO->WriteByte(COLUMN_INTERS.ray_vec_style);	//0.ray_vecΪֱ�Ӷ�������߾��Է���;>0:ָ��������ʼ�ߣ�������ray_angleȷ�����߷���
		pIO->WritePoint(COLUMN_INTERS.RayVector.ray_vec);			//���߷���
		pIO->WriteDouble(COLUMN_INTERS.ray_angle);		//ray_vec_style==1ʱ���߷�����������ʼ�ߵ���ʱ����ת�Ƕ�,��
		pIO->WriteDouble(COLUMN_INTERS.column_r);		//����뾶
		pIO->WriteDouble(COLUMN_INTERS.ray_para_offset);//����ƽ���ƶ����루��ֱ���߷������ƽ�����ƶ�,����������������ƶ�Ϊ������һ��Ϊ����
		pIO->WriteBooleanThin(COLUMN_INTERS.bSpecRo);	//true:����뾶�Ծ�����ͶӰ���Ծ��붨��false:����뾶���������߽������������ƫ��ȷ��
		pIO->WriteDouble(COLUMN_INTERS.ray_offset_r);	//bSpecRoByOffset=true:����뾶; bSpecRoByOffset=false:���߷�������ƫ����
	}
}
void POINT_PARA::FromBuffer(BUFFER_IO* pIO)
{
	pIO->ReadByte(&ciLocationType);
	if(ciLocationType==0)
		pIO->ReadPoint(this->xDatumPos);	//��ȡֱ��ָ������ʱԤ��ǰλ��
	if(ciLocationType==1||ciLocationType==7)		//�Ǹ���㶨λ
	{
		pIO->ReadWord(&RODEND.direction);
		pIO->ReadInteger(&RODEND.bIncOddEffect);
		pIO->ReadInteger(&RODEND.hRod);
		pIO->ReadWord(&RODEND.offset_wing);
		pIO->ReadDouble(&RODEND.len_offset_dist);
		pIO->ReadWord(&RODEND.wing_offset_style);
		pIO->ReadDouble(&RODEND.wing_offset_dist);
		pIO->ReadString(RODEND.norm_offset);

	}
	else if(ciLocationType==2)	//�Ǹֽڵ㶨λ
	{
		pIO->ReadWord(&RODNODE.direction);
		pIO->ReadInteger(&RODNODE.hRod);
		pIO->ReadInteger(&RODNODE.hNode);
		pIO->ReadWord(&RODNODE.bFaceOffset);
		pIO->ReadWord(&RODNODE.offset_wing);
		pIO->ReadDouble(&RODNODE.len_offset_dist);
		pIO->ReadWord(&RODNODE.wing_offset_style);
		pIO->ReadDouble(&RODNODE.wing_offset_dist);
		pIO->ReadPoint(RODNODE.vPlaneNormal);//sizeof(double)*3);
		pIO->ReadString(RODNODE.norm_offset);
	}
	else if(ciLocationType==3||ciLocationType==4)	//��׼�߿ռ佻��
	{
		pIO->ReadInteger(&AXIS_INTERS.hDatum1);
		pIO->ReadInteger(&AXIS_INTERS.hDatum2);
		pIO->ReadInteger(&AXIS_INTERS.wing_offset_dist1);
		pIO->ReadInteger(&AXIS_INTERS.wing_offset_dist2);
		pIO->ReadWord(&AXIS_INTERS.wing_offset_style1);
		pIO->ReadWord(&AXIS_INTERS.wing_offset_style2);
		pIO->ReadByte(&AXIS_INTERS.cProjectPlane);
		pIO->ReadDouble(&AXIS_INTERS.near_vector[0]);
		pIO->ReadDouble(&AXIS_INTERS.near_vector[1]);
		pIO->ReadDouble(&AXIS_INTERS.near_vector[2]);
		pIO->ReadString(AXIS_INTERS.norm_offset);
	}
	else if(ciLocationType==5)	//��׼�����������겻���
	{
		pIO->ReadInteger(&DATUMLINE.hDatumLine);
		pIO->ReadByte(&DATUMLINE.cMirTransType);
		pIO->ReadWord(&DATUMLINE.fix_axis_val_style_x0_y1_z2);
		pIO->ReadPoint(xDatumPos);	//��ȡֱ��ָ������ʱԤ��ǰλ��
	}
	else if(ciLocationType==6)	//��׼����ָ�������
	{
		pIO->ReadInteger(&DATUMPLANE.hDatumPlane);
		pIO->ReadByte(&DATUMPLANE.cMirTransType);
		pIO->ReadWord(&DATUMPLANE.fix_axis_style_x0_y1_z2);
		pIO->ReadPoint(xDatumPos);	//��ȡֱ��ָ������ʱԤ��ǰλ��
	}
	else if(ciLocationType==8)	//��ڵ�ͬλ
		pIO->ReadInteger(&hNode);
	else if(ciLocationType==9)	//ָ�������ϵ��������
	{
		pIO->ReadInteger(&hPart);
		pIO->ReadPoint(xDatumPos);
	}
	else if(ciLocationType==10)
	{	//���ڵ��ı����ȷֵ� wht 11-01-24
		pIO->ReadInteger(&SCALE_NODE.hLinePart);
		pIO->ReadInteger(&SCALE_NODE.hStartNode);
		pIO->ReadInteger(&SCALE_NODE.hEndNode);
		pIO->ReadDouble(&SCALE_NODE.start_offset_dist);
		pIO->ReadDouble(&SCALE_NODE.end_offset_dist);
		pIO->ReadDouble(&SCALE_NODE.offset_scale);
		pIO->ReadDouble(&SCALE_NODE.offset_dist);
	}
	else if(ciLocationType==11)
	{
		//if(version==0||(doc_type==5&&version>=1020100))
		pIO->ReadInteger(&COLUMN_INTERS.hLineTube);		//>0ʱ�Ըøֹܶ��������axis_vec,axis_pick,column_r��
		pIO->ReadByte(&COLUMN_INTERS.cPointAndVectorFlag);
		pIO->ReadPoint(COLUMN_INTERS.AxisVector.axis_vec);		//���������������
		pIO->ReadPoint(COLUMN_INTERS.SectCenter.sect_center);		//�������������Ľ��㣬��Բ���棨Բ����Բ����Բ�ĵ�
		pIO->ReadInteger(&COLUMN_INTERS.SectCenterParam.hNode);		//����Բ�ĵ�ƫ�ƻ�׼�ڵ�
		pIO->ReadDouble(&COLUMN_INTERS.SectCenterParam.len_offset);	//����Բ���Ի�׼�ڵ��ƫ����

		pIO->ReadPoint(COLUMN_INTERS.SectNorm.sect_norm);		//���淨��Ϊ��ʱ��ʾΪ����Ĵ�ֱ����
		pIO->ReadByte(&COLUMN_INTERS.ray_vec_style);	//0.ray_vecΪֱ�Ӷ�������߾��Է���;>0:ָ��������ʼ�ߣ�������ray_angleȷ�����߷���
		pIO->ReadPoint(COLUMN_INTERS.RayVector.ray_vec);			//���߷���
		pIO->ReadDouble(&COLUMN_INTERS.ray_angle);		//ray_vec_style==1ʱ���߷�����������ʼ�ߵ���ʱ����ת�Ƕ�,��
		pIO->ReadDouble(&COLUMN_INTERS.column_r);		//����뾶
		pIO->ReadDouble(&COLUMN_INTERS.ray_para_offset);//����ƽ���ƶ����루��ֱ���߷������ƽ�����ƶ�,����������������ƶ�Ϊ������һ��Ϊ����
		pIO->ReadBooleanThin(&COLUMN_INTERS.bSpecRo);	//true:����뾶�Ծ�����ͶӰ���Ծ��붨��false:����뾶���������߽������������ƫ��ȷ��
		pIO->ReadDouble(&COLUMN_INTERS.ray_offset_r);	//bSpecRoByOffset=true:����뾶; bSpecRoByOffset=false:���߷�������ƫ����
	}
}
IModel* CLDSPoint::m_pTower=NULL;
CLDSPoint::CLDSPoint()
{
	hAgentBlockRef=0;
	iNo=datum_pos_style=0;
	m_nClassTypeId=CLS_GEPOINT;
	//strcpy(m_sClassName,"CLDSPoint");
	strcpy(description,"");
	int n=sizeof(des_para);
	memset(&des_para,0,sizeof(des_para));
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN��������Ա���;
}
//�����趨�ɽǸ����(�����������ʽ)�Ļ�׼�����,����offsetWing=0:X֫ƫ��;1:Y֫ƫ��;����Ϊ�������˫֫����ƫ��
void CLDSPoint::SetRapidAngleBer(THANDLE hJg,BYTE direction,BYTE wing_x0_y1_both2/*=0*/,double offsetDist/*=0.0*/,BYTE gStyle/*=4*/)
{
	if(wing_x0_y1_both2==0||wing_x0_y1_both2==1)
	{	//��Ϊ�Ǹֶ����
		datum_pos_style=1;
		des_para.RODEND.hRod=hJg;
		des_para.RODEND.direction=direction;
		des_para.RODEND.offset_wing=wing_x0_y1_both2;
		des_para.RODEND.wing_offset_style=gStyle;
		des_para.RODEND.wing_offset_dist=offsetDist;
	}
	else
	{	//��Ϊ�������
		datum_pos_style=7;
		des_para.RODEND.hRod=hJg;
		des_para.RODEND.direction=direction;
		CDesignThickPara offset(offsetDist);
		strncpy(des_para.RODEND.norm_offset,offset.key_str,50);
	}
}
void CLDSPoint::ToParaBuffer(BUFFER_IO* pIO)	//���Ѳ�������λ�еļ��β�����Ϣ��ȡΪPOINT_PARA��ʽд�뻺��
{
	pIO->WriteByte(datum_pos_style);
	if(datum_pos_style==0)
		pIO->WritePoint(datum_pos);
	if(datum_pos_style==1||datum_pos_style==7)		//�Ǹ���㶨λ
	{
		pIO->WriteWord(des_para.RODEND.direction);
		pIO->WriteInteger(des_para.RODEND.bIncOddEffect);
		pIO->WriteInteger(des_para.RODEND.hRod);
		pIO->WriteWord(des_para.RODEND.offset_wing);
		pIO->WriteDouble(des_para.RODEND.len_offset_dist);
		pIO->WriteWord(des_para.RODEND.wing_offset_style);
		pIO->WriteDouble(des_para.RODEND.wing_offset_dist);
		pIO->WriteString(des_para.RODEND.norm_offset);
	}
	else if(datum_pos_style==2)	//�Ǹֽڵ㶨λ
	{
		pIO->WriteWord(des_para.RODNODE.direction);
		pIO->WriteInteger(des_para.RODNODE.hRod);
		pIO->WriteInteger(des_para.RODNODE.hNode);
		pIO->WriteWord(des_para.RODNODE.bFaceOffset);
		pIO->WriteWord(des_para.RODNODE.offset_wing);
		pIO->WriteDouble(des_para.RODNODE.len_offset_dist);
		pIO->WriteWord(des_para.RODNODE.wing_offset_style);
		pIO->WriteDouble(des_para.RODNODE.wing_offset_dist);
		pIO->WritePoint(des_para.RODNODE.vPlaneNormal);
		pIO->WriteString(des_para.RODNODE.norm_offset);
	}
	else if(datum_pos_style==3||datum_pos_style==4)	//��׼�߿ռ佻��
	{
		pIO->WriteInteger(des_para.AXIS_INTERS.hDatum1);
		pIO->WriteInteger(des_para.AXIS_INTERS.hDatum2);
		pIO->WriteInteger(des_para.AXIS_INTERS.wing_offset_dist1);
		pIO->WriteInteger(des_para.AXIS_INTERS.wing_offset_dist2);
		pIO->WriteWord(des_para.AXIS_INTERS.wing_offset_style1);
		pIO->WriteWord(des_para.AXIS_INTERS.wing_offset_style2);
		pIO->WriteByte(des_para.AXIS_INTERS.cProjectPlane);
		pIO->WriteDouble(des_para.AXIS_INTERS.near_vector[0]);
		pIO->WriteDouble(des_para.AXIS_INTERS.near_vector[1]);
		pIO->WriteDouble(des_para.AXIS_INTERS.near_vector[2]);
		pIO->WriteString(des_para.AXIS_INTERS.norm_offset);
	}
	else if(datum_pos_style==5)	//��׼�����������겻���
	{
		pIO->WriteInteger(des_para.DATUMLINE.hDatumLine);
		pIO->WriteByte(des_para.DATUMLINE.cMirTransType);
		pIO->WriteWord(des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2);
		pIO->WritePoint(datum_pos);
	}
	else if(datum_pos_style==6)	//��׼����ָ�������
	{
		pIO->WriteInteger(des_para.DATUMPLANE.hDatumPlane);
		pIO->WriteByte(des_para.DATUMPLANE.cMirTransType);
		pIO->WriteWord(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2);
		pIO->WritePoint(datum_pos);
	}
	else if(datum_pos_style==8)	//��ڵ�ͬλ
		pIO->WriteInteger(des_para.hNode);
	else if(datum_pos_style==9)	//ָ�������ϵ��������
	{
		pIO->WriteInteger(des_para.hPart);
		pIO->WritePoint(datum_pos);
	}
	else if(datum_pos_style==10)//���ڵ��ı����ȷֵ� wht 11-01-24
	{
		pIO->WriteInteger(des_para.SCALE_NODE.hLinePart);
		pIO->WriteInteger(des_para.SCALE_NODE.hStartNode);
		pIO->WriteInteger(des_para.SCALE_NODE.hEndNode);
		pIO->WriteDouble(des_para.SCALE_NODE.start_offset_dist);
		pIO->WriteDouble(des_para.SCALE_NODE.end_offset_dist);
		pIO->WriteDouble(des_para.SCALE_NODE.offset_scale);
		pIO->WriteDouble(des_para.SCALE_NODE.offset_dist);
	}
	else if(datum_pos_style==11)
	{
		pIO->WriteInteger(des_para.COLUMN_INTERS.hLineTube);		//>0ʱ�Ըøֹܶ��������axis_vec,axis_pick,column_r��
		pIO->WriteByte(des_para.COLUMN_INTERS.cPointAndVectorFlag);
		pIO->WritePoint(des_para.COLUMN_INTERS.AxisVector.axis_vec);		//���������������
		pIO->WritePoint(des_para.COLUMN_INTERS.SectCenter.sect_center);		//�������������Ľ��㣬��Բ���棨Բ����Բ����Բ�ĵ�
		pIO->WriteInteger(des_para.COLUMN_INTERS.SectCenterParam.hNode);		//����Բ�ĵ�ƫ�ƻ�׼�ڵ�
		pIO->WriteDouble(des_para.COLUMN_INTERS.SectCenterParam.len_offset);	//����Բ���Ի�׼�ڵ��ƫ����

		pIO->WritePoint(des_para.COLUMN_INTERS.SectNorm.sect_norm);		//���淨��Ϊ��ʱ��ʾΪ����Ĵ�ֱ����
		pIO->WriteByte(des_para.COLUMN_INTERS.ray_vec_style);	//0.ray_vecΪֱ�Ӷ�������߾��Է���;>0:ָ��������ʼ�ߣ�������ray_angleȷ�����߷���
		pIO->WritePoint(des_para.COLUMN_INTERS.RayVector.ray_vec);			//���߷���
		pIO->WriteDouble(des_para.COLUMN_INTERS.ray_angle);		//ray_vec_style==1ʱ���߷�����������ʼ�ߵ���ʱ����ת�Ƕ�,��
		pIO->WriteDouble(des_para.COLUMN_INTERS.column_r);		//����뾶
		pIO->WriteDouble(des_para.COLUMN_INTERS.ray_para_offset);//����ƽ���ƶ����루��ֱ���߷������ƽ�����ƶ�,����������������ƶ�Ϊ������һ��Ϊ����
		pIO->WriteBooleanThin(des_para.COLUMN_INTERS.bSpecRo);	//true:����뾶�Ծ�����ͶӰ���Ծ��붨��false:����뾶���������߽������������ƫ��ȷ��
		pIO->WriteDouble(des_para.COLUMN_INTERS.ray_offset_r);	//bSpecRoByOffset=true:����뾶; bSpecRoByOffset=false:���߷�������ƫ����
	}
}
void CLDSPoint::FromParaBuffer(BUFFER_IO* pIO)//��POINT_PARA���������ж����������λ�еļ��β�����Ϣ
{
	pIO->ReadByte(&datum_pos_style);
	if(datum_pos_style==0)
		pIO->ReadPoint(datum_pos);	//��ȡֱ��ָ������ʱԤ��ǰλ��
	if(datum_pos_style==1||datum_pos_style==7)		//�Ǹ���㶨λ
	{
		pIO->ReadWord(&des_para.RODEND.direction);
		pIO->ReadInteger(&des_para.RODEND.bIncOddEffect);
		pIO->ReadInteger(&des_para.RODEND.hRod);
		pIO->ReadWord(&des_para.RODEND.offset_wing);
		pIO->ReadDouble(&des_para.RODEND.len_offset_dist);
		pIO->ReadWord(&des_para.RODEND.wing_offset_style);
		pIO->ReadDouble(&des_para.RODEND.wing_offset_dist);
		pIO->ReadString(des_para.RODEND.norm_offset);

	}
	else if(datum_pos_style==2)	//�Ǹֽڵ㶨λ
	{
		pIO->ReadWord(&des_para.RODNODE.direction);
		pIO->ReadInteger(&des_para.RODNODE.hRod);
		pIO->ReadInteger(&des_para.RODNODE.hNode);
		pIO->ReadWord(&des_para.RODNODE.bFaceOffset);
		pIO->ReadWord(&des_para.RODNODE.offset_wing);
		pIO->ReadDouble(&des_para.RODNODE.len_offset_dist);
		pIO->ReadWord(&des_para.RODNODE.wing_offset_style);
		pIO->ReadDouble(&des_para.RODNODE.wing_offset_dist);
		pIO->ReadPoint(des_para.RODNODE.vPlaneNormal);//sizeof(double)*3);
		pIO->ReadString(des_para.RODNODE.norm_offset);
	}
	else if(datum_pos_style==3||datum_pos_style==4)	//��׼�߿ռ佻��
	{
		pIO->ReadInteger(&des_para.AXIS_INTERS.hDatum1);
		pIO->ReadInteger(&des_para.AXIS_INTERS.hDatum2);
		pIO->ReadInteger(&des_para.AXIS_INTERS.wing_offset_dist1);
		pIO->ReadInteger(&des_para.AXIS_INTERS.wing_offset_dist2);
		pIO->ReadWord(&des_para.AXIS_INTERS.wing_offset_style1);
		pIO->ReadWord(&des_para.AXIS_INTERS.wing_offset_style2);
		pIO->ReadByte(&des_para.AXIS_INTERS.cProjectPlane);
		pIO->ReadDouble(&des_para.AXIS_INTERS.near_vector[0]);
		pIO->ReadDouble(&des_para.AXIS_INTERS.near_vector[1]);
		pIO->ReadDouble(&des_para.AXIS_INTERS.near_vector[2]);
		pIO->ReadString(des_para.AXIS_INTERS.norm_offset);
	}
	else if(datum_pos_style==5)	//��׼�����������겻���
	{
		pIO->ReadInteger(&des_para.DATUMLINE.hDatumLine);
		pIO->ReadByte(&des_para.DATUMLINE.cMirTransType);
		pIO->ReadWord(&des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2);
		pIO->ReadPoint(datum_pos);	//��ȡֱ��ָ������ʱԤ��ǰλ��
	}
	else if(datum_pos_style==6)	//��׼����ָ�������
	{
		pIO->ReadInteger(&des_para.DATUMPLANE.hDatumPlane);
		pIO->ReadByte(&des_para.DATUMPLANE.cMirTransType);
		pIO->ReadWord(&des_para.DATUMPLANE.fix_axis_style_x0_y1_z2);
		pIO->ReadPoint(datum_pos);	//��ȡֱ��ָ������ʱԤ��ǰλ��
	}
	else if(datum_pos_style==8)	//��ڵ�ͬλ
		pIO->ReadInteger(&des_para.hNode);
	else if(datum_pos_style==9)	//ָ�������ϵ��������
	{
		pIO->ReadInteger(&des_para.hPart);
		pIO->ReadPoint(datum_pos);
	}
	else if(datum_pos_style==10)
	{	//���ڵ��ı����ȷֵ� wht 11-01-24
		pIO->ReadInteger(&des_para.SCALE_NODE.hLinePart);
		pIO->ReadInteger(&des_para.SCALE_NODE.hStartNode);
		pIO->ReadInteger(&des_para.SCALE_NODE.hEndNode);
		pIO->ReadDouble(&des_para.SCALE_NODE.start_offset_dist);
		pIO->ReadDouble(&des_para.SCALE_NODE.end_offset_dist);
		pIO->ReadDouble(&des_para.SCALE_NODE.offset_scale);
		pIO->ReadDouble(&des_para.SCALE_NODE.offset_dist);
	}
	else if(datum_pos_style==11)
	{
		//if(version==0||(doc_type==5&&version>=1020100))
		pIO->ReadInteger(&des_para.COLUMN_INTERS.hLineTube);		//>0ʱ�Ըøֹܶ��������axis_vec,axis_pick,column_r��
		pIO->ReadByte(&des_para.COLUMN_INTERS.cPointAndVectorFlag);
		pIO->ReadPoint(des_para.COLUMN_INTERS.AxisVector.axis_vec);		//���������������
		pIO->ReadPoint(des_para.COLUMN_INTERS.SectCenter.sect_center);		//�������������Ľ��㣬��Բ���棨Բ����Բ����Բ�ĵ�
		pIO->ReadInteger(&des_para.COLUMN_INTERS.SectCenterParam.hNode);		//����Բ�ĵ�ƫ�ƻ�׼�ڵ�
		pIO->ReadDouble(&des_para.COLUMN_INTERS.SectCenterParam.len_offset);	//����Բ���Ի�׼�ڵ��ƫ����

		pIO->ReadPoint(des_para.COLUMN_INTERS.SectNorm.sect_norm);		//���淨��Ϊ��ʱ��ʾΪ����Ĵ�ֱ����
		pIO->ReadByte(&des_para.COLUMN_INTERS.ray_vec_style);	//0.ray_vecΪֱ�Ӷ�������߾��Է���;>0:ָ��������ʼ�ߣ�������ray_angleȷ�����߷���
		pIO->ReadPoint(des_para.COLUMN_INTERS.RayVector.ray_vec);			//���߷���
		pIO->ReadDouble(&des_para.COLUMN_INTERS.ray_angle);		//ray_vec_style==1ʱ���߷�����������ʼ�ߵ���ʱ����ת�Ƕ�,��
		pIO->ReadDouble(&des_para.COLUMN_INTERS.column_r);		//����뾶
		pIO->ReadDouble(&des_para.COLUMN_INTERS.ray_para_offset);//����ƽ���ƶ����루��ֱ���߷������ƽ�����ƶ�,����������������ƶ�Ϊ������һ��Ϊ����
		pIO->ReadBooleanThin(&des_para.COLUMN_INTERS.bSpecRo);	//true:����뾶�Ծ�����ͶӰ���Ծ��붨��false:����뾶���������߽������������ƫ��ȷ��
		pIO->ReadDouble(&des_para.COLUMN_INTERS.ray_offset_r);	//bSpecRoByOffset=true:����뾶; bSpecRoByOffset=false:���߷�������ƫ����
	}
}
void CLDSPoint::CloneParaTo(CLDSPoint* pPoint)
{
	char pool[256]={0};	//sizeof(DES_PARA)<200
	CBuffer buff(pool,256);
	this->ToParaBuffer(&buff);
	buff.SeekToBegin();
	pPoint->FromParaBuffer(&buff);
	pos=pPoint->xRsltPosition;	//�û�ָ������׼�ߺͻ�׼�����������겻���
}
//<DEVELOP_PROCESS_MARK nameId="CLDSPoint::Serialize">
//doc_tma1_lma2_tsa3_lds4_default0
void CLDSPoint::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	//��ȡӰ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
		return;
	if(CTower::IsPrevDeflateVersion(version,doc_type))
	{
		long ll;
		buffer.ReadInteger(&ll);
		iNo=(short)ll;
		buffer.ReadInteger(&ll);
		datum_pos_style=(BYTE)ll;
	}
	else
	{
		buffer.ReadWord(&iNo);
		buffer.ReadByte(&datum_pos_style);
	}
	bool bHasArcLiftBeforePos=false;
	if(	version==0||(doc_type==1&&version>=4020001)||((doc_type==2||doc_type==4||doc_type==5)&&version>=1020001))
		bHasArcLiftBeforePos=true;	//�Ѿ߱��Զ�Ԥ������
	buffer.ReadString(description);
	buffer.ReadDword(&dwPermission);
	if(CTower::IsPrevDeflateVersion(version,doc_type) ||
		datum_pos_style==0||datum_pos_style==5||datum_pos_style==6)
		buffer.ReadPoint(pos);
	if(datum_pos_style==0)
	{
		if(bHasArcLiftBeforePos)
			buffer.ReadPoint(datum_pos);	//��ȡֱ��ָ������ʱԤ��ǰλ��
		else
			datum_pos=pos;
	}
	if(datum_pos_style==1||datum_pos_style==7)		//�Ǹ���㶨λ
	{
		buffer.ReadWord(&des_para.RODEND.direction);
		buffer.ReadInteger(&des_para.RODEND.bIncOddEffect);
		buffer.ReadInteger(&des_para.RODEND.hRod);
		buffer.ReadWord(&des_para.RODEND.offset_wing);
		buffer.ReadDouble(&des_para.RODEND.len_offset_dist);
		buffer.ReadWord(&des_para.RODEND.wing_offset_style);
		buffer.ReadDouble(&des_para.RODEND.wing_offset_dist);
		buffer.ReadString(des_para.RODEND.norm_offset);

	}
	else if(datum_pos_style==2)	//�Ǹֽڵ㶨λ
	{
		buffer.ReadWord(&des_para.RODNODE.direction);
		buffer.ReadInteger(&des_para.RODNODE.hRod);
		buffer.ReadInteger(&des_para.RODNODE.hNode);
		buffer.ReadWord(&des_para.RODNODE.bFaceOffset);
		buffer.ReadWord(&des_para.RODNODE.offset_wing);
		buffer.ReadDouble(&des_para.RODNODE.len_offset_dist);
		buffer.ReadWord(&des_para.RODNODE.wing_offset_style);
		buffer.ReadDouble(&des_para.RODNODE.wing_offset_dist);
		buffer.ReadPoint(des_para.RODNODE.vPlaneNormal);//sizeof(double)*3);
		buffer.ReadString(des_para.RODNODE.norm_offset);
	}
	else if(datum_pos_style==3||datum_pos_style==4)	//��׼�߿ռ佻��
	{
		buffer.ReadInteger(&des_para.AXIS_INTERS.hDatum1);
		buffer.ReadInteger(&des_para.AXIS_INTERS.hDatum2);
		buffer.ReadInteger(&des_para.AXIS_INTERS.wing_offset_dist1);
		buffer.ReadInteger(&des_para.AXIS_INTERS.wing_offset_dist2);
		buffer.ReadWord(&des_para.AXIS_INTERS.wing_offset_style1);
		buffer.ReadWord(&des_para.AXIS_INTERS.wing_offset_style2);
		if(	version==0||
			(doc_type==1&&version>=4020009)||
			(doc_type==2&&version>=1020009)||
			(doc_type==3&&version>=2000009)||
			(doc_type==4&&version>=1020009)||
			(doc_type==5&&version>=1020009))
			buffer.ReadByte(&des_para.AXIS_INTERS.cProjectPlane);
		else
			des_para.AXIS_INTERS.cProjectPlane=0;
		buffer.ReadDouble(&des_para.AXIS_INTERS.near_vector[0]);
		buffer.ReadDouble(&des_para.AXIS_INTERS.near_vector[1]);
		buffer.ReadDouble(&des_para.AXIS_INTERS.near_vector[2]);
		buffer.ReadString(des_para.AXIS_INTERS.norm_offset);
	}
	else if(datum_pos_style==5)	//��׼�����������겻���
	{
		buffer.ReadInteger(&des_para.DATUMLINE.hDatumLine);
		if( version==0||
			(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
			(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
			(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
			(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
			buffer.ReadByte(&des_para.DATUMLINE.cMirTransType);
		buffer.ReadWord(&des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2);
		if(bHasArcLiftBeforePos)
			buffer.ReadPoint(datum_pos);	//��ȡֱ��ָ������ʱԤ��ǰλ��
		else
			datum_pos=pos;
	}
	else if(datum_pos_style==6)	//��׼����ָ�������
	{
		buffer.ReadInteger(&des_para.DATUMPLANE.hDatumPlane);
		if( version==0||
			(doc_type==1&&version>=4010005)|| //TMA V4.1.0.5
			(doc_type==2&&version>=1010005)|| //LMA V1.1.0.5
			(doc_type==4&&version>=1010005)|| //LDS V1.1.0.5
			(doc_type==5&&version>=1010005))  //TDA V1.1.0.5
			buffer.ReadByte(&des_para.DATUMPLANE.cMirTransType);
		buffer.ReadWord(&des_para.DATUMPLANE.fix_axis_style_x0_y1_z2);
		if(bHasArcLiftBeforePos)
			buffer.ReadPoint(datum_pos);	//��ȡֱ��ָ������ʱԤ��ǰλ��
		else
			datum_pos=pos;
	}
	else if(datum_pos_style==8)	//��ڵ�ͬλ
		buffer.ReadInteger(&des_para.hNode);
	else if(datum_pos_style==9)	//ָ�������ϵ��������
	{
		buffer.ReadInteger(&des_para.hPart);
		buffer.ReadPoint(datum_pos);
	}
	else if(datum_pos_style==10)
	{	//���ڵ��ı����ȷֵ� wht 11-01-24
		if( version==0||
			(doc_type==1&&version>=4000101)|| //TMA V4.0.1.1
			(doc_type==2&&version>=1000101)|| //LMA V1.0.1.1
			(doc_type==3&&version>=1070301)|| //TSA V1.7.3.1
			(doc_type==4&&version>=1000301)||
			(doc_type==5))  //LDS V1.0.3.1
		{
			buffer.ReadInteger(&des_para.SCALE_NODE.hLinePart);
			buffer.ReadInteger(&des_para.SCALE_NODE.hStartNode);
			buffer.ReadInteger(&des_para.SCALE_NODE.hEndNode);
			buffer.ReadDouble(&des_para.SCALE_NODE.start_offset_dist);
			buffer.ReadDouble(&des_para.SCALE_NODE.end_offset_dist);
			buffer.ReadDouble(&des_para.SCALE_NODE.offset_scale);
			buffer.ReadDouble(&des_para.SCALE_NODE.offset_dist);
		}
	}
	else if(datum_pos_style==11)
	{
		//if(version==0||(doc_type==5&&version>=1020100))
		buffer.ReadInteger(&des_para.COLUMN_INTERS.hLineTube);		//>0ʱ�Ըøֹܶ��������axis_vec,axis_pick,column_r��
		buffer.ReadByte(&des_para.COLUMN_INTERS.cPointAndVectorFlag);
		buffer.ReadPoint(des_para.COLUMN_INTERS.AxisVector.axis_vec);		//���������������
		buffer.ReadPoint(des_para.COLUMN_INTERS.SectCenter.sect_center);		//�������������Ľ��㣬��Բ���棨Բ����Բ����Բ�ĵ�
	if(  version==0||
		(doc_type==1&&version>=5000300)||	//TMA V5.0.3.0
		(doc_type==2&&version>=2000300)||	//LMA V2.0.3.0
		(doc_type==3&&version>=2010300)||	//TSA V2.1.3.0
		(doc_type==4&&version>=1030300)||	//LDS V1.3.3.0
		(doc_type==5&&version>=1030300))	//TDA V1.3.3.0
		{
			buffer.ReadInteger(&des_para.COLUMN_INTERS.SectCenterParam.hNode);		//����Բ�ĵ�ƫ�ƻ�׼�ڵ�
			buffer.ReadDouble(&des_para.COLUMN_INTERS.SectCenterParam.len_offset);	//����Բ���Ի�׼�ڵ��ƫ����
		}
		buffer.ReadPoint(des_para.COLUMN_INTERS.SectNorm.sect_norm);		//���淨��Ϊ��ʱ��ʾΪ����Ĵ�ֱ����
		buffer.ReadByte(&des_para.COLUMN_INTERS.ray_vec_style);	//0.ray_vecΪֱ�Ӷ�������߾��Է���;>0:ָ��������ʼ�ߣ�������ray_angleȷ�����߷���
		buffer.ReadPoint(des_para.COLUMN_INTERS.RayVector.ray_vec);			//���߷���
		buffer.ReadDouble(&des_para.COLUMN_INTERS.ray_angle);		//ray_vec_style==1ʱ���߷�����������ʼ�ߵ���ʱ����ת�Ƕ�,��
		buffer.ReadDouble(&des_para.COLUMN_INTERS.column_r);		//����뾶
		buffer.ReadDouble(&des_para.COLUMN_INTERS.ray_para_offset);//����ƽ���ƶ����루��ֱ���߷������ƽ�����ƶ�,����������������ƶ�Ϊ������һ��Ϊ����
		buffer.ReadBooleanThin(&des_para.COLUMN_INTERS.bSpecRo);	//true:����뾶�Ծ�����ͶӰ���Ծ��붨��false:����뾶���������߽������������ƫ��ȷ��
		buffer.ReadDouble(&des_para.COLUMN_INTERS.ray_offset_r);	//bSpecRoByOffset=true:����뾶; bSpecRoByOffset=false:���߷�������ƫ����
	}
}
void CLDSPoint::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//����Ӱ�乹�������͹���������� wht 12-09-14
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
		return;
	buffer.WriteWord(iNo);
	buffer.WriteByte(datum_pos_style);
	buffer.WriteString(description);
	buffer.WriteDword(dwPermission);
	if(datum_pos_style==0||datum_pos_style==5||datum_pos_style==6)
		buffer.WritePoint(pos);	//�û�ָ������׼�ߺͻ�׼�����������겻���
	bool bHasArcLiftBeforePos=false;
	if(	version==0||(doc_type==1&&version>=4020001)||((doc_type==2||doc_type==4||doc_type==5)&&version>=1020001))
		bHasArcLiftBeforePos=true;	//�Ѿ߱��Զ�Ԥ������
	if(datum_pos_style==0&&bHasArcLiftBeforePos)
		buffer.WritePoint(datum_pos);
	if(datum_pos_style==1||datum_pos_style==7)		//�Ǹ���㶨λ
	{
		buffer.WriteWord(des_para.RODEND.direction);
		buffer.WriteInteger(des_para.RODEND.bIncOddEffect);
		buffer.WriteInteger(des_para.RODEND.hRod);
		buffer.WriteWord(des_para.RODEND.offset_wing);
		buffer.WriteDouble(des_para.RODEND.len_offset_dist);
		buffer.WriteWord(des_para.RODEND.wing_offset_style);
		buffer.WriteDouble(des_para.RODEND.wing_offset_dist);
		buffer.WriteString(des_para.RODEND.norm_offset);
	}
	else if(datum_pos_style==2)	//�Ǹֽڵ㶨λ
	{
		buffer.WriteWord(des_para.RODNODE.direction);
		buffer.WriteInteger(des_para.RODNODE.hRod);
		buffer.WriteInteger(des_para.RODNODE.hNode);
		buffer.WriteWord(des_para.RODNODE.bFaceOffset);
		buffer.WriteWord(des_para.RODNODE.offset_wing);
		buffer.WriteDouble(des_para.RODNODE.len_offset_dist);
		buffer.WriteWord(des_para.RODNODE.wing_offset_style);
		buffer.WriteDouble(des_para.RODNODE.wing_offset_dist);
		buffer.WritePoint(des_para.RODNODE.vPlaneNormal);
		buffer.WriteString(des_para.RODNODE.norm_offset);
	}
	else if(datum_pos_style==3||datum_pos_style==4)	//��׼�߿ռ佻��
	{
		buffer.WriteInteger(des_para.AXIS_INTERS.hDatum1);
		buffer.WriteInteger(des_para.AXIS_INTERS.hDatum2);
		buffer.WriteInteger(des_para.AXIS_INTERS.wing_offset_dist1);
		buffer.WriteInteger(des_para.AXIS_INTERS.wing_offset_dist2);
		buffer.WriteWord(des_para.AXIS_INTERS.wing_offset_style1);
		buffer.WriteWord(des_para.AXIS_INTERS.wing_offset_style2);
		if(	version==0||
			(doc_type==1&&version>=4020009)||
			(doc_type==2&&version>=1020009)||
			(doc_type==3&&version>=2000009)||
			(doc_type==4&&version>=1020009)||
			(doc_type==5&&version>=1020009))
		buffer.WriteByte(des_para.AXIS_INTERS.cProjectPlane);
		buffer.WriteDouble(des_para.AXIS_INTERS.near_vector[0]);
		buffer.WriteDouble(des_para.AXIS_INTERS.near_vector[1]);
		buffer.WriteDouble(des_para.AXIS_INTERS.near_vector[2]);
		buffer.WriteString(des_para.AXIS_INTERS.norm_offset);
	}
	else if(datum_pos_style==5)	//��׼�����������겻���
	{
		buffer.WriteInteger(des_para.DATUMLINE.hDatumLine);
		if( version==0||
			(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
			(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
			(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
			(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
			buffer.WriteByte(des_para.DATUMLINE.cMirTransType);
		buffer.WriteWord(des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2);
		if(bHasArcLiftBeforePos)
			buffer.WritePoint(datum_pos);
	}
	else if(datum_pos_style==6)	//��׼����ָ�������
	{
		buffer.WriteInteger(des_para.DATUMPLANE.hDatumPlane);
		buffer.WriteByte(des_para.DATUMPLANE.cMirTransType);
		buffer.WriteWord(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2);
		if(bHasArcLiftBeforePos)
			buffer.WritePoint(datum_pos);
	}
	else if(datum_pos_style==8)	//��ڵ�ͬλ
		buffer.WriteInteger(des_para.hNode);
	else if(datum_pos_style==9)	//ָ�������ϵ��������
	{
		buffer.WriteInteger(des_para.hPart);
		buffer.WritePoint(datum_pos);
	}
	else if(datum_pos_style==10)//���ڵ��ı����ȷֵ� wht 11-01-24
	{
		buffer.WriteInteger(des_para.SCALE_NODE.hLinePart);
		buffer.WriteInteger(des_para.SCALE_NODE.hStartNode);
		buffer.WriteInteger(des_para.SCALE_NODE.hEndNode);
		buffer.WriteDouble(des_para.SCALE_NODE.start_offset_dist);
		buffer.WriteDouble(des_para.SCALE_NODE.end_offset_dist);
		buffer.WriteDouble(des_para.SCALE_NODE.offset_scale);
		buffer.WriteDouble(des_para.SCALE_NODE.offset_dist);
	}
	else if(datum_pos_style==11)
	{
		buffer.WriteInteger(des_para.COLUMN_INTERS.hLineTube);		//>0ʱ�Ըøֹܶ��������axis_vec,axis_pick,column_r��
		buffer.WriteByte(des_para.COLUMN_INTERS.cPointAndVectorFlag);
		buffer.WritePoint(des_para.COLUMN_INTERS.AxisVector.axis_vec);		//���������������
		buffer.WritePoint(des_para.COLUMN_INTERS.SectCenter.sect_center);		//�������������Ľ��㣬��Բ���棨Բ����Բ����Բ�ĵ�
		if(  version==0||
			(doc_type==1&&version>=5000300)||	//TMA V5.0.3.0
			(doc_type==2&&version>=2000300)||	//LMA V2.0.3.0
			(doc_type==3&&version>=2010300)||	//TSA V2.1.3.0
			(doc_type==4&&version>=1030300)||	//LDS V1.3.3.0
			(doc_type==5&&version>=1030300))	//TDA V1.3.3.0
		{
			buffer.WriteInteger(des_para.COLUMN_INTERS.SectCenterParam.hNode);		//����Բ�ĵ�ƫ�ƻ�׼�ڵ�
			buffer.WriteDouble(des_para.COLUMN_INTERS.SectCenterParam.len_offset);	//����Բ���Ի�׼�ڵ��ƫ����
		}
		buffer.WritePoint(des_para.COLUMN_INTERS.SectNorm.sect_norm);		//���淨��Ϊ��ʱ��ʾΪ����Ĵ�ֱ����
		buffer.WriteByte(des_para.COLUMN_INTERS.ray_vec_style);	//0.ray_vecΪֱ�Ӷ�������߾��Է���;>0:ָ��������ʼ�ߣ�������ray_angleȷ�����߷���
		buffer.WritePoint(des_para.COLUMN_INTERS.RayVector.ray_vec);			//���߷���
		buffer.WriteDouble(des_para.COLUMN_INTERS.ray_angle);		//ray_vec_style==1ʱ���߷�����������ʼ�ߵ���ʱ����ת�Ƕ�,��
		buffer.WriteDouble(des_para.COLUMN_INTERS.column_r);		//����뾶
		buffer.WriteDouble(des_para.COLUMN_INTERS.ray_para_offset);//����ƽ���ƶ����루��ֱ���߷������ƽ�����ƶ�,����������������ƶ�Ϊ������һ��Ϊ����
		buffer.WriteBooleanThin(des_para.COLUMN_INTERS.bSpecRo);	//true:����뾶�Ծ�����ͶӰ���Ծ��붨��false:����뾶���������߽������������ƫ��ȷ��
		buffer.WriteDouble(des_para.COLUMN_INTERS.ray_offset_r);	//bSpecRoByOffset=true:����뾶; bSpecRoByOffset=false:���߷�������ƫ����
	}
}
//</DEVELOP_PROCESS_MARK>
void CLDSPoint::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<��׼����Ʋ���>\n");
	fprintf(fp,"<��� iNo=\"%d\"/>\n",iNo);
	/*  0.��λ�������Ϣ,ֱ��ָ��1.�Ǹ��������Ϊ��׼;2.�˼��Ͻڵ�Ϊ��׼;
	3.�Ǹ����߽���;4.��׼�߽���;5.��׼�����������겻���;
	6.��׼�����������겻���,7.�ؽǸ���֫����ƫ�Ƶİ������,8.��ڵ�ͬλ,
	9.ָ�������ϵ��������,10.���ڵ������ȷֵ�(�����ͽ�������嶨λ,���ڻ�׼��Ի�������ʾ)
	*/
	if (datum_pos_style==0)
		fprintf(fp,"<���㶨λ��ʽ datum_pos_style=\"0.ֱ��ָ��\"/>\n");
	else if (datum_pos_style==1)
		fprintf(fp,"<���㶨λ��ʽ datum_pos_style=\"1.�Ǹ��������Ϊ��׼\"/>\n");
	else if (datum_pos_style==2)
		fprintf(fp,"<���㶨λ��ʽ datum_pos_style=\"2.�˼��Ͻڵ�Ϊ��׼\"/>\n");
	else if (datum_pos_style==3)
		fprintf(fp,"<���㶨λ��ʽ datum_pos_style=\"3.�Ǹ����߽���\"/>\n");
	else if (datum_pos_style==4)
		fprintf(fp,"<���㶨λ��ʽ datum_pos_style=\"4.��׼�߽���\"/>\n");
	else if (datum_pos_style==5)
		fprintf(fp,"<���㶨λ��ʽ datum_pos_style=\"5.��׼�����������겻���\"/>\n");
	else if (datum_pos_style==6)
		fprintf(fp,"<���㶨λ��ʽ datum_pos_style=\"6.��׼�����������겻���\"/>\n");
	else if (datum_pos_style==7)
		fprintf(fp,"<���㶨λ��ʽ datum_pos_style=\"7.�ؽǸ���֫����ƫ�Ƶİ������\"/>\n");
	else if (datum_pos_style==8)
		fprintf(fp,"<���㶨λ��ʽ datum_pos_style=\"8.��ڵ�ͬλ\"/>\n");
	else if (datum_pos_style==9)
		fprintf(fp,"<���㶨λ��ʽ datum_pos_style=\"9.ָ�������ϵ��������\"/>\n");
	else if (datum_pos_style==10)
		fprintf(fp,"<���㶨λ��ʽ datum_pos_style=\"10.���ڵ������ȷֵ�\"/>\n");
	fprintf(fp,"<���� description=\"%s\"/>\n",description);
	if(datum_pos_style==0||datum_pos_style==5||datum_pos_style==6)
	{	//�û�ָ������׼�ߺͻ�׼�����������겻���
		sprintf(sText,"%f",pos.x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",pos.y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",pos.z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<λ�� pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	}
	if(datum_pos_style==0)
	{
		sprintf(sText,"%f",datum_pos.x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",datum_pos.y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",datum_pos.z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<������� datum_pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	}
	if(datum_pos_style==1||datum_pos_style==7)		//�Ǹ���㶨λ
	{
		if (des_para.RODEND.direction==0)
			fprintf(fp,"<��׼�Ǹַ��� des_para.RODEND.direction=\"0.ʼ->��\"/>\n");
		else if (des_para.RODEND.direction==1)
			fprintf(fp,"<��׼�Ǹַ��� des_para.RODEND.direction=\"1.��->ʼ\"/>\n");
		if (des_para.RODEND.bIncOddEffect)
			fprintf(fp,"<����ƫ��ֵ��������ͷӰ�� des_para.RODEND.bIncOddEffect=\"true\"/>\n");
		else
			fprintf(fp,"<����ƫ��ֵ��������ͷӰ�� des_para.RODEND.bIncOddEffect=\"false\"/>\n");
		fprintf(fp,"<�Ǹ־�� des_para.RODEND.hRod=\"0X%X\"/>\n",des_para.RODEND.hRod);
		if (des_para.RODEND.offset_wing==0)
			fprintf(fp,"<ƫ��֫ des_para.RODEND.offset_wing=\"0.X֫\"/>\n");
		else if (des_para.RODEND.offset_wing==1)
			fprintf(fp,"<ƫ��֫ des_para.RODEND.offset_wing=\"1.Y֫\"/>\n");
		sprintf(sText,"%f",des_para.RODEND.len_offset_dist);
		SimplifiedNumString(sText);
		fprintf(fp,"<����ƫ��ֵ des_para.RODEND.len_offset_dist=\"%s\"/>\n",sText);
		if (des_para.RODEND.wing_offset_style==0)	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			fprintf(fp,"<֫��ƫ������ des_para.RODEND.wing_offset_style=\"0.g\"/>\n");
		else if (des_para.RODEND.wing_offset_style==1)
			fprintf(fp,"<֫��ƫ������ des_para.RODEND.wing_offset_style=\"1.g1\"/>\n");
		else if (des_para.RODEND.wing_offset_style==2)
			fprintf(fp,"<֫��ƫ������ des_para.RODEND.wing_offset_style=\"2.g2\"/>\n");
		else if (des_para.RODEND.wing_offset_style==3)
			fprintf(fp,"<֫��ƫ������ des_para.RODEND.wing_offset_style=\"3.g3\"/>\n");
		else if (des_para.RODEND.wing_offset_style==4)
			fprintf(fp,"<֫��ƫ������ des_para.RODEND.wing_offset_style=\"4.�Զ���\"/>\n");
		sprintf(sText,"%f",des_para.RODEND.wing_offset_dist);
		SimplifiedNumString(sText);
		fprintf(fp,"<֫��ƫ��ֵ des_para.RODEND.wing_offset_dist=\"%s\"/>\n",sText);
		fprintf(fp,"<����ƫ���� des_para.RODEND.norm_offset=\"%s\"/>\n",des_para.RODEND.norm_offset);
	}
	else if(datum_pos_style==2)	//�Ǹֽڵ㶨λ
	{
		if (des_para.RODEND.direction==0)
			fprintf(fp,"<��׼�Ǹַ��� des_para.LINEPARTNODE.direction=\"ʼ->��\"/>\n");
		else if (des_para.RODEND.direction==1)
			fprintf(fp,"<��׼�Ǹַ��� des_para.LINEPARTNODE.direction=\"��->ʼ\"/>\n");
		fprintf(fp,"<�Ǹ־�� des_para.LINEPARTNODE.hLinePart=\"0X%X\"/>\n",des_para.RODNODE.hRod);
		fprintf(fp,"<��׼�ڵ��� des_para.LINEPARTNODE.hNode=\"0X%X\"/>\n",des_para.RODNODE.hNode);
		fprintf(fp,"<bFaceOffset des_para.LINEPARTNODE.bFaceOffset=\"%d\"/>\n",des_para.RODNODE.bFaceOffset);
		if (des_para.RODNODE.offset_wing==0)
			fprintf(fp,"<ƫ��֫ des_para.LINEPARTNODE.offset_wing=\"0.X֫\"/>\n");
		else if (des_para.RODNODE.offset_wing==1)
			fprintf(fp,"<ƫ��֫ des_para.LINEPARTNODE.offset_wing=\"1.Y֫\"/>\n");
		sprintf(sText,"%f",des_para.RODNODE.len_offset_dist);
		SimplifiedNumString(sText);
		fprintf(fp,"<����ƫ��ֵ des_para.LINEPARTNODE.len_offset_dist=\"%s\"/>\n",sText);
		if (des_para.RODNODE.wing_offset_style==0)	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			fprintf(fp,"<֫��ƫ������ des_para.LINEPARTNODE.wing_offset_style=\"0.g\"/>\n");
		else if (des_para.RODNODE.wing_offset_style==1)					
			fprintf(fp,"<֫��ƫ������ des_para.LINEPARTNODE.wing_offset_style=\"1.g1\"/>\n");
		else if (des_para.RODNODE.wing_offset_style==2)					
			fprintf(fp,"<֫��ƫ������ des_para.LINEPARTNODE.wing_offset_style=\"2.g2\"/>\n");
		else if (des_para.RODNODE.wing_offset_style==3)					
			fprintf(fp,"<֫��ƫ������ des_para.LINEPARTNODE.wing_offset_style=\"3.g3\"/>\n");
		else if (des_para.RODNODE.wing_offset_style==4)					
			fprintf(fp,"<֫��ƫ������ des_para.LINEPARTNODE.wing_offset_style=\"4.�Զ���\"/>\n");
		sprintf(sText,"%f",des_para.RODNODE.wing_offset_dist);
		SimplifiedNumString(sText);
		fprintf(fp,"<֫��ƫ��ֵ des_para.LINEPARTNODE.wing_offset_dist=\"%s\"/>\n",sText);
		fprintf(fp,"<����ƫ���� des_para.LINEPARTNODE.norm_offset=\"%s\"/>\n",des_para.RODEND.norm_offset);
		sprintf(sText,"%f",des_para.RODNODE.vPlaneNormal.x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",des_para.RODNODE.vPlaneNormal.y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",des_para.RODNODE.vPlaneNormal.z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<ƽ���淨�� des_para.LINEPARTNODE.face_norm_offset=\"[%s,%s,%s]\"/>\n",sText,sText2,sText3);
	}
	else if(datum_pos_style==3||datum_pos_style==4)	//��׼�߿ռ佻��
	{
		fprintf(fp,"<��׼��1��� des_para.AXIS_INTERS.hDatum1=\"0X%X\"/>\n",des_para.AXIS_INTERS.hDatum1);
		fprintf(fp,"<��׼��2��� des_para.AXIS_INTERS.hDatum2=\"0X%X\"/>\n",des_para.AXIS_INTERS.hDatum2);
		fprintf(fp,"<֫��ƫ��ֵ1 des_para.AXIS_INTERS.wing_offset_dist1=\"%d\"/>\n",des_para.AXIS_INTERS.wing_offset_dist1);
		fprintf(fp,"<֫��ƫ��ֵ2 des_para.AXIS_INTERS.wing_offset_dist2=\"%d\"/>\n",des_para.AXIS_INTERS.wing_offset_dist2);
		if (des_para.AXIS_INTERS.wing_offset_style1==0)	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			fprintf(fp,"<֫��ƫ������1 des_para.AXIS_INTERS.wing_offset_style1=\"0.g\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style1==1)					 
			fprintf(fp,"<֫��ƫ������1 des_para.AXIS_INTERS.wing_offset_style1=\"1.g1\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style1==2)					 
			fprintf(fp,"<֫��ƫ������1 des_para.AXIS_INTERS.wing_offset_style1=\"2.g2\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style1==3)					 
			fprintf(fp,"<֫��ƫ������1 des_para.AXIS_INTERS.wing_offset_style1=\"3.g3\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style1==4)					 
			fprintf(fp,"<֫��ƫ������1 des_para.AXIS_INTERS.wing_offset_style1=\"4.�Զ���\"/>\n");
		if (des_para.AXIS_INTERS.wing_offset_style2==0)	//0.g;1.g1;2.g2;3.g3;4.�Զ���
			fprintf(fp,"<֫��ƫ������1 des_para.AXIS_INTERS.wing_offset_style2=\"0.g\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style2==1)					 
			fprintf(fp,"<֫��ƫ������1 des_para.AXIS_INTERS.wing_offset_style2=\"1.g1\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style2==2)					 
			fprintf(fp,"<֫��ƫ������1 des_para.AXIS_INTERS.wing_offset_style2=\"2.g2\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style2==3)					 
			fprintf(fp,"<֫��ƫ������1 des_para.AXIS_INTERS.wing_offset_style2=\"3.g3\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style2==4)					 
			fprintf(fp,"<֫��ƫ������1 des_para.AXIS_INTERS.wing_offset_style2=\"4.�Զ���\"/>\n");
		sprintf(sText,"%f",des_para.AXIS_INTERS.near_vector[0]);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",des_para.AXIS_INTERS.near_vector[1]);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",des_para.AXIS_INTERS.near_vector[2]);
		SimplifiedNumString(sText3);
		fprintf(fp,"<���Ʒ��� des_para.AXIS_INTERS.near_vector=\"[%s,%s,%s]\"/>\n",sText,sText2,sText3);
		fprintf(fp,"<����ƫ���� des_para.AXIS_INTERS.norm_offset=\"%s\"/>\n",des_para.AXIS_INTERS.norm_offset);
	}
	else if(datum_pos_style==5)	//��׼�����������겻���
	{
		fprintf(fp,"<��׼�߾�� des_para.DATUMLINE.hDatumLine=\"0X%X\"/>\n",des_para.DATUMLINE.hDatumLine);
		if (des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			fprintf(fp,"<ָ������ des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=\"0.X��\"/>\n");
		else if (des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			fprintf(fp,"<ָ������ des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=\"1.Y��\"/>\n");
		else if (des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==2)
			fprintf(fp,"<ָ������ des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=\"2.Z��\"/>\n");
		sprintf(sText,"%f",datum_pos.x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",datum_pos.y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",datum_pos.z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<������� datum_pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	}
	else if(datum_pos_style==6)	//��׼����ָ�������
	{
		fprintf(fp,"<��׼���� des_para.DATUMPLANE.hDatumPlane=\"0X%X\"/>\n",des_para.DATUMPLANE.hDatumPlane);
		fprintf(fp,"<�ԳƱ任���� des_para.DATUMPLANE.cMirTransType=\"%c\"/>\n",des_para.DATUMPLANE.cMirTransType);
		if (des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			fprintf(fp,"<ָ������ des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=\"0.X��\"/>\n");
		else if (des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)				   
			fprintf(fp,"<ָ������ des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=\"1.Y��\"/>\n");
		else if (des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==2)				   
			fprintf(fp,"<ָ������ des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=\"2.Z��\"/>\n");
		sprintf(sText,"%f",datum_pos.x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",datum_pos.y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",datum_pos.z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<������� datum_pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	}
	else if(datum_pos_style==8)	//��ڵ�ͬλ
		fprintf(fp,"<��׼�ڵ��� des_para.hNode=\"0X%X\"/>\n",des_para.hNode);
	else if(datum_pos_style==9)	//ָ�������ϵ��������
	{
		fprintf(fp,"<��׼������� des_para.hPart=\"0X%X\"/>\n",des_para.hPart);
		sprintf(sText,"%f",datum_pos.x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",datum_pos.y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",datum_pos.z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<������� datum_pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	}
	else if(datum_pos_style==10)//���ڵ��ı����ȷֵ� wht 11-01-24
	{
		fprintf(fp,"<��׼�˼���� des_para.SCALE_NODE.hLinePart=\"0X%X\"/>\n",des_para.SCALE_NODE.hLinePart);
		fprintf(fp,"<ʼ�˽ڵ��� des_para.SCALE_NODE.hStartNode=\"0X%X\"/>\n",des_para.SCALE_NODE.hStartNode);
		fprintf(fp,"<�ն˽ڵ��� des_para.SCALE_NODE.hEndNode=\"0X%X\"/>\n",des_para.SCALE_NODE.hEndNode);
		sprintf(sText,"%f",des_para.SCALE_NODE.start_offset_dist);
		SimplifiedNumString(sText);
		fprintf(fp,"<ʼ��ƫ���� des_para.SCALE_NODE.start_offset_dist=\"%s\"/>\n",sText);
		sprintf(sText,"%f",des_para.SCALE_NODE.end_offset_dist);
		SimplifiedNumString(sText);
		fprintf(fp,"<�ն�ƫ���� des_para.SCALE_NODE.end_offset_dist=\"%s\"/>\n",sText);
		sprintf(sText,"%f",des_para.SCALE_NODE.offset_scale);
		SimplifiedNumString(sText);
		fprintf(fp,"<ƫ�Ʊ��� des_para.SCALE_NODE.offset_scale=\"%s\"/>\n",sText);
		sprintf(sText,"%f",des_para.SCALE_NODE.offset_dist);
		SimplifiedNumString(sText);
		fprintf(fp,"<�ڵȷֻ����ϵ�ƫ��ֵ des_para.SCALE_NODE.offset_dist=\"%s\"/>\n",sText);
	}
	fprintf(fp,"</��׼����Ʋ���>\n");
}
void CLDSPoint::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//�ѵ��ù�)
		return;
	if(handle>0x20)	//ֻ���ȫ�ֻ�׼��
		_console->AddToObjSetBuf(this);
	CLDSPart *pPart;
	CLDSNode *pNode;
	bAddRelaObj=bAddRelaObj&&bAddSubLevelRelaObj;
	if(datum_pos_style==1||datum_pos_style==7)
	{	//�Ǹ���㶨λ
		pPart=m_pModel->FromPartHandle(des_para.RODEND.hRod);
		if(pPart!=NULL)
			pPart->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==2)
	{	//�˼��ڵ㶨λ
		pPart=m_pModel->FromPartHandle(des_para.RODNODE.hRod);
		if(pPart!=NULL)
			pPart->AddToConsoleObjBuf(bAddRelaObj);
		pNode=m_pModel->FromNodeHandle(des_para.RODNODE.hNode);
		if(pNode!=NULL)
			pNode->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==3)
	{	//���߽���
		pPart=m_pModel->FromPartHandle(des_para.AXIS_INTERS.hDatum1);
		if(pPart!=NULL)
			pPart->AddToConsoleObjBuf(bAddRelaObj);
		pPart=m_pModel->FromPartHandle(des_para.AXIS_INTERS.hDatum2);
		if(pPart!=NULL)
			pPart->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==4)
	{	//��׼�߽���
		CLDSLine *pLine=m_pModel->FromLineHandle(des_para.AXIS_INTERS.hDatum1);
		if(pLine!=NULL)
			pLine->AddToConsoleObjBuf(bAddRelaObj);
		pLine=m_pModel->FromLineHandle(des_para.AXIS_INTERS.hDatum2);
		if(pLine!=NULL)
			pLine->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==5)
	{	//��׼�����������겻���
		CLDSLine *pLine=m_pModel->FromLineHandle(des_para.DATUMLINE.hDatumLine);
		if(pLine!=NULL)
			pLine->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==6)
	{	//��׼�����������겻���
		CLDSPlane *pPlane=m_pModel->FromPlaneHandle(des_para.DATUMPLANE.hDatumPlane);
		if(pPlane!=NULL)
			pPlane->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==8)
	{	//��ڵ�ͬλ
		pNode=m_pModel->FromNodeHandle(des_para.hNode);
		if(pNode!=NULL)
			pNode->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==9)
	{	//ָ�������ϵ��������
		pPart=m_pModel->FromPartHandle(des_para.hPart);
		if(pPart!=NULL)
			pPart->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==10)
	{	//���ڵ������ȷֵ�
		pPart=m_pModel->FromPartHandle(des_para.SCALE_NODE.hLinePart);
		if(pPart!=NULL)
			pPart->AddToConsoleObjBuf(bAddRelaObj);
		pNode=m_pModel->FromNodeHandle(des_para.SCALE_NODE.hStartNode);
		if(pNode!=NULL)
			pNode->AddToConsoleObjBuf(bAddRelaObj);
		pNode=m_pModel->FromNodeHandle(des_para.SCALE_NODE.hEndNode);
		if(pNode!=NULL)
			pNode->AddToConsoleObjBuf(bAddRelaObj);
	}
}

CLDSLine::CLDSLine()
{
	m_nClassTypeId=CLS_GELINE;
	//strcpy(m_sClassName,"CLDSLine");
	strcpy(description,"");
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN��������Ա���;
}
bool CLDSLine::ReplaceReferenceHandle(CHandleReplacer& replacer)
{
	bool replaced=false;
	if(replacer.hOld==0)
		handle+=replacer.hNew;
	else if(handle==replacer.hOld&&replacer.IsReplaceObjSelfHandle())
		handle=replacer.hNew;
	replaced|=replacer.VerifyAndReplace(&hPointArr[0].handle);
	replaced|=replacer.VerifyAndReplace(&hPointArr[1].handle);
	return replaced;
}
long CLDSLine::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	pListHandlers->Append(HANDLE_ADDR(&handle));
	pListHandlers->Append(HANDLE_ADDR(&hPointArr[0].handle));
	pListHandlers->Append(HANDLE_ADDR(&hPointArr[1].handle));
	return 3;
}
void CLDSLine::CleanInvalidObjRef(IModel *pModel/*=NULL*/)
{
	if(pModel==NULL)
	{
		if(m_pModel!=NULL)
			pModel=m_pModel;
	}
	if(pModel==NULL)
		return;
	pModel->ValidateObjRefH(&hPointArr[0].handle,true);
	pModel->ValidateObjRefH(&hPointArr[1].handle,true);
}
BOOL CLDSLine::UpdateLine(IModel *pTower)
{
	CLDSPoint tempPointArr[2];
	CLDSPoint *pStartPoint=NULL;
	//ʼ��
	if(hPointArr[0].bSimpleNodePoint)
	{
		tempPointArr[0].datum_pos_style=8;
		tempPointArr[0].des_para.hNode=hPointArr[0].handle;
		pStartPoint=&tempPointArr[0];
	}
	else if(hPointArr[0].bRodTipStart)
	{
		tempPointArr[0].datum_pos_style=1;	//�˼��˵㶨λ
		tempPointArr[0].des_para.RODEND.hRod=hPointArr[0].handle;
		tempPointArr[0].des_para.RODEND.wing_offset_style=4;
		tempPointArr[0].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[0].des_para.RODEND.len_offset_dist=0;
		tempPointArr[0].des_para.RODEND.direction=0;
		pStartPoint=&tempPointArr[0];
	}
	else if(hPointArr[0].bRodTipEnd)
	{
		tempPointArr[0].datum_pos_style=1;	//�˼��˵㶨λ
		tempPointArr[0].des_para.RODEND.hRod=hPointArr[0].handle;
		tempPointArr[0].des_para.RODEND.wing_offset_style=4;
		tempPointArr[0].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[0].des_para.RODEND.len_offset_dist=0;
		tempPointArr[0].des_para.RODEND.direction=1;
		pStartPoint=&tempPointArr[0];
	}
	else
		pStartPoint=pTower->FromPointHandle(hPointArr[0]);
	if(pStartPoint)
	{
		pStartPoint->UpdatePos(pTower);
		start_pos=pStartPoint->Position();
	}
	//�ն�
	CLDSPoint *pEndPoint=NULL;
	if(hPointArr[1].bSimpleNodePoint)
	{
		tempPointArr[1].datum_pos_style=8;
		tempPointArr[1].des_para.hNode=hPointArr[1].handle;
		pEndPoint=&tempPointArr[1];
	}
	else if(hPointArr[1].bRodTipStart)
	{
		tempPointArr[1].datum_pos_style=1;	//�˼��˵㶨λ
		tempPointArr[1].des_para.RODEND.hRod=hPointArr[1].handle;
		tempPointArr[1].des_para.RODEND.wing_offset_style=4;
		tempPointArr[1].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[1].des_para.RODEND.len_offset_dist=0;
		tempPointArr[1].des_para.RODEND.direction=0;
		pEndPoint=&tempPointArr[1];
	}
	else if(hPointArr[1].bRodTipEnd)
	{
		tempPointArr[1].datum_pos_style=1;	//�˼��˵㶨λ
		tempPointArr[1].des_para.RODEND.hRod=hPointArr[1].handle;
		tempPointArr[1].des_para.RODEND.wing_offset_style=4;
		tempPointArr[1].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[1].des_para.RODEND.len_offset_dist=0;
		tempPointArr[1].des_para.RODEND.direction=1;
		pEndPoint=&tempPointArr[1];
	}
	else
		pEndPoint=pTower->FromPointHandle(hPointArr[1]);	
	if(pEndPoint)
	{
		pEndPoint->UpdatePos(pTower);
		end_pos=pEndPoint->Position();
	}
	return TRUE;
}
void CLDSLine::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_tma1_lma2_tsa3_lds4_default0/*=0*/)
{
	//��ȡӰ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_tma1_lma2_tsa3_lds4_default0))
		return;
	buffer.ReadInteger(&iNo);
	buffer.ReadDword(&dwPermission);
	buffer.ReadString(description);
	buffer.ReadInteger(&hPointArr[0].handle);
	buffer.ReadInteger(&hPointArr[1].handle);
	buffer.ReadDouble(&start_pos.x);
	buffer.ReadDouble(&start_pos.y);
	buffer.ReadDouble(&start_pos.z);
	buffer.ReadDouble(&end_pos.x);
	buffer.ReadDouble(&end_pos.y);
	buffer.ReadDouble(&end_pos.z);
}
void CLDSLine::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//����Ӱ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
		return;
	buffer.WriteInteger(iNo);
	buffer.WriteDword(dwPermission);
	buffer.WriteString(description);
	buffer.WriteInteger(hPointArr[0].handle);
	buffer.WriteInteger(hPointArr[1].handle);
	buffer.WriteDouble(start_pos.x);
	buffer.WriteDouble(start_pos.y);
	buffer.WriteDouble(start_pos.z);
	buffer.WriteDouble(end_pos.x);
	buffer.WriteDouble(end_pos.y);
	buffer.WriteDouble(end_pos.z);
}
void CLDSLine::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<�� handle=\"0X%X\">\n",handle);
	fprintf(fp,"<��� iNo=\"%d\"/>\n",iNo);
	fprintf(fp,"<Ȩ���� dwPermission=\"%d\"/>\n",dwPermission);
	fprintf(fp,"<���� description=\"%s\"/>\n",description);
	fprintf(fp,"<������ hPointArr=\"[0X%X,0X%X]\"/>\n",hPointArr[0],hPointArr[1]);
	sprintf(sText,"%f",start_pos.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",start_pos.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",start_pos.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<���λ�� start_pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",end_pos.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",end_pos.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",end_pos.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<�յ�λ�� end_pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	fprintf(fp,"</��>\n");
}
void CLDSLine::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//�ѵ��ù�)
		return;
	if(handle>0x20)
		_console->AddToObjSetBuf(this);
}
CSupplementPart::CSupplementPart()
{
	//strcpy(m_sClassName,"CSupplementPart");
	m_nClassTypeId=CLS_SUPPLEMENTPART;
	cMaterial = 'S';		//����:'S'-Q235;'H'-Q345
	strcpy(sPartNo,"");			//�������
	strcpy(sNotes,"");			//��ע
	length= 0;			//����
	piece_weight = 0;	//������
	nQuantity = 1;		//��������
	part_type=0;		//��������
	bLegPart=FALSE;		//�Ȳ�����
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN��������Ա���;
	m_uStatMatCoef=1;
}
void CSupplementPart::FromBuffer(CBuffer &buffer,long version,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	buffer.ReadByte(&_material);		//����:'S'-Q235;'H'-Q345
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.ReadByte(&_cQualityLevel);
	buffer.ReadString(sPartNo);			//�������
	buffer.ReadString((char*)guige);			//���
	buffer.ReadString(sNotes,51);			//��ע
	buffer.ReadDouble(&length);			//����
	buffer.ReadDouble(&piece_weight);	//������
	buffer.ReadInteger(&nQuantity);		//��������
	buffer.ReadInteger(&_iSeg.iSeg);		//�ֶκ�
	buffer.ReadInteger(&part_type);		//��������
	buffer.ReadInteger(&bLegPart);		//�Ȳ�����
}
void CSupplementPart::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	buffer.WriteByte(cMaterial);		//����:'S'-Q235;'H'-Q345
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.WriteByte(m_cQualityLevel);
	buffer.WriteString(sPartNo);		//�������
	buffer.WriteString((char*)guige);			//���
	buffer.WriteString(sNotes);			//��ע
	buffer.WriteDouble(length);			//����
	buffer.WriteDouble(piece_weight);	//������
	buffer.WriteInteger(nQuantity);		//��������
	buffer.WriteInteger(iSeg);		//�ֶκ�
	buffer.WriteInteger(part_type);		//��������
	buffer.WriteInteger(bLegPart);		//�Ȳ�����
}

void CSupplementPart::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="";
	fprintf(fp,"<���ӹ������� handle=\"0X%X\">\n",handle);
	fprintf(fp,"<��ĺ� cfgword.word=\"%d,%d,%d,%d,%d\"/>\n",cfgword.flag.word[0],cfgword.flag.word[1],cfgword.flag.word[2],cfgword.flag.word[3],cfgword.flag.word[4]);
	fprintf(fp,"<���� cMaterial=\"%c\"/>\n",cMaterial);
	fprintf(fp,"<������� sPartNo=\"%s\"/>\n",(char*)sPartNo);
	fprintf(fp,"<��� guige=\"%s\"/>\n",(char*)guige);
	fprintf(fp,"<��ע sNotes=\"%s\"/>\n",sNotes);
	sprintf(sText,"%f",length);
	SimplifiedNumString(sText);
	fprintf(fp,"<���� length=\"%s\"/>\n",sText);
	sprintf(sText,"%f",piece_weight);
	SimplifiedNumString(sText);
	fprintf(fp,"<������ piece_weight=\"%s\"/>\n",sText);
	fprintf(fp,"<�������� nQuantity=\"%d\"/>\n",nQuantity);
	fprintf(fp,"<�ֶκ� iSeg=\"%d\"/>\n",iSeg);
	if (part_type==0)
		fprintf(fp,"<�������� part_type=\"0.�Ǹ�\"/>\n");
	else if (part_type==1)				  
		fprintf(fp,"<�������� part_type=\"1.�ְ�\"/>\n");
	else if (part_type==2)
		fprintf(fp,"<�������� part_type=\"2.����\"/>\n");
	if (bLegPart)
		fprintf(fp,"<�Ȳ����� bLegPart=\"true\"/>\n");
	else
		fprintf(fp,"<�Ȳ����� bLegPart=\"false\"/>\n");
	fprintf(fp,"</���ӹ�������>\n");
}

CLDSPlane::CLDSPlane()
{
	m_nClassTypeId=CLS_GEPLANE;
	//strcpy(m_sClassName,"CLDSPlane");
	strcpy(description,"");
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN��������Ա���;
}
bool CLDSPlane::IsSameWith(const double* pxPlaneOrg,const double* pvPlaneStdNormal)
{
	GEPOINT xPlaneOrg(pxPlaneOrg),vPlaneStdNormal(pvPlaneStdNormal);
	double cosa=fabs(work_norm*vPlaneStdNormal);
	if(cosa<EPS_COS)
		return false;
	GEPOINT vOrgDistVec=xPlaneOrg-GEPOINT(base_pos);
	double zDepth=vOrgDistVec*vPlaneStdNormal;
	if(fabs(zDepth)>0.01)
		return false;	//Z���������0.01mm�������������ڹ����Ͼ����㹻
	return true;
}
BOOL CLDSPlane::UpdatePlane(IModel *pTower,CLDSArcLift* pLift,bool bReverseLift/*=false*/)
{
	//try{
	if(pTower==NULL)
		pTower=this->m_pModel;
	CLDSPoint* pFirPoint,*pSecPoint,*pThirPoint;
	CLDSPoint tempPointArr[3];
	if(hPointArr[0].bSimpleNodePoint)
	{
		tempPointArr[0].datum_pos_style=8;
		tempPointArr[0].des_para.hNode=hPointArr[0].handle;
		pFirPoint=&tempPointArr[0];
	}
	else if(hPointArr[0].bRodTipStart)
	{
		tempPointArr[0].datum_pos_style=1;	//�˼��˵㶨λ
		tempPointArr[0].des_para.RODEND.hRod=hPointArr[0].handle;
		tempPointArr[0].des_para.RODEND.wing_offset_style=4;
		tempPointArr[0].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[0].des_para.RODEND.len_offset_dist=0;
		tempPointArr[0].des_para.RODEND.direction=0;
		
		pFirPoint=&tempPointArr[0];
	}
	else if(hPointArr[0].bRodTipEnd)
	{
		tempPointArr[0].datum_pos_style=1;	//�˼��˵㶨λ
		tempPointArr[0].des_para.RODEND.hRod=hPointArr[0].handle;
		tempPointArr[0].des_para.RODEND.wing_offset_style=4;
		tempPointArr[0].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[0].des_para.RODEND.len_offset_dist=0;
		tempPointArr[0].des_para.RODEND.direction=1;
		pFirPoint=&tempPointArr[0];
	}
	else
		pFirPoint=pTower->FromPointHandle(hPointArr[0]);
	if(hPointArr[1].bSimpleNodePoint)
	{
		tempPointArr[1].datum_pos_style=8;
		tempPointArr[1].des_para.hNode=hPointArr[1].handle;
		pSecPoint=&tempPointArr[1];
	}
	else if(hPointArr[1].bRodTipStart)
	{
		tempPointArr[1].datum_pos_style=1;	//�˼��˵㶨λ
		tempPointArr[1].des_para.RODEND.hRod=hPointArr[1].handle;
		tempPointArr[1].des_para.RODEND.wing_offset_style=4;
		tempPointArr[1].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[1].des_para.RODEND.len_offset_dist=0;
		tempPointArr[1].des_para.RODEND.direction=0;
		pSecPoint=&tempPointArr[1];
	}
	else if(hPointArr[1].bRodTipEnd)
	{
		tempPointArr[1].datum_pos_style=1;	//�˼��˵㶨λ
		tempPointArr[1].des_para.RODEND.hRod=hPointArr[1].handle;
		tempPointArr[1].des_para.RODEND.wing_offset_style=4;
		tempPointArr[1].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[1].des_para.RODEND.len_offset_dist=0;
		tempPointArr[1].des_para.RODEND.direction=1;
		pSecPoint=&tempPointArr[1];
	}
	else
		pSecPoint=pTower->FromPointHandle(hPointArr[1]);
	if(hPointArr[2].bSimpleNodePoint)
	{
		tempPointArr[2].datum_pos_style=8;
		tempPointArr[2].des_para.hNode=hPointArr[2].handle;
		pThirPoint=&tempPointArr[2];
	}
	else if(hPointArr[2].bRodTipStart)
	{
		tempPointArr[2].datum_pos_style=1;	//�˼��˵㶨λ
		tempPointArr[2].des_para.RODEND.hRod=hPointArr[2].handle;
		tempPointArr[2].des_para.RODEND.wing_offset_style=4;
		tempPointArr[2].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[2].des_para.RODEND.len_offset_dist=0;
		tempPointArr[2].des_para.RODEND.direction=0;
		pThirPoint=&tempPointArr[2];
	}
	else if(hPointArr[2].bRodTipEnd)
	{
		tempPointArr[2].datum_pos_style=1;	//�˼��˵㶨λ
		tempPointArr[2].des_para.RODEND.hRod=hPointArr[2].handle;
		tempPointArr[2].des_para.RODEND.wing_offset_style=4;
		tempPointArr[2].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[2].des_para.RODEND.len_offset_dist=0;
		tempPointArr[2].des_para.RODEND.direction=1;
		pThirPoint=&tempPointArr[2];
	}
	else
		pThirPoint=pTower->FromPointHandle(hPointArr[2]);
	//
		if(pFirPoint)
			pFirPoint->UpdatePos(pTower,pLift,bReverseLift);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Datum plane 0X%X's parametric point 0X%X  has been deleted,the plane update failed��",handle,hPointArr[0]);
#else 
			logerr.Log("��׼��0X%X�ڵ�0X%X���������ѱ�ɾ����ƽ�����ʧ�ܣ�",handle,hPointArr[0].handle);
#endif
		if(pSecPoint)
			pSecPoint->UpdatePos(pTower,pLift,bReverseLift);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Datum plane 0X%X's parametric point 0X%X  has been deleted,the plane update failed��",handle,hPointArr[1]);
#else 
			logerr.Log("��׼��0X%X�ڵ�0X%X���������ѱ�ɾ����ƽ�����ʧ�ܣ�",handle,hPointArr[1].handle);
#endif
		if(pThirPoint)
			pThirPoint->UpdatePos(pTower,pLift,bReverseLift);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Datum plane 0X%X's parametric point 0X%X  has been deleted,the plane update failed��",handle,hPointArr[2]);
#else 
			logerr.Log("��׼��0X%X�ڵ�0X%X���������ѱ�ɾ����ƽ�����ʧ�ܣ�",handle,hPointArr[2].handle);
#endif
		if(pFirPoint&&pSecPoint&&pThirPoint)
		{
			f3dPoint vec1=pSecPoint->Position()-pFirPoint->Position();
			f3dPoint vec2=pThirPoint->Position()-pSecPoint->Position();
			normalize(vec1);
			normalize(vec2);
			if(fabs(vec1*vec2)>EPS_COS)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("The three parametric point of datum plane 0X%X are collinear,the plane update failed��",handle);
#else 
				logerr.Log("��׼��0X%X�ڵ������������㹲�ߣ�ƽ�����ʧ�ܣ�",handle);
#endif
				return FALSE;	//����׼�㹲��
			}
			work_norm=vec1^vec2;
			normalize(work_norm);
			base_pos=pFirPoint->Position();
		}
		else
			return FALSE;
	/*}
	catch(CPtrException &e)
	{
		//AfxMessageBox(e.sError);
		return FALSE;
	}*/
	return TRUE;
}
long CLDSPlane::StatObjHandleReference(IXhList<HANDLE_ADDR>* pListHandlers)
{
	pListHandlers->Append(HANDLE_ADDR(&handle));
	for (int i = 0; i < 3; i++)
	{
		//if (hPointArr[i].bSimpleNodePoint)
		//{
		//	pListHandlers->Append(HANDLE_ADDR(&hPointArr[i].handle));
		//}
		//else if (hPointArr[i].bRodTipStart || hPointArr[i].bRodTipEnd)
		//{
		//	pListHandlers->Append(HANDLE_ADDR(&hPointArr[i].handle));
		//}
		//else
			pListHandlers->Append(HANDLE_ADDR(&hPointArr[i].handle));
	}
	return 3;
}

BOOL CLDSPlane::IsValidPlane()
{
	/*f3dPoint vec1=point_sec.pos-point_fir.pos;
	f3dPoint vec2=point_thir.pos-point_sec.pos;
	normalize(vec1);
	normalize(vec2);
	if(fabs(vec1*vec2)>EPS_COS)
		return FALSE;	//������ƽ�У����Ϸ�ƽ��
	else*/
		return TRUE;	//������ƽ�У��Ϸ�ƽ��

}
f3dPoint CLDSPlane::GetWorkNorm()
{
	if(IsShadowObject())
	{
		if(m_iShadowObjType==1)
		{
			CTower *pTower=(CTower*)m_pModel;
			CParaCS *pAcs=pTower->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
			if(pAcs)
				work_norm=pAcs->GetParamInfo().m_xMirInfo.MirVector(work_norm);
		}
		else if(m_iShadowObjType==2||m_iShadowObjType==3)
		{
			CBlockParameter blockPara;
			if(GetBlockParameter(blockPara))
			{
				UCS_STRU cs=blockPara.m_pBlock->TransToACS(blockPara.m_pBlockRef->GetACS());
				vector_trans(work_norm,cs,TRUE);
			}
		}
	}
	return work_norm;
}
f3dPoint CLDSPlane::GetBasePos()
{
	if(IsShadowObject())
	{
		if(m_iShadowObjType==1)
		{
			CTower *pTower=(CTower*)m_pModel;
			CParaCS *pAcs=pTower->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
			if(pAcs)
				base_pos=pAcs->GetParamInfo().m_xMirInfo.MirPoint(base_pos);
		}
		else if(m_iShadowObjType==2||m_iShadowObjType==3)
		{
			CBlockParameter blockPara;
			if(GetBlockParameter(blockPara))
			{
				UCS_STRU cs=blockPara.m_pBlock->TransToACS(blockPara.m_pBlockRef->GetACS());
				coord_trans(base_pos,cs,TRUE);
			}
		}
	}
	return base_pos;
}
void CLDSPlane::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//��ȡӰ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
		return;
	buffer.ReadInteger(&iNo);
	buffer.ReadDword(&dwPermission);
	buffer.ReadString(description);
	buffer.ReadPoint(base_pos);
	buffer.ReadPoint(work_norm);
	if( (version==0&&doc_type!=3)||
		(doc_type==1&&version>=5020000)||	//TMA V5.2.0.0
		(doc_type==2&&version>=2010000)||	//LMA V2.1.0.0
		(doc_type==4&&version>=1030804)||	//LDS V1.3.8.4
		(doc_type==5&&version>=1030804))	//TDA V1.3.8.4
	{
		buffer.ReadByte(&hPointArr[0].ciHidType);
		buffer.ReadByte(&hPointArr[1].ciHidType);
		buffer.ReadByte(&hPointArr[2].ciHidType);
	}
	buffer.ReadInteger(&hPointArr[0].handle);
	buffer.ReadInteger(&hPointArr[1].handle);
	buffer.ReadInteger(&hPointArr[2].handle);
}
bool CLDSPlane::PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long version/*=0*/,long doc_type/*=0*/)
{
	if(pOldVerInfo==NULL)
		return false;
	if(  version>=0&&
		(doc_type==1&&version<5020000)||(doc_type==2&&version<2010000)||	//TMA V5.2.0.0|LMA V2.1.0.0
		(doc_type==4&&version<1030804)||(doc_type==5&&version<1030804))		//LDS V1.3.8.4|TDA V1.3.8.4
	{
		for(int j=0;j<3;j++)
		{
			pIO->WriteByte(hPointArr[j].ciHidType);
			pIO->WriteInteger(hPointArr[j].handle);
			COldVerDataFile::SIMPLE_DATUMPOINT* pSimpleDatumPoint=pOldVerInfo->hashTmpDatumPoints.GetValue(hPointArr[j].handle);
			if(pSimpleDatumPoint!=NULL)
				hPointArr[j].handle=pSimpleDatumPoint->hNewTmpDatumPoint;
		}
	}
	return true;
}
bool CLDSPlane::PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long version/*=0*/,long doc_type/*=0*/)
{
	if(pOldVerInfo==NULL)
		return false;
	pIO->SeekToBegin();
	if(  version>=0&&
		(doc_type==1&&version<5020000)||(doc_type==2&&version<2010000)||	//TMA V5.2.0.0|LMA V2.1.0.0
		(doc_type==4&&version<1030804)||(doc_type==5&&version<1030804))		//LDS V1.3.8.4|TDA V1.3.8.4
	{
		for(int j=0;j<3;j++)
		{
			pIO->ReadByte(&hPointArr[j].ciHidType);
			pIO->ReadInteger(&hPointArr[j].handle);
		}
	}
	return true;
}
void CLDSPlane::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//����Ӱ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
		return;
	buffer.WriteInteger(iNo);
	buffer.WriteDword(dwPermission);
	buffer.WriteString(description);
	buffer.WriteDouble(base_pos.x);
	buffer.WriteDouble(base_pos.y);
	buffer.WriteDouble(base_pos.z);
	buffer.WriteDouble(work_norm.x);
	buffer.WriteDouble(work_norm.y);
	buffer.WriteDouble(work_norm.z);
	if( (version==0&&doc_type!=3)||
		(doc_type==1&&version>=5020000)||	//TMA V5.2.0.0
		(doc_type==2&&version>=2010000)||	//LMA V2.1.0.0
		(doc_type==4&&version>=1030804)||	//LDS V1.3.8.4
		(doc_type==5&&version>=1030804))	//TDA V1.3.8.4
	{
		buffer.WriteByte(hPointArr[0].ciHidType);
		buffer.WriteByte(hPointArr[1].ciHidType);
		buffer.WriteByte(hPointArr[2].ciHidType);
	}
	buffer.WriteInteger(hPointArr[0].handle);
	buffer.WriteInteger(hPointArr[1].handle);
	buffer.WriteInteger(hPointArr[2].handle);
}

void CLDSPlane::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<�� handle=\"0X%X\">\n",handle);
	fprintf(fp,"<��� iNo=\"%d\"/>\n",iNo);
	fprintf(fp,"<Ȩ���� dwPermission=\"%d\"/>\n",dwPermission);
	fprintf(fp,"<���� description=\"%s\"/>\n",description);
	sprintf(sText,"%f",base_pos.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",base_pos.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",base_pos.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<���� base_pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",work_norm.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",work_norm.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",work_norm.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<�������� work_norm=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	fprintf(fp,"<������ hPointArr=\"[0X%X,0X%X,0X%X]\"/>\n",hPointArr[0],hPointArr[1],hPointArr[2]);
	fprintf(fp,"</��>\n");	
}
void CLDSPlane::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//�ѵ��ù�)
		return;
	if(handle>0x20)
		_console->AddToObjSetBuf(this);
	for(int i=0;i<3;i++)
	{
		CLDSPoint *pPoint=m_pModel->FromPointHandle(hPointArr[i]);
		if(pPoint)
			pPoint->AddToConsoleObjBuf(bAddRelaObj,false);
	}
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
DESIGN_LS_POS::DESIGN_LS_POS()
{
	cLocationStyle=0;
	R=-1;
	m_biWingOffsetStyle=4;
	direction=0;
	offset_angle=offset_wing=0;
	wing_offset_dist=len_offset_dist=0;
}
void DESIGN_LS_POS::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type)
{
	//����λ���������
	datumPoint.FromBuffer(buffer,version,doc_type);
	if (version==0||(
		(doc_type==1&&version>=5020200)||	//TMA V5,2,2,0
		(doc_type==2&&version>=2010200)||	//LMA V2,1,2,0
		(doc_type==3&&version>=2010401)||	//TSA V2,1,4,1
		(doc_type==4&&version>=1031000)||	//LDS V1,3,10,0
		(doc_type==5&&version>=1031000)))	//TDA V1,3,10,0
		buffer.ReadBooleanThin((bool*)&cLocationStyle);
	else
		buffer.ReadBoolean((bool*)&cLocationStyle);
	buffer>>hPart;
	buffer>>len_offset_dist;
	buffer.ReadDouble(&wing_offset_dist);
	if(  version==0||
		(doc_type==1&&version>=5000301)||	//TMA V5.0.3.1
		(doc_type==2&&version>=2000301)||	//LMA V2.0.3.1
		(doc_type==3&&version>=2010301)||	//TSA V2.1.3.1
		(doc_type==4&&version>=1030301)||	//LDS V1.3.3.1
		(doc_type==5&&version>=1030301))	//TDA V1.3.3.1
		buffer.ReadByte(&m_biWingOffsetStyle);
	if(  version>0&&(
		(doc_type==1&&version<5010002)||	//TMA V5.1.0.2
		(doc_type==2&&version<2000500)||	//LMA V2.0.5.0
		(doc_type==4&&version<1030500)||	//LDS V1.3.5.0
		(doc_type==5&&version<1030500)))	//TDA V1.3.5.0
		m_biWingOffsetStyle=4;
	buffer.ReadByte(&direction);
	buffer.ReadByte(&offset_wing);
	buffer.ReadString(norm_offset.key_str,51);
	buffer.ReadDouble(&offset_angle);
	buffer.ReadDouble(&R);
}

void DESIGN_LS_POS::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	datumPoint.ToBuffer(buffer,version,doc_type);
	if (version==0||(
		(doc_type==1&&version>=5020200)||	//TMA V5,2,2,0
		(doc_type==2&&version>=2010200)||	//LMA V2,1,2,0
		(doc_type==3&&version>=2010401)||	//TSA V2,1,4,1
		(doc_type==4&&version>=1031000)||	//LDS V1,3,10,0
		(doc_type==5&&version>=1031000)))	//TDA V1,3,10,0
		buffer.WriteBooleanThin(cLocationStyle!=0);
	else
		buffer.WriteBoolean(cLocationStyle!=0);
	buffer<<hPart;
	buffer<<len_offset_dist;
	buffer<<wing_offset_dist;
	if(  version==0||
		(doc_type==1&&version>=5000301)||	//TMA V5.0.3.1
		(doc_type==2&&version>=2000301)||	//LMA V2.0.3.1
		(doc_type==3&&version>=2010301)||	//TSA V2.1.3.1
		(doc_type==4&&version>=1030301)||	//LDS V1.3.3.1
		(doc_type==5&&version>=1030301))	//TDA V1.3.3.1
		buffer.WriteByte(m_biWingOffsetStyle);
	buffer.WriteByte(direction);
	buffer.WriteByte(offset_wing);
	buffer.WriteString(norm_offset.key_str);
	buffer.WriteDouble(offset_angle);
	buffer.WriteDouble(R);
}

void DESIGN_LS_POS::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="";
	fprintf(fp,"<��˨λ����Ʋ���>\n");
	datumPoint.ToXmlFile(fp,schema);
	if (cLocationStyle)	//true��������Ϊ��׼�������������,false��������Ϊ��������
		fprintf(fp,"<��������Ϊ��׼������������� bDatumPointInUcs=\"true\"/>\n");
	else
		fprintf(fp,"<��������Ϊ��׼������������� bDatumPointInUcs=\"false\"/>\n");
	fprintf(fp,"<��λ��׼������� hPart=\"OX%X\"/>\n",hPart);
	sprintf(sText,"%f",len_offset_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<����ƫ��ֵ len_offset_dist=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wing_offset_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<֫��ƫ��ֵ wing_offset_dist=\"%s\"/>\n",sText);
	if (direction==0)
		fprintf(fp,"<��׼�Ǹַ��� direction=\"0.ʼ->��\"/>\n");
	else if (direction==1)					  
		fprintf(fp,"<��׼�Ǹַ��� direction=\"1.��->ʼ\"/>\n");
	if (offset_wing==0)
		fprintf(fp,"<ƫ��֫ offset_wing=\"0.X֫\"/>\n");
	else if (offset_wing==1)			  
		fprintf(fp,"<ƫ��֫ offset_wing=\"1.Y֫\"/>\n");
	fprintf(fp,"<����ƫ�����ַ��� norm_offset.key_str=\"%s\"/>\n",(char*)norm_offset.key_str);
	sprintf(sText,"%f",offset_angle);
	SimplifiedNumString(sText);
	fprintf(fp,"<��˨���ڻ�׼�ֹܵ�ƫ�ƽǶ� offset_angle=\"%s\"/>\n",sText);
	sprintf(sText,"%f",R);
	SimplifiedNumString(sText);
	fprintf(fp,"<��˨����Բ�İ뾶 R=\"%s\"/>\n",sText);
	fprintf(fp,"</��˨λ����Ʋ���>\n");
}

CDesignProfileVertex::CDesignProfileVertex()
{
	hJg=0;					//�Ǹ־��
	hNode=0;				//��׼�ڵ���
	g=ber_space=edge_space=0;//�ľ�;����������һ���ƫ����;������֫��һ���ƫ����
	direction=0;			//0.��׼�Ǹַ���:ʼ->��;1.��->ʼ
	offset_wing=0;			//0.X֫;1.Y֫
	len_offset_dist=0;		//����ƫ��ֵ
	face_i=1;				//1��ʾ��׼��,2��ʾ��һ������,3��ʾ�ڶ�������
}
BOOL CDesignProfileVertex::UpdateVertex(IModel* pTower)
{
	CLDSLineAngle *pCommJg=(CLDSLineAngle*)pTower->FromPartHandle(hJg,CLS_LINEANGLE);
	CLDSNode *pNode=pTower->FromNodeHandle(hNode);
	f3dPoint len_vec,base_pos;
	if(pCommJg==NULL||pNode==NULL)
		return FALSE;
	if(direction==0)
		len_vec=pCommJg->End()-pCommJg->Start();
	else
		len_vec=pCommJg->Start()-pCommJg->End();
	normalize(len_vec);
	if(pCommJg->pStart->handle==hNode)
		base_pos=pCommJg->Start();
	else if(pCommJg->pEnd->handle==hNode)
		base_pos=pCommJg->End();
	else
		base_pos=pCommJg->GetDatumPosBer(pNode);
	if(offset_wing==0)	//X֫
	{
		edge_pos=base_pos+len_vec*len_offset_dist+pCommJg->GetWingVecX()*(g+edge_space);
		ber_pos=base_pos+len_vec*len_offset_dist+pCommJg->GetWingVecX()*(g-ber_space);
	}
	else				//Y֫
	{
		edge_pos=base_pos+len_vec*len_offset_dist+pCommJg->GetWingVecY()*(g+edge_space);
		ber_pos=base_pos+len_vec*len_offset_dist+pCommJg->GetWingVecY()*(g-ber_space);
	}
	return TRUE;
}

CLDSSphere::CLDSSphere()
{
	D=d=0.0;
	//strcpy(m_sClassName,"CLDSSphere");
	m_nClassTypeId=CLS_SPHERE;
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN��������Ա���;
	crMaterial=GetColor(CLS_PLATE,-1,'S');
}
int CLDSSphere::GetSpecification(char *spec,BOOL bIncMat/*=TRUE*/)
{
	if(spec!=NULL)
	{
		sprintf(spec,"��%sX%s",(char*)CXhChar16(D),(char*)CXhChar16(thick));
		return strlen(spec);
	}
	return 0;
}
CLDSPart *CLDSSphere::GetMirPart(MIRMSG mirmsg,const double *pPickPos/*=NULL*/,BOOL bCheckProfile/*=TRUE*/)
{
	for(RELATIVE_OBJECT *pItem=relativeObjs.GetFirst();pItem;pItem=relativeObjs.GetNext())
	{	//���ڹ��������б��в���
		CLDSSphere *pSphere=NULL;
		if(pItem->mirInfo.IsSameMirMsg(mirmsg))
		{
			pSphere=(CLDSSphere*)BelongModel()->FromPartHandle(pItem->hObj,CLS_SPHERE);
			if(pSphere&&ModuleTogetherWith(pSphere))
				return pSphere;
			else
				break;
		}
	}
	BOOL pushed=m_pModel->PushPartStack(); //���������б�ǰӦ��ѹջ wht 12-03-05
	f3dPoint mir_origin=GetMirPos(ucs.origin,mirmsg);
	for(CLDSPart *pMirPart=m_pModel->EnumPartFirst(CLS_SPHERE);pMirPart;pMirPart=m_pModel->EnumPartNext(CLS_SPHERE))
	{
		CLDSSphere *pMirSphere=(CLDSSphere*)pMirPart;
		if(pMirSphere->cMaterial!=cMaterial)
			continue;
		if(pMirSphere->d!=d||pMirSphere->D!=D)
			continue;
		if(pMirSphere->ucs.origin==mir_origin)
		{
			m_pModel->PopPartStack(pushed);
			return pMirSphere;
		}
	}
	m_pModel->PopPartStack(pushed);
	return NULL;
}
CSolidPart CLDSSphere::GetSolidPartObject()
{
	CSolidPart solidPart;
	solidPart.m_hPart = handle;
	solidPart.m_idClassType = GetClassTypeId();
	solidPart.acs = ucs;
	solidPart.m_iLineType = 0;
	solidPart.pBody = pSolidBody;
	solidPart.dfSphereRadius=D/2.0;
	return solidPart;
}

void CLDSSphere::CopyProperty(CLDSDbObject *pObj)
{
	if(pObj->GetClassTypeId()!=CLS_SPHERE)
		return;
	CLDSSphere *pSphere=(CLDSSphere*)pObj;
	D=pSphere->D;
	d=pSphere->d;
	cMaterial=pSphere->cMaterial;
	cfgword=pSphere->cfgword;
	iSeg=pSphere->iSeg;
	m_uStatMatCoef=pSphere->m_uStatMatCoef;
	m_bDisplayHole=pSphere->m_bDisplayHole;
	strcpy(sPartNo,pSphere->sPartNo);
	strcpy(sNotes,pSphere->sNotes);
	ucs=pSphere->ucs;
}
BOOL CLDSSphere::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	if(CreateShadowPart3dSolidModel(bDisplayHole,scale_of_user2scr,sample_len,smoothness))
		return TRUE;

	if(pSolidBody!=NULL&& !IsModified())
		return TRUE;
	else
	{
		if(pSolidBody == NULL)
			pSolidBody = new CSolidBody;
		else
			pSolidBody->Empty();
	}
	SetModified(FALSE);
	fBody body;
	fBody *pBody=&body;
	int i,j;
	COLORREF plank_color=GetColor(CLS_PLATE,-1,cMaterial);	//Ĭ�ϰ���ɫ
	double radius = D*0.5;
	int uQuadSlices = CalArcResolution(radius,0.5*Pi,scale_of_user2scr,sample_len,smoothness);
	uQuadSlices=max(uQuadSlices,3);
	uQuadSlices=min(uQuadSlices,18);
	int uSlices=4*uQuadSlices;
	double piece_angle=0.5*Pi/uQuadSlices;
	double *cosa_arr=new double[uSlices];
	double *sina_arr=new double[uSlices];
	cosa_arr[0]=sina_arr[uQuadSlices]=1.0;
	cosa_arr[uQuadSlices]=sina_arr[0]=0.0;
	for(i=1;i<uQuadSlices;i++)
	{
		cosa_arr[i]=cos(piece_angle*i);
		sina_arr[i]=sin(piece_angle*i);
	}
	for(i=0;i<=uQuadSlices;i++)
	{
		cosa_arr[  uQuadSlices+i]=-cosa_arr[-i+uQuadSlices];
		sina_arr[  uQuadSlices+i]= sina_arr[-i+uQuadSlices];
		cosa_arr[2*uQuadSlices+i]=-cosa_arr[ i];
		sina_arr[2*uQuadSlices+i]=-sina_arr[ i];
		if(i<uQuadSlices)
		{
			cosa_arr[3*uQuadSlices+i]= cosa_arr[-i+uQuadSlices];
			sina_arr[3*uQuadSlices+i]=-sina_arr[-i+uQuadSlices];
		}
	}
	f3dPoint vertice;
	vertice=ucs.origin+ucs.axis_z*radius;
	pBody->vertex.append(vertice);	//������
	for(i=uQuadSlices-1;i>-uQuadSlices;i--)	//γ��
	{
		for(j=0;j<uSlices;j++)
		{
			int index=i;
			if(i<0)
				index=-i;
			double latitude_radius=cosa_arr[index]*radius;
			double z_level=sina_arr[index]*radius;
			if(i<0)
				z_level*=-1.0;
			vertice.Set(latitude_radius*cosa_arr[j],latitude_radius*sina_arr[j],z_level);
			coord_trans(vertice,ucs,TRUE,TRUE);
			pBody->vertex.append(vertice);	//������
		}
	}
	vertice=ucs.origin-ucs.axis_z*radius;
	pBody->vertex.append(vertice);	//�׼���
	int num=pBody->vertex.GetNodeNum();
	delete[]cosa_arr;
	delete[]sina_arr;
	//��ʼ������
	CCreateFace createFace;
	createFace.InitVertexList(pBody);

	f3dPolyFace *pFace=NULL;
	for(i=1-uQuadSlices;i<=uQuadSlices;i++)
	{
		int base_index=(i+uQuadSlices-1)*uSlices+1;
		if(i==1-uQuadSlices)	//������
		{
			for(j=0;j<uSlices;j++)
			{
				pFace=pBody->faceList.append();
				pFace->material=plank_color;
				createFace.NewOutterEdgeLine(pFace,j+base_index,0);
				createFace.NewOutterEdgeLine(pFace,(j+1)%uSlices+base_index);
				createFace.NewOutterEdgeLine(pFace,0);
			}
		}
		else if(i<uQuadSlices)
		{
			for(j=0;j<uSlices;j++)
			{
				pFace=pBody->faceList.append();
				pFace->material=plank_color;
				createFace.NewOutterEdgeLine(pFace,base_index+(j+1)%uSlices,base_index+j);
				createFace.NewOutterEdgeLine(pFace,base_index+(j+1)%uSlices-uSlices);
				createFace.NewOutterEdgeLine(pFace,base_index+j);
				pFace=pBody->faceList.append();
				pFace->material=plank_color;
				createFace.NewOutterEdgeLine(pFace,base_index+j-uSlices,base_index+(j+1)%uSlices-uSlices);
				createFace.NewOutterEdgeLine(pFace,base_index+j);
				createFace.NewOutterEdgeLine(pFace,base_index+(j+1)%uSlices-uSlices);
			}
		}
		else
		{	//�׼���
			for(j=0;j<uSlices;j++)
			{
				pFace=pBody->faceList.append();
				pFace->material=plank_color;
				createFace.NewOutterEdgeLine(pFace,base_index+j-uSlices,base_index+(j+1)%uSlices-uSlices);
				createFace.NewOutterEdgeLine(pFace,num-1);
				createFace.NewOutterEdgeLine(pFace,base_index+(j+1)%uSlices-uSlices);
			}
		}
	}
	pSolidBody->ConvertFrom(pBody);
	return TRUE;
}
//���Թ���
int CLDSSphere::GetPropValueStr(long id,char* valueStr,UINT nMaxStrBufLen/*=100*/)
{
	char sText[100]="";
	if(GetPropID("basicInfo")==0)
		strcpy(sText,"");
	else if(GetPropID("handle")==id)
		sprintf(sText,"0X%X",handle);
	else if(GetPropID("layer")==id)
		strcpy(sText,layer());
	else if(GetPropID("cMaterial")==id)	
	{
		char matStr[20];
		if(QuerySteelMatMark(cMaterial,matStr))
			sprintf(sText,"%s",matStr);
	}
	else if(GetPropID("m_cQualityLevel")==id)
	{
		if(isalpha(m_cQualityLevel))
			sprintf(sText,"%C",m_cQualityLevel);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Default");
#else
			strcpy(sText,"Ĭ��");
#endif
	}
	else if(GetPropID("specification")==id)
	{	
		char s_D[200],s_thick[200];
		sprintf(s_D,"%f",D);
		sprintf(s_thick,"%f",0.5*(D-d));
		SimplifiedNumString(s_D);
		SimplifiedNumString(s_thick);
		sprintf(sText,"%sX%s", s_D,s_thick);
	}
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("sPartNo")==id)
		sprintf(sText,"%s",(char*)GetPartNo());
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(cfgword,valueStr);
	{
		if(layer(0)=='L')	//���ȹ���
			return cfgword.GetLegScopeStr(valueStr,50,true);
		else
			return cfgword.GetBodyScopeStr(valueStr);
	}
	else if(GetPropID("m_uStatMatCoef")==id)
		sprintf(sText,"%d",m_uStatMatCoef);
	else if(GetPropID("m_hPartWeldParent")==id)
		sprintf(sText,"0X%X",m_hPartWeldParent);
	else if(GetPropID("m_bDisplayHole")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bDisplayHole)
			strcpy(sText,"Yes");
		else
			strcpy(sText,"No");
#else 
		if(m_bDisplayHole)
			strcpy(sText,"��");
		else
			strcpy(sText,"��");
#endif
	}
	else if(GetPropID("D")==id)
	{
		sprintf(sText,"%f",D);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("d")==id)
	{
		sprintf(sText,"%f",d);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_bUserSpecColor")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(m_bUserSpecColor)
			sprintf(sText,"Yes");
		else
			sprintf(sText,"No");
#else 
		if(m_bUserSpecColor)
			sprintf(sText,"��");
		else
			sprintf(sText,"��");
#endif
	}
	else if(GetPropID("crMaterial")==id)
		sprintf(sText,"RGB%X",crMaterial);
	else if(GetPropID("sNotes")==id)
		strcpy(sText,sNotes);
	else if(GetPropID("hDatumNode")==id)
		sprintf(sText,"0X%X",hDatumNode);
	else if(GetPropID("ucs.origin.x")==id)
	{
		sprintf(sText,"%f",ucs.origin.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.y")==id)
	{
		sprintf(sText,"%f",ucs.origin.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.origin.z")==id)
	{
		sprintf(sText,"%f",ucs.origin.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_x.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_x.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_y.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_y.z);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.x")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.x);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.y")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.y);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("ucs.axis_z.z")==id)
	{
		sprintf(sText,"%f",ucs.axis_z.z);
		SimplifiedNumString(sText);
	}
	if(valueStr)
		StrCopy(valueStr,sText,nMaxStrBufLen);
	return strlen(sText);
}
long CLDSSphere::GetPropID(char* propStr)
{
	PROPLIST_ITEM *pItem =CLDSSphere::propHashtable.GetValue(propStr);
	if(pItem!=NULL)
		return pItem->id;
	else
	{
		/*CString prompt="û�ҵ���Ϊ\"";
		prompt+=propStr;
		prompt+="\"������ID��";
		AfxMessageBox(prompt);*/
		return 0;
	}
}

int CLDSSphere::GetPropStr(long id,char* propStr)
{
	char sText[100]="";
	if(GetPropID("basicInfo")==0)
		strcpy(sText,"basicInfo");
	else if(GetPropID("handle")==id)
		strcpy(sText,"handle");
	else if(GetPropID("layer")==id)
		strcpy(sText,"layer");
	else if(GetPropID("cMaterial")==id)
		strcpy(sText,"cMaterial");
	else if(GetPropID("m_cQualityLevel")==id)
		strcpy(sText,"m_cQualityLevel");
	else if(GetPropID("specification")==id)
		strcpy(sText,"specification");
	else if(GetPropID("iSeg")==id)
		strcpy(sText,"iSeg");
	else if(GetPropID("sPartNo")==id)
		strcpy(sText,"sPartNo");
	else if(GetPropID("cfgword")==id)
		strcpy(sText,"cfgword");
	else if(GetPropID("m_uStatMatCoef")==id)
		strcpy(sText,"m_uStatMatCoef");
	else if(GetPropID("m_hPartWeldParent")==id)
		strcpy(sText,"m_hPartWeldParent");
	else if(GetPropID("m_bDisplayHole")==id)
		strcpy(sText,"m_bDisplayHole");
	else if(GetPropID("sNotes")==id)
		strcpy(sText,"sNotes");
	else if(GetPropID("assemblePos")==id)
		strcpy(sText,"assemblePos");
	else if(GetPropID("ucs.origin")==id)
		strcpy(sText,"ucs.origin");
	else if(GetPropID("hDatumNode")==id)
		strcpy(sText,"hDatumNode");
	else if(GetPropID("ucs.origin.x")==id)
		strcpy(sText,"ucs.origin.x");
	else if(GetPropID("ucs.origin.y")==id)
		strcpy(sText,"ucs.origin.y");
	else if(GetPropID("ucs.origin.z")==id)
		strcpy(sText,"ucs.origin.z");
	else if(GetPropID("ucs.axis_x")==id)
		strcpy(sText,"ucs.axis_x");
	else if(GetPropID("ucs.axis_x.x")==id)
		strcpy(sText,"ucs.axis_x.x");
	else if(GetPropID("ucs.axis_x.y")==id)
		strcpy(sText,"ucs.axis_x.y");
	else if(GetPropID("ucs.axis_x.z")==id)
		strcpy(sText,"ucs.axis_x.z");
	else if(GetPropID("ucs.axis_y")==id)
		strcpy(sText,"ucs.axis_y");
	else if(GetPropID("ucs.axis_y.x")==id)
		strcpy(sText,"ucs.axis_y.x");
	else if(GetPropID("ucs.axis_y.y")==id)
		strcpy(sText,"ucs.axis_y.y");
	else if(GetPropID("ucs.axis_y.z")==id)
		strcpy(sText,"ucs.axis_y.z");
	else if(GetPropID("ucs.axis_z")==id)
		strcpy(sText,"ucs.axis_z");
	else if(GetPropID("ucs.axis_z.x")==id)
		strcpy(sText,"ucs.axis_z.x");
	else if(GetPropID("ucs.axis_z.y")==id)
		strcpy(sText,"ucs.axis_z.y");
	else if(GetPropID("ucs.axis_z.z")==id)
		strcpy(sText,"ucs.axis_z.z");
	else if(GetPropID("sphereLinkPoles")==id)
		strcpy(sText,"sphereLinkPoles");
	if(propStr)
		strcpy(propStr,sText);
	return strlen(sText);
}

BOOL CLDSSphere::IsPropEqual(CHashSet<CLDSObject*> &selectObjs, long idProp)
{
	if(selectObjs.GetNodeNum()<=1)
		return TRUE;
	char valueStr[500]="";
	selectObjs.push_stack();
	CLDSSphere *pSphere=NULL, *pFirstSphere = (CLDSSphere*)selectObjs.GetFirst();
	pFirstSphere->GetPropValueStr(idProp,valueStr);
	for(pSphere=(CLDSSphere*)selectObjs.GetNext();pSphere;pSphere=(CLDSSphere*)selectObjs.GetNext())
	{
		char valueStr2[500]="";
		pSphere->GetPropValueStr(idProp,valueStr2);
		if(stricmp(valueStr,valueStr2)!=0)
		{
			selectObjs.pop_stack();
			return FALSE;
		}
	}
	selectObjs.pop_stack();
	return TRUE;
}

int CLDSSphere::GetPropStatus(long id)
{
	DWORD dwStatus;
	if(CLDSSphere::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//������
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//����
	}
	return -1;	//δ�ҵ������״̬
}

BOOL CLDSSphere::SetPropStatus(long id, BOOL bHide)
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSSphere::propStatusHashtable.SetValueAt(id,dwStatus);
}

void CLDSSphere::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//����Ӱ�乹�������͹���������� wht 12-09-14
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
		return;
	buffer.WriteDword(dwPermission);
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	buffer.WriteString(sPartNo);
	buffer.WriteDword(m_uStatMatCoef);
	buffer.WriteInteger(m_hPartWeldParent);
	buffer.WriteString(layer());
	buffer.WriteInteger(iSeg);
	buffer.WriteByte(cMaterial);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.WriteByte(m_cQualityLevel);
	buffer.WriteBooleanThin(m_bDisplayHole);
	buffer.WriteDouble(D);
	buffer.WriteDouble(d);
	buffer.WriteInteger(hDatumNode); //buffer.WritePoint(ucs.origin);
	buffer.WritePoint(ucs.axis_x);
	buffer.WritePoint(ucs.axis_y);
	buffer.WritePoint(ucs.axis_z);
	buffer.WriteString(sNotes);
}
void CLDSSphere::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<�� handle=\"0X%X\">\n",handle);
	fprintf(fp,"<Ȩ���� dwPermission=\"%d\"/>\n",dwPermission);
	fprintf(fp,"<��ĺ� cfgword.word=\"%d,%d,%d,%d,%d\"/>\n",cfgword.flag.word[0],cfgword.flag.word[1],cfgword.flag.word[2],cfgword.flag.word[3],cfgword.flag.word[4]);
	fprintf(fp,"<������� sPartNo=\"%s\"/>\n",(char*)sPartNo);
	fprintf(fp,"<ͳ��ϵ�� m_uStatMatCoef=\"%d\"/>\n",m_uStatMatCoef);
	fprintf(fp,"<���Ӹ����� m_hPartWeldParent=\"0X%X\"/>\n",m_hPartWeldParent);
	fprintf(fp,"<ͼ�� layer=\"%s\"/>\n",layer());
	fprintf(fp,"<�ֶκ� iSeg=\"%d\"/>\n",iSeg);
	fprintf(fp,"<���� cMaterial=\"%c\"/>\n",cMaterial);
	if (m_bDisplayHole)
		fprintf(fp,"<��ʾ�׼��и��� m_bDisplayHole=\"true\"/>\n");
	else
		fprintf(fp,"<��ʾ�׼��и��� m_bDisplayHole=\"false\"/>\n");
	sprintf(sText,"%f",D);
	SimplifiedNumString(sText);
	fprintf(fp,"<����ֱ�� D=\"%s\"/>\n",sText);
	sprintf(sText,"%f",d);
	SimplifiedNumString(sText);
	fprintf(fp,"<����ֱ�� d=\"%s\"/>\n",sText);
	fprintf(fp,"<��׼�ڵ��� hDatumNode=\"0X%X\"/>\n",hDatumNode);
	sprintf(sText,"%f",ucs.axis_x.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.axis_x.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.axis_x.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<X�� ucs.axis_x=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",ucs.axis_y.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.axis_y.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.axis_y.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<Y�� ucs.axis_y=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",ucs.axis_z.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.axis_z.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.axis_z.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<Z�� ucs.axis_z=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	fprintf(fp,"<��ע sNotes=\"%s\"/>\n",sNotes);
	fprintf(fp,"</��>\n");
}
void CLDSSphere::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	//��ȡӰ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
		return;
	DWORD dw;
	bool bPrevDeflateVersion=false;	//�ļ�����֮ǰ�İ汾
/*#if defined(__TMA_FILE_)||defined(__TMA_)
	if(version>0&&version<4000027)//version!=NULL&&compareVersion(version,"4.0.0.27")<0)
#elif defined(__LMA_FILE_)||defined(__LMA_)
	if(version>0&&version<1000082)//version!=NULL&&compareVersion(version,"1.0.0.82")<0)
#else
	if(version>0&&version<1000009)//version!=NULL&&compareVersion(version,"1.0.0.09")<0)
#endif*/
	if( version>0&&((doc_type==1&&version<4000027)||
		(doc_type==2&&version<1000082)||(doc_type==4&&version<1000009)))
		bPrevDeflateVersion=true;
	buffer.ReadDword(&dwPermission);
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	buffer.ReadString(sPartNo);
	buffer.ReadDword(&m_uStatMatCoef);
	buffer.ReadInteger(&_hPartWeldParent);
	buffer.ReadString(layer(),4);
	buffer.ReadInteger(&_iSeg.iSeg);
	buffer.ReadByte(&_material);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.ReadByte(&_cQualityLevel);
	if(bPrevDeflateVersion)
	{
		buffer.ReadInteger(&dw);//��ʾ��˨�׼��и���
		m_bDisplayHole=(dw!=0);
	}
	else
 		buffer.ReadBooleanThin(&m_bDisplayHole);
	buffer.ReadDouble(&D);
	buffer.ReadDouble(&d);
	thick=0.5*(D-d);
	buffer.ReadInteger(&hDatumNode);
	CLDSNode *pDatumNode=BelongModel()->FromNodeHandle(hDatumNode);
	if(pDatumNode)
		ucs.origin=pDatumNode->Position(true);
	buffer.ReadPoint(ucs.axis_x);
	buffer.ReadPoint(ucs.axis_y);
	buffer.ReadPoint(ucs.axis_z);
	buffer.ReadString(sNotes,51);
}

bool CLDSSphere::IsEqual(CLDSSphere* pMirSphere,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
						   int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{
	if(d-pMirSphere->d!=0)
	{
		if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(error_info,"{%s}Inner radius is different",(char*)sPartNo);
#else 
			sprintf(error_info,"{%s}�ھ���ͬ",(char*)sPartNo);
#endif
		return false;
	}
	else if(D-pMirSphere->D!=0)
	{
		if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(error_info,"{%s}Outer radius is different",(char*)sPartNo);
#else 
			sprintf(error_info,"{%s}�⾶��ͬ",(char*)sPartNo);
#endif
		return false;
	}
	else if(cMaterial!=pMirSphere->cMaterial)
	{
		if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(error_info,"{%s}Material is different",(char*)sPartNo);
#else 
			sprintf(error_info,"{%s}���ʲ�ͬ",(char*)sPartNo);
#endif
		return false;
	}
	return true;
}	
//////////////////////////////////////////////////////////////////////////
// BOLTHOLE_INFO
BOLTHOLE_INFO::BOLTHOLE_INFO()
{
	pBolt=NULL;
	cFuncType=0;
	dwRayNo=0;
	hole_d=bolt_d=0;
	waistLen=0;
	hole_d_increment=1.5;
}
BOLTHOLE_INFO::BOLTHOLE_INFO(const COORD3D& _center,CLsRef* pLsRef/*=NULL*/)
{
	this->centre=_center;
	if(pLsRef)
	{
		this->pBolt=pLsRef->pBolt;
		bolt_d=(*pLsRef)->get_d();
		cFuncType=(*pLsRef)->FuncType();
		hole_d=bolt_d+(*pLsRef)->hole_d_increment;
		waistLen=pLsRef->waistLen;
		hole_d_increment=(float)(*pLsRef)->hole_d_increment;
	}
}
BOLTHOLE_INFO::BOLTHOLE_INFO(const COORD3D& _center, short waistLen /*= 0*/, CLDSBolt* pBolt/*=NULL*/)
{
	this->centre=_center;
	this->waistLen= waistLen;
	if(pBolt)
	{
		this->pBolt= pBolt;
		bolt_d= pBolt->get_d();
		cFuncType= pBolt->FuncType();
		hole_d=bolt_d+ pBolt->hole_d_increment;
		hole_d_increment=(float)pBolt->hole_d_increment;
	}
}
long BOLTHOLE_INFO::BoltHandle() const
{
	if(pBolt)
		return pBolt->handle;
	else
		return 0;
}
void BOLTHOLE_INFO::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if( version==0||
		(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
	{
		buffer.ReadDouble(&hole_d);
		buffer.ReadPoint(centre);
		buffer.ReadWord(&waistLen);
		if(waistLen>0)
			buffer.ReadPoint(axis_x);
		buffer.ReadByte(&cFuncType);
	}
}
void BOLTHOLE_INFO::ToBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if( version==0||
		(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030400))	//TDA(V1,3,4,0)
	{
		buffer.WriteDouble(hole_d);
		buffer.WritePoint(centre);
		buffer.WriteWord(waistLen);
		if(waistLen>0)
			buffer.WritePoint(axis_x);
		buffer.WriteByte(cFuncType);
	}
}
//////////////////////////////////////////////////////////////////////////
// CLsRef
CLsRef::CLsRef()
{
	hAgentBlockRef=feature=0;
	pLs=NULL;
	start1_end2_mid0=0;
	dwRayNo=0;
	waistLen=0;
	cFlag=CLsRef::PUNCH_HOLE;
}
void CLsRef::FromBuffer(CBuffer &buffer,IModel* pTower,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	bool bPrevDeflateVersion=false;	//�ļ�����֮ǰ�İ汾
	if( version>0&&((doc_type==1&&version<4000027)||(doc_type==2&&version<1000082)||(doc_type==4&&version<1000009)))
		bPrevDeflateVersion=true;
	int ls_handle;
	buffer.ReadInteger(&ls_handle);
	pLs = (CLDSBolt*)pTower->FromPartHandle(ls_handle,CLS_BOLT);
	buffer.ReadDword(&dwRayNo);
	buffer.ReadByte(&start1_end2_mid0);
	buffer.ReadByte(&cFlag);
	if(bPrevDeflateVersion)
	{
		double ff;
		buffer.ReadDouble(&ff);
		waistLen=(short)ff;
	}
	else
		buffer.ReadWord(&waistLen);
}
void CLsRef::ToBuffer(CBuffer &buffer)
{
	if(pLs)
		buffer.WriteInteger(pLs->handle);
	else
		buffer.WriteInteger(0);
	buffer.WriteDword(dwRayNo);
	buffer.WriteByte(start1_end2_mid0);
	buffer.WriteByte(cFlag);
	buffer.WriteWord(waistLen);
}
void CLsRef::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="";
	fprintf(fp,"<��˨����>\n");
	if(pLs)
		fprintf(fp,"<��� handle=\"0X%X\"/>\n",pLs->handle);
	fprintf(fp,"<���ڵ����������� dwRayNo=\"%d\"/>\n",dwRayNo);
	if (start1_end2_mid0==0)
		fprintf(fp,"<��˨λ�� start1_end2_mid0=\"0.�Ǹ��м�������˼���\"/>\n");
	else if (start1_end2_mid0==1)
		fprintf(fp,"<��˨λ�� start1_end2_mid0=\"1.�Ǹ�ʼ��\"/>\n");
	else if (start1_end2_mid0==2)
		fprintf(fp,"<��˨λ�� start1_end2_mid0=\"2.�Ǹ��ն�\"/>\n");
	fprintf(fp,"<��Բ������ waistLen=\"%d\"/>\n",waistLen);
	fprintf(fp,"</��˨����>\n");
}
//������
CLDSWeldLine::CLDSWeldLine()
{
	weld_type = 0;	//���ֺ�
	weld_wide=weld_high = 12;
	thick=10;
}
double CLDSWeldLine::Volume()
{
	double len=weldline.Length();
	if(weld_type==0)		//���ֺ�
		return len*weld_wide*weld_high;
	else if(weld_type==1)	//�
		return 0.5*len*weld_wide*weld_high;
	else					//�Ժ�
		return 2*len*weld_wide*weld_high;
}
// ����
CLDSCube::CLDSCube(void)
{
	b = 40;
	cMaterial = 'S';
}

// Բ��
CLDSCylinder::CLDSCylinder(void)
{
	d = 40;
	cMaterial = 'S';
}
#endif
void CLDSLinePart::ClearFlag()
{
	UnlockStart();
	UnlockEnd();
}

CLDSLinePart* CLDSLinePart::MotherLinePart()
{
	CLDSObject *pMotherObj=MotherObject();
	if(pMotherObj==NULL||pMotherObj->GetClassTypeId()!=m_nClassTypeId)
#ifdef AFX_TARG_ENU_ENGLISH
		throw "The part type of projective mother part is wrong��";
#else 
		throw "Ӱ�乹��ĸ���������ʹ���";
#endif
	else 
		return (CLDSLinePart*)pMotherObj;
}
bool CLDSLinePart::ShadowDesign()
{
#ifdef __COMMON_PART_INC_
	if(!IsShadowObject())
		return false;
	CLDSLinePart *pMotherRod = (CLDSLinePart*)MotherObject(false);
	size=pMotherRod->size;
	cMaterial=pMotherRod->cMaterial;
	connectStart=pMotherRod->connectStart;
	connectEnd=pMotherRod->connectEnd;
	connectMid=pMotherRod->connectMid;
	start_oddment=pMotherRod->startOdd();
	end_oddment=pMotherRod->endOdd();
	//
	CalPosition();
#endif
	return true;
}
bool CLDSLinePart::IsBelongTo(int iClassTypeId)const
{
	if(iClassTypeId==m_nClassTypeId||iClassTypeId==CLS_PART||iClassTypeId==CLS_DBOBJECT)
		return true;
	else
		return false;
}
int CLDSLinePart::StartLinkNodeType()
{
	if(pStart)
		return LINKNODE_ACTUAL;
	else if(IsShadowObject())
	{
		CLDSLinePart* pMotherRod=(CLDSLinePart*)MotherPart();
		if(pMotherRod->pStart)
			return LINKNODE_SHADOW;
	}
	return LINKNODE_NULL;
}
int CLDSLinePart::EndLinkNodeType()
{
	if(pEnd)
		return LINKNODE_ACTUAL;
	else if(IsShadowObject())
	{
		CLDSLinePart* pMotherRod=(CLDSLinePart*)MotherPart();
		if(pMotherRod->pEnd)
			return LINKNODE_SHADOW;
	}
	return LINKNODE_NULL;
}
f3dPoint CLDSLinePart::GetDatumPos(CLDSNode* pNode)
{
	f3dPoint datum_point;
	if(pNode==pStart)
	{	//ʼ�˽ڵ�
		return Start();
	}
	else if(pNode==pEnd)
	{	//�ն˽ڵ�
		return End();
	}
	else
	{	
		f3dPoint perp;
		SnapPerp(&perp,Start(),End(),pNode->Position(true));
		return perp;
		//�м�ڵ��г�����㣬��������ڵ㸸�Ǹ־�ӦΪ��ǰ�Ǹ�
		//return f3dPoint(0,0,0);
	}
}

f3dPoint CLDSLinePart::StartLinkNodePos(bool bArcLiftPos/*=true*/)
{
	if(pStart)
		return pStart->Position(bArcLiftPos);
	else if(IsShadowObject())
	{
		CLDSLinePart* pMotherRod=(CLDSLinePart*)MotherPart(false);
		if(m_iShadowObjType==SHADOW_ISOLATED)
		{
			CParaCS *pAcs=BelongModel()->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
			pAcs->SetModifiedState();
			pAcs->UpdateACS(pMotherRod);
			f3dPoint pos=pMotherRod->StartLinkNodePos(bArcLiftPos);
			return pAcs->CoordTrans(pos);
		}
		else if(m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
		{
			CBlockParameter blockPara;
			if(!GetBlockParameter(blockPara))
				return f3dPoint(0,0,0);
			UCS_STRU cs=blockPara.m_pBlock->TransToACS(blockPara.m_pBlockRef->GetACS());
			f3dPoint pos=pMotherRod->StartLinkNodePos(bArcLiftPos);
			coord_trans(pos,cs,TRUE);
			return pos;
		}
	}
	return f3dPoint(0,0,0);
}
f3dPoint CLDSLinePart::EndLinkNodePos(bool bArcLiftPos/*=true*/)
{
	if(pEnd)
		return pEnd->Position(bArcLiftPos);
	else if(IsShadowObject())
	{
		CLDSLinePart* pMotherRod=(CLDSLinePart*)MotherPart(false);
		if(m_iShadowObjType==SHADOW_ISOLATED)
		{
			CParaCS *pAcs=BelongModel()->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
			pAcs->SetModifiedState();
			pAcs->UpdateACS(pMotherRod);
			f3dPoint pos=pMotherRod->EndLinkNodePos(bArcLiftPos);
			return pAcs->CoordTrans(pos);
		}
		else if(m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
		{
			CBlockParameter blockPara;
			if(!GetBlockParameter(blockPara))
				return f3dPoint(0,0,0);
			UCS_STRU cs=blockPara.m_pBlock->TransToACS(blockPara.m_pBlockRef->GetACS());
			f3dPoint pos=pMotherRod->EndLinkNodePos(bArcLiftPos);
			coord_trans(pos,cs,TRUE);
			return pos;
		}
	}
	return f3dPoint(0,0,0);
}
bool CLDSLinePart::CalShadowRodPos()//BOOL bStart,CLDSArcLift *pLift)
{
	if(!IsShadowObject())
		return false;
	//Ӱ��Ǹ�
	CLDSLinePart *pMotherRod=MotherLinePart();
	if(m_iShadowObjType==SHADOW_ISOLATED)
	{
		pMotherRod->CalPosition();
		CParaCS *pAcs=BelongModel()->FromAcsHandle(shadow.COMMON_OBJ.m_hAcs);
		if(pAcs&&((pAcs->GetDefStyle()==CParaCS::CS_PARAM&&pAcs->UpdateACS(m_pModel))||
			(pAcs->GetDefStyle()!=CParaCS::CS_PARAM&&pAcs->UpdateACS(pMotherRod))))
		{
			m_xStartBeforeLift=pMotherRod->StartBeforeLift();
			m_xEndBeforeLift=pMotherRod->EndBeforeLift();
			m_xStartBeforeLift=pAcs->CoordTrans(m_xStartBeforeLift);
			m_xEndBeforeLift=pAcs->CoordTrans(m_xEndBeforeLift);

			f3dPoint posStart=pAcs->CoordTrans(pMotherRod->Start());//pLift!=NULL));
			f3dPoint posEnd=pAcs->CoordTrans(pMotherRod->End());//pLift!=NULL));
			baseline.SetStart(posStart);	
			baseline.SetEnd(posEnd);
		}
	}
	else if(m_iShadowObjType==SHADOW_EMBEDDED||m_iShadowObjType==SHADOW_EXTERBLK)
	{
		CBlockParameter blockPara;
		f3dPoint startPos=pMotherRod->Start();//pLift!=NULL);
		f3dPoint endPos=pMotherRod->End();//pLift!=NULL);
		if(GetBlockParameter(blockPara))
		{
			UCS_STRU cs=blockPara.m_pBlock->TransToACS(blockPara.m_pBlockRef->GetACS());
			m_xStartBeforeLift=pMotherRod->StartBeforeLift();
			m_xEndBeforeLift=pMotherRod->EndBeforeLift();
			coord_trans(m_xStartBeforeLift,cs,TRUE,TRUE);
			coord_trans(m_xEndBeforeLift,cs,TRUE,TRUE);

			coord_trans(startPos,cs,TRUE,TRUE);
			coord_trans(endPos,cs,TRUE,TRUE);
			baseline.SetStart(startPos);
			baseline.SetEnd(endPos);
		}
	}
	SetModified();
	return true;
}

void CLDSLinePart::CalPosition(bool bIgnoreRepeatFlag/*=FALSE*/)//BOOL bStart,CLDSArcLift *pLift)
{
	if(IsShadowObject())
		CalShadowRodPos();
}
bool CLDSLinePart::IterCalPosition(double tolerance/*=0.5*/,UINT nMaxIterCount/*=2*/)
{
	for(UINT i=0;i<nMaxIterCount;i++)
	{
		GEPOINT oldPosStart=baseline.Start(),oldPosEnd=baseline.End();
		CalPosition(true);
		GEPOINT nowPosStart=baseline.Start(),nowPosEnd=baseline.End();
		GEPOINT vStartOffset=nowPosStart-oldPosStart;
		GEPOINT vEndOffset=nowPosEnd-oldPosEnd;
		if( fabs(vStartOffset.x)+fabs(vStartOffset.y)+fabs(vStartOffset.z)<tolerance)	//���С��0.1mmʱ��������˨�Զ���λ���
			return true;
	}
	return false;
}

#if !defined(__TSA_)&&!defined(__TSA_FILE_)
CLsRefList *CLDSLinePart::GetLsRefList(){
	if(pDelegatePart)
		return pDelegatePart->GetLsRefList();
	return pLsRefList;
}
CLsRef* CLDSLinePart::FindLsByHandle(THANDLE h)
{
	if(pDelegatePart)
		return pDelegatePart->FindLsByHandle(h);
	if (this->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		CLsRef* pLsRef;
		CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)this;
		for (int i=0;i<4;i++)
		{
			if (pGroupAngle->son_jg_h[i]==0)
				continue;
			CLDSLineAngle* pAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
			if (pAngle!=NULL&&(pLsRef=pAngle->FindLsByHandle(h))!=NULL)
				return pLsRef;
		}
		return NULL;
	}
	else
		return pLsRefList->FromHandle(h);
}
CLsRef* CLDSLinePart::AppendLsRef(CLsRef &ls_ref,BOOL bNearLsCheck/*=TRUE*/)
{
	return AppendMidLsRef(ls_ref,bNearLsCheck);
}
CLsRef* CLDSLinePart::AppendStartLsRef(CLsRef ls_ref,BOOL bNearLsCheck/*=TRUE*/)
{
	if(pDelegatePart)
		return pDelegatePart->AppendLsRef(ls_ref,bNearLsCheck);
	ls_ref.start1_end2_mid0=1;
	if(m_nClassTypeId==CLS_GROUPLINEANGLE)
	{	//��֪��Ϊʲô��ȥ��ϽǸ������ص�������˨�ĺ�������ȥ��˴��������ѭ��
		//�������ϽǸ���������˨ʱ��Ϊ�ǽ���˨�ֱ����뵽ÿ������������˨���ӽǸ���  wht 09-12-25
		int i=0;
		CLsRef *pLsRef=NULL;
		CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)this;
		for(i=0;i<4;i++)
		{
			CLDSLineAngle *pSonAngle=NULL;
			if(pGroupAngle->son_jg_h[i]>0x20)
				pSonAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
			if(pSonAngle==NULL||pSonAngle->m_bVirtualPart)
				continue;	//�������ӽǸֻ��ӽǸ�Ϊ���⹹�� wht 11-08-05
			//��˨���ӽǸ��Ͽ�������
			if(pSonAngle->IsLsInWing(ls_ref.GetLsPtr()))
				pLsRef=pSonAngle->AppendStartLsRef(ls_ref,bNearLsCheck);
		}
		return pLsRef;
		//return ((CLDSGroupLineAngle*)this)->AppendStartLsRef(ls_ref,bNearLsCheck);
	}
	else
	{
		if(bNearLsCheck)	//��鸽����˨
		{
			CLsRef *pLsRef=pLsRefList->FindLsByPos(ls_ref->ucs.origin,ls_ref->get_norm(),0.5);
			if(pLsRef&&(*pLsRef)->get_d()==ls_ref->get_d())
			{
				pLsRef->start1_end2_mid0=1;
				return pLsRef;
			}
		}
		SetModified();
		return pLsRefList->append(ls_ref);
	}
	return NULL;
}
CLsRef* CLDSLinePart::AppendMidLsRef(CLsRef ls_ref,BOOL bNearLsCheck/*=TRUE*/)
{
	if(pDelegatePart)
		return pDelegatePart->AppendLsRef(ls_ref,bNearLsCheck);
	ls_ref.start1_end2_mid0=0;
	if(m_nClassTypeId==CLS_GROUPLINEANGLE)
	{	//��֪��Ϊʲô��ȥ��ϽǸ������ص�������˨�ĺ�������ȥ��˴��������ѭ��
		//�������ϽǸ���������˨ʱ��Ϊ�ǽ���˨�ֱ����뵽ÿ������������˨���ӽǸ���  wht 09-12-25
		int i=0;
		CLsRef *pLsRef=NULL;
		CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)this;
		for(i=0;i<4;i++)
		{
			CLDSLineAngle *pSonAngle=NULL;
			if(pGroupAngle->son_jg_h[i]>0x20)
				pSonAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
			if(pSonAngle==NULL||pSonAngle->m_bVirtualPart)
				continue;	//�������ӽǸֻ��ӽǸ�Ϊ���⹹�� wht 11-08-05
			//��˨���ӽǸ��Ͽ�������
			if(pSonAngle->IsLsInWing(ls_ref.GetLsPtr()))
				pLsRef=pSonAngle->AppendMidLsRef(ls_ref,bNearLsCheck);
		}
		return pLsRef;
		//return ((CLDSGroupLineAngle*)this)->AppendMidLsRef(ls_ref,bNearLsCheck);
	}
	else
	{
		if(bNearLsCheck)	//��鸽����˨
		{
			CLsRef *pLsRef=pLsRefList->FindLsByPos(ls_ref->ucs.origin,ls_ref->get_norm(),0.5);
			if(pLsRef&&(*pLsRef)->get_d()==ls_ref->get_d())
			{
				pLsRef->start1_end2_mid0=0;
				return pLsRef;
			}
		}
		SetModified();
		return pLsRefList->append(ls_ref);
	}
	return NULL;
}
CLsRef* CLDSLinePart::AppendEndLsRef(CLsRef ls_ref,BOOL bNearLsCheck/*=TRUE*/)
{
	if(pDelegatePart)
		return pDelegatePart->AppendLsRef(ls_ref,bNearLsCheck);
	ls_ref.start1_end2_mid0=2;
	if(m_nClassTypeId==CLS_GROUPLINEANGLE)
	{	//��֪��Ϊʲô��ȥ��ϽǸ������ص�������˨�ĺ�������ȥ��˴��������ѭ��
		//�������ϽǸ���������˨ʱ��Ϊ�ǽ���˨�ֱ����뵽ÿ������������˨���ӽǸ���  wht 09-12-25
		int i=0;
		CLsRef *pLsRef=NULL;
		CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)this;
		for(i=0;i<4;i++)
		{
			CLDSLineAngle *pSonAngle=NULL;
			if(pGroupAngle->son_jg_h[i]>0x20)
				pSonAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
			if(pSonAngle==NULL||pSonAngle->m_bVirtualPart)
				continue;	//�������ӽǸֻ��ӽǸ�Ϊ���⹹�� wht 11-08-05
			//��˨���ӽǸ��Ͽ�������
			if(pSonAngle->IsLsInWing(ls_ref.GetLsPtr()))
				pLsRef=pSonAngle->AppendEndLsRef(ls_ref,bNearLsCheck);
		}
		return pLsRef;
		//return ((CLDSGroupLineAngle*)this)->AppendEndLsRef(ls_ref,bNearLsCheck);
	}
	else
	{
		if(bNearLsCheck)	//��鸽����˨
		{
			CLsRef *pLsRef=pLsRefList->FindLsByPos(ls_ref->ucs.origin,ls_ref->get_norm(),0.5);
			if(pLsRef&&(*pLsRef)->get_d()==ls_ref->get_d())
			{
				pLsRef->start1_end2_mid0=2;
				return pLsRef;
			}
		}
		SetModified();
		return pLsRefList->append(ls_ref);
	}
	return NULL;
}
int CLDSLinePart::GetLsCount(short d/*=0*/)
{
	if(pDelegatePart)
		return pDelegatePart->GetLsCount(d);
	else
		return CLDSPart::GetLsCount(d);
}
int CLDSLinePart::GetLocalLsCount(short start1_end2_mid0/*=0*/,double length_scope/*=0*/)
{
	if(length_scope<=0)
		return pLsRefList->GetLocalLsCount(start1_end2_mid0);
	else if(GetLsCount()>0)
	{
		double length=GetLength();
		f3dPoint inters_pos;
		f3dPoint lenStdVec=End()-Start();
		normalize(lenStdVec);
		int hits=0;
		for(CLsRef* pLsRef=GetFirstLsRef();pLsRef;pLsRef=GetNextLsRef())
		{
			if(!GetBoltIntersPos(pLsRef->GetLsPtr(),inters_pos))
				continue;
			f3dPoint lenVec=inters_pos-Start();
			double lengthpos=lenVec*lenStdVec+start_oddment;
			if(start1_end2_mid0==1&&lengthpos<=length_scope)	//ʼ����˨
				hits++;
			else if(start1_end2_mid0==2&&lengthpos>=length-length_scope)	//ʼ����˨
				hits++;
			else if(start1_end2_mid0==0)
				hits++;
		}
		return hits;
	}
	else
		return 0;
}

long CLDSLinePart::get_hStartDatumRod(){
	return (pStart!=NULL)?pStart->hFatherPart:0;
}
long CLDSLinePart::get_hEndDatumRod(){
	return (pEnd!=NULL)?pEnd->hFatherPart:0;
}
BOOL CLDSLinePart::SetStartOdd(double start_odd,bool bToFL/*=false*/)//BOOL bCorrectLsPosPara/*=FALSE*/,BOOL bCorrectPlateProfile/*=FALSE*/,bool bToFL/*=false*/)
{
	if(IsStartPosLocked())
		return FALSE;
	if(GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)this)->bHuoQuStart)
		return FALSE;
	if(GetClassTypeId()==CLS_LINETUBE && bToFL)
	{
		CLDSParamPlate* pFL=NULL;
		if(((CLDSLineTube*)this)->m_tJointStart.type==2||((CLDSLineTube*)this)->m_tJointStart.type==6)
			pFL=(CLDSParamPlate*)BelongModel()->FromPartHandle(((CLDSLineTube*)this)->m_tJointStart.hLinkObj,CLS_PARAMPLATE);
		if(pFL!=NULL)
		{
			double H=0;
			CLDSStdPartModel stdFlModel;
			if(!pFL->m_bStdPart)
				pFL->GetDesignItemValue('H',&H);
			else if(pFL->GetStdPartModel(&stdFlModel))
			{
				if(pFL->m_iParamType==TYPE_FLD)
					H=stdFlModel.param.fl_param.H;
				else if(pFL->m_iParamType==TYPE_FLP)
					H=stdFlModel.param.fl_param.H1;
				else if(pFL->m_iParamType==TYPE_FLG||pFL->m_iParamType==TYPE_FLR)
					H=stdFlModel.param.fl_param2.fH;
			}
			start_odd-=H;
		}
	}
	double diff=start_odd-start_oddment;
	if(fabs(diff)>eps2)
		SetModified();
	if(fabs(diff)>ODDMENT_MINIMAL_MODIFIED&&(_localUpdateMovePartsFlag&DISABLE_RECORD_RELA_PARTS)==0)
	{
		if(IsAngle())
		{
			CLDSLineAngle* pAngle=(CLDSLineAngle*)this;
			CLDSLineAngle::AppendModifiedAngles(pAngle);
			pAngle->GatherOddRelaParts(1);
		}
		else if(GetClassTypeId()==CLS_LINETUBE)
		{
			CLDSLineTube* pTube=(CLDSLineTube*)this;
			CLDSLineTube::AppendModifiedTube(pTube);
			pTube->GatherOddRelaParts(1);
		}
	}
	if(GetClassTypeId()==CLS_GROUPLINEANGLE&&((CLDSGroupLineAngle*)this)->m_bUpdateSonAngleOddment)
	{
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)this;
		for(int i=0;i<4;i++)
		{
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
			if(pLineAngle)
			{
				pLineAngle->SetStartOdd(start_odd);
				if(fabs(diff)>EPS2)
					SetModified();
				if(fabs(diff)>ODDMENT_MINIMAL_MODIFIED&&(_localUpdateMovePartsFlag&DISABLE_RECORD_RELA_PARTS)==0)
				{
					CLDSLineAngle::AppendModifiedAngles(pLineAngle);
					pLineAngle->GatherOddRelaParts(1);
				}
			}
		}
	}
#ifdef __GGG_
	if (GetClassTypeId()==CLS_CONETUBE)
	{
		CLDSConeTube *pConeTube = (CLDSConeTube *)this;
		pConeTube->size_wide -= (pConeTube->size_wideUpSide-pConeTube->size_wide)/pConeTube->longth*(start_odd-start_oddment);
	}
#endif
	start_oddment=start_odd;
	return TRUE;
}

BOOL CLDSLinePart::SetEndOdd(double end_odd,bool bToFL/*=false*/)//,BOOL bCorrectLsPosPara/*=FALSE*/,BOOL bCorrectPlateProfile/*=FALSE*/,bool bToFL/*=false*/)
{
	if(IsEndPosLocked())
		return FALSE;
	if(GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)this)->bHuoQuEnd)
		return FALSE;
	if(GetClassTypeId()==CLS_LINETUBE && bToFL)
	{
		CLDSParamPlate* pFL=NULL;
		if(((CLDSLineTube*)this)->m_tJointEnd.type==2||((CLDSLineTube*)this)->m_tJointEnd.type==6)
			pFL=(CLDSParamPlate*)BelongModel()->FromPartHandle(((CLDSLineTube*)this)->m_tJointEnd.hLinkObj,CLS_PARAMPLATE);
		if(pFL!=NULL)
		{
			double H=0;
			CLDSStdPartModel stdFlModel;
			if(!pFL->m_bStdPart)
				pFL->GetDesignItemValue('H',&H);
			else if(pFL->GetStdPartModel(&stdFlModel))
			{
				if(pFL->m_iParamType==TYPE_FLD)
					H=stdFlModel.param.fl_param.H;
				else if(pFL->m_iParamType==TYPE_FLP)
					H=stdFlModel.param.fl_param.H1;
				else if(pFL->m_iParamType==TYPE_FLG||pFL->m_iParamType==TYPE_FLR)
					H=stdFlModel.param.fl_param2.fH;
			}
			end_odd-=H;
		}
	}
	double diff=end_odd-end_oddment;
	if(fabs(diff)>eps2)
		SetModified();
	if(fabs(diff)>ODDMENT_MINIMAL_MODIFIED&&(_localUpdateMovePartsFlag&DISABLE_RECORD_RELA_PARTS)==0)
	{
		if(IsAngle())
		{
			CLDSLineAngle* pAngle=(CLDSLineAngle*)this;
			CLDSLineAngle::AppendModifiedAngles(pAngle);
			pAngle->GatherOddRelaParts(2);
		}
		else if(GetClassTypeId()==CLS_LINETUBE)
		{
			CLDSLineTube* pTube=(CLDSLineTube*)this;
			CLDSLineTube::AppendModifiedTube(pTube);
			pTube->GatherOddRelaParts(2);
		}
	}
	if(GetClassTypeId()==CLS_GROUPLINEANGLE&&((CLDSGroupLineAngle*)this)->m_bUpdateSonAngleOddment)
	{
		CLDSGroupLineAngle *pGroupJg=(CLDSGroupLineAngle*)this;
		for(int i=0;i<4;i++)
		{
			CLDSLineAngle *pLineAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pGroupJg->son_jg_h[i],CLS_LINEANGLE);
			if(pLineAngle)
			{
				pLineAngle->SetEndOdd(end_odd);
				if(fabs(diff)>EPS2)
					SetModified();
				if(fabs(diff)>ODDMENT_MINIMAL_MODIFIED&&(_localUpdateMovePartsFlag&DISABLE_RECORD_RELA_PARTS)==0)
				{
					CLDSLineAngle::AppendModifiedAngles(pLineAngle);
					pLineAngle->GatherOddRelaParts(2);
				}
			}
		}
	}
#ifdef __GGG_
	if (GetClassTypeId()==CLS_CONETUBE)
	{
		CLDSConeTube *pConeTube = (CLDSConeTube *)this;
		pConeTube->size_wideUpSide += (pConeTube->size_wideUpSide-pConeTube->size_wide)/pConeTube->longth*(end_odd-end_oddment);
	}
#endif
	end_oddment=end_odd;
	return TRUE;
}
void CLDSLinePart::CalStartOddment(char ciAutoCalMode/*=0*/,int liInitOddment/*=0*/)
{
	if (desStartOdd.m_iOddCalStyle == 0 || (ciAutoCalMode == 1 && desStartOdd.m_iOddCalStyle == 2))
	{	//������ײ��������ͷ
		CLDSPart *pPartArr[2]={NULL};
		pPartArr[0]=BelongModel()->FromPartHandle(desStartOdd.m_hRefPart1);
		pPartArr[1]=BelongModel()->FromPartHandle(desStartOdd.m_hRefPart2);
		if(pPartArr[0]||pPartArr[1])
			start_oddment=liInitOddment;
		else
			return;
		if(pPartArr[0]&&pPartArr[1]&&(pPartArr[0]->handle==pPartArr[1]->handle))
			pPartArr[1]=NULL;	//�����ظ�����
		BOOL bCollide=TRUE;
		while(bCollide)
		{
			bCollide = FALSE;
			for(int i=0;i<2;i++)
			{
				if(pPartArr[i])
				{
					if(IsPartsCollide(pPartArr[i],this,ftoi(desStartOdd.m_fCollideDist)))
					{
						start_oddment-=5;
						bCollide=TRUE;
						break;
					}
				}
			}
		}
	}
}
void CLDSLinePart::CalEndOddment(char ciAutoCalMode/*=0*/,int liInitOddment/*=0*/)
{
	if(desEndOdd.m_iOddCalStyle==0||(ciAutoCalMode==1&&desEndOdd.m_iOddCalStyle==2))
	{	//������ײ��������ͷ
		CLDSPart *pPartArr[2]={NULL};
		pPartArr[0]=BelongModel()->FromPartHandle(desEndOdd.m_hRefPart1);
		pPartArr[1]=BelongModel()->FromPartHandle(desEndOdd.m_hRefPart2);
		if(pPartArr[0]||pPartArr[1])
			end_oddment=liInitOddment;
		else
			return;
		if(pPartArr[0]&&pPartArr[1]&&(pPartArr[0]->handle==pPartArr[1]->handle))
			pPartArr[1]=NULL;	//�����ظ�����
		BOOL bCollide=TRUE;
		while(bCollide)
		{
			bCollide = FALSE;
			for(int i=0;i<2;i++)
			{
				if(pPartArr[i])
				{
					if(IsPartsCollide(pPartArr[i],this,ftoi(desEndOdd.m_fCollideDist)))
					{
						end_oddment-=5;
						bCollide=TRUE;
						break;
					}
				}
			}
		}
	}
}
#endif
void CLDSLinePart::IndependentShadowPropFromBuffer(CBuffer &buffer,long version/* =0 */,long doc_type/* =0 */)
{
#ifdef __PART_DESIGN_INC_//defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if( version==0||
		(doc_type==1&&version>=4020004)||	//TMA(V4.2.0.4)
		(doc_type==2&&version>=1020004)||	//LMA(V1.2.0.4)
		(doc_type==3&&version>=2000004)||	//TSA(V2.0.0.4)
		(doc_type==4&&version>=1020004)||	//LDS(V1.2.0.4)
		(doc_type==5&&version>=1020004))	//TDA(V1.2.0.4)
	{
		CfgwordFromBuffer(cfgword,buffer,version,doc_type);
		BelongModel()->EvolveCfgword(cfgword,version,doc_type);
		buffer.Read(layer_tag,4);
		long h=0;
		buffer.ReadInteger(&h);
		pStart=m_pModel->FromNodeHandle(h);
		buffer.ReadInteger(&h);
		pEnd=m_pModel->FromNodeHandle(h);
		buffer.ReadWord(&m_iElemType);
		buffer.ReadInteger(&m_bNeedSelMat);
		buffer.ReadInteger(&m_bNeedSelSize);

		buffer.ReadInteger(&CalLenCoef.hRefPole);
		buffer.ReadInteger(&CalLenCoef.hHorizPole);
		buffer.ReadInteger(&CalLenCoef.minR.hStartNode);
		buffer.ReadInteger(&CalLenCoef.minR.hEndNode);
		buffer.ReadDouble(&CalLenCoef.minR.coef);
		buffer.ReadInteger(&CalLenCoef.paraR.hStartNode);
		buffer.ReadInteger(&CalLenCoef.paraR.hEndNode);
		buffer.ReadDouble(&CalLenCoef.paraR.coef);
		buffer.ReadInteger(&CalLenCoef.iTypeNo);
		buffer.ReadInteger(&CalLenCoef.iSelRFlag);
		buffer.ReadDouble(&connectStart.maxTension);
		buffer.ReadDouble(&connectStart.maxCompression);
		buffer.ReadDouble(&connectStart.maxMoment);
		buffer.ReadDouble(&connectEnd.maxTension);
		buffer.ReadDouble(&connectEnd.maxCompression);
		buffer.ReadDouble(&connectEnd.maxMoment);
		buffer.ReadDouble(&hole_out);
		buffer.ReadInteger(&m_bAutoCalHoleOutCoef);
		buffer.ReadInteger(&connectStart.d);   	
		buffer.Read(connectStart.grade,4);   	
		buffer.Read(&connectStart.uiLegacyN,4);   	
		buffer.ReadInteger(&connectEnd.d);   	
		buffer.Read(connectEnd.grade,4);   	
		buffer.Read(&connectEnd.uiLegacyN,4);   	
		int iConnType=0;
		buffer.ReadInteger(&iConnType);
		connectStart.m_iConnectType=(BYTE)iConnType;
		buffer.ReadString(connectStart.flNo);
		buffer.ReadInteger(&iConnType);
		connectEnd.m_iConnectType=(BYTE)iConnType;
		buffer.ReadString(connectEnd.flNo);
		int n=0;
		buffer.ReadInteger(&n);
		for(int j=0;j<n;j++)
		{
			RELATIVE_OBJECT *pMirObj=relativeObjs.Add(buffer.ReadInteger());
			buffer.ReadByte(&pMirObj->mirInfo.axis_flag);
			buffer.ReadPoint(pMirObj->mirInfo.origin);
			if((pMirObj->mirInfo.axis_flag&0x08)>0)
			{
				buffer.ReadWord(&pMirObj->mirInfo.rotate_angle);
				buffer.ReadByte(&pMirObj->mirInfo.array_num);
			}
			if(pMirObj->mirInfo.axis_flag&0x10)	//����
				buffer.ReadPoint(pMirObj->mirInfo.mir_norm);
		}
	}
#endif
}
void CLDSLinePart::IndependentShadowPropToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
#ifdef __PART_DESIGN_INC_//defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	buffer.Write(cfgword.flag.word,24);
	buffer.Write(layer_tag,4);
	if(pStart)
		buffer.WriteInteger(pStart->handle);
	else
		buffer.WriteInteger(0);
	if(pEnd)
		buffer.WriteInteger(pEnd->handle);
	else
		buffer.WriteInteger(0);
	buffer.WriteWord(m_iElemType);
	buffer.WriteInteger(m_bNeedSelMat);
	buffer.WriteInteger(m_bNeedSelSize);
	buffer.WriteInteger(CalLenCoef.hRefPole);
	buffer.WriteInteger(CalLenCoef.hHorizPole);
	buffer.WriteInteger(CalLenCoef.minR.hStartNode);
	buffer.WriteInteger(CalLenCoef.minR.hEndNode);
	buffer.WriteDouble(CalLenCoef.minR.coef);
	buffer.WriteInteger(CalLenCoef.paraR.hStartNode);
	buffer.WriteInteger(CalLenCoef.paraR.hEndNode);
	buffer.WriteDouble(CalLenCoef.paraR.coef);
	buffer.WriteInteger(CalLenCoef.iTypeNo);
	buffer.WriteInteger(CalLenCoef.iSelRFlag);
	
	buffer.WriteDouble(connectStart.maxTension);
	buffer.WriteDouble(connectStart.maxCompression);
	buffer.WriteDouble(connectStart.maxMoment);
	buffer.WriteDouble(connectEnd.maxTension);
	buffer.WriteDouble(connectEnd.maxCompression);
	buffer.WriteDouble(connectEnd.maxMoment);
	
	buffer.WriteDouble(hole_out);
	buffer.WriteInteger(m_bAutoCalHoleOutCoef);
	buffer.WriteInteger(connectStart.d);   	
	buffer.Write(connectStart.grade,4);   	
	buffer.Write(&connectStart.uiLegacyN,4);   	
	buffer.WriteInteger(connectEnd.d);   	
	buffer.Write(connectEnd.grade,4);   	
	buffer.Write(&connectEnd.uiLegacyN,4);   	
	buffer.WriteInteger(connectStart.m_iConnectType);
	buffer.WriteString(connectStart.flNo);
	buffer.WriteInteger(connectEnd.m_iConnectType);
	buffer.WriteString(connectEnd.flNo);
	BUFFERPOP stack(&buffer,relativeObjs.GetNodeNum());
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for(RELATIVE_OBJECT *pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pMirObj->hObj);
		buffer.WriteByte(pMirObj->mirInfo.axis_flag);
		buffer.WritePoint(pMirObj->mirInfo.origin);
		if((pMirObj->mirInfo.axis_flag&8)>0)
		{
			buffer.WriteWord(pMirObj->mirInfo.rotate_angle);
			buffer.WriteByte(pMirObj->mirInfo.array_num);
		}
		if(pMirObj->mirInfo.axis_flag&0x10)	//����
			buffer.WritePoint(pMirObj->mirInfo.mir_norm);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of relative object is wrong!");
#else
		CLDSObject::Log2File()->Log("����������¼�����ִ���!");
#endif
#endif
}
void CLDSLinePart::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
#if defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
{
	buffer.Write(cfgword.flag.word,24);
	buffer.Write(layer_tag,4);
	if(pStart)
		buffer.WriteInteger(pStart->handle);
	else
		buffer.WriteInteger(0);
	if(pEnd)
		buffer.WriteInteger(pEnd->handle);
	else
		buffer.WriteInteger(0);
	buffer.WriteWord(m_iElemType);
	buffer.WriteInteger(m_bNeedSelMat);
	buffer.WriteInteger(m_bNeedSelSize);
	
	buffer.WriteWord((WORD)start_joint_type);
	buffer.WriteWord((WORD)end_joint_type);
	buffer.WriteWord((WORD)start_force_type);
	buffer.WriteWord((WORD)end_force_type);
	
	buffer.WriteInteger(CalLenCoef.hRefPole);
	buffer.WriteInteger(CalLenCoef.hHorizPole);
	buffer.WriteInteger(CalLenCoef.minR.hStartNode);
	buffer.WriteInteger(CalLenCoef.minR.hEndNode);
	buffer.WriteDouble(CalLenCoef.minR.coef);
	buffer.WriteInteger(CalLenCoef.paraR.hStartNode);
	buffer.WriteInteger(CalLenCoef.paraR.hEndNode);
	buffer.WriteDouble(CalLenCoef.paraR.coef);
	buffer.WriteInteger(CalLenCoef.iTypeNo);
	buffer.WriteInteger(CalLenCoef.iSelRFlag);
	buffer.WriteInteger(size_idClassType);	
	buffer.WriteByte(size_cSubClassType);
	if(size_idClassType==CLS_CABLE)
	{	//��������
		buffer.WriteString(GetWireType());
		buffer.WriteString(GetSpec());
		buffer.WriteDouble(m_fPreTension);
	}
	buffer.WriteDouble(size_wide);
	buffer.WriteDouble(size_thick);
	buffer.WriteByte(cMaterial);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.WriteByte(m_cQualityLevel);
	buffer.WriteDword(m_uStatMatNo);			//ͳ�����
	buffer.WriteDouble(m_fWeightRaiseCoef);	// �����Ŵ�ϵ�����������塢��˨�ȶ������ķŴ�����
	if(	version==0|| (doc_type==4&&version>=1030800))	//LDS V1.3.8.0
		buffer.WriteByte(m_cbRodFlag);
	buffer.WriteDouble(connectStart.maxTension);
	buffer.WriteDouble(connectStart.maxCompression);
	buffer.WriteDouble(connectStart.maxMoment);
	buffer.WriteDouble(connectEnd.maxTension);
	buffer.WriteDouble(connectEnd.maxCompression);
	buffer.WriteDouble(connectEnd.maxMoment);
	// --------- 3. ��������: ��˨��Ϣ�͸˼����� -----------
	buffer.WriteDouble(hole_out);
	buffer.WriteInteger(m_bAutoCalHoleOutCoef);
	buffer.WriteInteger(connectStart.d);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Write(connectStart.grade,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Write(&connectStart.uiLegacyN,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.WriteInteger(connectEnd.d);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Write(connectEnd.grade,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Write(&connectEnd.uiLegacyN,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.WriteInteger(connectStart.m_iConnectType);
	buffer.WriteString(connectStart.flNo);
	buffer.WriteInteger(connectEnd.m_iConnectType);
	buffer.WriteString(connectEnd.flNo);
	
	if(size_idClassType==0)
	{
		buffer.WriteDouble(userdef.E);
		buffer.WriteDouble(userdef.area);
		buffer.WriteDouble(userdef.ft);
		buffer.WriteDouble(userdef.fc);
		buffer.WriteDouble(userdef.unit_weight);
		buffer.WriteDouble(userdef.pre_stress);
	}
	
	BUFFERPOP stack(&buffer,relativeObjs.GetNodeNum());
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for(RELATIVE_OBJECT *pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pMirObj->hObj);
		buffer.WriteByte(pMirObj->mirInfo.axis_flag);
		buffer.WritePoint(pMirObj->mirInfo.origin);
		if((pMirObj->mirInfo.axis_flag&8)>0)
		{
			buffer.WriteWord(pMirObj->mirInfo.rotate_angle);
			buffer.WriteByte(pMirObj->mirInfo.array_num);
		}
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of relative object is wrong!");
#else
		CLDSObject::Log2File()->Log("����������¼�����ִ���!");
#endif
}
#else
{;}
#endif
void CLDSLinePart::FromCdfBuffer(CBuffer &buffer,long version/*=0*/,bool bReadForCheck/*=false*/)
{
	WORD w;
	long h;
	buffer.Read(cfgword.flag.word,24);
	buffer.Read(layer_tag,4);
	layer_tag[3]='\0';
	if(bReadForCheck)
	{
		buffer.ReadInteger(&h);
		hStartNode=h;
		buffer.ReadInteger(&h);
		hEndNode=h;
	}
	else
	{
		buffer.ReadInteger(&h);
		pStart=m_pModel->FromNodeHandle(h);
		buffer.ReadInteger(&h);
		pEnd=m_pModel->FromNodeHandle(h);
	}
	buffer.ReadInteger(&_iSeg.iSeg);
	buffer.ReadBooleanThin(&_bVirtualPart);

	buffer.ReadWord(&w);
	start_joint_type=(JOINT_TYPE)w;	//JOINT_HINGE(0),JOINT_RIGID(1),JOINT_AUTOJUSTIFY(2);
	buffer.ReadWord(&w);
	end_joint_type=(JOINT_TYPE)w;	//JOINT_HINGE(0),JOINT_RIGID(1),JOINT_AUTOJUSTIFY(2);
	buffer.ReadWord(&w);
	start_force_type=w;	//CENTRIC_FORCE;ECCENTRIC_FORCE
	buffer.ReadWord(&w);
	end_force_type=w;	//CENTRIC_FORCE;ECCENTRIC_FORCE

	buffer.ReadInteger(&size.idClassType);	
	buffer.ReadByte(&size.cSubClassType);
	if(size.idClassType==CLS_CABLE)
	{	//��������
		buffer.ReadString(size.wireType);
		buffer.ReadString(size.spec);
		//buffer.ReadDouble(&m_fPreTension);
	}
	buffer.ReadDouble(&size.wide);
	buffer.ReadDouble(&size.thick);
	buffer.ReadByte(&_material);
	buffer.ReadDword(&_uStatMatNo);			//ͳ�����
	// --------- 3. ��������: ��˨��Ϣ�͸˼����� -----------
	buffer.ReadInteger(&connectStart.d);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Read(connectStart.grade,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Read(&connectStart.uiLegacyN,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.ReadInteger(&connectEnd.d);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Read(connectEnd.grade,4);   		//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Read(&connectEnd.uiLegacyN,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	//������Ϣ
	int iConnType=0;
	buffer.ReadInteger(&iConnType);
	connectStart.m_iConnectType=(BYTE)iConnType;
	buffer.ReadString(connectStart.flNo);
	buffer.ReadInteger(&iConnType);
	connectEnd.m_iConnectType=(BYTE)iConnType;
	buffer.ReadString(connectEnd.flNo);

	buffer.ReadInteger(&h);
	for(int j=0;j<h;j++)
	{
		RELATIVE_OBJECT *pMirObj=relativeObjs.Add(buffer.ReadInteger());
		buffer.ReadByte(&pMirObj->mirInfo.axis_flag);
		buffer.ReadPoint(pMirObj->mirInfo.origin);
		if((pMirObj->mirInfo.axis_flag&8)>0)
		{
			buffer.ReadWord(&pMirObj->mirInfo.rotate_angle);
			buffer.ReadByte(&pMirObj->mirInfo.array_num);
		}
	}
}
void CLDSLinePart::ToCdfBuffer(CBuffer &buffer,long version/*=0*/)
{
	buffer.Write(cfgword.flag.word,24);
	buffer.Write(layer_tag,4);
	if(pStart)
		buffer.WriteInteger(pStart->handle);
	else
		buffer.WriteInteger(0);
	if(pEnd)
		buffer.WriteInteger(pEnd->handle);
	else
		buffer.WriteInteger(0);
	buffer.WriteInteger(_iSeg);
	buffer.WriteBooleanThin(_bVirtualPart);
	
	buffer.WriteWord((WORD)start_joint_type);
	buffer.WriteWord((WORD)end_joint_type);
	buffer.WriteWord((WORD)start_force_type);
	buffer.WriteWord((WORD)end_force_type);
	
	buffer.WriteInteger(size_idClassType);	
	buffer.WriteByte(size_cSubClassType);
	if(size_idClassType==CLS_CABLE)
	{	//��������
		buffer.WriteString(GetWireType());
		buffer.WriteString(GetSpec());
		//buffer.WriteDouble(m_fPreTension);
	}
	buffer.WriteDouble(size_wide);
	buffer.WriteDouble(size_thick);
	buffer.WriteByte(cMaterial);
	buffer.WriteDword(m_uStatMatNo);			//ͳ�����
	// --------- 3. ��������: ��˨��Ϣ�͸˼����� -----------
	buffer.WriteInteger(connectStart.d);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Write(connectStart.grade,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Write(&connectStart.uiLegacyN,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.WriteInteger(connectEnd.d);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Write(connectEnd.grade,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Write(&connectEnd.uiLegacyN,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.WriteInteger(connectStart.m_iConnectType);
	buffer.WriteString(connectStart.flNo);
	buffer.WriteInteger(connectEnd.m_iConnectType);
	buffer.WriteString(connectEnd.flNo);
	
	BUFFERPOP stack(&buffer,relativeObjs.GetNodeNum());
	buffer.WriteInteger(relativeObjs.GetNodeNum());
	for(RELATIVE_OBJECT *pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		buffer.WriteInteger(pMirObj->hObj);
		buffer.WriteByte(pMirObj->mirInfo.axis_flag);
		buffer.WritePoint(pMirObj->mirInfo.origin);
		if((pMirObj->mirInfo.axis_flag&8)>0)
		{
			buffer.WriteWord(pMirObj->mirInfo.rotate_angle);
			buffer.WriteByte(pMirObj->mirInfo.array_num);
		}
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of relative object is wrong!");
#else
		CLDSObject::Log2File()->Log("����������¼�����ִ���!");
#endif
}

void CLDSLinePart::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
#ifdef __PART_DESIGN_INC_//defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	WORD w;
	long h;
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	buffer.Read(layer_tag,4);
	//if(layer_tag[1]=='B')
	//	layer_tag[1]='F';
	layer_tag[3]='\0';
	buffer.ReadInteger(&h);
	pStart=m_pModel->FromNodeHandle(h);
	buffer.ReadInteger(&h);
	pEnd=m_pModel->FromNodeHandle(h);
	buffer.ReadWord(&m_iElemType);
	buffer.ReadInteger(&m_bNeedSelMat);
	buffer.ReadInteger(&m_bNeedSelSize);

	buffer.ReadWord(&w);
	if(w==0)
		start_joint_type=JOINT_HINGE;
	else if(w==1)
		start_joint_type=JOINT_RIGID;
	else
		start_joint_type=JOINT_AUTOJUSTIFY;
	buffer.ReadWord(&w);
	if(w==0)
		end_joint_type=JOINT_HINGE;
	else if(w==1)
		end_joint_type=JOINT_RIGID;
	else
		end_joint_type=JOINT_AUTOJUSTIFY;
	buffer.ReadWord(&w);
	if(w==0)
		start_force_type=CENTRIC_FORCE;
	else
		start_force_type=ECCENTRIC_FORCE;
	buffer.ReadWord(&w);
	if(w==0)
		end_force_type=CENTRIC_FORCE;
	else
		end_force_type=ECCENTRIC_FORCE;

	buffer.ReadInteger(&CalLenCoef.hRefPole);
	buffer.ReadInteger(&CalLenCoef.hHorizPole);
	if(version==0||(doc_type==3&&version>=1070101)||(doc_type==4&&version>=1000101))
	{
		buffer.ReadInteger(&CalLenCoef.minR.hStartNode);
		buffer.ReadInteger(&CalLenCoef.minR.hEndNode);
		buffer.ReadDouble(&CalLenCoef.minR.coef);
		buffer.ReadInteger(&CalLenCoef.paraR.hStartNode);
		buffer.ReadInteger(&CalLenCoef.paraR.hEndNode);
		buffer.ReadDouble(&CalLenCoef.paraR.coef);
	}
	else
	{
		CalLenCoef.minR.hStartNode=CalLenCoef.minR.hEndNode=CalLenCoef.paraR.hStartNode=CalLenCoef.paraR.hEndNode=0;
		buffer.ReadDouble(&CalLenCoef.minR.coef);
		buffer.ReadDouble(&CalLenCoef.paraR.coef);
		if(CalLenCoef.minR.coef==0)
			CalLenCoef.minR.coef=1.0;
		if(CalLenCoef.paraR.coef==0)
			CalLenCoef.paraR.coef=1.0;
		if(doc_type==3&&version<=1070101)
		{
			if(toupper(layer(0))!='L'&&toupper(layer(0))!='X')	//�����Ȳ�б��
				CalLenCoef.iTypeNo=0;
		}
	}
	buffer.ReadInteger(&CalLenCoef.iTypeNo);
	if(version>0&&version<1000012)
	{
		if(CalLenCoef.iTypeNo<4)
			CalLenCoef.iTypeNo=0;
		else if(CalLenCoef.iTypeNo<8)
			CalLenCoef.iTypeNo-=3;
		else //
			CalLenCoef.iTypeNo=5;	//��ָ�����㳤��ͳһ��Ϊƽ������С��ͬʱָ��һ������
	}
	buffer.ReadInteger(&CalLenCoef.iSelRFlag);
	buffer.ReadInteger(&size.idClassType);	
	buffer.ReadByte(&size.cSubClassType);
	if(size.idClassType==CLS_CABLE)
	{	//��������
		buffer.ReadString(size.wireType);
		buffer.ReadString(size.spec);
		buffer.ReadDouble(&m_fPreTension);
	}
	buffer.ReadDouble(&size.wide);
	buffer.ReadDouble(&size.thick);
	buffer.ReadByte(&_material);
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.ReadByte(&_cQualityLevel);
	buffer.ReadDword(&_uStatMatNo);			//ͳ�����
	buffer.ReadDouble(&m_fWeightRaiseCoef);	// �����Ŵ�ϵ�����������塢��˨�ȶ������ķŴ�����
	if(	version==0|| (doc_type==4&&version>=1030800))	//LDS V1.3.8.0
		buffer.ReadByte(&m_cbRodFlag);
	if(version==0||(doc_type==3&&version>=1070306)||(doc_type==4&&version>=1000306))
	{
		buffer.ReadDouble(&connectStart.maxTension);
		buffer.ReadDouble(&connectStart.maxCompression);
		buffer.ReadDouble(&connectStart.maxMoment);
		buffer.ReadDouble(&connectEnd.maxTension);
		buffer.ReadDouble(&connectEnd.maxCompression);
		buffer.ReadDouble(&connectEnd.maxMoment);
	}
	// --------- 3. ��������: ��˨��Ϣ�͸˼����� -----------
	buffer.ReadDouble(&hole_out);
	buffer.ReadInteger(&m_bAutoCalHoleOutCoef);
	buffer.ReadInteger(&connectStart.d);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Read(connectStart.grade,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Read(&connectStart.uiLegacyN,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.ReadInteger(&connectEnd.d);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Read(connectEnd.grade,4);   		//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	buffer.Read(&connectEnd.uiLegacyN,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
	if(version==0||(doc_type==3&&version>=1070201)||(doc_type==4&&version>=1000201))
	{
		int iConnType=0;
		buffer.ReadInteger(&iConnType);
		connectStart.m_iConnectType=(BYTE)iConnType;
		buffer.ReadString(connectStart.flNo);
		buffer.ReadInteger(&iConnType);
		connectEnd.m_iConnectType=(BYTE)iConnType;
		buffer.ReadString(connectEnd.flNo);
	}

	if(size.idClassType==0)
	{
		buffer.ReadDouble(&userdef.E);
		buffer.ReadDouble(&userdef.area);
		buffer.ReadDouble(&userdef.ft);
		buffer.ReadDouble(&userdef.fc);
		buffer.ReadDouble(&userdef.unit_weight);
		buffer.ReadDouble(&userdef.pre_stress);
	}

	buffer.ReadInteger(&h);
	for(int j=0;j<h;j++)
	{
		RELATIVE_OBJECT *pMirObj=relativeObjs.Add(buffer.ReadInteger());
		buffer.ReadByte(&pMirObj->mirInfo.axis_flag);
		buffer.ReadPoint(pMirObj->mirInfo.origin);
		if((pMirObj->mirInfo.axis_flag&8)>0)
		{
			buffer.ReadWord(&pMirObj->mirInfo.rotate_angle);
			buffer.ReadByte(&pMirObj->mirInfo.array_num);
		}
	}
#endif
}

void CLDSLinePart::ToPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
{
#ifdef __PART_DESIGN_INC_//defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	//����Ӱ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdToBuffer(buffer))
	{
		IndependentShadowPropToBuffer(buffer);
		return;
	}
	//Ȩ��
	buffer.WriteDword(dwPermission);
	buffer.WriteDword(dwStartPermission);
	buffer.WriteDword(dwEndPermission);
	//������Ϣ
	if(dwPermission&dwUpdateWord)
	{
		buffer.Write(cfgword.flag.word,24);
		buffer.Write(layer_tag,4);
		buffer.WriteWord(m_iElemType);
		buffer.WriteInteger(m_bNeedSelMat);
		buffer.WriteInteger(m_bNeedSelSize);
		buffer.WriteInteger(sel_mat_method); //ѡ�ķ�ʽ
		
		buffer.WriteInteger(CalLenCoef.hRefPole);
		buffer.WriteInteger(CalLenCoef.hHorizPole);
		buffer.WriteInteger(CalLenCoef.minR.hStartNode);
		buffer.WriteInteger(CalLenCoef.minR.hEndNode);
		buffer.WriteDouble(CalLenCoef.minR.coef);
		buffer.WriteInteger(CalLenCoef.paraR.hStartNode);
		buffer.WriteInteger(CalLenCoef.paraR.hEndNode);
		buffer.WriteDouble(CalLenCoef.paraR.coef);
		buffer.WriteInteger(CalLenCoef.iTypeNo);
		buffer.WriteInteger(CalLenCoef.iSelRFlag);
		buffer.WriteInteger(size_idClassType);	
		buffer.WriteByte(size_cSubClassType);
		if(size_idClassType==CLS_CABLE)
		{	//��������
			buffer.WriteString(GetWireType());
			buffer.WriteString(GetSpec());
			buffer.WriteDouble(m_fPreTension);
		}
		buffer.WriteDouble(size_wide);
		buffer.WriteDouble(size_thick);
		buffer.WriteByte(cMaterial);
		buffer.WriteByte(m_cQualityLevel);
		buffer.WriteDword(m_uStatMatNo);		//ͳ�����
		buffer.WriteDouble(m_fWeightRaiseCoef);	// �����Ŵ�ϵ�����������塢��˨�ȶ������ķŴ�����
		buffer.WriteByte(m_cbRodFlag);
		//��˨��Ϣ
		buffer.WriteDouble(hole_out);
		buffer.WriteInteger(m_bAutoCalHoleOutCoef);
		//�˼�����
		if(size_idClassType==0)
		{
			buffer.WriteDouble(userdef.E);
			buffer.WriteDouble(userdef.area);
			buffer.WriteDouble(userdef.ft);
			buffer.WriteDouble(userdef.fc);
			buffer.WriteDouble(userdef.unit_weight);
			buffer.WriteDouble(userdef.pre_stress);
		}
		//������Ϣ
		BUFFERPOP stack(&buffer,relativeObjs.GetNodeNum());
		buffer.WriteInteger(relativeObjs.GetNodeNum());
		for(RELATIVE_OBJECT *pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
		{
			buffer.WriteInteger(pMirObj->hObj);
			buffer.WriteByte(pMirObj->mirInfo.axis_flag);
			buffer.WritePoint(pMirObj->mirInfo.origin);
			if((pMirObj->mirInfo.axis_flag&8)>0)
			{
				buffer.WriteWord(pMirObj->mirInfo.rotate_angle);
				buffer.WriteByte(pMirObj->mirInfo.array_num);
			}
			stack.Increment();
		}
		if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
			CLDSObject::Log2File()->Log("The number record of relative object is wrong!");
#else
			CLDSObject::Log2File()->Log("����������¼�����ִ���!");
#endif
	}
	//ʼ����Ϣ
	if(dwStartPermission&dwUpdateWord)
	{
		if(pStart)
			buffer.WriteInteger(pStart->handle);
		else
			buffer.WriteInteger(0);
		buffer.WriteWord((WORD)start_joint_type);
		buffer.WriteWord((WORD)start_force_type);
		buffer.WriteDouble(connectStart.maxTension);
		buffer.WriteDouble(connectStart.maxCompression);
		buffer.WriteDouble(connectStart.maxMoment);
		//��˨��Ϣ
		buffer.WriteInteger(connectStart.d);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
		buffer.Write(connectStart.grade,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
		buffer.Write(&connectStart.uiLegacyN,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
		buffer.WriteInteger(connectStart.m_iConnectType);
		buffer.WriteString(connectStart.flNo);
	}
	//�ն���Ϣ
	if(dwEndPermission&dwUpdateWord)
	{
		if(pEnd)
			buffer.WriteInteger(pEnd->handle);
		else
			buffer.WriteInteger(0);
		buffer.WriteWord((WORD)end_joint_type);
		buffer.WriteWord((WORD)end_force_type);
		buffer.WriteDouble(connectEnd.maxTension);
		buffer.WriteDouble(connectEnd.maxCompression);
		buffer.WriteDouble(connectEnd.maxMoment);
		//��˨��Ϣ
		buffer.WriteInteger(connectEnd.d);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
		buffer.Write(connectEnd.grade,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
		buffer.Write(&connectEnd.uiLegacyN,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
		buffer.WriteInteger(connectEnd.m_iConnectType);
		buffer.WriteString(connectEnd.flNo);	
	}
#endif
}

void CLDSLinePart::FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
{
#ifdef __PART_DESIGN_INC_//defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	//��ȡӰ�乹�������͹���������� wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer))
	{
		IndependentShadowPropFromBuffer(buffer);
		return;
	}
	long h=0;
	//Ȩ��
	buffer.ReadDword(&dwPermission);	
	buffer.ReadDword(&dwStartPermission);
	buffer.ReadDword(&dwEndPermission);
	//������Ϣ
	if(dwPermission&dwUpdateWord)
	{
		buffer.Read(cfgword.flag.word,24);
		buffer.Read(layer_tag,4);
		layer_tag[3]='\0';
		buffer.ReadWord(&m_iElemType);
		buffer.ReadInteger(&m_bNeedSelMat);
		buffer.ReadInteger(&m_bNeedSelSize);
		buffer.ReadInteger(&h);					//����ѡ�ķ�ʽ
		sel_mat_method=AUX_POLE_SEL_METHOD(h);
		
		buffer.ReadInteger(&CalLenCoef.hRefPole);
		buffer.ReadInteger(&CalLenCoef.hHorizPole);
		buffer.ReadInteger(&CalLenCoef.minR.hStartNode);
		buffer.ReadInteger(&CalLenCoef.minR.hEndNode);
		buffer.ReadDouble(&CalLenCoef.minR.coef);
		buffer.ReadInteger(&CalLenCoef.paraR.hStartNode);
		buffer.ReadInteger(&CalLenCoef.paraR.hEndNode);
		buffer.ReadDouble(&CalLenCoef.paraR.coef);
		buffer.ReadInteger(&CalLenCoef.iTypeNo);
		buffer.ReadInteger(&CalLenCoef.iSelRFlag);
		buffer.ReadInteger(&size.idClassType);	
		buffer.ReadByte(&size.cSubClassType);
		if(size.idClassType==CLS_CABLE)
		{	//��������
			buffer.ReadString(size.wireType);
			buffer.ReadString(size.spec);
			buffer.ReadDouble(&m_fPreTension);
		}
		buffer.ReadDouble(&size.wide);
		buffer.ReadDouble(&size.thick);
		buffer.ReadByte(&_material);
		buffer.ReadByte(&_cQualityLevel);
		buffer.ReadDword(&_uStatMatNo);		//ͳ�����
		buffer.ReadDouble(&m_fWeightRaiseCoef);	// �����Ŵ�ϵ�����������塢��˨�ȶ������ķŴ�����
		buffer.ReadByte(&m_cbRodFlag);
		//��˨��Ϣ
		buffer.ReadDouble(&hole_out);
		buffer.ReadInteger(&m_bAutoCalHoleOutCoef);
		//�˼�����
		if(size.idClassType==0)
		{
			buffer.ReadDouble(&userdef.E);
			buffer.ReadDouble(&userdef.area);
			buffer.ReadDouble(&userdef.ft);
			buffer.ReadDouble(&userdef.fc);
			buffer.ReadDouble(&userdef.unit_weight);
			buffer.ReadDouble(&userdef.pre_stress);
		}
		//������Ϣ
		buffer.ReadInteger(&h);
		for(int j=0;j<h;j++)
		{
			RELATIVE_OBJECT *pMirObj=relativeObjs.Add(buffer.ReadInteger());
			buffer.ReadByte(&pMirObj->mirInfo.axis_flag);
			buffer.ReadPoint(pMirObj->mirInfo.origin);
			if((pMirObj->mirInfo.axis_flag&8)>0)
			{
				buffer.ReadWord(&pMirObj->mirInfo.rotate_angle);
				buffer.ReadByte(&pMirObj->mirInfo.array_num);
			}
		}
	}
	WORD w;
	//ʼ����Ϣ
	if(dwStartPermission&dwUpdateWord)
	{
		buffer.ReadInteger(&h);
		pStart=m_pModel->FromNodeHandle(h);
		
		buffer.ReadWord(&w);
		if(w==JOINT_HINGE)
			start_joint_type=JOINT_HINGE;
		else if(w==JOINT_RIGID)
			start_joint_type=JOINT_RIGID;
		else 
			start_joint_type=JOINT_AUTOJUSTIFY;
		buffer.ReadWord(&w);
		if(w==CENTRIC_FORCE)
			start_force_type=CENTRIC_FORCE;
		else
			start_force_type=ECCENTRIC_FORCE;
		buffer.ReadDouble(&connectStart.maxTension);
		buffer.ReadDouble(&connectStart.maxCompression);
		buffer.ReadDouble(&connectStart.maxMoment);
		//��˨��Ϣ
		buffer.ReadInteger(&connectStart.d);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
		buffer.Read(connectStart.grade,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
		buffer.Read(&connectStart.uiLegacyN,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
		int iConnType=0;
		buffer.ReadInteger(&iConnType);
		connectStart.m_iConnectType=iConnType;
		buffer.ReadString(connectStart.flNo);
	}
	//�ն���Ϣ
	if(dwEndPermission&dwUpdateWord)
	{
		buffer.ReadInteger(&h);
		pEnd=m_pModel->FromNodeHandle(h);
		
		buffer.ReadWord(&w);
		if(w==JOINT_HINGE)
			end_joint_type=JOINT_HINGE;
		else if(w==JOINT_RIGID)
			end_joint_type=JOINT_RIGID;
		else
			end_joint_type=JOINT_AUTOJUSTIFY;
		buffer.ReadWord(&w);
		if(w==CENTRIC_FORCE)
			end_force_type=CENTRIC_FORCE;
		else
			end_force_type=ECCENTRIC_FORCE;
		buffer.ReadDouble(&connectEnd.maxTension);
		buffer.ReadDouble(&connectEnd.maxCompression);
		buffer.ReadDouble(&connectEnd.maxMoment);
		//��˨��Ϣ
		buffer.ReadInteger(&connectEnd.d);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
		buffer.Read(connectEnd.grade,4);   		//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
		buffer.Read(&connectEnd.uiLegacyN,4);   	//(��ˮƽ�˼����϶˻�ˮƽ�˼���)��ʼ����˨��Ϣ
		int iConnType=0;
		buffer.ReadInteger(&iConnType);
		connectEnd.m_iConnectType=(BYTE)iConnType;
		buffer.ReadString(connectEnd.flNo);
	}
#endif
}

SIZESPEC CLDSLinePart::GetSizeSpec()
{
	CLDSLinePart* pMotherLinePart=(CLDSLinePart*)MotherPart();
	if(pMotherLinePart!=this)
		return pMotherLinePart->size;
	else
		return size;

}

void CLDSLinePart::SetSizeSpec(SIZESPEC size_info)
{
	CLDSLinePart* pMotherLinePart=(CLDSLinePart*)MotherObject();
	size=size_info;
	if(pMotherLinePart!=this)
		pMotherLinePart->size=size_info;
}

#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
bool CLDSLinePart::get_bLegVRod()
{
	if((m_cbRodFlag&0x01)==1)
		return true;
	else if(IsLegObj()&&layer(1)=='X'&&CalLenCoef.iTypeNo!=5)
		return true;
	else
		return false;
}
bool CLDSLinePart::set_bLegVRod(bool blLegVRod)
{
	if(blLegVRod)
		m_cbRodFlag|=0x01;
	else
		m_cbRodFlag&=0xfe;
	return blLegVRod;
}
bool CLDSLinePart::get_blCheckByBrace()
{
	return (m_cbRodFlag&0x02)>0;
}
bool CLDSLinePart::set_blCheckByBrace(bool blBraceRod)
{
	if(blBraceRod)
		m_cbRodFlag|=0x02;
	else
		m_cbRodFlag&=0xfd;
	return blBraceRod;
}
//���ص�ǰ���ϵĵ���ģ��ֵ(MPa)
double CLDSLinePart::GetElasticityModulus(double para/*=1.0*/)
{
	if(library)
		return library->GetElasticityModulus(cMaterial);
	/*else if(guige.pole_type==TRUSS_POLE_USERDEF)
		return userdef.E;*/
	else
		return 2.06e5;
}
//���ظ˼�ָ�����Ǹ�ʱpara=0ƽ�����para=1��С�ᣩ���Ծ�(mm4)
double CLDSLinePart::GetWorkI(int para/*=0*/)
{
	if (para != 0 && para != 1)
	{	//���ݸ˼��������С��ƽ���������Զ��ж�
		para = 0;
		if (CalLenCoef.iTypeNo == 5)
		{
			if (CalLenCoef.minR.coef > EPS&&CalLenCoef.paraR.coef<EPS)
				para = 1;
		}
	}
	if(library)
		return library->GetWorkI(size_wide,size_thick,0,para,size_idClassType,size_cSubClassType);
	else
		return 0;
}
//���ظ˼�ָ�����Ǹ�ʱpara=0ƽ�����para=1��С�ᣩ��ת�뾶(mm4)
double CLDSLinePart::GetWorkR(int para/*=0*/)
{
	if(library)
		return library->GetWorkR(size_wide,size_thick,0,para,size_idClassType,size_cSubClassType);
	else
		return 0;
}
//���ظ˼�ָ�����Ǹ�ʱpara=0ƽ�����para=1��С�ᣩ��ת�뾶(mm3)
double CLDSLinePart::GetWorkW(int para/*=0*/)
{
	if(library)
		return library->GetW(size_wide,size_thick,0,para,size_idClassType,size_cSubClassType);
	else
		return 0;
}
double CLDSLinePart::CalSpecLamdaLenMinR()
{
	if(m_pModel==NULL)
		return CalLenCoef.minR.coef;
	BOOL bPush=m_pModel->PushNodeStack();
	CLDSNode *pTempStart=m_pModel->FromNodeHandle(CalLenCoef.minR.hStartNode);
	CLDSNode *pTempEnd=m_pModel->FromNodeHandle(CalLenCoef.minR.hEndNode);
	if(bPush)
		m_pModel->PopNodeStack();
	if(pTempStart&&pTempEnd)
		return DISTANCE(pTempStart->Position(false),pTempEnd->Position(false))*CalLenCoef.minR.coef;
	else
		return DISTANCE(pStart->Position(false),pEnd->Position(false))*CalLenCoef.minR.coef;
}
double CLDSLinePart::CalSpecLamdaLenParaR()
{
	if(m_pModel==NULL)
		return CalLenCoef.paraR.coef;
	BOOL bPush=m_pModel->PushNodeStack();
	CLDSNode *pTempStart=m_pModel->FromNodeHandle(CalLenCoef.paraR.hStartNode);
	CLDSNode *pTempEnd=m_pModel->FromNodeHandle(CalLenCoef.paraR.hEndNode);
	if(bPush)
		m_pModel->PopNodeStack();
	if(pTempStart&&pTempEnd)
		return DISTANCE(pTempStart->Position(false),pTempEnd->Position(false))*CalLenCoef.paraR.coef;
	else
		return DISTANCE(pStart->Position(false),pEnd->Position(false))*CalLenCoef.paraR.coef;
}
#if defined(__TSA_)||defined(__LDS_)	//���õ���GetSysPath����������������MapLma.arx�е���
//����ר�ò�ѯ����
//���½����,mm2
double CLDSLinePart::GetWireArea()
{
	if(library)
		return library->GetWireArea(GetWireType(),GetSpec());
	else
		return 0;
}
//��λ������,kg/km
double CLDSLinePart::GetWireUnitWeight()
{
	if(library)
		return library->GetWireUnitWeightPerKM(GetWireType(),GetSpec());
	else
		return 0;
}
//�������,N
double CLDSLinePart::GetWireForcePermit()
{
	if(library)
		return library->GetWireForcePermit(GetWireType(),GetSpec());
	else
		return 0;
}
//����ϵ��(ģ��),MPa
double CLDSLinePart::GetWireElasticCoef()
{
	if(library)
		return library->GetWireElasticCoef(GetWireType(),GetSpec());
	else
		return 0;
}
//�¶�������ϵ��,
double CLDSLinePart::GetWireExpansionCoef()
{
	if(library)
		return library->GetWireExpansionCoef(GetWireType(),GetSpec());
	else
		return 0;
}
#endif
extern int MirPointI(int quad,MIRMSG msg);
void swap_pointI(int& startI,int& endI)
{
	int temp=startI;
	startI=endI;
	endI=temp;
}
BOOL CLDSLinePart::IsFatherObject()
{
	RELATIVE_OBJECT *pMirObj=NULL;
	f3dPoint startPos,endPos;
	int quad_start,quad_end,quad_mir_start,quad_mir_end;
	bool bRevert=false;
	if(pStart->point_i>pEnd->point_i)
		bRevert=true;
	if(!bRevert)
	{
		startPos=pStart->Position(false);
		endPos=pEnd->Position(false);
		quad_start=pStart->point_i;
		quad_end=pEnd->point_i;
	}
	else
	{	//����С����ʼ��
		endPos=pStart->Position(false);
		startPos=pEnd->Position(false);
		quad_start=pEnd->point_i;
		quad_end=pStart->point_i;
	}
	for(pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		if(pMirObj->mirInfo.axis_flag==0x01)	//X��Գ�
		{
			if(fabs(startPos.y)<EPS)	//X�����϶Գƽڵ����Լ�
				quad_mir_start=quad_start;
			else
				quad_mir_start=MirPointI(quad_start,pMirObj->mirInfo);
			if(fabs(endPos.y)<EPS)
				quad_mir_end=quad_end;
			else
				quad_mir_end=MirPointI(quad_end,pMirObj->mirInfo);
			if(quad_mir_start<0||quad_mir_end<0)
				continue;	//��������
			if(quad_mir_start>quad_mir_end)
				swap_pointI(quad_mir_start,quad_mir_end);
			if(quad_start>quad_mir_start||(quad_start==quad_mir_start&&quad_end>quad_mir_end))
				return FALSE;	//�и�С��ŵĹ�������
		}
		else if(pMirObj->mirInfo.axis_flag==0x02)	//Y��Գ�
		{
			if(fabs(startPos.x)<EPS)	//X�����϶Գƽڵ����Լ�
				quad_mir_start=quad_start;
			else
				quad_mir_start=MirPointI(quad_start,pMirObj->mirInfo);
			if(fabs(endPos.x)<EPS)
				quad_mir_end=quad_end;
			else
				quad_mir_end=MirPointI(quad_end,pMirObj->mirInfo);
			if(quad_mir_start<0||quad_mir_end<0)
				continue;	//��������
			if(quad_mir_start>quad_mir_end)
				swap_pointI(quad_mir_start,quad_mir_end);
			if(quad_start>quad_mir_start||(quad_start==quad_mir_start&&quad_end>quad_mir_end))
				return FALSE;	//�и�С��ŵĹ�������
		}
		else if(pMirObj->mirInfo.axis_flag==0x04)	//Z��Գ�
		{
			quad_mir_start=MirPointI(quad_start,pMirObj->mirInfo);
			quad_mir_end=MirPointI(quad_end,pMirObj->mirInfo);
			/*if(bRevert)
			{
				int temp=quad_mir_start;
				quad_mir_start=quad_mir_end;
				quad_mir_end=temp;
			}*/
			if(quad_mir_start<0||quad_mir_end<0)
				continue;	//��������
			if(quad_mir_start>quad_mir_end)
				swap_pointI(quad_mir_start,quad_mir_end);
			if(quad_start>quad_mir_start||(quad_start==quad_mir_start&&quad_end>quad_mir_end))
				return FALSE;	//�и�С��ŵĹ�������
		}
	}
	return TRUE;
}
BOOL CLDSLinePart::GetFatherMirObj(RELATIVE_OBJECT *pRelaObj)
{
	RELATIVE_OBJECT *pMirObj=NULL;
	RELATIVE_OBJECT relaFatherObj;
	relaFatherObj.hObj=handle;
	f3dPoint startPos,endPos;
	int quad_start,quad_end,quad_mir_start,quad_mir_end,quad_start_min,quad_end_min;
	if(pStart->point_i<pEnd->point_i)
	{
		startPos=pStart->Position(false);
		endPos=pEnd->Position(false);
		quad_start_min=quad_start=pStart->point_i;
		quad_end_min=quad_end=pEnd->point_i;
	}
	else
	{	//����С����ʼ��
		endPos=pStart->Position(false);
		startPos=pEnd->Position(false);
		quad_end_min=quad_end=pStart->point_i;
		quad_start_min=quad_start=pEnd->point_i;
	}

	for(pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		if(pMirObj->mirInfo.axis_flag&0x08)	//��ת�Գ�����
			continue;
		if(pMirObj->mirInfo.axis_flag==0x01)	//X��Գ�
		{
			if(fabs(startPos.y)<EPS)	//X�����϶Գƽڵ����Լ�
				quad_mir_start=quad_start;
			else
				quad_mir_start=MirPointI(quad_start,pMirObj->mirInfo);
			if(fabs(endPos.y)<EPS)
				quad_mir_end=quad_end;
			else
				quad_mir_end=MirPointI(quad_end,pMirObj->mirInfo);
			if(quad_mir_start<0||quad_mir_end<0)
				continue;	//��������
			if(quad_mir_start>quad_mir_end)
				swap_pointI(quad_mir_start,quad_mir_end);
			if(quad_start_min>quad_mir_start||(quad_start_min==quad_mir_start&&quad_end_min>quad_mir_end))
			{
				quad_start_min=quad_mir_start;
				quad_end_min=quad_mir_end;
				relaFatherObj = *pMirObj;	//�и�С��ŵĹ�������
			}
		}
		else if(pMirObj->mirInfo.axis_flag==0x02)	//Y��Գ�
		{
			if(fabs(startPos.x)<EPS)	//X�����϶Գƽڵ����Լ�
				quad_mir_start=quad_start;
			else
				quad_mir_start=MirPointI(quad_start,pMirObj->mirInfo);
			if(fabs(endPos.x)<EPS)
				quad_mir_end=quad_end;
			else
				quad_mir_end=MirPointI(quad_end,pMirObj->mirInfo);
			if(quad_mir_start<0||quad_mir_end<0)
				continue;	//��������
			if(quad_mir_start>quad_mir_end)
				swap_pointI(quad_mir_start,quad_mir_end);
			if(quad_start_min>quad_mir_start||(quad_start_min==quad_mir_start&&quad_end_min>quad_mir_end))
			{
				quad_start_min=quad_mir_start;
				quad_end_min=quad_mir_end;
				relaFatherObj = *pMirObj;	//�и�С��ŵĹ�������
			}
		}
		else if(pMirObj->mirInfo.axis_flag==0x04)	//Z��Գ�
		{
			quad_mir_start=MirPointI(quad_start,pMirObj->mirInfo);
			quad_mir_end=MirPointI(quad_end,pMirObj->mirInfo);
			if(quad_mir_start<0||quad_mir_end<0)
				continue;	//��������
			if(quad_mir_start>quad_mir_end)
				swap_pointI(quad_mir_start,quad_mir_end);
			if(quad_start_min>quad_mir_start||(quad_start_min==quad_mir_start&&quad_end_min>quad_mir_end))
			{
				quad_start_min=quad_mir_start;
				quad_end_min=quad_mir_end;
				relaFatherObj = *pMirObj;	//�и�С��ŵĹ�������
			}
		}
	}
	if(relaFatherObj.mirInfo.axis_flag==0)
		return FALSE;
	else if(pRelaObj)
		*pRelaObj=relaFatherObj;
	return TRUE;
}
#endif
BOOL CLDSLinePart::IsAuxPole()	//�Ƿ��Ǹ�����
{
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
	if( (layer_tag[1]=='F'||layer_tag[1]=='f') &&
		(sysPara==NULL||!sysPara->m_bAuxPoleAnalysisInc))
#else
	if( layer_tag[1]=='F'||layer_tag[1]=='f')
#endif
		return TRUE;
	else
		return FALSE;
}
BOOL CLDSLinePart::IsPostRod()	//�Ƿ��Ǹ�����
{
	if(toupper(layer_tag[1])=='Z')
		return TRUE;
	else
		return FALSE;
}
BOOL CLDSLinePart::IsPrimaryRod(bool bOnlyBevelRod/*=false*/)	//�Ƿ�Ϊ����ͨ������(��б�ĺ�ĵ�)
{
	if(toupper(layer_tag[1])=='X')
		return TRUE;
	else if(bOnlyBevelRod||IsAuxPole())
		return FALSE;	//������б��ʱ����Ĳ���Ϊ��Ҫ������(Primary)�˼�
	else	//��ĵ�������
		return TRUE;
}

UCS_STRU CLDSLinePart::GetUnifiedACS()
{	//���㹹��װ������ϵʱ���Ǹ˼���ͷ���ְ巨��ƫ���� wjh 16-1-03
	UCS_STRU acs=BuildUCS();
	acs.origin-=acs.axis_z*startOdd();
	return acs;
}
//��λΪmm2
double CLDSLinePart::GetArea()
{
#if defined(__TSA_)||defined(__LDS_)	//���õ���GetSysPath����������������MapLma.arx�е���
	if(size_idClassType==CLS_CABLE)
		return GetWireArea();
	else 
#endif
	if(library)
		return library->GetPartArea(size_wide,size_thick,size_height,size_idClassType,size_cSubClassType);
	else
		return 0.0;
}
double CLDSLinePart::GetWeight(BOOL bSolidLength/*=TRUE*/)	//��λΪkg
{
	double unit_weight=0;
	if(library)
		unit_weight=library->GetWeightOf1Meter(size_wide,size_thick,size_height,size_idClassType,size_cSubClassType);
	else
		unit_weight=GetArea()*7.85e-3;
	double length;
	if(bSolidLength||pStart==NULL||pEnd==NULL)
		length=GetLength();
	else
		length=DISTANCE(pStart->Position(false),pEnd->Position(false));
	return length*0.001*unit_weight;
}
char* CLDSLinePart::GetBriefPartTypeMark()
{
	CLDSPart *pMotherPart=MotherPart(false);
	if(pMotherPart!=this&&pMotherPart->IsLinePart())
		return ((CLDSLinePart*)pMotherPart)->GetBriefPartTypeMark();
	if(size_idClassType==CLS_LINEANGLE)
		return "L";
	else if(size_idClassType==CLS_GROUPLINEANGLE)
	{
		if(size_cSubClassType=='D')
			return "DL";
		else if(size_cSubClassType=='T')
			return "TL";
		else if(size_cSubClassType=='X')
			return "XL";
		else
			return "?L";
	}
	else if(size_idClassType==CLS_LINETUBE)
		return "��";
	else if(size_idClassType==CLS_LINEFLAT)
		return "��";
	else if(size_idClassType==CLS_LINESLOT)
		return "[";
	else
		return "U";
}
int CLDSLinePart::GetSpecification(char *spec,BOOL bIncMat/*=TRUE*/)
{
	CLDSPart *pMotherPart=MotherPart(false);
	if(pMotherPart!=this&&pMotherPart->IsLinePart())
		return ((CLDSLinePart*)pMotherPart)->GetSpecification(spec,bIncMat);
	char steelmark[20]="";
	char tmp_spec[200],tmp_spec2[200];
	if(bIncMat&&cMaterial!='S')
		QuerySteelMatMark(cMaterial,steelmark);
	if( GetClassTypeId()==CLS_LINEANGLE||size_idClassType==CLS_LINEANGLE||
		GetClassTypeId()==CLS_GROUPLINEANGLE||size_idClassType==CLS_GROUPLINEANGLE)
	{
		if(size_cSubClassType!='L')
			_snprintf(tmp_spec,199,"%s%CL%f",steelmark,size_cSubClassType,size_wide);
		else
			_snprintf(tmp_spec,199,"%s L%f",steelmark,size_wide);
		SimplifiedNumString(tmp_spec);
	}
	else
	{
		_snprintf(tmp_spec,199,"%s%s%f",steelmark,GetBriefPartTypeMark(),size_wide);
		SimplifiedNumString(tmp_spec);
	}
	_snprintf(tmp_spec2,199,"%sx%f",tmp_spec,size_thick);
	SimplifiedNumString(tmp_spec2);
	if(spec)
		strcpy(spec,tmp_spec2);
	return strlen(tmp_spec2);
}

double CLDSLinePart::startOdd(bool bToFL/*=false*/)// const
{
	CLDSPart *pMotherPart=MotherPart(false);
	if(pMotherPart!=this&&pMotherPart->IsLinePart())
		return ((CLDSLinePart*)pMotherPart)->startOdd(bToFL);
	return start_oddment;
}
double CLDSLinePart::endOdd(bool bToFL/*=false*/)// const
{
	CLDSPart *pMotherPart=MotherPart(false);
	if(pMotherPart!=this&&pMotherPart->IsLinePart())
		return ((CLDSLinePart*)pMotherPart)->endOdd(bToFL);
	return end_oddment;
}
double CLDSLinePart::GetWidth()
{
	CLDSPart *pMotherPart=MotherPart(false);
	if(pMotherPart!=this)
		return pMotherPart->GetWidth();
	return size_wide;
}	//��λΪmm
double CLDSLinePart::GetThick()
{
	CLDSPart *pMotherPart=MotherPart(false);
	if(pMotherPart!=this)
		return pMotherPart->GetThick();
	return size_thick;
}
double CLDSLinePart::GetThick2()
{
	CLDSPart *pMotherPart=MotherPart(false);
	if(pMotherPart!=this)
		return pMotherPart->GetThick2();
	return size_thick2;
}
//��λΪmm
double CLDSLinePart::GetHeight()
{
	CLDSPart *pMotherPart=MotherPart(false);
	if(pMotherPart!=this)
		return pMotherPart->GetHeight();
	return size_height;
}	//��λΪmm
double CLDSLinePart::GetLength()
{
	CLDSPart *pMotherPart=MotherPart(false);
	if(pMotherPart!=this)
		return pMotherPart->GetLength();
	f3dPoint vec;
	if(GetClassTypeId()==CLS_LINEPART&&pStart&&pEnd)
		vec=pEnd->Position(false)-pStart->Position(false);
	else
		vec=End()-Start();
#ifndef __TSA_FILE_
	if(bHuoQuStart)
		start_oddment=0;
	if(bHuoQuEnd)
		end_oddment=0;
	double len = vec.mod()+start_oddment+end_oddment;
	return len;
#else
	return vec.mod();
#endif
}
void CLDSLinePart::TransToCS(GECS& cs)
{
	baseline.CoordTrans(UCS_STRU(cs),false);
	SetModified();
}
void CLDSLinePart::Offset(const double* offset)
{
	GEPOINT offset3d(offset);
	baseline.SetStart(baseline.Start()+offset3d);
	baseline.SetEnd(baseline.End()+offset3d);
	SetModified();
}
void CLDSLinePart::CopyProperty(CLDSDbObject* pObj)
{
	CSuperSmartPtr<CLDSPart>pPart;
	if(!pObj->IsPart())
		return;
	pPart=(CLDSPart*)pObj;
	if(!pPart->IsLinePart())
		return;
    size = pPart.LinePartPointer()->size;
    SetLayer(pPart->layer());						// ���Ǹ�����ͼ��.
	iSeg = pPart->iSeg;
	m_bVirtualPart = pPart->m_bVirtualPart;
	is_data_modified = TRUE;
	cMaterial = pPart->cMaterial;
	cfgword = pPart->cfgword;
#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__TSA_)
	m_fWeightRaiseCoef=pPart.LinePartPointer()->m_fWeightRaiseCoef;
	m_fPreTension=pPart.LinePartPointer()->m_fPreTension;
	m_uStatMatNo=pPart.LinePartPointer()->m_uStatMatNo;
	m_bNeedSelSize=pPart.LinePartPointer()->m_bNeedSelSize;
	start_force_type=pPart.LinePartPointer()->start_force_type;
	end_force_type=pPart.LinePartPointer()->end_force_type;
	start_joint_type=pPart.LinePartPointer()->start_joint_type;
	end_joint_type=pPart.LinePartPointer()->end_joint_type;
	CalLenCoef.iTypeNo=pPart.LinePartPointer()->CalLenCoef.iTypeNo;
	hole_out=pPart.LinePartPointer()->hole_out;
	m_bAutoCalHoleOutCoef=pPart.LinePartPointer()->m_bAutoCalHoleOutCoef;
	m_cbRodFlag=pPart.pRod->m_cbRodFlag;
#endif

}

f3dPoint CLDSLinePart::GetMirPickPos(int start0_end1)
{
	if(start0_end1==0)
	{	//ʼ��
		if(pStart&&pEnd)
			return pStart->Position(false);
		else
			return Start();
	}
	else //if(start0_end1==1)
	{	//�ն�
		if(pStart&&pEnd)
			return pEnd->Position(false);
		else
			return End();
	}
}
void CLDSLinePart::UpdateRodEndNodeRelation(CLDSNode* pOldNode,CLDSNode *pNewNode)
{
	if(pOldNode==NULL||pNewNode==NULL||pOldNode==pNewNode)
		return;
	BOOL pushed=BelongModel()->PushNodeStack();
	for(CLDSNode* pNode=BelongModel()->EnumNodeFirst();pNode;pNode=BelongModel()->EnumNodeNext())
	{	
		if(pNode->m_cPosCalType==0 || pNode==pOldNode || pNode==pNewNode)
			continue;
		if(pNode->m_cPosCalType==4)
		{	//����ڵ�
			if(pNode->arrRelationPole[0]!=this->handle&&pNode->arrRelationPole[1]!=this->handle)
				continue;
			for(int i=0;i<4;i++)
			{
				if(pNode->arrRelationNode[i]==pOldNode->handle)
				{
					pNode->arrRelationNode[i]=pNewNode->handle;
					break;
				}
			}
		}
		else if(pNode->hFatherPart==this->handle)
		{	//��ǰ���߸˼��ϵ������ڵ�
			for(int i=0;i<4;i++)
			{
				if(pNode->arrRelationNode[i]==pOldNode->handle)
				{
					pNode->arrRelationNode[i]=pNewNode->handle;
					break;
				}
			}
		}
	}
	BelongModel()->PopNodeStack(pushed);
	//����CLDSPoint����ص��滻��Ӧ����ϵͳȫ�ֵļ����𹹼��ֲ��Ĳ������㣬��������CLDSPoint::ReplaceRodEndNode������wjh-2018.1.25
	for(CLDSPoint* pPoint=BelongModel()->EnumPointFirst();pPoint;pPoint=BelongModel()->EnumPointNext())
		pPoint->ReplaceRodEndNode(handle,pOldNode->handle,pNewNode->handle);
	CLdsPartListStack stack(BelongModel());
	for(SmartPartPtr pPart=BelongModel()->EnumPartFirst();pPart.IsHasPtr();pPart=BelongModel()->EnumPartNext())
	{
		if(pPart->IsAngle())
		{
			if(pPart.pAngle->desStartPos.spatialOperationStyle==0)
				pPart.pAngle->desStartPos.datumPoint.ReplaceRodEndNode(handle,pOldNode->handle,pNewNode->handle);
			else if(pPart.pAngle->desStartPos.spatialOperationStyle==16||pPart.pAngle->desStartPos.spatialOperationStyle==17)
			{
				pPart.pAngle->desStartPos.datumLineStart.ReplaceRodEndNode(handle,pOldNode->handle,pNewNode->handle);
				pPart.pAngle->desStartPos.datumLineEnd.ReplaceRodEndNode(handle,pOldNode->handle,pNewNode->handle);
			}
			if(pPart.pAngle->desEndPos.spatialOperationStyle==0)
				pPart.pAngle->desEndPos.datumPoint.ReplaceRodEndNode(handle,pOldNode->handle,pNewNode->handle);
			else if(pPart.pAngle->desEndPos.spatialOperationStyle==16||pPart.pAngle->desEndPos.spatialOperationStyle==17)
			{
				pPart.pAngle->desEndPos.datumLineStart.ReplaceRodEndNode(handle,pOldNode->handle,pNewNode->handle);
				pPart.pAngle->desEndPos.datumLineEnd.ReplaceRodEndNode(handle,pOldNode->handle,pNewNode->handle);
			}
		}
		else if(pPart->GetClassTypeId()==CLS_BOLT)
			pPart.pBolt->des_base_pos.datumPoint.ReplaceRodEndNode(handle,pOldNode->handle,pNewNode->handle);
		else if(pPart->GetClassTypeId()==CLS_PLATE)
			pPart.pPlate->designInfo.origin.ReplaceRodEndNode(handle,pOldNode->handle,pNewNode->handle);
	}
}
void CLDSLinePart::UpdateMirLinePartEndNode(MIRMSG mirmsg,CLDSNode *pNewStart,CLDSNode *pNewEnd)
{
	int flagArr[3]={1,2,4};
	CLDSLinePart* pMirLinePart=NULL;
	for(int i=0;i<3;i++)
	{
		if((mirmsg.axis_flag&flagArr[i])==0)
			continue;
		MIRMSG msg=mirmsg.GetSubMirItem(i+1);
		pMirLinePart=(CLDSLinePart*)GetMirPart(msg);
		if(pMirLinePart==this)
			pMirLinePart=NULL;
		if(pMirLinePart!=NULL)
		{	//����ʼĩ�˽ڵ�Գƹ�����Ϣʱ������������relativeObjs�д��ڵĶԳ���ϢΪ�����������ͬʱ�߱�X/Y��Գ���Z��ԳƷ������� wjh-2016.7.6
			RELATIVE_OBJECT* pRelaObj=relativeObjs.GetValue(pMirLinePart->handle);
			if(pRelaObj&&!pRelaObj->mirInfo.IsSameMirMsg(msg))
				pMirLinePart=NULL;
		}
		if(pMirLinePart==NULL)
			continue;
		CLDSNode* pMirStart=NULL,*pMirEnd=NULL,*pOldNode=NULL;
		if(pNewStart)
		{
			pOldNode=pMirLinePart->pStart;
			pMirStart=pNewStart->GetMirNode(msg);
			if(pMirStart)
			{
				pMirLinePart->pStart=pMirStart;
				pMirLinePart->UpdateRodEndNodeRelation(pOldNode,pMirStart);
			}
		}
		if(pNewEnd)
		{
			pOldNode=pMirLinePart->pEnd;
			pMirEnd=pNewEnd->GetMirNode(msg);
			if(pMirEnd)
			{
				pMirLinePart->pEnd=pMirEnd;
				pMirLinePart->UpdateRodEndNodeRelation(pOldNode,pMirEnd);
			}
		}
	}
}


void CLDSPart::GetArcSimuPolyVertex(f3dArcLine* pArcLine, f3dPoint vertex_arr[], int slices)
{
	int i;
	double slice_angle = pArcLine->SectorAngle()/slices;
	if(pArcLine->WorkNorm()==pArcLine->ColumnNorm())	//Բ��
	{
		double sina = sin(slice_angle);	//��Ƭ������
		double cosa = cos(slice_angle);	//��Ƭ������
		vertex_arr[0].Set(pArcLine->Radius());
		for(i=1;i<=slices;i++)
		{
			vertex_arr[i].x=vertex_arr[i-1].x*cosa-vertex_arr[i-1].y*sina;
			vertex_arr[i].y=vertex_arr[i-1].y*cosa+vertex_arr[i-1].x*sina;
		}
		GEPOINT origin=pArcLine->Center();
		f3dPoint axis_x=pArcLine->Start()-origin;
		normalize(axis_x);
		f3dPoint axis_y=pArcLine->WorkNorm()^axis_x;
		for(i=0;i<=slices;i++)
		{
			vertex_arr[i].Set(	origin.x+vertex_arr[i].x*axis_x.x+vertex_arr[i].y*axis_y.x,
				origin.y+vertex_arr[i].x*axis_x.y+vertex_arr[i].y*axis_y.y,
				origin.z+vertex_arr[i].x*axis_x.z+vertex_arr[i].y*axis_y.z);
		}
	}
	else	//��Բ��
	{
		for(i=0;i<=slices;i++)
			vertex_arr[i] = pArcLine->PositionInAngle(i*slice_angle);
	}
}
struct SLICE{double radAngle,sina,cosa;};
SLICE SLICE6_ARR[6]={
	{0.000000, 0.0000000000, 1.0000000000},	{1.047198, 0.8660254038, 0.5000000000},
	{2.094395, 0.8660254038,-0.5000000000},	{3.141593, 0.0000000000,-1.0000000000},
	{4.188790,-0.8660254038,-0.5000000000},	{5.235988,-0.8660254038, 0.5000000000},
};
SLICE SLICE8_ARR[8]={
	{0.000000, 0.0000000000, 1.0000000000},	{0.785398, 0.7071067812, 0.7071067812},
	{1.570796, 1.0000000000, 0.0000000000},	{2.356194, 0.7071067812,-0.7071067812},
	{3.141593, 0.0000000000,-1.0000000000},	{3.926991,-0.7071067812,-0.7071067812},
	{4.712389,-1.0000000000,-0.0000000000},	{5.497787,-0.7071067812, 0.7071067812},
};
SLICE SLICE12_ARR[12]={
	{0.000000, 0.0000000000, 1.0000000000},	{0.523599, 0.5000000000, 0.8660254038},
	{1.047198, 0.8660254038, 0.5000000000},	{1.570796, 1.0000000000, 0.0000000000},
	{2.094395, 0.8660254038,-0.5000000000},	{2.617994, 0.5000000000,-0.8660254038},
	{3.141593, 0.0000000000,-1.0000000000},	{3.665191,-0.5000000000,-0.8660254038},
	{4.188790,-0.8660254038,-0.5000000000},	{4.712389,-1.0000000000,-0.0000000000},
	{5.235988,-0.8660254038, 0.5000000000},	{5.759587,-0.5000000000, 0.8660254038},
};
SLICE SLICE16_ARR[16]={
	{0.000000, 0.0000000000, 1.0000000000},	{0.392699, 0.3826834324, 0.9238795325},
	{0.785398, 0.7071067812, 0.7071067812},	{1.178097, 0.9238795325, 0.3826834324},
	{1.570796, 1.0000000000, 0.0000000000},	{1.963495, 0.9238795325,-0.3826834324},
	{2.356194, 0.7071067812,-0.7071067812},	{2.748894, 0.3826834324,-0.9238795325},
	{3.141593, 0.0000000000,-1.0000000000},	{3.534292,-0.3826834324,-0.9238795325},
	{3.926991,-0.7071067812,-0.7071067812},	{4.319690,-0.9238795325,-0.3826834324},
	{4.712389,-1.0000000000,-0.0000000000},	{5.105088,-0.9238795325, 0.3826834324},
	{5.497787,-0.7071067812, 0.7071067812},	{5.890486,-0.3826834324, 0.9238795325}
};
SLICE SLICE24_ARR[24]={
	{0.000000, 0.0000000000, 1.0000000000},	{0.261799, 0.2588190451, 0.9659258263},
	{0.523599, 0.5000000000, 0.8660254038},	{0.785398, 0.7071067812, 0.7071067812},
	{1.047198, 0.8660254038, 0.5000000000},	{1.308997, 0.9659258263, 0.2588190451},
	{1.570796, 1.0000000000, 0.0000000000},	{1.832596, 0.9659258263,-0.2588190451},
	{2.094395, 0.8660254038,-0.5000000000},	{2.356194, 0.7071067812,-0.7071067812},
	{2.617994, 0.5000000000,-0.8660254038},	{2.879793, 0.2588190451,-0.9659258263},
	{3.141593, 0.0000000000,-1.0000000000},	{3.403392,-0.2588190451,-0.9659258263},
	{3.665191,-0.5000000000,-0.8660254038},	{3.926991,-0.7071067812,-0.7071067812},
	{4.188790,-0.8660254038,-0.5000000000},	{4.450590,-0.9659258263,-0.2588190451},
	{4.712389,-1.0000000000,-0.0000000000},	{4.974188,-0.9659258263, 0.2588190451},
	{5.235988,-0.8660254038, 0.5000000000},	{5.497787,-0.7071067812, 0.7071067812},
	{5.759587,-0.5000000000, 0.8660254038},	{6.021386,-0.2588190451, 0.9659258263},
};
SLICE SLICE32_ARR[32]={
	{0.000000, 0.0000000000, 1.0000000000},{0.196350, 0.1950903220, 0.9807852804},
	{0.392699, 0.3826834324, 0.9238795325},{0.589049, 0.5555702330, 0.8314696123},
	{0.785398, 0.7071067812, 0.7071067812},{0.981748, 0.8314696123, 0.5555702330},
	{1.178097, 0.9238795325, 0.3826834324},{1.374447, 0.9807852804, 0.1950903220},
	{1.570796, 1.0000000000, 0.0000000000},{1.767146, 0.9807852804,-0.1950903220},
	{1.963495, 0.9238795325,-0.3826834324},{2.159845, 0.8314696123,-0.5555702330},
	{2.356194, 0.7071067812,-0.7071067812},{2.552544, 0.5555702330,-0.8314696123},
	{2.748894, 0.3826834324,-0.9238795325},{2.945243, 0.1950903220,-0.9807852804},
	{3.141593, 0.0000000000,-1.0000000000},{3.337942,-0.1950903220,-0.9807852804},
	{3.534292,-0.3826834324,-0.9238795325},{3.730641,-0.5555702330,-0.8314696123},
	{3.926991,-0.7071067812,-0.7071067812},{4.123340,-0.8314696123,-0.5555702330},
	{4.319690,-0.9238795325,-0.3826834324},{4.516039,-0.9807852804,-0.1950903220},
	{4.712389,-1.0000000000,-0.0000000000},{4.908739,-0.9807852804, 0.1950903220},
	{5.105088,-0.9238795325, 0.3826834324},{5.301438,-0.8314696123, 0.5555702330},
	{5.497787,-0.7071067812, 0.7071067812},{5.694137,-0.5555702330, 0.8314696123},
	{5.890486,-0.3826834324, 0.9238795325},{6.086836,-0.1950903220, 0.9807852804}};
SLICE SLICE48_ARR[48]={
	{0.000000, 0.0000000000, 1.0000000000},{0.130900, 0.1305261922, 0.9914448614},
	{0.261799, 0.2588190451, 0.9659258263},{0.392699, 0.3826834324, 0.9238795325},
	{0.523599, 0.5000000000, 0.8660254038},{0.654498, 0.6087614290, 0.7933533403},
	{0.785398, 0.7071067812, 0.7071067812},{0.916298, 0.7933533403, 0.6087614290},
	{1.047198, 0.8660254038, 0.5000000000},{1.178097, 0.9238795325, 0.3826834324},
	{1.308997, 0.9659258263, 0.2588190451},{1.439897, 0.9914448614, 0.1305261922},
	{1.570796, 1.0000000000, 0.0000000000},{1.701696, 0.9914448614,-0.1305261922},
	{1.832596, 0.9659258263,-0.2588190451},{1.963495, 0.9238795325,-0.3826834324},
	{2.094395, 0.8660254038,-0.5000000000},{2.225295, 0.7933533403,-0.6087614290},
	{2.356194, 0.7071067812,-0.7071067812},{2.487094, 0.6087614290,-0.7933533403},
	{2.617994, 0.5000000000,-0.8660254038},{2.748894, 0.3826834324,-0.9238795325},
	{2.879793, 0.2588190451,-0.9659258263},{3.010693, 0.1305261922,-0.9914448614},
	{3.141593, 0.0000000000,-1.0000000000},{3.272492,-0.1305261922,-0.9914448614},
	{3.403392,-0.2588190451,-0.9659258263},{3.534292,-0.3826834324,-0.9238795325},
	{3.665191,-0.5000000000,-0.8660254038},{3.796091,-0.6087614290,-0.7933533403},
	{3.926991,-0.7071067812,-0.7071067812},{4.057891,-0.7933533403,-0.6087614290},
	{4.188790,-0.8660254038,-0.5000000000},{4.319690,-0.9238795325,-0.3826834324},
	{4.450590,-0.9659258263,-0.2588190451},{4.581489,-0.9914448614,-0.1305261922},
	{4.712389,-1.0000000000,-0.0000000000},{4.843289,-0.9914448614, 0.1305261922},
	{4.974188,-0.9659258263, 0.2588190451},{5.105088,-0.9238795325, 0.3826834324},
	{5.235988,-0.8660254038, 0.5000000000},{5.366887,-0.7933533403, 0.6087614290},
	{5.497787,-0.7071067812, 0.7071067812},{5.628687,-0.6087614290, 0.7933533403},
	{5.759587,-0.5000000000, 0.8660254038},{5.890486,-0.3826834324, 0.9238795325},
	{6.021386,-0.2588190451, 0.9659258263},{6.152286,-0.1305261922, 0.9914448614}};

void CLDSPart::GetCircleSimuPolyVertex(double radius, f3dPoint vertex_arr[], int slices)
{
	SLICE* pPresetArr=NULL;
	if(slices==6)
		pPresetArr=SLICE6_ARR;
	else if(slices==8)
		pPresetArr=SLICE8_ARR;
	else if(slices==12)
		pPresetArr=SLICE12_ARR;
	else if(slices==16)
		pPresetArr=SLICE16_ARR;
	else if(slices==24)
		pPresetArr=SLICE24_ARR;
	else if(slices==32)
		pPresetArr=SLICE32_ARR;
	else if(slices==48)
		pPresetArr=SLICE48_ARR;
	if(pPresetArr)
	{
		for(int i=0;i<slices;i++)
		{
			vertex_arr[i].x=radius*pPresetArr[i].cosa;
			vertex_arr[i].y=radius*pPresetArr[i].sina;
			vertex_arr[i].z=0;
		}
		return;
	}
	double slice_angle = 2*Pi/slices;
	double sina = sin(slice_angle);	//��Ƭ������
	double cosa = cos(slice_angle);	//��Ƭ������
	vertex_arr[0].Set(radius);
	for(int i=1;i<slices;i++)
	{
		vertex_arr[i].x=vertex_arr[i-1].x*cosa-vertex_arr[i-1].y*sina;
		vertex_arr[i].y=vertex_arr[i-1].y*cosa+vertex_arr[i-1].x*sina;
		vertex_arr[i].z=0;
	}
}

//itemType--�������� -1��ʾ�����øò��� cMaterial--����
COLORREF CLDSPart::GetColor(int idClassType, long itemType,BYTE cMaterial)
{
	if(library)
		return library->GetColor(idClassType,itemType,cMaterial);
	else
		return RGB(0,160,180);
};

long CLDSNode::RootFatherPartHandle()
{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	CLDSLineAngle *pLineAngle=(CLDSLineAngle*)m_pModel->FromPartHandle(hFatherPart,CLS_LINEANGLE);
	if(pLineAngle&&pLineAngle->group_father_jg_h>0x20)
		return pLineAngle->group_father_jg_h;
	else
#endif
		return hFatherPart;
}
CLDSPart* CLDSNode::RootFatherPart()
{
	CSuperSmartPtr<CLDSPart> pPart=m_pModel->FromPartHandle(hFatherPart);
	if(pPart.IsNULL())
		return NULL;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(pPart->GetClassTypeId()!=CLS_LINEANGLE)
		return pPart;
	else if(pPart.LineAnglePointer()->group_father_jg_h>0x20)
		return m_pModel->FromPartHandle(pPart.LineAnglePointer()->group_father_jg_h,CLS_GROUPLINEANGLE);
#endif
	else
		return pPart;
}
//�õ�����������Ϣ�����ļ�buffer
union BUFFER_VERSION{
	struct{WORD major,revision;};
	UINT uiVersion;
	long version;
	BYTE bytes[4];
	BUFFER_VERSION(long v=0){version=v;}
};
int CLDSPart::ToPPIBuffer(CBuffer &buffer,DWORD dwInheritFlag/*=0*/)
{
	int nCurLen = buffer.GetLength();
	//д��汾�ţ��Ӱ汾��1000003��ʼӦ��CProccessPart::ToPPIbufferһ��
	BUFFER_VERSION v(1000006);	//�汾�����ֵΪ:16777215(0x00FFFFFF)
	if(m_nClassTypeId!=CLS_PLATE)
		v.version=1000005;	//1000006��Ҫ��������ͨ�ְ�ľ��ƫƫ����Ϣ
	v.bytes[3]=0xFF;			//ǰ�����ֽ�������ʾ�汾�ţ����ĸ��ֽ������ж��Ƿ�Ϊ�汾��
	buffer.WriteInteger(v.version);		//�汾��
	v.bytes[3]=0x0;	
	BUFFER_VERSION serial;
	if(m_pModel&&m_pModel->IsTowerModel())
		serial.major=((CTower*)m_pModel)->m_wiVerSerial;
	buffer.WriteInteger(serial.version);		//��ˮ��
	//1.д�빹��������Ϣ
	int cls_id=GetClassTypeId();
	if(GetClassTypeId()==CLS_POLYPART)
	{
		CLDSLinePart *pFirstRod=((CLDSPolyPart*)this)->segset.GetFirst();
		if(pFirstRod)
			cls_id=pFirstRod->GetClassTypeId();
	}
	if(cls_id==CLS_LINEANGLE)
		buffer.WriteByte((BYTE)STEELTYPE_ANGLE);
	else if(cls_id==CLS_BOLT)
		buffer.WriteByte((BYTE)STEELTYPE_BOLT);
	else if(cls_id==CLS_PLATE||cls_id==CLS_PARAMPLATE)
		buffer.WriteByte((BYTE)STEELTYPE_PLATE);
	else if(cls_id==CLS_LINETUBE)
		buffer.WriteByte((BYTE)STEELTYPE_TUBE);
	else if(cls_id==CLS_LINEFLAT)
		buffer.WriteByte((BYTE)STEELTYPE_FLAT);
	else if(cls_id==CLS_LINESLOT)
		buffer.WriteByte((BYTE)STEELTYPE_SLOT);
	else
		buffer.WriteByte(0);
	buffer.WriteByte(cMaterial);			//д����ʼ��ַ�
	buffer.WriteInteger(iSeg);				//д������ֶκ�
	buffer.WriteFloat((float)GetWidth());	//д����
	buffer.WriteFloat((float)GetThick());	//д����
	buffer.WriteFloat((float)GetHeight());	//д��߶�
	buffer.WriteWord((WORD)ftoi(GetLength()));	//д�볤��(mm)
	buffer.WriteFloat((float)GetWeight());	//д�뵥��(kg)
	if(v.version>=1000005)
	{
		WORD wAttachBuff=2;
		//if(cls_id==CLS_PLATE||cls_id==CLS_PARAMPLATE)
		//	wAttachBuff=3;
		buffer.WriteWord(wAttachBuff);
		//1.д�빹��������Ϣ
		buffer.WriteInteger(1);	//���������ͱ�ʶ, 1:�������ӻ�����
		DWORD stackPos=buffer.CursorPosition;
		buffer.WriteInteger(0);	//Ԥ���������ӻ���������ռλ�ֽ�
		
		//����������Ϣ�İ汾��ˮ��>=V1000005
		buffer.WriteDword(0x00000040);	//m_dwInheritPropFlag=CProcessPart::PATTERN_INHERIT
		buffer.WriteInteger(handle);	//m_idSrcObject
		buffer.WriteString(sPartNo);
		CXhChar50 spec;
		GetSpecification(spec,TRUE);
		buffer.WriteString(spec);
		buffer.WriteString(sNotes);	//m_sNote
		buffer.WriteString("");		//m_sRelatePartNo);
		DWORD currPos=buffer.CursorPosition;
		buffer.SeekPosition(stackPos);
		buffer.WriteInteger(currPos-stackPos-4);
		buffer.SeekPosition(currPos);
		//2.д�빤����Ϣ
		buffer.WriteInteger(2);	//���������ͱ�ʶ, :���ո��ӻ�����
		CBuffer processBuf;
		WriteProcessBuffer(processBuf,true,v.version);
		buffer.WriteInteger(processBuf.Length);
		buffer.Write(processBuf.GetBufferPtr(),processBuf.GetLength());

		//if(cls_id==CLS_PLATE||cls_id==CLS_PARAMPLATE)
		//{	//3.�ְ��и����Ϣ
		//	buffer.WriteInteger(3);	//���������ͱ�ʶ, 3:�ְ��и����Ϣ���ӻ�����
		//	stackPos=buffer.CursorPosition;
		//	buffer.WriteInteger(0);	//Ԥ���������ӻ���������ռλ�ֽ�
		//	((CProcessPlate*)this)->CutPointToBuffer(buffer,v.version);
		//	currPos=buffer.CursorPosition;
		//	buffer.SeekPosition(stackPos);
		//	buffer.WriteInteger(currPos-stackPos-4);
		//	buffer.SeekPosition(currPos);
		//}
	}
	else
	{
		//2.д�빹��������Ϣ
		buffer.WriteString(GetPartNo());
		CXhChar50 spec;
		GetSpecification(spec,TRUE);
		buffer.WriteString(spec);
		buffer.WriteString(sNotes);
		//3.д�빹��������Ϣ
		CBuffer processInfoBuffer;
		WriteProcessBuffer(processInfoBuffer,true,v.version);
		buffer.WriteInteger(processInfoBuffer.GetLength());
		if(processInfoBuffer.GetLength()>0)
			buffer.Write(processInfoBuffer.GetBufferPtr(),processInfoBuffer.GetLength());
	}
	return buffer.GetLength()-nCurLen; 
}
COldVerLdsObjLife::COldVerLdsObjLife()
{
	m_bRestore=false;
	m_iVersion=0;
	m_iDocType=1;
	m_pSrcObj=NULL;
}
COldVerLdsObjLife::COldVerLdsObjLife(COldVerDataFile* pOldVerInfo,CLDSObject* pObj,long version/*=0*/,long doc_type/*=0*/)
{
	m_pSrcObj = pObj;
	m_iVersion=version;
	m_iDocType=doc_type;
	m_xBuffer.SetBlockSize(10000);
	m_pOldVerInfo=pOldVerInfo;
	if(m_pSrcObj!=NULL&&pOldVerInfo!=NULL)
		m_bRestore=m_pSrcObj->PushObjBuffer(pOldVerInfo,&m_xBuffer,m_iVersion,m_iDocType);
	else
		m_bRestore=false;
}
COldVerLdsObjLife::~COldVerLdsObjLife()
{
	if(m_bRestore&&m_pSrcObj)
		m_pSrcObj->PopObjBuffer(m_pOldVerInfo,&m_xBuffer,m_iVersion,m_iDocType);	
}
bool CLDSPoint::PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long version/*=0*/,long doc_type/*=0*/)
{
	return false;
}
bool CLDSPoint::PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long version/*=0*/,long doc_type/*=0*/)
{
	return false;
}
bool CLDSVector::PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long version/*=0*/,long doc_type/*=0*/)
{
	return false;
}
bool CLDSVector::PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long version/*=0*/,long doc_type/*=0*/)
{
	return false;
}
bool CLDSBolt::PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long version/*=0*/,long doc_type/*=0*/)
{
	return false;
}
bool CLDSBolt::PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long version/*=0*/,long doc_type/*=0*/)
{
	return false;
}
bool CLDSLineAngle::PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long version/*=0*/,long doc_type/*=0*/)
{
	return false;
}
bool CLDSLineAngle::PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long version/*=0*/,long doc_type/*=0*/)
{
	return false;
}
bool CLDSPlate::PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long version/*=0*/,long doc_type/*=0*/)
{
	return false;
}
bool CLDSPlate::PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long version/*=0*/,long doc_type/*=0*/)
{
	return false;
}
bool CLDSParamPlate::PushObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long version/*=0*/,long doc_type/*=0*/)
{
	return false;
}
bool CLDSParamPlate::PopObjBuffer(COldVerDataFile* pOldVerInfo,BUFFER_IO* pIO,long version/*=0*/,long doc_type/*=0*/)
{
	return false;
}

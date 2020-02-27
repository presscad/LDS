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
		m_bVerifyGroupAngleReplaced=false;	//只进行一次组合角钢替换验证判断 wjh-2016.11.10
	if( m_bVerifyGroupAngleReplaced && !m_bCollectiveTransition &&
		hNew>0x20&&(pNewRod==NULL||pNewRod->handle!=hNew)&&CLDSPart::_console!=NULL)
		pNewRod=CLDSPart::_console->FromRodHandle(hNew);
	if(pNewRod==NULL||pNewRod->GetClassTypeId()!=CLS_GROUPLINEANGLE)
		m_bVerifyGroupAngleReplaced=false;	//只进行一次组合角钢替换验证判断 wjh-2016.11.10
	if( pOldRod&&pNewRod&& (*hCurrent)>0 &&
		pOldRod->GetClassTypeId()==CLS_GROUPLINEANGLE&&pNewRod->GetClassTypeId()==CLS_GROUPLINEANGLE)
	{
		for(int i=0;i<4;i++)
		{
			if(((CLDSGroupLineAngle*)pOldRod)->son_jg_h[i]==0)
				break;
			if(_hOldSonAngles[i]==*hCurrent&&(!bObjSelfHandle||IsReplaceObjSelfHandle()))
			{	//此处不能使用pOldRod->son_jg_h，因为pOldRod->son_jg_h有可能在之前的句柄替换中发生改变 wjh-2016.11.10
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
		return hNew;	//不进行线段区间过滤
	GEPOINT point(pos_coord);
	CMaxDouble justify;	//查找重叠度最高的杆件
	SECTION* pPerfectSect=NULL;
	for(SECTION* pSect=sections.GetFirst();pSect;pSect=sections.GetNext())
	{
		GEPOINT extend_vec=point-pSect->lineFilterStart;
		double dist=extend_vec*pSect->lineStdVec;
		double oldScalar=justify.number;
		if(!justify.IsInited())
			pPerfectSect=pSect;
		double factor=dist;	//位置与区间段的重叠度判断因子
		if(dist<0)
			factor=dist;
		else if(dist>pSect->sect_length)
			factor=pSect->sect_length-dist;
		else
			factor=min(dist,pSect->sect_length-dist);
		justify.Update(factor);
		if(fabs(oldScalar-justify.number)>EPS)	//有显著变化(指变化量超出计算精度误差EPS时)
			pPerfectSect=pSect;
		else if(fabs(oldScalar-factor)<EPS&&pPerfectSect->priorSerial<pSect->priorSerial)
			pPerfectSect=pSect;	//最大因子值与当前区间段因子值近似相等时，取优先度较高的区间段
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
//0.自定义 1.X+ 2.X- 3.Y+ 4.Y- 5.Z+ 6.Z- 
f3dPoint CStdVector::GetVector(int nVecType)	//根据方向类型得到标准方向
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
int CStdVector::GetVectorType(const double* vec_coord)		//根据方向得到标准方向类型
{
	GEPOINT vec(vec_coord);
	if(fabs(vec.y)<EPS&&fabs(vec.z)<EPS)
	{
		if(vec.x>0)
			return 1;	//X+方向
		else if(vec.x<0)
			return 2;	//X-方向
	}
	else if(fabs(vec.x)<EPS&&fabs(vec.z)<EPS)
	{
		if(vec.y>0)
			return 3;	//Y+方向
		else if(vec.y<0)
			return 4;	//Y-方向
	}
	else if(fabs(vec.x)<EPS&&fabs(vec.y)<EPS)
	{
		if(vec.z>0)
			return 5;	//Z+方向
		else if(vec.z<0)
			return 6;	//Z-方向
	}
	return 0;	//自定义方向
}
CString CStdVector::MakeVectorSetStr(bool bHasEditPara/*=false*/)			//标准方向字符串
{
	CString sVec;
#ifdef AFX_TARG_ENU_ENGLISH
	if(bHasEditPara)
		sVec="0.User Specify|1.X+|2.X-|3.Y+|4.Y-|5.Z+|6.Z-|7.Modify Parameter...";
	else
		sVec="0.User Specify|1.X+|2.X-|3.Y+|4.Y-|5.Z+|6.Z-";
#else 
	if(bHasEditPara)
		sVec="0.自定义|1.X+|2.X-|3.Y+|4.Y-|5.Z+|6.Z-|7.修改参数...";
	else
		sVec="0.自定义|1.X+|2.X-|3.Y+|4.Y-|5.Z+|6.Z-";
#endif
	return sVec;
}

CString CStdVector::GetVecTypeString(int vectorType)//得到输入类型对应的字符串
{
	CString sText;
	if(vectorType==0)
#ifdef AFX_TARG_ENU_ENGLISH
		sText="0.User Specify";
#else 
		sText="0.自定义";
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
		sText="7.修改参数...";
#endif
	return sText;
}

CString CStdVector::GetVecTypeString(const double* vec_coord)	//得到输入方向对应的字符串
{
	int nVecType=GetVectorType(vec_coord);
	return GetVecTypeString(nVecType);
}
///////////////////////
IConsole* CLDSObject::_console=NULL;//操作控制平台
ISysPara* CLDSObject::sysPara=NULL;	//环境配置参数
ILog2File* CLDSObject::log2file=NULL;	//错误日志文件
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
	CLDSDbObject::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"Lift Arc"));		//预拱项
#else 
	CLDSDbObject::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"数据对象基本信息"));
	CLDSDbObject::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSDbObject::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSDbObject::AddPropItem("layer1",PROPLIST_ITEM(id++,"图层第一字母"));
	CLDSDbObject::AddPropItem("layer2",PROPLIST_ITEM(id++,"图层第二字母"));
	CLDSDbObject::AddPropItem("layer3",PROPLIST_ITEM(id++,"图层第三字母"));
	CLDSDbObject::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSDbObject::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSDbObject::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
	//
	CLDSDbObject::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"预拱项"));		//预拱项
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
		Log2File()->Log("%s同步对称属性项缺失",propKeyStr);
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
		return NULL;	//不支持自对称
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
			return FALSE;	//有更小句柄的关联构件
	}
	return TRUE;
}
RELATIVE_OBJECT* CLDSDbObject::GetRelativeObjInfo(long h,bool bIncRotateMir/*=true*/)
{
	RELATIVE_OBJECT *pMirObj=NULL;
	for(pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		if(!bIncRotateMir&&(pMirObj->mirInfo.axis_flag&0x08))
			continue;	//搜索信息不包括旋转对称
		if(pMirObj->hObj==h)
			return pMirObj;	//是关联构件
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
			relaFatherObj = *pMirObj;	//有更小句柄的关联构件
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

BOOL CLDSNode::m_bDisplayBeforeArcLift=TRUE;//显示预拱前位置
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
	propMirSync.AddPropItem("layer",PROPLIST_ITEM(id++,"图层名"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("pos",PROPLIST_ITEM(id++,"坐标"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("hFatherPart",PROPLIST_ITEM(id++,"父杆件"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("xFatherAngleZhunJu",PROPLIST_ITEM(id++,"准据设定"));
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
	propMirSync.AddPropItem("point_i",PROPLIST_ITEM(id++,"编号"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("force_type",PROPLIST_ITEM(id++,"节点传力判断"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("hangingInfo",PROPLIST_ITEM(id++,"挂线信息"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("restrict",PROPLIST_ITEM(id++,"约束"));
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
	CLDSNode::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"Lift Arc Coordinate"));		//预拱项
	CLDSNode::AddPropItem("ArcLiftDirection",PROPLIST_ITEM(id++,"Lift Direction"));		//预拱项
	CLDSNode::AddPropItem("m_xArcLiftPos",PROPLIST_ITEM(id++,"Prelift Coordinate"));	//预拱后位置
	CLDSNode::AddPropItem("m_xArcLiftPos.x",PROPLIST_ITEM(id++,"X"));
	CLDSNode::AddPropItem("m_xArcLiftPos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSNode::AddPropItem("m_xArcLiftPos.z",PROPLIST_ITEM(id++,"Z"));
	CLDSNode::AddPropItem("iSeg",PROPLIST_ITEM(id++,"Segment Number"));
	CLDSNode::AddPropItem("cfgword",PROPLIST_ITEM(id++,"Configure Word"));
	CLDSNode::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"Relative Node"));
	CLDSNode::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//影射信息
	CLDSNode::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Project Mother Node"));//影射母构件
	CLDSNode::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//归属的部件引用
	CLDSNode::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//归属的部件
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
	//内力分析需要的节点编号和传力类型
	CLDSNode::AddPropItem("point_i",PROPLIST_ITEM(id++,"Serial Number"));
	CLDSNode::AddPropItem("force_type",PROPLIST_ITEM(id++,"Node force Judgment"));
	//挂位信息
	CLDSNode::AddPropItem("m_idNodePlace",PROPLIST_ITEM(id++,"Hang Wire Serial Number"));
	//附加荷载
	CLDSNode::AddPropItem("attachLoad",PROPLIST_ITEM(id++,"Attached Load"));
	CLDSNode::AddPropItem("attachVolLoad",PROPLIST_ITEM(id++,"Variable Attached Load(kN)"));//附加可变外荷载
	CLDSNode::AddPropItem("attachVolLoad_X",PROPLIST_ITEM(id++,"X Component"));
	CLDSNode::AddPropItem("attachVolLoad_Y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSNode::AddPropItem("attachVolLoad_Z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSNode::AddPropItem("coefCqi",PROPLIST_ITEM(id++,""));//效应系数
	CLDSNode::AddPropItem("attachForeverLoad",PROPLIST_ITEM(id++,"Forever Attached Load(kN)"));//附加永久外荷载
	CLDSNode::AddPropItem("attachForeverLoad_X",PROPLIST_ITEM(id++,"X Component"));
	CLDSNode::AddPropItem("attachForeverLoad_Y",PROPLIST_ITEM(id++,"Y Component"));
	CLDSNode::AddPropItem("attachForeverLoad_Z",PROPLIST_ITEM(id++,"Z Component"));
	CLDSNode::AddPropItem("permanentFz",PROPLIST_ITEM(id++,"Z_2 Component"));
	
	//位移约束
	CLDSNode::AddPropItem("restrict",PROPLIST_ITEM(id++,"Restrict"));
	CLDSNode::AddPropItem("restrict_x",PROPLIST_ITEM(id++,"X Displacement Restrict"));
	CLDSNode::AddPropItem("restrict_y",PROPLIST_ITEM(id++,"Y Displacement Restrict"));
	CLDSNode::AddPropItem("restrict_z",PROPLIST_ITEM(id++,"Z Displacement Restrict"));
	CLDSNode::AddPropItem("restrict_rot_x",PROPLIST_ITEM(id++,"X Rotation Restrict"));
	CLDSNode::AddPropItem("restrict_rot_y",PROPLIST_ITEM(id++,"Y Rotation Restrict"));
	CLDSNode::AddPropItem("restrict_rot_z",PROPLIST_ITEM(id++,"Z Rotation Restrict"));
	CLDSNode::AddPropItem("restrict_all",PROPLIST_ITEM(id++,"All"));
#else 
	CLDSNode::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"基本信息"));
	CLDSNode::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSNode::AddPropItem("wireInfo",PROPLIST_ITEM(id++,"挂线"));
	CLDSNode::AddPropItem("hangingInfo",PROPLIST_ITEM(id++,"挂线节点",NULL,"是|否"));
	CLDSNode::AddPropItem("setupWireInfo",PROPLIST_ITEM(id++,""));
	CLDSNode::AddPropItem("hangingWireType",PROPLIST_ITEM(id++,"(已注释)挂接类型"));
	CLDSNode::AddPropItem("hangingStyle",PROPLIST_ITEM(id++,"(已注释)"));
	CLDSNode::AddPropItem("voltage",PROPLIST_ITEM(id++,"(已注释)"));
	CLDSNode::AddPropItem("splitNumber",PROPLIST_ITEM(id++,"(已注释)"));
	CLDSNode::AddPropItem("landHigh",PROPLIST_ITEM(id++,"(已注释)"));
	CLDSNode::AddPropItem("attachmentSpec",PROPLIST_ITEM(id++,"(已注释)"));
	CLDSNode::AddPropItem("attachmentWeight",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("attachmentWindShieldArea",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("anchorLineAngle",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("tractionLineAngle",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("wireType",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("wireSpec",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("wireDiameter",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("wireArea",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("unitWeight",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("forcePermit",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("saftyCoef",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("maxTension",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("elasticityCoef",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("expansionCoef",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("powerLineInfo",PROPLIST_ITEM(id++,"无"));
	CLDSNode::AddPropItem("horiSpace",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("vertSpace",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("representSpace",PROPLIST_ITEM(id++,"(已注释)代表档距"));
	CLDSNode::AddPropItem("HtoL",PROPLIST_ITEM(id++,"(已注释)高差系数"));
	CLDSNode::AddPropItem("allocHoriCoef",PROPLIST_ITEM(id++,"(已注释)水平档距分配系数"));
	CLDSNode::AddPropItem("allocVertCoef",PROPLIST_ITEM(id++,"(已注释)垂直档距分配系数"));
	CLDSNode::AddPropItem("xianluAngle",PROPLIST_ITEM(id++,"已注释"));
	CLDSNode::AddPropItem("layer",PROPLIST_ITEM(id++,"图层名"));
	CLDSNode::AddPropItem("hFatherPart",PROPLIST_ITEM(id++,"父杆件"));
	CLDSNode::AddPropItem("m_bVirtualObj",PROPLIST_ITEM(id++,"虚拟节点"));
	CLDSNode::AddPropItem("pos",PROPLIST_ITEM(id++,"坐标"));
	CLDSNode::AddPropItem("pos.x",PROPLIST_ITEM(id++,"X"));
	CLDSNode::AddPropItem("pos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSNode::AddPropItem("pos.z",PROPLIST_ITEM(id++,"Z"));
	CLDSNode::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"预拱坐标"));		//预拱项
	CLDSNode::AddPropItem("ArcLiftDirection",PROPLIST_ITEM(id++,"预拱方向"));		//预拱项
	CLDSNode::AddPropItem("m_xArcLiftPos",PROPLIST_ITEM(id++,"预拱后节点坐标"));	//预拱后位置
	CLDSNode::AddPropItem("m_xArcLiftPos.x",PROPLIST_ITEM(id++,"X"));
	CLDSNode::AddPropItem("m_xArcLiftPos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSNode::AddPropItem("m_xArcLiftPos.z",PROPLIST_ITEM(id++,"Z"));
	CLDSNode::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSNode::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSNode::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"关联节点"));
	CLDSNode::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"影射信息"));//影射信息
	CLDSNode::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"影射母节点"));//影射母构件
	CLDSNode::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"归属部件引用"));//归属的部件引用
	CLDSNode::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"归属部件"));//归属的部件
	CLDSNode::AddPropItem("xFatherAngleZhunJu",PROPLIST_ITEM(id++,"准距设定"));
	CLDSNode::AddPropItem("attachRelation",PROPLIST_ITEM(id++,"依附关系"));
	CLDSNode::AddPropItem("m_cPosCalType",PROPLIST_ITEM(id++,"坐标依附类型"));
	CLDSNode::AddPropItem("m_cPosCalViceType",PROPLIST_ITEM(id++,"参照点对称方式"));
	CLDSNode::AddPropItem("attachNode",PROPLIST_ITEM(id++,"依附节点句柄"));
	CLDSNode::AddPropItem("attachNode1",PROPLIST_ITEM(id++,"依附节点1"));
	CLDSNode::AddPropItem("attachNode2",PROPLIST_ITEM(id++,"依附节点2"));
	CLDSNode::AddPropItem("attachNode3",PROPLIST_ITEM(id++,"依附节点3"));
	CLDSNode::AddPropItem("attachNode4",PROPLIST_ITEM(id++,"依附节点4"));
	CLDSNode::AddPropItem("attachPole",PROPLIST_ITEM(id++,"依附杆件句柄"));
	CLDSNode::AddPropItem("attachPole1",PROPLIST_ITEM(id++,"依附杆件1"));
	CLDSNode::AddPropItem("attachPole2",PROPLIST_ITEM(id++,"依附杆件2"));
	CLDSNode::AddPropItem("attach_scale",PROPLIST_ITEM(id++,"比例点所占比例"));
	CLDSNode::AddPropItem("attach_offset",PROPLIST_ITEM(id++,"偏移点偏移距离"));
	CLDSNode::AddPropItem("hRelationPlane",PROPLIST_ITEM(id++,"基准面"));
	CLDSNode::AddPropItem("hPlaneRefNode#0",PROPLIST_ITEM(id++,"参照节点1"));
	CLDSNode::AddPropItem("hPlaneRefNode#1",PROPLIST_ITEM(id++,"参照节点2"));
	CLDSNode::AddPropItem("hPlaneRefNode#2",PROPLIST_ITEM(id++,"参照节点3"));
	CLDSNode::AddPropItem("hDatumPoint",PROPLIST_ITEM(id++,"基准点"));
	//内力分析需要的节点编号和传力类型
	CLDSNode::AddPropItem("point_i",PROPLIST_ITEM(id++,"编号"));
	CLDSNode::AddPropItem("force_type",PROPLIST_ITEM(id++,"节点传力判断"));
	//挂位信息
	CLDSNode::AddPropItem("m_idNodePlace",PROPLIST_ITEM(id++,"挂位编号"));
	//附加荷载
	CLDSNode::AddPropItem("attachLoad",PROPLIST_ITEM(id++,"附加荷载"));
	CLDSNode::AddPropItem("attachVolLoad",PROPLIST_ITEM(id++,"可变外荷载(kN)"));//附加可变外荷载
	CLDSNode::AddPropItem("attachVolLoad_X",PROPLIST_ITEM(id++,"X分量"));
	CLDSNode::AddPropItem("attachVolLoad_Y",PROPLIST_ITEM(id++,"Y分量"));
	CLDSNode::AddPropItem("attachVolLoad_Z",PROPLIST_ITEM(id++,"Z分量"));
	CLDSNode::AddPropItem("coefCqi",PROPLIST_ITEM(id++,""));//效应系数
	CLDSNode::AddPropItem("attachForeverLoad",PROPLIST_ITEM(id++,"永久外荷载(kN)"));//附加永久外荷载
	CLDSNode::AddPropItem("attachForeverLoad_X",PROPLIST_ITEM(id++,"X分量"));
	CLDSNode::AddPropItem("attachForeverLoad_Y",PROPLIST_ITEM(id++,"Y分量"));
	CLDSNode::AddPropItem("attachForeverLoad_Z",PROPLIST_ITEM(id++,"Z分量"));
	CLDSNode::AddPropItem("permanentFz",PROPLIST_ITEM(id++,"Z_2分量"));
	
	//位移约束
	CLDSNode::AddPropItem("restrict",PROPLIST_ITEM(id++,"约束"));
	CLDSNode::AddPropItem("restrict_x",PROPLIST_ITEM(id++,"X轴向位移约束"));
	CLDSNode::AddPropItem("restrict_y",PROPLIST_ITEM(id++,"Y轴向位移约束"));
	CLDSNode::AddPropItem("restrict_z",PROPLIST_ITEM(id++,"Z轴向位移约束"));
	CLDSNode::AddPropItem("restrict_rot_x",PROPLIST_ITEM(id++,"X轴向旋转约束"));
	CLDSNode::AddPropItem("restrict_rot_y",PROPLIST_ITEM(id++,"Y轴向旋转约束"));
	CLDSNode::AddPropItem("restrict_rot_z",PROPLIST_ITEM(id++,"Z轴向旋转约束"));
	CLDSNode::AddPropItem("restrict_all",PROPLIST_ITEM(id++,"全部"));
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
	CLDSPart::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//归属的部件引用
	CLDSPart::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//归属的部件
	CLDSPart::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Shadow Info"));		//影射信息
	CLDSPart::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Mother Object"));	//母对象
	CLDSPart::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"BOM Number"));
	CLDSPart::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"Weld Parent Part"));
	CLDSPart::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"Hole Visible"));
	CLDSPart::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"Virtual Ancillary Part"));
	CLDSPart::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"Specify Part Color"));
	CLDSPart::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"Part Color"));
	CLDSPart::AddPropItem("sNotes",PROPLIST_ITEM(id++,"Notes"));
#else 
	CLDSPart::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"基本信息"));
	CLDSPart::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSPart::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSPart::AddPropItem("layer1",PROPLIST_ITEM(id++,"第一字符"));
	CLDSPart::AddPropItem("layer2",PROPLIST_ITEM(id++,"第二字符"));
	CLDSPart::AddPropItem("layer3",PROPLIST_ITEM(id++,"第三字符"));
	CLDSPart::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	CLDSPart::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级",NULL,"默认|A|B|C|D|"));
	CLDSPart::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSPart::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	CLDSPart::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSPart::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//归属的部件引用
	CLDSPart::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//归属的部件
	CLDSPart::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Shadow Info"));		//影射信息
	CLDSPart::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Mother Object"));	//母对象
	CLDSPart::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	CLDSPart::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件"));
	CLDSPart::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	CLDSPart::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助构件"));
	CLDSPart::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色"));
	CLDSPart::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	CLDSPart::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
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
	CLDSLinePart::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//归属的部件
	CLDSLinePart::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Shadow Info"));		//影射信息
	CLDSLinePart::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Mother Object"));	//母对象
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
	//线缆类型及规格参数
	CLDSLinePart::AddPropItem("m_sWireType",PROPLIST_ITEM(id++,"Wire Type"));		//线缆类型
	CLDSLinePart::AddPropItem("m_sWireSpec",PROPLIST_ITEM(id++,"Wire Specification"));		//线缆规格
	CLDSLinePart::AddPropItem("m_fWireArea",PROPLIST_ITEM(id++,"Section Area"));		//线缆截面积
	CLDSLinePart::AddPropItem("m_fUnitWeight",PROPLIST_ITEM(id++,"Unit Weight(kg/km)"));	//单位长重量
	CLDSLinePart::AddPropItem("m_fForcePermit",PROPLIST_ITEM(id++,"Nominal Tensile Strength"));	//额定抗拉力
	CLDSLinePart::AddPropItem("m_fElasticCoef",PROPLIST_ITEM(id++,"Elastic Coefficient(MPa)"));	//弹性系数(模量)
	CLDSLinePart::AddPropItem("m_fExpansionCoef",PROPLIST_ITEM(id++,"Temperature Expand Coefficient"));//温度线膨胀系数
	CLDSLinePart::AddPropItem("m_fPreTension",PROPLIST_ITEM(id++,"Pre-tensioning Force"));//温度线膨胀系数
	//用户定制截面参数
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
	//内力分析所需要的设计信息属性
	CLDSLinePart::AddPropItem("designInfo",PROPLIST_ITEM(id++,"Design Information"));
	CLDSLinePart::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"Statistic Material Serial Number"));	//统材编号
	CLDSLinePart::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"Gravity Zoom Out Coefficient"));
	CLDSLinePart::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"Elem Type"));
	CLDSLinePart::AddPropItem("sel_mat_method",PROPLIST_ITEM(id++,"Brace member check rule"));
	CLDSLinePart::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Whether Material Optimization"));
	CLDSLinePart::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"Whether Specification Optimization"));
	CLDSLinePart::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"Calculate Automatic Bolt Hole Out"));
	CLDSLinePart::AddPropItem("hole_out",PROPLIST_ITEM(id++,"Bolt Hole Out"));
	//计算长度信息
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
	//始端设计信息
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
	//终端设计信息
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
	CLDSLinePart::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"单元基本信息"));
	CLDSLinePart::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSLinePart::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSLinePart::AddPropItem("layer1",PROPLIST_ITEM(id++,"第一字符"));
	CLDSLinePart::AddPropItem("layer2",PROPLIST_ITEM(id++,"第二字符"));
	CLDSLinePart::AddPropItem("layer3",PROPLIST_ITEM(id++,"第三字符"));
	CLDSLinePart::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	CLDSLinePart::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级",NULL,"默认|A|B|C|D|"));
	CLDSLinePart::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSLinePart::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	CLDSLinePart::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSLinePart::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"影射信息"));//影射信息
	CLDSLinePart::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"影射母构件"));//影射母构件
	CLDSLinePart::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"归属部件"));//归属的部件
	CLDSLinePart::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"关联杆件"));
	CLDSLinePart::AddPropItem("size.idClassType",PROPLIST_ITEM(id++,"杆件类型"));
	CLDSLinePart::AddPropItem("specification",PROPLIST_ITEM(id++,"杆件规格"));
	CLDSLinePart::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	CLDSLinePart::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件"));
	CLDSLinePart::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	CLDSLinePart::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助构件"));
	CLDSLinePart::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色"));
	CLDSLinePart::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	CLDSLinePart::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
	CLDSLinePart::AddPropItem("oddMent",PROPLIST_ITEM(id++,""));
	CLDSLinePart::AddPropItem("nodeInfo",PROPLIST_ITEM(id++,"端点信息"));
	CLDSLinePart::AddPropItem("startInfo",PROPLIST_ITEM(id++,"始端位置"));
	CLDSLinePart::AddPropItem("hStart",PROPLIST_ITEM(id++,"始端节点"));
	CLDSLinePart::AddPropItem("oddStart",PROPLIST_ITEM(id++,"始端"));
	CLDSLinePart::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSLinePart::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSLinePart::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSLinePart::AddPropItem("endInfo",PROPLIST_ITEM(id++,"终端位置"));
	CLDSLinePart::AddPropItem("hEnd",PROPLIST_ITEM(id++,"终端节点"));
	CLDSLinePart::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"终端"));
	CLDSLinePart::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSLinePart::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSLinePart::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z坐标"));
	//线缆类型及规格参数
	CLDSLinePart::AddPropItem("m_sWireType",PROPLIST_ITEM(id++,"线缆类型"));		//线缆类型
	CLDSLinePart::AddPropItem("m_sWireSpec",PROPLIST_ITEM(id++,"线缆规格"));		//线缆规格
	CLDSLinePart::AddPropItem("m_fWireArea",PROPLIST_ITEM(id++,"截面积"));		//线缆截面积
	CLDSLinePart::AddPropItem("m_fUnitWeight",PROPLIST_ITEM(id++,"单位重量(kg/km)"));	//单位长重量
	CLDSLinePart::AddPropItem("m_fForcePermit",PROPLIST_ITEM(id++,"额定抗拉强度"));	//额定抗拉力
	CLDSLinePart::AddPropItem("m_fElasticCoef",PROPLIST_ITEM(id++,"弹性系数(MPa)"));	//弹性系数(模量)
	CLDSLinePart::AddPropItem("m_fExpansionCoef",PROPLIST_ITEM(id++,"温度线膨胀系数"));//温度线膨胀系数
	CLDSLinePart::AddPropItem("m_fPreTension",PROPLIST_ITEM(id++,"预张紧力"));//温度线膨胀系数
	//用户定制截面参数
	CLDSLinePart::AddPropItem("userdef",PROPLIST_ITEM(id++,""));
	CLDSLinePart::AddPropItem("userdef.size",PROPLIST_ITEM(id++,"自定义规格"));
	CLDSLinePart::AddPropItem("userdef.area",PROPLIST_ITEM(id++,"截面积"));
	CLDSLinePart::AddPropItem("userdef.E",PROPLIST_ITEM(id++,"弹性模量"));
	CLDSLinePart::AddPropItem("userdef.fc",PROPLIST_ITEM(id++,"受压强度设计值"));
	CLDSLinePart::AddPropItem("userdef.ft",PROPLIST_ITEM(id++,"抗拉强度设计值"));
	CLDSLinePart::AddPropItem("userdef.pre_stress",PROPLIST_ITEM(id++,"预应力"));
	CLDSLinePart::AddPropItem("userdef.unit_weight",PROPLIST_ITEM(id++,"单位重量"));
	CLDSLinePart::AddPropItem("length",PROPLIST_ITEM(id++,"单元长度"));
	CLDSLinePart::AddPropItem("lengthSingleLine",PROPLIST_ITEM(id++,"单线长度"));
	//内力分析所需要的设计信息属性
	CLDSLinePart::AddPropItem("designInfo",PROPLIST_ITEM(id++,"设计信息"));
	CLDSLinePart::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"统材编号"));	//统材编号
	CLDSLinePart::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"重力放大系数"));
	CLDSLinePart::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"单元类型"));
	CLDSLinePart::AddPropItem("sel_mat_method",PROPLIST_ITEM(id++,"辅材验算依据"));
	CLDSLinePart::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"(已注释)是否进行材质优选"));
	CLDSLinePart::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"是否进行规格优选"));
	CLDSLinePart::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"自动计算螺栓减孔数"));
	CLDSLinePart::AddPropItem("hole_out",PROPLIST_ITEM(id++,"螺栓减孔数"));
	//计算长度信息
	CLDSLinePart::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,""));
	CLDSLinePart::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,"计算长度类型"));
	CLDSLinePart::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLinePart::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,"同时受压参照杆件"));
	CLDSLinePart::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,"参照横连杆"));
	CLDSLinePart::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,"最小轴长度"));
	CLDSLinePart::AddPropItem("CalLenCoef.minR.hStartNode",PROPLIST_ITEM(id++,"起始节点"));
	CLDSLinePart::AddPropItem("CalLenCoef.minR.hEndNode",PROPLIST_ITEM(id++,"终止节点"));
	CLDSLinePart::AddPropItem("CalLenCoef.minR.coef",PROPLIST_ITEM(id++,"折算系数"));
	CLDSLinePart::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,"平行轴长度"));
	CLDSLinePart::AddPropItem("CalLenCoef.paraR.hStartNode",PROPLIST_ITEM(id++,"起始节点"));
	CLDSLinePart::AddPropItem("CalLenCoef.paraR.hEndNode",PROPLIST_ITEM(id++,"终止节点"));
	CLDSLinePart::AddPropItem("CalLenCoef.paraR.coef",PROPLIST_ITEM(id++,"折算系数"));
	//始端设计信息
	CLDSLinePart::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"始端信息"));
	CLDSLinePart::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"始端约束类型"));
	CLDSLinePart::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"始端受力类型"));
	CLDSLinePart::AddPropItem("connectStart",PROPLIST_ITEM(id++,"始端连接"));
	CLDSLinePart::AddPropItem("connectStart.grade",PROPLIST_ITEM(id++,"螺栓级别"));
	CLDSLinePart::AddPropItem("connectStart.d",PROPLIST_ITEM(id++,"螺栓直径"));
	CLDSLinePart::AddPropItem("connectStart.N",PROPLIST_ITEM(id++,"连接螺栓数"));
	CLDSLinePart::AddPropItem("connectStart.summBoltN",PROPLIST_ITEM(id++,"计算螺栓数"));
	CLDSLinePart::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,"始端螺栓承载系数(未启用)"));
	CLDSLinePart::AddPropItem("connectStart.maxTension",PROPLIST_ITEM(id++,"始端最大连接轴向拉力"));
	CLDSLinePart::AddPropItem("connectStart.maxCompression",PROPLIST_ITEM(id++,"始端最大连接轴向压力"));
	//终端设计信息
	CLDSLinePart::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"终端信息"));
	CLDSLinePart::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"终端约束类型"));
	CLDSLinePart::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"终端受力类型"));
	CLDSLinePart::AddPropItem("connectEnd",PROPLIST_ITEM(id++,"终端连接"));
	CLDSLinePart::AddPropItem("connectEnd.grade",PROPLIST_ITEM(id++,"螺栓级别"));
	CLDSLinePart::AddPropItem("connectEnd.d",PROPLIST_ITEM(id++,"螺栓直径"));
	CLDSLinePart::AddPropItem("connectEnd.N",PROPLIST_ITEM(id++,"连接螺栓数"));
	CLDSLinePart::AddPropItem("connectEnd.summBoltN",PROPLIST_ITEM(id++,"计算螺栓数"));
	CLDSLinePart::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,"终端螺栓承载系数(未启用)"));
	CLDSLinePart::AddPropItem("connectEnd.maxTension",PROPLIST_ITEM(id++,"终端最大连接轴向拉力"));
	CLDSLinePart::AddPropItem("connectEnd.maxCompression",PROPLIST_ITEM(id++,"终端最大连接轴向压力"));
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
	//线缆类型及规格参数
	CElemInfo::AddPropItem("m_sWireType",PROPLIST_ITEM(id++,"Wire Type"));		//线缆类型
	CElemInfo::AddPropItem("m_sWireSpec",PROPLIST_ITEM(id++,"Wire Specification"));		//线缆规格
	CElemInfo::AddPropItem("m_fWireArea",PROPLIST_ITEM(id++,"Section Area"));		//线缆截面积
	CElemInfo::AddPropItem("m_fUnitWeight",PROPLIST_ITEM(id++,"Unit Weight(kg/km)"));		//单位长重量
	CElemInfo::AddPropItem("m_fForcePermit",PROPLIST_ITEM(id++,"Nominal Tensile Strength"));		//额定抗拉力
	CElemInfo::AddPropItem("m_fElasticCoef",PROPLIST_ITEM(id++,"Elastic Coefficient(MPa)"));		//弹性系数(模量)
	CElemInfo::AddPropItem("m_fExpansionCoef",PROPLIST_ITEM(id++,"Temperature Expand Coefficient"));	//温度线膨胀系数
	CElemInfo::AddPropItem("m_fPreTension",PROPLIST_ITEM(id++,"Pre-tensioning Force"));		//温度线膨胀系数
	//用户定制截面参数
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
	//内力分析所需要的设计信息属性
	CElemInfo::AddPropItem("designInfo",PROPLIST_ITEM(id++,"Design Information"));
	CElemInfo::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"Statistic Material Serial Number"));	//统材编号
	CElemInfo::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"Gravity Zoom Out Coefficient"));
	CElemInfo::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"Elem Type"));
	//CLDSLinePart::propHashtable.SetValueAt("m_bNeedSelMat",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"Whether Specification Optimization"));
	//设计信息
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
	//计算长度信息
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
	CElemInfo::AddPropItem("lamdaRxcInfo",PROPLIST_ITEM(id++,"Simultaneously Compression"));			//同时受压项
	//同时受压第一项
	CElemInfo::AddPropItem("smcItem1",PROPLIST_ITEM(id++,""));					
	CElemInfo::AddPropItem("smcItem1.cSMCType",PROPLIST_ITEM(id++,"Compression Type"));			//受压类型
	CElemInfo::AddPropItem("smcItem1.relaElem",PROPLIST_ITEM(id++,"Reference Elem"));			//参照单元
	CElemInfo::AddPropItem("smcItem1.gyreRadius",PROPLIST_ITEM(id++,"Gyre Radius"));		//回转半径
#else 
	CElemInfo::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"单元基本信息"));
	CElemInfo::AddPropItem("Id",PROPLIST_ITEM(id++,"标识"));
	CElemInfo::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CElemInfo::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	CElemInfo::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CElemInfo::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CElemInfo::AddPropItem("length",PROPLIST_ITEM(id++,"单元长度"));
	CElemInfo::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"关联杆件"));
	CElemInfo::AddPropItem("size.idClassType",PROPLIST_ITEM(id++,"杆件类型"));
	CElemInfo::AddPropItem("specification",PROPLIST_ITEM(id++,"杆件规格"));
	CElemInfo::AddPropItem("startInfo",PROPLIST_ITEM(id++,"始端信息"));
	CElemInfo::AddPropItem("hStart",PROPLIST_ITEM(id++,"始端节点"));
	//CLDSLinePart::propHashtable.SetValueAt("startPos.x",PROPLIST_ITEM(id++,""));
	//CLDSLinePart::propHashtable.SetValueAt("startPos.y",PROPLIST_ITEM(id++,""));
	//CLDSLinePart::propHashtable.SetValueAt("startPos.z",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("endInfo",PROPLIST_ITEM(id++,"终端位置"));
	CElemInfo::AddPropItem("hEnd",PROPLIST_ITEM(id++,"终端节点"));
	//CLDSLinePart::propHashtable.SetValueAt("endPos.x",PROPLIST_ITEM(id++,""));
	//CLDSLinePart::propHashtable.SetValueAt("endPos.y",PROPLIST_ITEM(id++,""));
	//CLDSLinePart::propHashtable.SetValueAt("endPos.z",PROPLIST_ITEM(id++,""));
	//线缆类型及规格参数
	CElemInfo::AddPropItem("m_sWireType",PROPLIST_ITEM(id++,"线缆类型"));		//线缆类型
	CElemInfo::AddPropItem("m_sWireSpec",PROPLIST_ITEM(id++,"线缆规格"));		//线缆规格
	CElemInfo::AddPropItem("m_fWireArea",PROPLIST_ITEM(id++,"截面积"));		//线缆截面积
	CElemInfo::AddPropItem("m_fUnitWeight",PROPLIST_ITEM(id++,"单位重量(kg/km)"));		//单位长重量
	CElemInfo::AddPropItem("m_fForcePermit",PROPLIST_ITEM(id++,"额定抗拉强度"));		//额定抗拉力
	CElemInfo::AddPropItem("m_fElasticCoef",PROPLIST_ITEM(id++,"弹性系数(MPa)"));		//弹性系数(模量)
	CElemInfo::AddPropItem("m_fExpansionCoef",PROPLIST_ITEM(id++,"温度线膨胀系数"));	//温度线膨胀系数
	CElemInfo::AddPropItem("m_fPreTension",PROPLIST_ITEM(id++,"预张紧力"));		//温度线膨胀系数
	//用户定制截面参数
	CElemInfo::AddPropItem("userdef",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("userdef.size",PROPLIST_ITEM(id++,"自定义规格"));
	CElemInfo::AddPropItem("userdef.area",PROPLIST_ITEM(id++,"截面积"));
	CElemInfo::AddPropItem("userdef.E",PROPLIST_ITEM(id++,"(已注释)弹性模量"));
	CElemInfo::AddPropItem("userdef.fc",PROPLIST_ITEM(id++,"(已注释)受压强度设计值"));
	CElemInfo::AddPropItem("userdef.ft",PROPLIST_ITEM(id++,"(已注释)抗拉强度设计值"));
	CElemInfo::AddPropItem("userdef.pre_stress",PROPLIST_ITEM(id++,"(已注释)预应力"));
	CElemInfo::AddPropItem("userdef.unit_weight",PROPLIST_ITEM(id++,"(已注释)单位重量"));
	CElemInfo::AddPropItem("length",PROPLIST_ITEM(id++,"单元长度"));
	CElemInfo::AddPropItem("lengthSingleLine",PROPLIST_ITEM(id++,"单线长度"));
	//内力分析所需要的设计信息属性
	CElemInfo::AddPropItem("designInfo",PROPLIST_ITEM(id++,"设计信息"));
	CElemInfo::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"统材编号"));	//统材编号
	CElemInfo::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"重力放大系数"));
	CElemInfo::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"单元类型"));
	//CLDSLinePart::propHashtable.SetValueAt("m_bNeedSelMat",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"是否进行规格优选"));
	//设计信息
	CElemInfo::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"始端约束类型"));
	CElemInfo::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"始端受力类型"));
	CElemInfo::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"终端约束类型"));
	CElemInfo::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"终端受力类型"));
	CElemInfo::AddPropItem("boltRelaInfo",PROPLIST_ITEM(id++,"螺栓相关信息"));
	CElemInfo::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"自动计算螺栓减孔数"));
	CElemInfo::AddPropItem("hole_out",PROPLIST_ITEM(id++,"螺栓减孔数"));
	CElemInfo::AddPropItem("bolt_type",PROPLIST_ITEM(id++,"螺栓规格"));
	CElemInfo::AddPropItem("bolt_N",PROPLIST_ITEM(id++,"计算螺栓数"));
	CElemInfo::AddPropItem("bolt_bearloadcoef",PROPLIST_ITEM(id++,""));
	//计算长度信息
	CElemInfo::AddPropItem("lamdaInfo",PROPLIST_ITEM(id++,"计算长度信息"));
	CElemInfo::AddPropItem("cCalLenType",PROPLIST_ITEM(id++,"长细比计算类型"));
	CElemInfo::AddPropItem("lamdaRyoInfo",PROPLIST_ITEM(id++,"最小轴"));
	CElemInfo::AddPropItem("LoRyo",PROPLIST_ITEM(id++,"计算长度"));
	CElemInfo::AddPropItem("LoRyo.CalType",PROPLIST_ITEM(id++,"基准长度类型"));
	CElemInfo::AddPropItem("LoRyo.coef",PROPLIST_ITEM(id++,"折算系数"));
	CElemInfo::AddPropItem("LoRyo.hStartNode",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("LoRyo.hEndNode",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("lamdaRxInfo",PROPLIST_ITEM(id++,"平行轴"));
	CElemInfo::AddPropItem("LoRx",PROPLIST_ITEM(id++,"计算长度"));
	CElemInfo::AddPropItem("LoRx.CalType",PROPLIST_ITEM(id++,"基准长度类型"));
	CElemInfo::AddPropItem("LoRx.coef",PROPLIST_ITEM(id++,"折算系数"));
	CElemInfo::AddPropItem("LoRx.hStartNode",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("LoRx.hEndNode",PROPLIST_ITEM(id++,""));
	CElemInfo::AddPropItem("lamdaRxcInfo",PROPLIST_ITEM(id++,"同时受压"));			//同时受压项
	//同时受压第一项
	CElemInfo::AddPropItem("smcItem1",PROPLIST_ITEM(id++,""));					
	CElemInfo::AddPropItem("smcItem1.cSMCType",PROPLIST_ITEM(id++,"受压类型"));			//受压类型
	CElemInfo::AddPropItem("smcItem1.relaElem",PROPLIST_ITEM(id++,"参照单元"));			//参照单元
	CElemInfo::AddPropItem("smcItem1.gyreRadius",PROPLIST_ITEM(id++,"回转半径"));		//回转半径
#endif
	CElemInfo::AddPropItem("smcItem1.LoRxc",PROPLIST_ITEM(id++,""));				//计算长度
	CElemInfo::AddPropItem("smcItem1.LoRxc.coef",PROPLIST_ITEM(id++,""));		//折算系数
	CElemInfo::AddPropItem("smcItem1.LoRxc.CalType",PROPLIST_ITEM(id++,""));		//基准长度类型
	CElemInfo::AddPropItem("smcItem1.LoRxc.hStartNode",PROPLIST_ITEM(id++,""));	//指定长度的起始节点号";
	CElemInfo::AddPropItem("smcItem1.LoRxc.hEndNode",PROPLIST_ITEM(id++,""));	//指定长度的终止节点号
	//同时受压第二项
	CElemInfo::AddPropItem("smcItem2",PROPLIST_ITEM(id++,""));					
	CElemInfo::AddPropItem("smcItem2.cSMCType",PROPLIST_ITEM(id++,""));			//受压类型
	CElemInfo::AddPropItem("smcItem2.relaElem",PROPLIST_ITEM(id++,""));			//参照单元
	CElemInfo::AddPropItem("smcItem2.gyreRadius",PROPLIST_ITEM(id++,""));		//回转半径
	CElemInfo::AddPropItem("smcItem2.LoRxc",PROPLIST_ITEM(id++,""));				//计算长度
	CElemInfo::AddPropItem("smcItem2.LoRxc.coef",PROPLIST_ITEM(id++,""));		//折算系数
	CElemInfo::AddPropItem("smcItem2.LoRxc.CalType",PROPLIST_ITEM(id++,""));		//基准长度类型
	CElemInfo::AddPropItem("smcItem2.LoRxc.hStartNode",PROPLIST_ITEM(id++,""));	//指定长度的起始节点号";
	CElemInfo::AddPropItem("smcItem2.LoRxc.hEndNode",PROPLIST_ITEM(id++,""));	//指定长度的终止节点号
	//同时受压第三项
	CElemInfo::AddPropItem("smcItem3",PROPLIST_ITEM(id++,""));					
	CElemInfo::AddPropItem("smcItem3.cSMCType",PROPLIST_ITEM(id++,""));			//受压类型
	CElemInfo::AddPropItem("smcItem3.relaElem",PROPLIST_ITEM(id++,""));			//参照单元
	CElemInfo::AddPropItem("smcItem3.gyreRadius",PROPLIST_ITEM(id++,""));		//回转半径
	CElemInfo::AddPropItem("smcItem3.LoRxc",PROPLIST_ITEM(id++,""));				//计算长度
	CElemInfo::AddPropItem("smcItem3.LoRxc.coef",PROPLIST_ITEM(id++,""));		//折算系数
	CElemInfo::AddPropItem("smcItem3.LoRxc.CalType",PROPLIST_ITEM(id++,""));		//基准长度类型
	CElemInfo::AddPropItem("smcItem3.LoRxc.hStartNode",PROPLIST_ITEM(id++,""));	//指定长度的起始节点号";
	CElemInfo::AddPropItem("smcItem3.LoRxc.hEndNode",PROPLIST_ITEM(id++,""));	//指定长度的终止节点号
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
	CLDSArcPart::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"环形构件基本信息"));
	CLDSArcPart::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSArcPart::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSArcPart::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	CLDSArcPart::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级",NULL,"默认|A|B|C|D|"));
	CLDSArcPart::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSArcPart::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	CLDSArcPart::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSArcPart::AddPropItem("m_fZoomLenCoef",PROPLIST_ITEM(id++,"下料系数"));
	CLDSArcPart::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"关联杆件"));
	CLDSArcPart::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	CLDSArcPart::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件"));
	CLDSArcPart::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	CLDSArcPart::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助构件"));
	CLDSArcPart::AddPropItem("m_uDisplaySlices",PROPLIST_ITEM(id++,"(已注释)显示片数"));
	CLDSArcPart::AddPropItem("neutro_radius",PROPLIST_ITEM(id++,"中性层圆弧半径"));
	CLDSArcPart::AddPropItem("start_oddment",PROPLIST_ITEM(id++,"始端正负头"));
	CLDSArcPart::AddPropItem("end_oddment",PROPLIST_ITEM(id++,"终端正负头"));
	CLDSArcPart::AddPropItem("offset_h",PROPLIST_ITEM(id++,"横向位移"));
	CLDSArcPart::AddPropItem("offset_v",PROPLIST_ITEM(id++,"竖向位移"));
	CLDSArcPart::AddPropItem("arc_start",PROPLIST_ITEM(id++,""));
	CLDSArcPart::AddPropItem("arc_start.x",PROPLIST_ITEM(id++,""));
	CLDSArcPart::AddPropItem("arc_start.y",PROPLIST_ITEM(id++,""));
	CLDSArcPart::AddPropItem("arc_start.z",PROPLIST_ITEM(id++,""));
	CLDSArcPart::AddPropItem("start_rot_angle",PROPLIST_ITEM(id++,"起始偏转角"));
	CLDSArcPart::AddPropItem("sector_angle",PROPLIST_ITEM(id++,"圆弧扇形角"));
	CLDSArcPart::AddPropItem("m_siLayoutStyle",PROPLIST_ITEM(id++,"摆放方式"));
	CLDSArcPart::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色"));
	CLDSArcPart::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	CLDSArcPart::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
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
	//始终端
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
	CLDSArcAngle::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"环形角钢基本信息"));
	CLDSArcAngle::AddPropItem("geometryInfo",PROPLIST_ITEM(id++,"形位信息"));
	CLDSArcAngle::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSArcAngle::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSArcAngle::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	CLDSArcAngle::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级",NULL,"默认|A|B|C|D|"));
	CLDSArcAngle::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSArcAngle::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	CLDSArcAngle::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSArcAngle::AddPropItem("m_fZoomLenCoef",PROPLIST_ITEM(id++,"下料系数"));
	CLDSArcAngle::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	CLDSArcAngle::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件"));
	CLDSArcAngle::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	CLDSArcAngle::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助构件"));
	CLDSArcAngle::AddPropItem("m_uDisplaySlices",PROPLIST_ITEM(id++,"(已注释)显示片数"));
	CLDSArcAngle::AddPropItem("neutro_radius",PROPLIST_ITEM(id++,"中性层圆弧半径"));
	CLDSArcAngle::AddPropItem("arc_radius",PROPLIST_ITEM(id++,"圆弧半径"));
	CLDSArcAngle::AddPropItem("offset_h",PROPLIST_ITEM(id++,"横向位移"));
	CLDSArcAngle::AddPropItem("offset_v",PROPLIST_ITEM(id++,"竖向位移"));
	CLDSArcAngle::AddPropItem("arc_start",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("arc_start.x",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("arc_start.y",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("arc_start.z",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("start_rot_angle",PROPLIST_ITEM(id++,"起始偏转角"));
	CLDSArcAngle::AddPropItem("sector_angle",PROPLIST_ITEM(id++,"圆弧扇形角"));
	CLDSArcAngle::AddPropItem("des_arc_center",PROPLIST_ITEM(id++,"圆心位置"));
	CLDSArcAngle::AddPropItem("des_arc_start",PROPLIST_ITEM(id++,"圆弧起点"));
	CLDSArcAngle::AddPropItem("des_arc_norm",PROPLIST_ITEM(id++,"工作面法线"));
	CLDSArcAngle::AddPropItem("m_siLayoutStyle",PROPLIST_ITEM(id++,"摆放方式"));
	CLDSArcAngle::AddPropItem("start_oddment",PROPLIST_ITEM(id++,"始端正负头"));
	CLDSArcAngle::AddPropItem("end_oddment",PROPLIST_ITEM(id++,"终端正负头"));
	CLDSArcAngle::AddPropItem("specification",PROPLIST_ITEM(id++,"规格型号"));
	CLDSArcAngle::AddPropItem("wing_wide",PROPLIST_ITEM(id++,"肢宽"));
	CLDSArcAngle::AddPropItem("wing_thick",PROPLIST_ITEM(id++,"肢厚"));
	CLDSArcAngle::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色"));
	CLDSArcAngle::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	CLDSArcAngle::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
	CLDSArcAngle::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"装配方位"));
	CLDSArcAngle::AddPropItem("center",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("center.x",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("center.y",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("center.z",PROPLIST_ITEM(id++,""));
	CLDSArcAngle::AddPropItem("workNorm",PROPLIST_ITEM(id++,"工作法线"));
	CLDSArcAngle::AddPropItem("workNorm.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSArcAngle::AddPropItem("workNorm.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSArcAngle::AddPropItem("workNorm.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSArcAngle::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"位置"));
	CLDSArcAngle::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSArcAngle::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSArcAngle::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSArcAngle::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X轴方向"));
	CLDSArcAngle::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSArcAngle::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSArcAngle::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSArcAngle::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y轴方向"));
	CLDSArcAngle::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSArcAngle::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSArcAngle::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSArcAngle::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z轴方向"));
	CLDSArcAngle::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSArcAngle::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSArcAngle::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	//始终端
	CLDSArcAngle::AddPropItem("startInfo",PROPLIST_ITEM(id++,"始端信息"));
	CLDSArcAngle::AddPropItem("hStart",PROPLIST_ITEM(id++,"起点句柄"));
	CLDSArcAngle::AddPropItem("startPos",PROPLIST_ITEM(id++,"始端设计坐标"));
	CLDSArcAngle::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"始端实际坐标"));
	CLDSArcAngle::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSArcAngle::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSArcAngle::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSArcAngle::AddPropItem("endInfo",PROPLIST_ITEM(id++,"终端信息"));
	CLDSArcAngle::AddPropItem("hEnd",PROPLIST_ITEM(id++,"终点句柄"));
	CLDSArcAngle::AddPropItem("endPos",PROPLIST_ITEM(id++,"终端设计坐标"));
	CLDSArcAngle::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"终端实际坐标"));
	CLDSArcAngle::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSArcAngle::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSArcAngle::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z坐标"));
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
	CLDSArcFlat::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"环形扁钢基本信息"));
	CLDSArcFlat::AddPropItem("geometryInfo",PROPLIST_ITEM(id++,"形位信息"));
	CLDSArcFlat::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSArcFlat::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSArcFlat::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	CLDSArcFlat::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级",NULL,"默认|A|B|C|D|"));
	CLDSArcFlat::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSArcFlat::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	CLDSArcFlat::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSArcFlat::AddPropItem("m_fZoomLenCoef",PROPLIST_ITEM(id++,"下料系数"));
	CLDSArcFlat::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	CLDSArcFlat::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件"));
	CLDSArcFlat::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	CLDSArcFlat::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助构件"));
	CLDSArcFlat::AddPropItem("m_uDisplaySlices",PROPLIST_ITEM(id++,"(已注释)显示片数"));
	CLDSArcFlat::AddPropItem("neutro_radius",PROPLIST_ITEM(id++,"中性层圆弧半径"));
	CLDSArcFlat::AddPropItem("arc_radius",PROPLIST_ITEM(id++,"圆弧半径"));
	CLDSArcFlat::AddPropItem("offset_h",PROPLIST_ITEM(id++,"横向位移"));
	CLDSArcFlat::AddPropItem("offset_v",PROPLIST_ITEM(id++,"竖向位移"));
	CLDSArcFlat::AddPropItem("arc_start",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("arc_start.x",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("arc_start.y",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("arc_start.z",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("start_rot_angle",PROPLIST_ITEM(id++,"起始偏转角"));
	CLDSArcFlat::AddPropItem("sector_angle",PROPLIST_ITEM(id++,"圆弧扇形角"));
	CLDSArcFlat::AddPropItem("des_arc_center",PROPLIST_ITEM(id++,"圆心位置"));
	CLDSArcFlat::AddPropItem("des_arc_start",PROPLIST_ITEM(id++,"圆弧起点"));
	CLDSArcFlat::AddPropItem("des_arc_norm",PROPLIST_ITEM(id++,"工作面法线"));
	CLDSArcFlat::AddPropItem("m_siLayoutStyle",PROPLIST_ITEM(id++,"摆放方式"));
	CLDSArcFlat::AddPropItem("start_oddment",PROPLIST_ITEM(id++,"始端正负头"));
	CLDSArcFlat::AddPropItem("end_oddment",PROPLIST_ITEM(id++,"终端正负头"));
	CLDSArcFlat::AddPropItem("specification",PROPLIST_ITEM(id++,"规格型号"));
	CLDSArcFlat::AddPropItem("wide",PROPLIST_ITEM(id++,"宽度"));
	CLDSArcFlat::AddPropItem("thick",PROPLIST_ITEM(id++,"厚度"));
	CLDSArcFlat::AddPropItem("m_bUSerSpecColor",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	CLDSArcFlat::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
	CLDSArcFlat::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"装配方位"));
	CLDSArcFlat::AddPropItem("center",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("center.x",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("center.y",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("center.z",PROPLIST_ITEM(id++,""));
	CLDSArcFlat::AddPropItem("workNorm",PROPLIST_ITEM(id++,"工作法线"));
	CLDSArcFlat::AddPropItem("workNorm.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSArcFlat::AddPropItem("workNorm.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSArcFlat::AddPropItem("workNorm.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSArcFlat::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"位置"));
	CLDSArcFlat::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSArcFlat::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSArcFlat::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSArcFlat::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X轴方向"));
	CLDSArcFlat::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSArcFlat::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSArcFlat::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSArcFlat::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y轴方向"));
	CLDSArcFlat::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSArcFlat::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSArcFlat::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSArcFlat::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z轴方向"));
	CLDSArcFlat::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSArcFlat::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSArcFlat::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	//始终端
	CLDSArcFlat::AddPropItem("startInfo",PROPLIST_ITEM(id++,"始端信息"));
	CLDSArcFlat::AddPropItem("hStart",PROPLIST_ITEM(id++,"起点句柄"));
	CLDSArcFlat::AddPropItem("oddStart",PROPLIST_ITEM(id++,"正负头"));
	CLDSArcFlat::AddPropItem("startPos",PROPLIST_ITEM(id++,"始端设计坐标"));
	CLDSArcFlat::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"始端实际坐标"));
	CLDSArcFlat::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSArcFlat::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSArcFlat::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSArcFlat::AddPropItem("endInfo",PROPLIST_ITEM(id++,"终端信息"));
	CLDSArcFlat::AddPropItem("hEnd",PROPLIST_ITEM(id++,"终点句柄"));
	CLDSArcFlat::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"正负头"));
	CLDSArcFlat::AddPropItem("endPos",PROPLIST_ITEM(id++,"终端设计坐标"));
	CLDSArcFlat::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"终端实际坐标"));
	CLDSArcFlat::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSArcFlat::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSArcFlat::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z坐标"));
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
	CLDSArcSlot::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"环形槽钢基本信息"));
	CLDSArcSlot::AddPropItem("geometryInfo",PROPLIST_ITEM(id++,"形位信息"));
	CLDSArcSlot::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSArcSlot::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSArcSlot::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	CLDSArcSlot::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级",NULL,"默认|A|B|C|D|"));
	CLDSArcSlot::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSArcSlot::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	CLDSArcSlot::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSArcSlot::AddPropItem("m_fZoomLenCoef",PROPLIST_ITEM(id++,"下料系数"));
	CLDSArcSlot::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	CLDSArcSlot::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件"));
	CLDSArcSlot::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	CLDSArcSlot::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助构件"));
	CLDSArcSlot::AddPropItem("m_uDisplaySlices",PROPLIST_ITEM(id++,"(已注释)显示片数"));
	CLDSArcSlot::AddPropItem("neutro_radius",PROPLIST_ITEM(id++,"中性层圆弧半径"));
	CLDSArcSlot::AddPropItem("arc_radius",PROPLIST_ITEM(id++,"圆弧半径"));
	CLDSArcSlot::AddPropItem("offset_h",PROPLIST_ITEM(id++,"横向位移"));
	CLDSArcSlot::AddPropItem("offset_v",PROPLIST_ITEM(id++,"竖向位移"));
	CLDSArcSlot::AddPropItem("arc_start",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("arc_start.x",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("arc_start.y",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("arc_start.z",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("start_rot_angle",PROPLIST_ITEM(id++,"起始偏转角"));
	CLDSArcSlot::AddPropItem("sector_angle",PROPLIST_ITEM(id++,"圆弧扇形角"));
	CLDSArcSlot::AddPropItem("des_arc_center",PROPLIST_ITEM(id++,"圆心位置"));
	CLDSArcSlot::AddPropItem("des_arc_start",PROPLIST_ITEM(id++,"圆弧起点"));
	CLDSArcSlot::AddPropItem("des_arc_norm",PROPLIST_ITEM(id++,"工作面法线"));
	CLDSArcSlot::AddPropItem("m_siLayoutStyle",PROPLIST_ITEM(id++,"摆放方式"));
	CLDSArcSlot::AddPropItem("start_oddment",PROPLIST_ITEM(id++,"始端正负头"));
	CLDSArcSlot::AddPropItem("end_oddment",PROPLIST_ITEM(id++,"终端正负头"));
	CLDSArcSlot::AddPropItem("specification",PROPLIST_ITEM(id++,"规格型号"));
	CLDSArcSlot::AddPropItem("height",PROPLIST_ITEM(id++,"截面高"));
	CLDSArcSlot::AddPropItem("wide",PROPLIST_ITEM(id++,"肢宽"));
	CLDSArcSlot::AddPropItem("thick",PROPLIST_ITEM(id++,"肢厚"));
	CLDSArcSlot::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色"));
	CLDSArcSlot::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	CLDSArcSlot::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
	CLDSArcSlot::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"装配方位"));
	CLDSArcSlot::AddPropItem("center",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("center.x",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("center.y",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("center.z",PROPLIST_ITEM(id++,""));
	CLDSArcSlot::AddPropItem("workNorm",PROPLIST_ITEM(id++,"工作法线"));
	CLDSArcSlot::AddPropItem("workNorm.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSArcSlot::AddPropItem("workNorm.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSArcSlot::AddPropItem("workNorm.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSArcSlot::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"位置"));
	CLDSArcSlot::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSArcSlot::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSArcSlot::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSArcSlot::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X轴方向"));
	CLDSArcSlot::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSArcSlot::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSArcSlot::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSArcSlot::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y轴方向"));
	CLDSArcSlot::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSArcSlot::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSArcSlot::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSArcSlot::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z轴方向"));
	CLDSArcSlot::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSArcSlot::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSArcSlot::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	//
	CLDSArcSlot::AddPropItem("startInfo",PROPLIST_ITEM(id++,"始端信息"));
	CLDSArcSlot::AddPropItem("hStart",PROPLIST_ITEM(id++,"起点句柄"));
	CLDSArcSlot::AddPropItem("oddStart",PROPLIST_ITEM(id++,"正负头"));
	CLDSArcSlot::AddPropItem("startPos",PROPLIST_ITEM(id++,"始端设计坐标"));
	CLDSArcSlot::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"始端实际坐标"));
	CLDSArcSlot::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSArcSlot::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSArcSlot::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSArcSlot::AddPropItem("endInfo",PROPLIST_ITEM(id++,"终端信息"));
	CLDSArcSlot::AddPropItem("hEnd",PROPLIST_ITEM(id++,"终点句柄"));
	CLDSArcSlot::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"正负头"));
	CLDSArcSlot::AddPropItem("endPos",PROPLIST_ITEM(id++,"终端设计坐标"));
	CLDSArcSlot::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"终端实际坐标"));
	CLDSArcSlot::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSArcSlot::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSArcSlot::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z坐标"));
#endif
}

MEMORY_BLOCK CLDSLineTube::MPCL;	//Mirror Property Control List
PROPLIST_TYPE CLDSLineTube::propMirSync;
void CLDSLineTube::InitPropMirBitsMap()
{
	int id=1;
	propMirSync.InitHashtableSize(32);
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	//propMirSync.AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("specification",PROPLIST_ITEM(id++,"规格"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	propMirSync.AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("Position",PROPLIST_ITEM(id++,"端点位置"));
	propMirSync.AddPropItem("oddStart",PROPLIST_ITEM(id++,"始端正负头"));
	propMirSync.AddPropItem("oddEnd",PROPLIST_ITEM(id++,"终端正负头"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("TransectFace",PROPLIST_ITEM(id++,"相贯信息"));
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
	CLDSLineTube::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//影射信息
	CLDSLineTube::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Project Mother Part"));//影射母构件
	CLDSLineTube::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//归属的部件引用
	CLDSLineTube::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//归属的部件
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
	//内力分析所需要的属性
	CLDSLineTube::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"Statistic Material Serial Number"));	//统材编号
	CLDSLineTube::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"Gravity Zoom Out Coefficient"));
	CLDSLineTube::AddPropItem("size.idClassType",PROPLIST_ITEM(id++,"Rod Type"));
	//设计信息
	CLDSLineTube::AddPropItem("designInfo",PROPLIST_ITEM(id++,"Rod Check Information"));
	CLDSLineTube::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"Elem Type"));
	CLDSLineTube::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"Whether Material Optimization"));
	CLDSLineTube::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"Whether Specification Optimization"));
	CLDSLineTube::AddPropItem("hole_out",PROPLIST_ITEM(id++,"Bolt Hole Out"));
	CLDSLineTube::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"Calculate Automatic Bolt Hole Out"));
		//计算长度信息
	CLDSLineTube::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,"Reduce Calculate Coefficient"));
	CLDSLineTube::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineTube::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,"Calculate Length Type"));
	CLDSLineTube::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,"Reference Horizon Pole"));
	CLDSLineTube::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,"Simultaneously Compression Reference Rod"));
	CLDSLineTube::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,"Minimum Axial Length"));
	CLDSLineTube::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,"Parallel Axial Length"));
		//始端设计信息
	CLDSLineTube::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"Start Information"));
	CLDSLineTube::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"Start Joint Type"));
	CLDSLineTube::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"Start Force Type"));
	CLDSLineTube::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
		//终端设计信息
	CLDSLineTube::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"End Information"));
	CLDSLineTube::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"End Joint Type"));
	CLDSLineTube::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"End Force Type"));
	CLDSLineTube::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,""));
#else 
	CLDSLineTube::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"钢管基本信息"));
	CLDSLineTube::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSLineTube::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSLineTube::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	CLDSLineTube::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级",NULL,"默认|A|B|C|D|"));
	CLDSLineTube::AddPropItem("specification",PROPLIST_ITEM(id++,"规格"));
	CLDSLineTube::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSLineTube::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	CLDSLineTube::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSLineTube::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"关联构件"));
	CLDSLineTube::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"影射信息"));//影射信息
	CLDSLineTube::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"影射母构件"));//影射母构件
	CLDSLineTube::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"归属部件引用"));//归属的部件引用
	CLDSLineTube::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"归属部件"));//归属的部件
	CLDSLineTube::AddPropItem("m_bLiveNeedSupplyPart",PROPLIST_ITEM(id++,"现场下料"));
	CLDSLineTube::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	CLDSLineTube::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件","如果此构件需要焊接到其它构件上,那么此属性就是本构件所依附的焊接构件的句柄;如果此构件不需要焊接,此值为0X0。"));
	CLDSLineTube::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔","是否显示孔","是|否"));
	CLDSLineTube::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助构件","虚拟辅助构件只用于辅助其它构件定位","是|否"));
	CLDSLineTube::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"预拱项"));
	CLDSLineTube::AddPropItem("length",PROPLIST_ITEM(id++,"实际长度"));
	CLDSLineTube::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色","构件颜色是否为用户指定","是|否"));
	CLDSLineTube::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	CLDSLineTube::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
	CLDSLineTube::AddPropItem("weldRoadAngle",PROPLIST_ITEM(id++,"焊道夹角"));
	CLDSLineTube::AddPropItem("m_iWeldRoadStyle",PROPLIST_ITEM(id++,"焊道线指定方式", "直接给定还是通过与基准边夹角定位", "0.直接指定|1.角度定位"));
	CLDSLineTube::AddPropItem("nearWeldRoadVec",PROPLIST_ITEM(id++,"焊道方向"));
	CLDSLineTube::AddPropItem("nearWeldRoadVec.x",PROPLIST_ITEM(id++,"焊道方向X分量"));
	CLDSLineTube::AddPropItem("nearWeldRoadVec.y",PROPLIST_ITEM(id++,"焊道方向Y分量"));
	CLDSLineTube::AddPropItem("nearWeldRoadVec.z",PROPLIST_ITEM(id++,"焊道方向Z分量"));
	CLDSLineTube::AddPropItem("workPlaneNorm",PROPLIST_ITEM(id++,"基准面法线"));
	CLDSLineTube::AddPropItem("hWorkNormRefPart",PROPLIST_ITEM(id++,"法线参照杆件"));
	CLDSLineTube::AddPropItem("m_iNormCalStyle",PROPLIST_ITEM(id++,"计算方式","直接给定还是通过父杆件给定", "0.直接指定|1.参照杆件|2.共面参照点"));
	CLDSLineTube::AddPropItem("workPlaneNorm.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSLineTube::AddPropItem("workPlaneNorm.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSLineTube::AddPropItem("workPlaneNorm.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSLineTube::AddPropItem("pickWorkPlane",PROPLIST_ITEM(id++,"基准面近似法线","钢管基准面近似法线一般由用户指定，系统可根据近似法线及钢管轴线精确计算出钢管的基准面法线。"));
	CLDSLineTube::AddPropItem("pickWorkPlane.x",PROPLIST_ITEM(id++,"X坐标分量","钢管基准面法线X轴方向的坐标"));
	CLDSLineTube::AddPropItem("pickWorkPlane.y",PROPLIST_ITEM(id++,"Y坐标分量","钢管基准面法线Y轴方向的坐标"));
	CLDSLineTube::AddPropItem("pickWorkPlane.z",PROPLIST_ITEM(id++,"Z坐标分量","钢管基准面法线Z轴方向的坐标"));
	CLDSLineTube::AddPropItem("cutPlane",PROPLIST_ITEM(id++,"切割面"));
	CLDSLineTube::AddPropItem("cutPlaneNorm",PROPLIST_ITEM(id++,"法线"));
	CLDSLineTube::AddPropItem("cutPlaneNorm.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSLineTube::AddPropItem("cutPlaneNorm.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSLineTube::AddPropItem("cutPlaneNorm.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSLineTube::AddPropItem("cutPlaneOrigin",PROPLIST_ITEM(id++,"基点"));
	CLDSLineTube::AddPropItem("cutPlaneOrigin.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSLineTube::AddPropItem("cutPlaneOrigin.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSLineTube::AddPropItem("cutPlaneOrigin.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSLineTube::AddPropItem("startInfo",PROPLIST_ITEM(id++,"始端信息"));
	CLDSLineTube::AddPropItem("hStart",PROPLIST_ITEM(id++,"起点句柄"));
	CLDSLineTube::AddPropItem("oddStart",PROPLIST_ITEM(id++,"正负头"));
	CLDSLineTube::AddPropItem("oddStart.m_iOddCalStyle", PROPLIST_ITEM(id++, "计算原则","","0.碰撞干涉|1.相贯检测|2.指定数值"));
	CLDSLineTube::AddPropItem("oddStart.m_fCollideDist", PROPLIST_ITEM(id++, "碰撞间隙"));
	CLDSLineTube::AddPropItem("oddStart.m_hRefPart1", PROPLIST_ITEM(id++, "参照构件1"));
	CLDSLineTube::AddPropItem("oddStart.m_hRefPart2", PROPLIST_ITEM(id++, "参照构件2"));
	CLDSLineTube::AddPropItem("jointStart",PROPLIST_ITEM(id++,"始端连接"));
	CLDSLineTube::AddPropItem("startPos",PROPLIST_ITEM(id++,"始端设计坐标"));
	CLDSLineTube::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"始端实际坐标"));
	CLDSLineTube::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSLineTube::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSLineTube::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSLineTube::AddPropItem("desStartPos",PROPLIST_ITEM(id++,""));
	CLDSLineTube::AddPropItem("desStartPos.endPosType",PROPLIST_ITEM(id++,"定位方式","","节点即基点|搭接连接|点线投影|心线交点|指定坐标"));
	CLDSLineTube::AddPropItem("desStartPos.datum_pos_style",PROPLIST_ITEM(id++,"基点定位方式"));
	CLDSLineTube::AddPropItem("desStartPos.hDatumPart",PROPLIST_ITEM(id++,"基准杆件"));
	CLDSLineTube::AddPropItem("desStartPos.fNormOffset",PROPLIST_ITEM(id++,"基准面法线偏移量"));
	CLDSLineTube::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"偏心距"));
	CLDSLineTube::AddPropItem("desStartPos.fLenVecOffsetDist",PROPLIST_ITEM(id++,"基点延伸方向偏移量"));
	CLDSLineTube::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"平推面法线"));
	CLDSLineTube::AddPropItem("desStartPos.hDatumStartPart",PROPLIST_ITEM(id++,"投影线始端所在基准钢管"));
	CLDSLineTube::AddPropItem("desStartPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"基准钢管的基准端点","","始端|终端"));
	CLDSLineTube::AddPropItem("desStartPos.hDatumEndPart",PROPLIST_ITEM(id++,"投影线终端所在基准钢管"));
	CLDSLineTube::AddPropItem("desStartPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"基准钢管的基准端点", "", "始端|终端"));
	CLDSLineTube::AddPropItem("endInfo",PROPLIST_ITEM(id++,"终端信息"));
	CLDSLineTube::AddPropItem("hEnd",PROPLIST_ITEM(id++,"终点句柄"));
	CLDSLineTube::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"正负头"));
	CLDSLineTube::AddPropItem("oddEnd.m_iOddCalStyle", PROPLIST_ITEM(id++, "计算原则", "", "0.碰撞干涉|1.相贯检测|2.指定数值"));
	CLDSLineTube::AddPropItem("oddEnd.m_fCollideDist", PROPLIST_ITEM(id++, "碰撞间隙"));
	CLDSLineTube::AddPropItem("oddEnd.m_hRefPart1", PROPLIST_ITEM(id++, "参照构件1"));
	CLDSLineTube::AddPropItem("oddEnd.m_hRefPart2", PROPLIST_ITEM(id++, "参照构件2"));
	CLDSLineTube::AddPropItem("jointEnd",PROPLIST_ITEM(id++,"终端连接"));
	CLDSLineTube::AddPropItem("endPos",PROPLIST_ITEM(id++,"终端设计坐标"));
	CLDSLineTube::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"终端实际坐标"));
	CLDSLineTube::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSLineTube::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSLineTube::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSLineTube::AddPropItem("desEndPos",PROPLIST_ITEM(id++,""));
	CLDSLineTube::AddPropItem("desEndPos.endPosType",PROPLIST_ITEM(id++,"定位方式","钢管终端基点位置", "节点即基点|搭接连接|点线投影|心线交点|指定坐标"));
	CLDSLineTube::AddPropItem("desEndPos.datum_pos_style",PROPLIST_ITEM(id++,"基点定位方式"));
	CLDSLineTube::AddPropItem("desEndPos.hDatumPart",PROPLIST_ITEM(id++,"基准杆件"));
	CLDSLineTube::AddPropItem("desEndPos.fNormOffset",PROPLIST_ITEM(id++,"基准面法线偏移量"));
	CLDSLineTube::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,"偏心距"));
	CLDSLineTube::AddPropItem("desEndPos.fLenVecOffsetDist",PROPLIST_ITEM(id++,"基点延伸方向偏移量"));
	CLDSLineTube::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,"平推面法线"));
	CLDSLineTube::AddPropItem("desEndPos.hDatumStartPart",PROPLIST_ITEM(id++,"投影线始端所在基准钢管"));
	CLDSLineTube::AddPropItem("desEndPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"基准钢管的基准端点", "", "始端|终端"));
	CLDSLineTube::AddPropItem("desEndPos.hDatumEndPart",PROPLIST_ITEM(id++,"投影线终端所在基准钢管"));
	CLDSLineTube::AddPropItem("desEndPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"基准钢管的基准端点", "", "始端|终端"));
	CLDSLineTube::AddPropItem("m_bHasWeldRoad",PROPLIST_ITEM(id++,"有焊道线","是否存在焊道线,对于无缝钢管来说不存在焊道线","是|否"));
	CLDSLineTube::AddPropItem("startWorkPlane",PROPLIST_ITEM(id++,"钢管工作面"));
	CLDSLineTube::AddPropItem("endWorkPlane",PROPLIST_ITEM(id++,"钢管工作面"));
	CLDSLineTube::AddPropItem("bushTube",PROPLIST_ITEM(id++,"套管定位参数", "钢管为套管时,根据依附钢板句柄、定位螺栓进行定位。"));
	CLDSLineTube::AddPropItem("bush.height",PROPLIST_ITEM(id++,"套管高度"));
	CLDSLineTube::AddPropItem("bush.hBushResidePlate",PROPLIST_ITEM(id++,"依赖钢板"));
	CLDSLineTube::AddPropItem("bush.hBushRelyBolt",PROPLIST_ITEM(id++,"定位螺栓"));
	//内力分析所需要的属性
	CLDSLineTube::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"统材编号"));	//统材编号
	CLDSLineTube::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"重力放大系数"));
	CLDSLineTube::AddPropItem("size.idClassType",PROPLIST_ITEM(id++,"杆件类型"));
	//设计信息
	CLDSLineTube::AddPropItem("designInfo",PROPLIST_ITEM(id++,"杆件验算信息"));
	CLDSLineTube::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"单元类型"));
	CLDSLineTube::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"是否进行材质优选"));
	CLDSLineTube::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"是否进行规格优选"));
	CLDSLineTube::AddPropItem("hole_out",PROPLIST_ITEM(id++,"螺栓减孔数"));
	CLDSLineTube::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"自动计算螺栓减孔数"));
		//计算长度信息
	CLDSLineTube::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,"折算系数"));
	CLDSLineTube::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineTube::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,"计算长度类型"));
	CLDSLineTube::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,"参照横连杆"));
	CLDSLineTube::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,"同时受压参照杆件"));
	CLDSLineTube::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,"最小轴长度"));
	CLDSLineTube::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,"平行轴长度"));
		//始端设计信息
	CLDSLineTube::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"始端信息"));
	CLDSLineTube::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"始端约束类型"));
	CLDSLineTube::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"始端受力类型"));
	CLDSLineTube::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
		//终端设计信息
	CLDSLineTube::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"终端信息"));
	CLDSLineTube::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"终端约束类型"));
	CLDSLineTube::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"终端受力类型"));
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
	CLDSLineFlat::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//影射信息
	CLDSLineFlat::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Project Mother Part"));//影射母构件
	CLDSLineFlat::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//归属的部件引用
	CLDSLineFlat::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//归属的部件
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
	//内力分析所需要的属性
	CLDSLineFlat::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"Statistic Material Serial Number"));	//统材编号
	CLDSLineFlat::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"Gravity Zoom Out Coefficient"));
	//设计信息
	CLDSLineFlat::AddPropItem("designInfo",PROPLIST_ITEM(id++,"Rod Check Information"));
	CLDSLineFlat::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"Elem Type"));
	CLDSLineFlat::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"Whether Material Optimization"));
	CLDSLineFlat::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"Whether Specification Optimization"));
	CLDSLineFlat::AddPropItem("hole_out",PROPLIST_ITEM(id++,"Bolt Hole Out"));
	CLDSLineFlat::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"Calculate Automatic Bolt Hole Out"));
		//计算长度信息
	CLDSLineFlat::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,"Reduce Calculate Coefficient"));
	CLDSLineFlat::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineFlat::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,"Calculate Length Type"));
	CLDSLineFlat::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,"Reference Horizon Pole"));
	CLDSLineFlat::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,"Simultaneously Compression Reference Rod"));
	CLDSLineFlat::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,"Minimum Axial Length"));
	CLDSLineFlat::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,"Parallel Axial Length"));
		//始端设计信息
	CLDSLineFlat::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"Start Information"));
	CLDSLineFlat::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"Start Joint Type"));
	CLDSLineFlat::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"Start Force Type"));
	CLDSLineFlat::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
		//终端设计信息
	CLDSLineFlat::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"End Information"));
	CLDSLineFlat::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"End Joint Type"));
	CLDSLineFlat::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"End Force Type"));
	CLDSLineFlat::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,""));
#else 
	CLDSLineFlat::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"扁钢基本信息"));
	CLDSLineFlat::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSLineFlat::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSLineFlat::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	CLDSLineFlat::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级",NULL,"默认|A|B|C|D|"));
	CLDSLineFlat::AddPropItem("specification",PROPLIST_ITEM(id++,"规格"));
	CLDSLineFlat::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSLineFlat::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	CLDSLineFlat::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSLineFlat::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"关联构件"));
	CLDSLineFlat::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"影射信息"));//影射信息
	CLDSLineFlat::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"影射母构件"));//影射母构件
	CLDSLineFlat::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"归属部件引用"));//归属的部件引用
	CLDSLineFlat::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"归属部件"));//归属的部件
	CLDSLineFlat::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	CLDSLineFlat::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件"));
	CLDSLineFlat::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	CLDSLineFlat::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助构件"));
	CLDSLineFlat::AddPropItem("length",PROPLIST_ITEM(id++,"实际长度"));
	CLDSLineFlat::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色"));
	CLDSLineFlat::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	CLDSLineFlat::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
	CLDSLineFlat::AddPropItem("workPlaneNorm",PROPLIST_ITEM(id++,"基准面法线"));
	CLDSLineFlat::AddPropItem("hWorkNormRefPart",PROPLIST_ITEM(id++,"法线参照杆件"));
	CLDSLineFlat::AddPropItem("workPlaneNorm.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSLineFlat::AddPropItem("workPlaneNorm.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSLineFlat::AddPropItem("workPlaneNorm.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSLineFlat::AddPropItem("nearWorkPlaneNorm",PROPLIST_ITEM(id++,"基准面近似法线"));
	CLDSLineFlat::AddPropItem("m_iNormCalStyle",PROPLIST_ITEM(id++,"计算方式"));
	CLDSLineFlat::AddPropItem("nearWorkPlaneNorm.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSLineFlat::AddPropItem("nearWorkPlaneNorm.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSLineFlat::AddPropItem("nearWorkPlaneNorm.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSLineFlat::AddPropItem("startInfo",PROPLIST_ITEM(id++,"始端信息"));
	CLDSLineFlat::AddPropItem("hStart",PROPLIST_ITEM(id++,"起点句柄"));
	CLDSLineFlat::AddPropItem("oddStart",PROPLIST_ITEM(id++,"正负头"));
	CLDSLineFlat::AddPropItem("startPos",PROPLIST_ITEM(id++,"始端设计坐标"));
	CLDSLineFlat::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"始端实际坐标"));
	CLDSLineFlat::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSLineFlat::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSLineFlat::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSLineFlat::AddPropItem("desStartPos",PROPLIST_ITEM(id++,""));
	CLDSLineFlat::AddPropItem("desStartPos.endPosType",PROPLIST_ITEM(id++,"定位方式"));
	CLDSLineFlat::AddPropItem("desStartPos.datum_pos_style",PROPLIST_ITEM(id++,"基点定位方式"));
	CLDSLineFlat::AddPropItem("desStartPos.hDatumPart",PROPLIST_ITEM(id++,"基准杆件"));
	CLDSLineFlat::AddPropItem("desStartPos.fNormOffset",PROPLIST_ITEM(id++,"基准面法线偏移量"));
	CLDSLineFlat::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"偏心距"));
	CLDSLineFlat::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"平推面法线"));
	CLDSLineFlat::AddPropItem("desStartPos.fTransverseOffset",PROPLIST_ITEM(id++,"横向偏移量"));
	CLDSLineFlat::AddPropItem("desStartPos.hDatumStartPart",PROPLIST_ITEM(id++,"投影线始端所在基准杆件"));
	CLDSLineFlat::AddPropItem("desStartPos.bDatumStartStartEnd", PROPLIST_ITEM(id++, "基准杆件的基准端点"));
	CLDSLineFlat::AddPropItem("desStartPos.hDatumEndPart",PROPLIST_ITEM(id++,"投影线终端所在基准杆件"));
	CLDSLineFlat::AddPropItem("desStartPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"基准杆件的基准端点"));
	CLDSLineFlat::AddPropItem("endInfo",PROPLIST_ITEM(id++,"终端信息"));
	CLDSLineFlat::AddPropItem("hEnd",PROPLIST_ITEM(id++,"终点句柄"));
	CLDSLineFlat::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"正负头"));
	CLDSLineFlat::AddPropItem("endPos",PROPLIST_ITEM(id++,"终端设计坐标"));
	CLDSLineFlat::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"终端实际坐标"));
	CLDSLineFlat::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSLineFlat::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSLineFlat::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSLineFlat::AddPropItem("desEndPos",PROPLIST_ITEM(id++,""));
	CLDSLineFlat::AddPropItem("desEndPos.endPosType",PROPLIST_ITEM(id++,"定位方式"));
	CLDSLineFlat::AddPropItem("desEndPos.datum_pos_style",PROPLIST_ITEM(id++,"基点定位方式"));
	CLDSLineFlat::AddPropItem("desEndPos.hDatumPart",PROPLIST_ITEM(id++,"基准杆件"));
	CLDSLineFlat::AddPropItem("desEndPos.fNormOffset",PROPLIST_ITEM(id++,"基准面法线偏移量"));
	CLDSLineFlat::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,"偏心距"));
	CLDSLineFlat::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,"平推面法线"));
	CLDSLineFlat::AddPropItem("desEndPos.fTransverseOffset",PROPLIST_ITEM(id++,"横向偏移量"));
	CLDSLineFlat::AddPropItem("desEndPos.hDatumStartPart",PROPLIST_ITEM(id++,"投影线始端所在基准杆件"));
	CLDSLineFlat::AddPropItem("desEndPos.bDatumStartStartEnd", PROPLIST_ITEM(id++, "基准杆件的基准端点"));
	CLDSLineFlat::AddPropItem("desEndPos.hDatumEndPart",PROPLIST_ITEM(id++,"投影线终端所在基准杆件"));
	CLDSLineFlat::AddPropItem("desEndPos.bDatumEndStartEnd",PROPLIST_ITEM(id++,"基准杆件的基准端点"));
	//内力分析所需要的属性
	CLDSLineFlat::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"统材编号"));	//统材编号
	CLDSLineFlat::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"重力放大系数"));
	//设计信息
	CLDSLineFlat::AddPropItem("designInfo",PROPLIST_ITEM(id++,"杆件验算信息"));
	CLDSLineFlat::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"单元类型"));
	CLDSLineFlat::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"是否进行材质优选"));
	CLDSLineFlat::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"是否进行规格优选"));
	CLDSLineFlat::AddPropItem("hole_out",PROPLIST_ITEM(id++,"螺栓减孔数"));
	CLDSLineFlat::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"自动计算螺栓减孔数"));
		//计算长度信息
	CLDSLineFlat::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,"折算系数"));
	CLDSLineFlat::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineFlat::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,"计算长度类型"));
	CLDSLineFlat::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,"参照横连杆"));
	CLDSLineFlat::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,"同时受压参照杆件"));
	CLDSLineFlat::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,"最小轴长度"));
	CLDSLineFlat::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,"平行轴长度"));
		//始端设计信息
	CLDSLineFlat::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"始端信息"));
	CLDSLineFlat::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"始端约束类型"));
	CLDSLineFlat::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"始端受力类型"));
	CLDSLineFlat::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
		//终端设计信息
	CLDSLineFlat::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"终端信息"));
	CLDSLineFlat::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"终端约束类型"));
	CLDSLineFlat::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"终端受力类型"));
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
	propMirSync.AddPropItem("manuProcess",PROPLIST_ITEM(id++,"Other Process","Cut Root、Cut Back、Weld Process"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("synchronize_scope",PROPLIST_ITEM(id++,"Property Synchronous Scope","Local Object Property Scope"));
#else 
	propMirSync.AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	//propMirSync.AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("specification",PROPLIST_ITEM(id++,"规格"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	propMirSync.AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	//CLDSLineAngle::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	//CLDSLineAngle::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	//CLDSLineAngle::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助构件"));
	//CLDSLineAngle::AddPropItem("m_bFootNail",PROPLIST_ITEM(id++,"脚钉角钢"));
	//CLDSLineAngle::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	//CLDSLineAngle::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
	//CLDSLineAngle::AddPropItem("hStart",PROPLIST_ITEM(id++,"起点句柄"));
	//CLDSLineAngle::AddPropItem("hEnd",PROPLIST_ITEM(id++,"终点句柄"));
	//CLDSLineAngle::AddPropItem("startInfo",PROPLIST_ITEM(id++,"始端信息"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("m_bEnableTeG",PROPLIST_ITEM(id++,"特殊准距"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("Position",PROPLIST_ITEM(id++,"楞点位置"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("oddStartOnSameLabel",PROPLIST_ITEM(id++,"同编号时始端属性"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("oddEndOnSameLabel",PROPLIST_ITEM(id++,"同编号时终端属性"));
	propMirSync.AddPropItem("oddStart",PROPLIST_ITEM(id++,"始端正负头"));
	//CLDSLineAngle::AddPropItem("endInfo",PROPLIST_ITEM(id++,"终端信息"));
	propMirSync.AddPropItem("oddEnd",PROPLIST_ITEM(id++,"终端正负头"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("linkInfo",PROPLIST_ITEM(id++,"连接螺栓信息"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("wingNormInfo",PROPLIST_ITEM(id++,"肢法线信息"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("kaihe_base_wing_x0_y1",PROPLIST_ITEM(id++,"开合肢"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cutAngleWing",PROPLIST_ITEM(id++,"切角切肢"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("push_flat",PROPLIST_ITEM(id++,"压扁信息"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("manuProcess",PROPLIST_ITEM(id++,"其余工艺信息","清根、铲背、焊接等工艺"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("synchronize_scope",PROPLIST_ITEM(id++,"属性同步范围","局部对象的属性同步范围"));
#endif
	//propMirSync.AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"预拱项"));
	//内力分析所需要的属性
#ifdef __PART_DESIGN_INC_
	propMirSync.AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"统材编号"));	//统材编号
	propMirSync.AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"重力放大系数"));
	propMirSync.AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"单元类型"));//设计信息
	propMirSync.AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"是否进行规格优选"));
	propMirSync.AddPropItem("hole_out",PROPLIST_ITEM(id++,"螺栓减孔数"));
	propMirSync.AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"自动计算螺栓减孔数"));
	propMirSync.AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,"计算长度"));	//计算长度信息
	propMirSync.AddPropItem("joint_type",PROPLIST_ITEM(id++,"约束类型"));	//始端设计信息
	propMirSync.AddPropItem("force_type",PROPLIST_ITEM(id++,"传力类型"));
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
	CLDSLineAngle::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//影射信息
	CLDSLineAngle::AddPropItem("m_bSyncShadowPartNo",PROPLIST_ITEM(id++,"Synchornize part label"));//影射信息
	CLDSLineAngle::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Project Mother Part"));//影射母构件
	CLDSLineAngle::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//归属的部件引用
	CLDSLineAngle::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//归属的部件
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

	CLDSLineAngle::AddPropItem("desStartPos.jgber_cal_style",PROPLIST_ITEM(id++,"Ridge Vertex Locate Mode"));	//0.直接指定楞点坐标；1.参数化定位
	CLDSLineAngle::AddPropItem("desStartPos.origin_pos_style",PROPLIST_ITEM(id++,"Start locating Point"));	//0.节点位置;1.基准点;2.基准角钢关键点;3.另一端节点坐标;4.另一端楞点坐标
	CLDSLineAngle::AddPropItem("desStartPos.datum_jg_h",PROPLIST_ITEM(id++,"Lap-joint Position Datum Angle"));	//搭接位置基准角钢
	//仅在origin_pos_style=2时有效
	CLDSLineAngle::AddPropItem("desStartPos.cDatumAngleWing",PROPLIST_ITEM(id++,"Key Point Affiliate Leg"));	//角钢'X'肢心线上关键点;'Y'肢心线上关键点;其余楞线上关键点
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset.operStyle",PROPLIST_ITEM(id++,"Key Point Compute Mode"));
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset.gStyle",PROPLIST_ITEM(id++,"Center Line Stadia Style"));
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset.offsetDist",PROPLIST_ITEM(id++,"Center Line Stadia"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint",PROPLIST_ITEM(id++,"Datum Point Position"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint.x",PROPLIST_ITEM(id++,"X Coordinate"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint.y",PROPLIST_ITEM(id++,"Y Coordinate"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint.z",PROPLIST_ITEM(id++,"Z Coordinate"));
	CLDSLineAngle::AddPropItem("desStartPos.spatialOperationStyle",PROPLIST_ITEM(id++,"Datum Point Locate Mode"));
	CLDSLineAngle::AddPropItem("desStartPos.len_offset_type",PROPLIST_ITEM(id++,"Datum Plane Offset Type"));		//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量
	CLDSLineAngle::AddPropItem("desStartPos.len_offset_dist",PROPLIST_ITEM(id++,"Datum Plane Offset"));		//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量
	CLDSLineAngle::AddPropItem("desStartPos.datum_offset_dist",PROPLIST_ITEM(id++,"Datum Point Nurmal Offset"));	//搭接角钢投影方式时,搭接点自基准肢沿法线的偏量
	CLDSLineAngle::AddPropItem("desStartPos.cFaceOffsetDatumLine",PROPLIST_ITEM(id++,"Offset Plane Datum Line"));
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"Offset Plane Normal"));
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm.x",PROPLIST_ITEM(id++,"Offset Plane Normal X"));	//平推面法线X坐标
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm.y",PROPLIST_ITEM(id++,"Offset Plane Normal Y"));	//平推面法线Y坐标
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm.z",PROPLIST_ITEM(id++,"Offset Plane Normal Z"));	//平推面法线Z坐标
	CLDSLineAngle::AddPropItem("desStartPos.huoqu_jg_h",PROPLIST_ITEM(id++,"Bending Datum Angle"));		//火曲基准角钢句柄
	CLDSLineAngle::AddPropItem("desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desStartPos.huoqu_dist",PROPLIST_ITEM(id++,"Bending Line Clearance"));		//火曲线间隙
	CLDSLineAngle::AddPropItem("desStartPos.offset_zhun",PROPLIST_ITEM(id++,"Offset Stadia"));		//偏移准线的偏移量
	CLDSLineAngle::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"Eccentric Distance"));	//偏心距==>当角钢搭接在钢管上时才需要此变量 wht 09-10-17
	//仅在spatialOperationStyle=15时有效
	CLDSLineAngle::AddPropItem("desStartPos.coordCtrlType",PROPLIST_ITEM(id++,"Master Control Leg Coordinate"));	//0:X-Z主控;1:Y-Z主控;X-Y主控
	CLDSLineAngle::AddPropItem("desStartPos.cMainCoordCtrlWing",PROPLIST_ITEM(id++,"Master Coordinate Control Leg"));		//角钢主控肢 'X'肢心线上关键点;'Y'肢心线上关键点;其余楞线上关键点
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing",PROPLIST_ITEM(id++,""));				//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing.operStyle",PROPLIST_ITEM(id++,"Master Control Point Locate Mode"));		//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing.gStyle",PROPLIST_ITEM(id++,"Master Control Line Stadia Style"));		//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing.offsetDist",PROPLIST_ITEM(id++,"Master Control Line Stadia"));	//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing",PROPLIST_ITEM(id++,""));				//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing.operStyle",PROPLIST_ITEM(id++,"Ancillary Control Point Locate Mode"));	//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing.gStyle",PROPLIST_ITEM(id++,"Ancillary Line Stadia Style"));		//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing.offsetDist",PROPLIST_ITEM(id++,"Ancillary Line Stadia"));	//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.fLenOffsetOfCoordWing",PROPLIST_ITEM(id++,"Offset Distance Of Coord_Wing"));
	//仅在spatialOperationStyle=16,17时有效
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
	//第二次偏移操作(基点->楞点的矢量偏移)，仅适用于搭接杆件端(楞)点的位置计算
	//BYTE wingXGStyle,wingYGStyle;		//0.垂向偏移;1.X-Y平面平推;2.Y-Z平面平推;3.X-Z平面平推;4.自定义平推面
	//double offset_x_dist,offset_y_dist;	//仅在gStyle=4时有效
	CLDSLineAngle::AddPropItem("desStartPos.datum_to_ber_style",PROPLIST_ITEM(id++,"Datum Point->Ridge Vertex Locate Mode"));
	CLDSLineAngle::AddPropItem("desStartPos.cWorkWingStyle",PROPLIST_ITEM(id++,"Locate Connect Leg"));//自基点进行偏移时当前角钢的基准肢0:自动判断;1:X肢;2:Y肢;3:双肢;其余自动判断
	CLDSLineAngle::AddPropItem("desStartPos.wing_x_offset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desStartPos.wing_x_offset.gStyle",PROPLIST_ITEM(id++,"X Leg Normal Offset Type"));
	CLDSLineAngle::AddPropItem("desStartPos.wing_x_offset.offsetDist",PROPLIST_ITEM(id++,"X Leg Normal Offset"));
	CLDSLineAngle::AddPropItem("desStartPos.wing_y_offset",PROPLIST_ITEM(id++,""));	//此时忽略WING_OFFSET_STYLE::operStyle属性
	CLDSLineAngle::AddPropItem("desStartPos.wing_y_offset.gStyle",PROPLIST_ITEM(id++,"Y Leg Normal Offset Type"));
	CLDSLineAngle::AddPropItem("desStartPos.wing_y_offset.offsetDist",PROPLIST_ITEM(id++,"Y Leg Normal Offset"));
	CLDSLineAngle::AddPropItem("endInfo",PROPLIST_ITEM(id++,"End Information"));
	CLDSLineAngle::AddPropItem("endPos",PROPLIST_ITEM(id++,"Ridge Line End Point Design Coordinate"));
	CLDSLineAngle::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"Ridge Line End Point Physical Coordinate"));
	CLDSLineAngle::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z"));

	CLDSLineAngle::AddPropItem("desEndPos.jgber_cal_style",PROPLIST_ITEM(id++,"Ridge Vertex Locate Mode"));	//0.直接指定楞点坐标；1.参数化定位
	CLDSLineAngle::AddPropItem("desEndPos.origin_pos_style",PROPLIST_ITEM(id++,"Start locating Point"));	//0.节点位置;1.基准点;2.基准角钢关键点;3.另一端节点坐标;4.另一端楞点坐标
	CLDSLineAngle::AddPropItem("desEndPos.datum_jg_h",PROPLIST_ITEM(id++,"Lap-joint Position Datum Rod"));	//搭接位置基准角钢
	//仅在origin_pos_style=2时有效
	CLDSLineAngle::AddPropItem("desEndPos.cDatumAngleWing",PROPLIST_ITEM(id++,"Key Point Affiliate Leg"));	//角钢'X'肢心线上关键点;'Y'肢心线上关键点;其余楞线上关键点
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
	CLDSLineAngle::AddPropItem("desEndPos.len_offset_type",PROPLIST_ITEM(id++,"Datum Plane Offset Type"));		//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量
	CLDSLineAngle::AddPropItem("desEndPos.len_offset_dist",PROPLIST_ITEM(id++,"Datum Plane Offset"));				//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量
	CLDSLineAngle::AddPropItem("desEndPos.datum_offset_dist",PROPLIST_ITEM(id++,"Datum Point Nurmal Offset"));	//搭接角钢投影方式时,搭接点自基准肢沿法线的偏量
	CLDSLineAngle::AddPropItem("desEndPos.cFaceOffsetDatumLine",PROPLIST_ITEM(id++,"Offset Plane Datum Line"));
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,"Offset Plane Normal "));
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm.x",PROPLIST_ITEM(id++,"X Component"));	//平推面法线X坐标
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm.y",PROPLIST_ITEM(id++,"Y Component"));	//平推面法线Y坐标
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm.z",PROPLIST_ITEM(id++,"Z Component"));	//平推面法线Z坐标
	CLDSLineAngle::AddPropItem("desEndPos.huoqu_jg_h",PROPLIST_ITEM(id++,"Bending Datum Angle"));		//火曲基准角钢句柄
	CLDSLineAngle::AddPropItem("desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desEndPos.huoqu_dist",PROPLIST_ITEM(id++,"Bending Line Clearance"));		//火曲线间隙
	CLDSLineAngle::AddPropItem("desEndPos.offset_zhun",PROPLIST_ITEM(id++,"Offset Stadia"));		//偏移准线的偏移量
	CLDSLineAngle::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,"Eccentric Distance"));	//偏心距==>当角钢搭接在钢管上时才需要此变量 wht 09-10-17
	//仅在spatialOperationStyle=15时有效
	CLDSLineAngle::AddPropItem("desEndPos.coordCtrlType",PROPLIST_ITEM(id++,"Master Control Leg Control Coordinate"));	//0:X-Z主控;1:Y-Z主控;X-Y主控
	CLDSLineAngle::AddPropItem("desEndPos.cMainCoordCtrlWing",PROPLIST_ITEM(id++,"Master Coordinate Control Leg"));	//角钢主控肢 'X'肢心线上关键点;'Y'肢心线上关键点;其余楞线上关键点
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing",PROPLIST_ITEM(id++,""));				//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing.operStyle",PROPLIST_ITEM(id++,"Master Control Point Locate Mode"));	//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing.gStyle",PROPLIST_ITEM(id++,"Master Control Line Stadia Style"));		//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing.offsetDist",PROPLIST_ITEM(id++,"Master Control Line Stadia"));	//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing",PROPLIST_ITEM(id++,""));			//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing.operStyle",PROPLIST_ITEM(id++,"Ancillary Point Locate Mode"));	//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing.gStyle",PROPLIST_ITEM(id++,"Ancillary Line Stadia Style"));		//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing.offsetDist",PROPLIST_ITEM(id++,"Ancillary Line Stadia"));	//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.fLenOffsetOfCoordWing",PROPLIST_ITEM(id++,"Offset Distance Of Coord_Wing"));
	//仅在spatialOperationStyle=16,17时有效
	CLDSLineAngle::AddPropItem("desEndPos.hDatumLine",PROPLIST_ITEM(id++,"Datum Line Definition"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos",PROPLIST_ITEM(id++,"Datum Line Start Point Coordinate"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos.x",PROPLIST_ITEM(id++,"X Coordinate："));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos.y",PROPLIST_ITEM(id++,"Y Coordinate："));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos.z",PROPLIST_ITEM(id++,"Z Coordinate："));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos",PROPLIST_ITEM(id++,"Datum Line End Point Coordinate"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos.x",PROPLIST_ITEM(id++,"X Coordinate："));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos.y",PROPLIST_ITEM(id++,"Y Coordinate："));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos.z",PROPLIST_ITEM(id++,"Z Coordinate："));
	CLDSLineAngle::AddPropItem("desEndPos.datumLineStart",PROPLIST_ITEM(id++,"Project Datum Line Start Point"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLineEnd",PROPLIST_ITEM(id++,"Project Datum Line End Point "));
	//第二次偏移操作(基点->楞点的矢量偏移)，仅适用于搭接杆件端(楞)点的位置计算
	CLDSLineAngle::AddPropItem("desEndPos.datum_to_ber_style",PROPLIST_ITEM(id++,"Datum Point->Ridge Vertex Locate Mode"));
	//BYTE wingXGStyle,wingYGStyle;		//0.垂向偏移;1.X-Y平面平推;2.Y-Z平面平推;3.X-Z平面平推;4.自定义平推面
	//double offset_x_dist,offset_y_dist;	//仅在gStyle=4时有效
	CLDSLineAngle::AddPropItem("desEndPos.cWorkWingStyle",PROPLIST_ITEM(id++,"Locate Connect Leg"));//自基点进行偏移时当前角钢的基准肢0:自动判断;1:X肢;2:Y肢;3:双肢;其余自动判断
	CLDSLineAngle::AddPropItem("desEndPos.wing_x_offset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desEndPos.wing_x_offset.gStyle",PROPLIST_ITEM(id++," Leg Normal Offset Type"));
	CLDSLineAngle::AddPropItem("desEndPos.wing_x_offset.offsetDist",PROPLIST_ITEM(id++," Leg Normal Offset"));
	CLDSLineAngle::AddPropItem("desEndPos.wing_y_offset",PROPLIST_ITEM(id++,""));	//此时忽略WING_OFFSET_STYLE::operStyle属性
	CLDSLineAngle::AddPropItem("desEndPos.wing_y_offset.gStyle",PROPLIST_ITEM(id++,"Y Leg Normal Offset Type"));
	CLDSLineAngle::AddPropItem("desEndPos.wing_y_offset.offsetDist",PROPLIST_ITEM(id++,"Y Leg Normal Offset"));

	CLDSLineAngle::AddPropItem("oddStart",PROPLIST_ITEM(id++,"Odd Length"));
	CLDSLineAngle::AddPropItem("oddStart.m_iOddCalStyle",PROPLIST_ITEM(id++,"Calculate Style"));	//0.表示根据碰撞参照杆件计算;1.根据螺栓位置计算;2.表示不计算正负头
	CLDSLineAngle::AddPropItem("oddStart.m_fCollideDist",PROPLIST_ITEM(id++,"Collision Distance"));
	CLDSLineAngle::AddPropItem("oddStart.m_hRefPart1",PROPLIST_ITEM(id++,"Refence Rod 1"));
	CLDSLineAngle::AddPropItem("oddStart.m_hRefPart2",PROPLIST_ITEM(id++,"Refence Rod 2"));
	CLDSLineAngle::AddPropItem("oddStartExtra",PROPLIST_ITEM(id++,"Fixed Odd Length"));
	CLDSLineAngle::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"Odd Length"));
	CLDSLineAngle::AddPropItem("oddEnd.m_iOddCalStyle",PROPLIST_ITEM(id++,"Calculate Style"));	//0.表示根据碰撞参照杆件计算;1.根据螺栓位置计算;2.表示不计算正负头
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
	//内力分析所需要的属性
	CLDSLineAngle::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"Statistic Material Serial Number"));	//统材编号
	CLDSLineAngle::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"Gravity Zoom Out Coefficient"));
	CLDSLineAngle::AddPropItem("size.idClassType",PROPLIST_ITEM(id++,""));
	//设计信息
	CLDSLineAngle::AddPropItem("designInfo",PROPLIST_ITEM(id++,"Rod Check Information"));
	CLDSLineAngle::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"Elem Type"));
	CLDSLineAngle::AddPropItem("sel_mat_method",PROPLIST_ITEM(id++,"Brace member check rule"));
	CLDSLineAngle::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"(Existing Explanatory Note)Whether Material Optimization"));
	CLDSLineAngle::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"Whether Specification Optimization"));
	CLDSLineAngle::AddPropItem("hole_out",PROPLIST_ITEM(id++,"Bolt Hole Out"));
	CLDSLineAngle::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"Calculate Automatic Bolt Hole Out"));
	//预拱项
	CLDSLineAngle::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"Lift Arc"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift",PROPLIST_ITEM(id++,"Start Prelift Coordinate"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift.z",PROPLIST_ITEM(id++,"Z"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift",PROPLIST_ITEM(id++,"End Prelift Coordinate"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift.z",PROPLIST_ITEM(id++,"Z"));
		//计算长度信息
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
		//始端设计信息
	CLDSLineAngle::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"Start Information"));
	CLDSLineAngle::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"Start Joint Type"));
	CLDSLineAngle::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"Start Force Type"));
	CLDSLineAngle::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
		//终端设计信息
	CLDSLineAngle::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"End Information"));
	CLDSLineAngle::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"End Joint Type"));
	CLDSLineAngle::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"End Force Type"));
	CLDSLineAngle::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,""));
#else 
	CLDSLineAngle::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"基本信息"));
	CLDSLineAngle::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSLineAngle::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSLineAngle::AddPropItem("layer1",PROPLIST_ITEM(id++,"第一字符"));
	CLDSLineAngle::AddPropItem("layer2",PROPLIST_ITEM(id++,"第二字符"));
	CLDSLineAngle::AddPropItem("layer3",PROPLIST_ITEM(id++,"第三字符"));
	CLDSLineAngle::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	CLDSLineAngle::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级",NULL,"默认|A|B|C|D|"));
	CLDSLineAngle::AddPropItem("specification",PROPLIST_ITEM(id++,"规格"));
	CLDSLineAngle::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSLineAngle::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	CLDSLineAngle::AddPropItem("group_father_jg_h",PROPLIST_ITEM(id++,"组合父角钢"));
	CLDSLineAngle::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSLineAngle::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"关联构件"));
	CLDSLineAngle::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"影射信息"));//影射信息
	CLDSLineAngle::AddPropItem("m_bSyncShadowPartNo",PROPLIST_ITEM(id++,"关联更新编号"));//影射信息
	CLDSLineAngle::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"影射母构件"));//影射母构件
	CLDSLineAngle::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"归属部件引用"));//归属的部件引用
	CLDSLineAngle::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"归属部件"));//归属的部件
	CLDSLineAngle::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	CLDSLineAngle::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件"));
	CLDSLineAngle::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	CLDSLineAngle::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助构件"));
	CLDSLineAngle::AddPropItem("m_bFootNail",PROPLIST_ITEM(id++,"脚钉角钢"));
	CLDSLineAngle::AddPropItem("length",PROPLIST_ITEM(id++,"实际长度"));
	CLDSLineAngle::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色"));
	CLDSLineAngle::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	CLDSLineAngle::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
	CLDSLineAngle::AddPropItem("hStart",PROPLIST_ITEM(id++,"起点句柄"));
	CLDSLineAngle::AddPropItem("hEnd",PROPLIST_ITEM(id++,"终点句柄"));
	CLDSLineAngle::AddPropItem("startInfo",PROPLIST_ITEM(id++,"始端信息"));
	CLDSLineAngle::AddPropItem("startPos",PROPLIST_ITEM(id++,"楞线起点设计坐标"));
	CLDSLineAngle::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"楞线起点实际坐标"));
	CLDSLineAngle::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z"));

	CLDSLineAngle::AddPropItem("desStartPos.jgber_cal_style",PROPLIST_ITEM(id++,"楞点定位方式"));	//0.直接指定楞点坐标；1.参数化定位
	//简化定位模式
	CLDSLineAngle::AddPropItem("desStartPos.SimpleDatumPoint",PROPLIST_ITEM(id++,"定位类型"));		//0.直接指定楞点坐标；1.参数化定位
	CLDSLineAngle::AddPropItem("desStartPos.SimpleDestLineMode",PROPLIST_ITEM(id++,"目标基准线"));	//0.直接指定楞点坐标；1.参数化定位
	CLDSLineAngle::AddPropItem("desStartPos.SimpleDenpendWing",PROPLIST_ITEM(id++,"依赖肢"));	//0.直接指定楞点坐标；1.参数化定位
	CLDSLineAngle::AddPropItem("desStartPos.SimpleDependRod",PROPLIST_ITEM(id++,"搭接依赖杆件"));	//0.直接指定楞点坐标；1.参数化定位
	CLDSLineAngle::AddPropItem("desStartPos.SimpleLenOffsetType",PROPLIST_ITEM(id++,"基面偏移类型"));		//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量
	CLDSLineAngle::AddPropItem("desStartPos.SimpleLenOffset",PROPLIST_ITEM(id++,"基面偏移量"));		//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量
	CLDSLineAngle::AddPropItem("desStartPos.SimpleNormOffset",PROPLIST_ITEM(id++,"法向偏移"));	//0.直接指定楞点坐标；1.参数化定位
	CLDSLineAngle::AddPropItem("desStartPos.SimpleBendLineAlongAngle",PROPLIST_ITEM(id++,"火曲线基准角钢"));
	CLDSLineAngle::AddPropItem("desStartPos.SimpleBendAngleWing",PROPLIST_ITEM(id++,"当前连接肢"));
	CLDSLineAngle::AddPropItem("desStartPos.SimpleBendClearance",PROPLIST_ITEM(id++,"火曲间隙"));
	CLDSLineAngle::AddPropItem("desStartPos.SimpleOffsetZhunju",PROPLIST_ITEM(id++,"偏移准距"));
	CLDSLineAngle::AddPropItem("desStartPos.SimpleSelfWorkWing",PROPLIST_ITEM(id++,"自身工作肢"));	//0.直接指定楞点坐标；1.参数化定位
	CLDSLineAngle::AddPropItem("desStartPos.SimpleWingXTipCtrlPoint",PROPLIST_ITEM(id++,"X(红)肢控制坐标"));
	CLDSLineAngle::AddPropItem("desStartPos.SimpleWingYTipCtrlPoint",PROPLIST_ITEM(id++,"Y(绿)肢控制坐标"));

	CLDSLineAngle::AddPropItem("desStartPos.origin_pos_style",PROPLIST_ITEM(id++,"初始定位点"));	//0.节点位置;1.基准点;2.基准角钢关键点;3.另一端节点坐标;4.另一端楞点坐标
	CLDSLineAngle::AddPropItem("desStartPos.datum_jg_h",PROPLIST_ITEM(id++,"搭接位置基准角钢"));	//搭接位置基准角钢
	//仅在origin_pos_style=2时有效
	CLDSLineAngle::AddPropItem("desStartPos.cDatumAngleWing",PROPLIST_ITEM(id++,"关键点从属肢"));	//角钢'X'肢心线上关键点;'Y'肢心线上关键点;其余楞线上关键点
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset.operStyle",PROPLIST_ITEM(id++,"关键点计算方式"));
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset.gStyle",PROPLIST_ITEM(id++,"心线准距类型"));
	CLDSLineAngle::AddPropItem("desStartPos.datumWingOffset.offsetDist",PROPLIST_ITEM(id++,"心线准距值"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint",PROPLIST_ITEM(id++,"基点位置"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSLineAngle::AddPropItem("desStartPos.datumPoint.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSLineAngle::AddPropItem("desStartPos.spatialOperationStyle",PROPLIST_ITEM(id++,"基点定位方式"));
	CLDSLineAngle::AddPropItem("desStartPos.LappedDatumWingG",PROPLIST_ITEM(id++,"搭接心距"));		
	CLDSLineAngle::AddPropItem("desStartPos.len_offset_type",PROPLIST_ITEM(id++,"基面偏移类型"));	//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量
	CLDSLineAngle::AddPropItem("desStartPos.len_offset_dist",PROPLIST_ITEM(id++,"基面偏移量"));		//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量
	CLDSLineAngle::AddPropItem("desStartPos.datum_offset_dist",PROPLIST_ITEM(id++,"基点法向偏移量"));	//搭接角钢投影方式时,搭接点自基准肢沿法线的偏量
	CLDSLineAngle::AddPropItem("desStartPos.cFaceOffsetDatumLine",PROPLIST_ITEM(id++,"平推面基准线"));
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"平推面法线"));
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm.x",PROPLIST_ITEM(id++,"平推面法线X坐标"));	//平推面法线X坐标
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm.y",PROPLIST_ITEM(id++,"平推面法线Y坐标"));	//平推面法线Y坐标
	CLDSLineAngle::AddPropItem("desStartPos.face_offset_norm.z",PROPLIST_ITEM(id++,"平推面法线Z坐标"));	//平推面法线Z坐标
	CLDSLineAngle::AddPropItem("desStartPos.huoqu_jg_h",PROPLIST_ITEM(id++,"火曲基准角钢"));		//火曲基准角钢句柄
	CLDSLineAngle::AddPropItem("desStartPos.huoqu_pos_auto_0_x_wing1_y_wing2",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desStartPos.huoqu_dist",PROPLIST_ITEM(id++,"火曲线间隙"));		//火曲线间隙
	CLDSLineAngle::AddPropItem("desStartPos.offset_zhun",PROPLIST_ITEM(id++,"偏移准距"));		//偏移准线的偏移量
	CLDSLineAngle::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"偏心距"));	//偏心距==>当角钢搭接在钢管上时才需要此变量 wht 09-10-17
	//仅在spatialOperationStyle=15时有效
	CLDSLineAngle::AddPropItem("desStartPos.coordCtrlType",PROPLIST_ITEM(id++,"主控肢控制坐标"));	//0:X-Z主控;1:Y-Z主控;X-Y主控
	CLDSLineAngle::AddPropItem("desStartPos.cMainCoordCtrlWing",PROPLIST_ITEM(id++,"端面控制点"));		//角钢主控肢 'X'肢心线上关键点;'Y'肢心线上关键点;其余楞线上关键点
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing",PROPLIST_ITEM(id++,""));				//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing.operStyle",PROPLIST_ITEM(id++,"主控点定位方式"));		//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing.gStyle",PROPLIST_ITEM(id++,"主控线准距类型"));		//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.ctrlWing.offsetDist",PROPLIST_ITEM(id++,"主控线准距"));	//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing",PROPLIST_ITEM(id++,""));				//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing.operStyle",PROPLIST_ITEM(id++,"辅控点定位方式"));	//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing.gStyle",PROPLIST_ITEM(id++,"辅控线准距类型"));		//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.otherWing.offsetDist",PROPLIST_ITEM(id++,"辅控线准距"));	//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desStartPos.fLenOffsetOfCoordWing",PROPLIST_ITEM(id++,"断面向外错位偏移"));
	//仅在spatialOperationStyle=16,17时有效
	CLDSLineAngle::AddPropItem("desStartPos.hDatumLine",PROPLIST_ITEM(id++,"基准线定义"));
	CLDSLineAngle::AddPropItem("desStartPos.cMirTransType",PROPLIST_ITEM(id++,"参照对称类型"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.startPos",PROPLIST_ITEM(id++,"基准线起点坐标"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.startPos.x",PROPLIST_ITEM(id++,"基准线起点X坐标"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.startPos.y",PROPLIST_ITEM(id++,"基准线起点Y坐标"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.startPos.z",PROPLIST_ITEM(id++,"基准线起点Z坐标"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.endPos",PROPLIST_ITEM(id++,"基准线终点坐标"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.endPos.x",PROPLIST_ITEM(id++,"基准线终点X坐标"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.endPos.y",PROPLIST_ITEM(id++,"基准线终点Y坐标"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLine.endPos.z",PROPLIST_ITEM(id++,"基准线终点Z坐标"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLineStart",PROPLIST_ITEM(id++,"投影基准线起点"));
	CLDSLineAngle::AddPropItem("desStartPos.datumLineEnd",PROPLIST_ITEM(id++,"投影基准线终点"));
	//第二次偏移操作(基点->楞点的矢量偏移)，仅适用于搭接杆件端(楞)点的位置计算
	//BYTE wingXGStyle,wingYGStyle;		//0.垂向偏移;1.X-Y平面平推;2.Y-Z平面平推;3.X-Z平面平推;4.自定义平推面
	//double offset_x_dist,offset_y_dist;	//仅在gStyle=4时有效
	CLDSLineAngle::AddPropItem("desStartPos.datum_to_ber_style",PROPLIST_ITEM(id++,"基点->楞点定位方式"));
	CLDSLineAngle::AddPropItem("desStartPos.cWorkWingStyle",PROPLIST_ITEM(id++,"定位连接肢"));//自基点进行偏移时当前角钢的基准肢0:自动判断;1:X肢;2:Y肢;3:双肢;其余自动判断
	CLDSLineAngle::AddPropItem("desStartPos.wing_x_offset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desStartPos.wing_x_offset.gStyle",PROPLIST_ITEM(id++,"X肢法向自偏移方式"));
	CLDSLineAngle::AddPropItem("desStartPos.wing_x_offset.offsetDist",PROPLIST_ITEM(id++,"X肢法向自偏移量"));
	CLDSLineAngle::AddPropItem("desStartPos.wing_y_offset",PROPLIST_ITEM(id++,""));	//此时忽略WING_OFFSET_STYLE::operStyle属性
	CLDSLineAngle::AddPropItem("desStartPos.wing_y_offset.gStyle",PROPLIST_ITEM(id++,"Y肢法向自偏移方式"));
	CLDSLineAngle::AddPropItem("desStartPos.wing_y_offset.offsetDist",PROPLIST_ITEM(id++,"Y肢法向自偏移量"));
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
	CLDSLineAngle::AddPropItem("endInfo",PROPLIST_ITEM(id++,"终端信息"));
	CLDSLineAngle::AddPropItem("endPos",PROPLIST_ITEM(id++,"楞线终点设计坐标"));
	CLDSLineAngle::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"楞线终点实际坐标"));
	CLDSLineAngle::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z"));

	CLDSLineAngle::AddPropItem("desEndPos.jgber_cal_style",PROPLIST_ITEM(id++,"楞点定位方式"));	//0.直接指定楞点坐标；1.参数化定位
	//简化定位模式
	CLDSLineAngle::AddPropItem("desEndPos.SimpleDatumPoint",PROPLIST_ITEM(id++,"定位类型"));	//0.直接指定楞点坐标；1.参数化定位
	CLDSLineAngle::AddPropItem("desEndPos.SimpleDestLineMode",PROPLIST_ITEM(id++,"目标基准线"));	//0.直接指定楞点坐标；1.参数化定位
	CLDSLineAngle::AddPropItem("desEndPos.SimpleDenpendWing",PROPLIST_ITEM(id++,"依赖肢"));	//0.直接指定楞点坐标；1.参数化定位
	CLDSLineAngle::AddPropItem("desEndPos.SimpleDependRod",PROPLIST_ITEM(id++,"搭接依赖杆件"));	//0.直接指定楞点坐标；1.参数化定位
	CLDSLineAngle::AddPropItem("desEndPos.SimpleLenOffsetType",PROPLIST_ITEM(id++,"基面偏移类型"));		//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量
	CLDSLineAngle::AddPropItem("desEndPos.SimpleLenOffset",PROPLIST_ITEM(id++,"基面偏移量"));		//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量
	CLDSLineAngle::AddPropItem("desEndPos.SimpleNormOffset",PROPLIST_ITEM(id++,"法向偏移"));	//0.直接指定楞点坐标；1.参数化定位
	CLDSLineAngle::AddPropItem("desEndPos.SimpleBendLineAlongAngle",PROPLIST_ITEM(id++,"火曲线基准角钢"));
	CLDSLineAngle::AddPropItem("desEndPos.SimpleBendAngleWing",PROPLIST_ITEM(id++,"当前连接肢"));
	CLDSLineAngle::AddPropItem("desEndPos.SimpleBendClearance",PROPLIST_ITEM(id++,"火曲间隙"));
	CLDSLineAngle::AddPropItem("desEndPos.SimpleOffsetZhunju",PROPLIST_ITEM(id++,"偏移准距"));
	CLDSLineAngle::AddPropItem("desEndPos.SimpleSelfWorkWing",PROPLIST_ITEM(id++,"自身工作肢"));	//0.直接指定楞点坐标；1.参数化定位
	CLDSLineAngle::AddPropItem("desEndPos.SimpleWingXTipCtrlPoint",PROPLIST_ITEM(id++,"X(红)肢控制坐标"));
	CLDSLineAngle::AddPropItem("desEndPos.SimpleWingYTipCtrlPoint",PROPLIST_ITEM(id++,"Y(绿)肢控制坐标"));

	CLDSLineAngle::AddPropItem("desEndPos.origin_pos_style",PROPLIST_ITEM(id++,"初始定位点"));	//0.节点位置;1.基准点;2.基准角钢关键点;3.另一端节点坐标;4.另一端楞点坐标
	CLDSLineAngle::AddPropItem("desEndPos.datum_jg_h",PROPLIST_ITEM(id++,"搭接位置基准杆件"));	//搭接位置基准角钢
	//仅在origin_pos_style=2时有效
	CLDSLineAngle::AddPropItem("desEndPos.cDatumAngleWing",PROPLIST_ITEM(id++,"关键点从属肢"));	//角钢'X'肢心线上关键点;'Y'肢心线上关键点;其余楞线上关键点
	CLDSLineAngle::AddPropItem("desEndPos.datumWingOffset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desEndPos.datumWingOffset.operStyle",PROPLIST_ITEM(id++,"关键点计算方式"));
	CLDSLineAngle::AddPropItem("desEndPos.datumWingOffset.gStyle",PROPLIST_ITEM(id++,"心线准距类型"));
	CLDSLineAngle::AddPropItem("desEndPos.datumWingOffset.offsetDist",PROPLIST_ITEM(id++,"心线准距值"));
	CLDSLineAngle::AddPropItem("desEndPos.datumPoint",PROPLIST_ITEM(id++,"基点位置"));
	CLDSLineAngle::AddPropItem("desEndPos.datumPoint.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSLineAngle::AddPropItem("desEndPos.datumPoint.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSLineAngle::AddPropItem("desEndPos.datumPoint.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSLineAngle::AddPropItem("desEndPos.spatialOperationStyle",PROPLIST_ITEM(id++,"基点定位方式"));
	CLDSLineAngle::AddPropItem("desStartPos.bendingLenOffsetComplement",PROPLIST_ITEM(id++,"制弯点纵向偏移量"));
	CLDSLineAngle::AddPropItem("desEndPos.bendingLenOffsetComplement",PROPLIST_ITEM(id++,"制弯点纵向偏移量"));
	CLDSLineAngle::AddPropItem("desEndPos.LappedDatumWingG",PROPLIST_ITEM(id++,"搭接心距"));		
	CLDSLineAngle::AddPropItem("desEndPos.len_offset_type",PROPLIST_ITEM(id++,"基面偏移类型"));		//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量
	CLDSLineAngle::AddPropItem("desEndPos.len_offset_dist",PROPLIST_ITEM(id++,"基面偏移量"));		//搭接角钢投影方式时,搭接点自基准肢沿基准角钢始至终延伸方向的偏量
	CLDSLineAngle::AddPropItem("desEndPos.datum_offset_dist",PROPLIST_ITEM(id++,"基点法向偏移量"));	//搭接角钢投影方式时,搭接点自基准肢沿法线的偏量
	CLDSLineAngle::AddPropItem("desEndPos.cFaceOffsetDatumLine",PROPLIST_ITEM(id++,"平推面基准线"));
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm",PROPLIST_ITEM(id++,"平推面法线"));
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm.x",PROPLIST_ITEM(id++,"X分量"));	//平推面法线X坐标
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm.y",PROPLIST_ITEM(id++,"Y分量"));	//平推面法线Y坐标
	CLDSLineAngle::AddPropItem("desEndPos.face_offset_norm.z",PROPLIST_ITEM(id++,"Z分量"));	//平推面法线Z坐标
	CLDSLineAngle::AddPropItem("desEndPos.huoqu_jg_h",PROPLIST_ITEM(id++,"火曲基准角钢"));		//火曲基准角钢句柄
	CLDSLineAngle::AddPropItem("desEndPos.huoqu_pos_auto_0_x_wing1_y_wing2",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desEndPos.huoqu_dist",PROPLIST_ITEM(id++,"火曲线间隙"));		//火曲线间隙
	CLDSLineAngle::AddPropItem("desEndPos.offset_zhun",PROPLIST_ITEM(id++,"偏移准距"));		//偏移准线的偏移量
	CLDSLineAngle::AddPropItem("desEndPos.fEccentricDist",PROPLIST_ITEM(id++,"偏心距"));	//偏心距==>当角钢搭接在钢管上时才需要此变量 wht 09-10-17
	//仅在spatialOperationStyle=15时有效
	CLDSLineAngle::AddPropItem("desEndPos.coordCtrlType",PROPLIST_ITEM(id++,"主控肢控制坐标"));	//0:X-Z主控;1:Y-Z主控;X-Y主控
	CLDSLineAngle::AddPropItem("desEndPos.cMainCoordCtrlWing",PROPLIST_ITEM(id++,"端面控制点"));	//角钢主控肢 'X'肢心线上关键点;'Y'肢心线上关键点;其余楞线上关键点
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing",PROPLIST_ITEM(id++,""));				//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing.operStyle",PROPLIST_ITEM(id++,"主控点定位方式"));	//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing.gStyle",PROPLIST_ITEM(id++,"主控线准距类型"));		//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.ctrlWing.offsetDist",PROPLIST_ITEM(id++,"主控线准距"));	//角钢主要控制肢(控制两个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing",PROPLIST_ITEM(id++,""));			//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing.operStyle",PROPLIST_ITEM(id++,"辅控点定位方式"));	//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing.gStyle",PROPLIST_ITEM(id++,"辅控线准距类型"));		//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.otherWing.offsetDist",PROPLIST_ITEM(id++,"辅控线准距"));	//角钢辅助控制肢(控制一个坐标分量的肢)
	CLDSLineAngle::AddPropItem("desEndPos.fLenOffsetOfCoordWing",PROPLIST_ITEM(id++,"断面向外错位偏移"));
	//仅在spatialOperationStyle=16,17时有效
	CLDSLineAngle::AddPropItem("desEndPos.hDatumLine",PROPLIST_ITEM(id++,"基准线定义"));
	CLDSLineAngle::AddPropItem("desEndPos.cMirTransType",PROPLIST_ITEM(id++,"参照对称类型"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos",PROPLIST_ITEM(id++,"基准线起点坐标"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos.x",PROPLIST_ITEM(id++,"X坐标："));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos.y",PROPLIST_ITEM(id++,"Y坐标："));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.startPos.z",PROPLIST_ITEM(id++,"Z坐标："));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos",PROPLIST_ITEM(id++,"基准线终点坐标"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos.x",PROPLIST_ITEM(id++,"X坐标："));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos.y",PROPLIST_ITEM(id++,"Y坐标："));
	CLDSLineAngle::AddPropItem("desEndPos.datumLine.endPos.z",PROPLIST_ITEM(id++,"Z坐标："));
	CLDSLineAngle::AddPropItem("desEndPos.datumLineStart",PROPLIST_ITEM(id++,"投影基准线起点"));
	CLDSLineAngle::AddPropItem("desEndPos.datumLineEnd",PROPLIST_ITEM(id++,"投影基准线终点"));
	//第二次偏移操作(基点->楞点的矢量偏移)，仅适用于搭接杆件端(楞)点的位置计算
	CLDSLineAngle::AddPropItem("desEndPos.datum_to_ber_style",PROPLIST_ITEM(id++,"基点->楞点定位方式"));
	//BYTE wingXGStyle,wingYGStyle;		//0.垂向偏移;1.X-Y平面平推;2.Y-Z平面平推;3.X-Z平面平推;4.自定义平推面
	//double offset_x_dist,offset_y_dist;	//仅在gStyle=4时有效
	CLDSLineAngle::AddPropItem("desEndPos.cWorkWingStyle",PROPLIST_ITEM(id++,"定位连接肢"));//自基点进行偏移时当前角钢的基准肢0:自动判断;1:X肢;2:Y肢;3:双肢;其余自动判断
	CLDSLineAngle::AddPropItem("desEndPos.wing_x_offset",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("desEndPos.wing_x_offset.gStyle",PROPLIST_ITEM(id++,"肢法向自偏移方式"));
	CLDSLineAngle::AddPropItem("desEndPos.wing_x_offset.offsetDist",PROPLIST_ITEM(id++,"肢法向自偏移量"));
	CLDSLineAngle::AddPropItem("desEndPos.wing_y_offset",PROPLIST_ITEM(id++,""));	//此时忽略WING_OFFSET_STYLE::operStyle属性
	CLDSLineAngle::AddPropItem("desEndPos.wing_y_offset.gStyle",PROPLIST_ITEM(id++,"Y肢法向自偏移方式"));
	CLDSLineAngle::AddPropItem("desEndPos.wing_y_offset.offsetDist",PROPLIST_ITEM(id++,"Y肢法向自偏移量"));
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

	CLDSLineAngle::AddPropItem("oddStart",PROPLIST_ITEM(id++,"正负头"));
	CLDSLineAngle::AddPropItem("oddStart.m_iOddCalStyle",PROPLIST_ITEM(id++,"计算原则"));	//0.表示根据碰撞参照杆件计算;1.根据螺栓位置计算;2.表示不计算正负头
	CLDSLineAngle::AddPropItem("oddStart.m_fCollideDist",PROPLIST_ITEM(id++,"碰撞间隙"));
	CLDSLineAngle::AddPropItem("oddStart.m_hRefPart1",PROPLIST_ITEM(id++,"参照杆件1"));
	CLDSLineAngle::AddPropItem("oddStart.m_hRefPart2",PROPLIST_ITEM(id++,"参照杆件2"));
	CLDSLineAngle::AddPropItem("oddStartExtra",PROPLIST_ITEM(id++,"固定正负头"));
	CLDSLineAngle::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"正负头"));
	CLDSLineAngle::AddPropItem("oddEnd.m_iOddCalStyle",PROPLIST_ITEM(id++,"计算原则"));	//0.表示根据碰撞参照杆件计算;1.根据螺栓位置计算;2.表示不计算正负头
	CLDSLineAngle::AddPropItem("oddEnd.m_fCollideDist",PROPLIST_ITEM(id++,"碰撞间隙"));
	CLDSLineAngle::AddPropItem("oddEnd.m_hRefPart1",PROPLIST_ITEM(id++,"参照杆件1"));
	CLDSLineAngle::AddPropItem("oddEnd.m_hRefPart2",PROPLIST_ITEM(id++,"参照杆件2"));
	CLDSLineAngle::AddPropItem("oddEndExtra",PROPLIST_ITEM(id++,"固定正负头"));
	CLDSLineAngle::AddPropItem("StartOddCanAcrossMiddleSect",PROPLIST_ITEM(id++,"负头可跨过中点","","0.不允许|1.允许"));
	CLDSLineAngle::AddPropItem("EndOddCanAcrossMiddleSect",PROPLIST_ITEM(id++,"负头可跨过中点","","0.不允许|1.允许"));
	CLDSLineAngle::AddPropItem("linkInfo",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("startBoltInfo",PROPLIST_ITEM(id++,"预布螺栓信息"));
	CLDSLineAngle::AddPropItem("connectStart.grade",PROPLIST_ITEM(id++,"级别"));
	CLDSLineAngle::AddPropItem("connectStart.d",PROPLIST_ITEM(id++,"螺栓规格"));
	CLDSLineAngle::AddPropItem("connectStart.m_iConnectWing",PROPLIST_ITEM(id++,"连接肢"));
	CLDSLineAngle::AddPropItem("connectStart.N",PROPLIST_ITEM(id++,"连接螺栓数"));
	CLDSLineAngle::AddPropItem("connectStart.summBoltN",PROPLIST_ITEM(id++,"计算螺栓数"));
	CLDSLineAngle::AddPropItem("connectStart.rows",PROPLIST_ITEM(id++,"螺栓排数"));
	CLDSLineAngle::AddPropItem("endBoltInfo",PROPLIST_ITEM(id++,"预布螺栓信息"));
	CLDSLineAngle::AddPropItem("connectEnd.grade",PROPLIST_ITEM(id++,"级别"));
	CLDSLineAngle::AddPropItem("connectEnd.d",PROPLIST_ITEM(id++,"螺栓规格"));
	CLDSLineAngle::AddPropItem("connectEnd.m_iConnectWing",PROPLIST_ITEM(id++,"连接肢"));
	CLDSLineAngle::AddPropItem("connectEnd.N",PROPLIST_ITEM(id++,"连接螺栓数"));
	CLDSLineAngle::AddPropItem("connectEnd.summBoltN",PROPLIST_ITEM(id++,"计算螺栓数"));
	CLDSLineAngle::AddPropItem("connectEnd.rows",PROPLIST_ITEM(id++,"螺栓排数"));
	CLDSLineAngle::AddPropItem("AttachBolts",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("startAttachBolts",PROPLIST_ITEM(id++,"已布螺栓"));
	CLDSLineAngle::AddPropItem("midAttachBolts",PROPLIST_ITEM(id++,"已布中间螺栓项"));
	CLDSLineAngle::AddPropItem("endAttachBolts",PROPLIST_ITEM(id++,"已布螺栓"));
	CLDSLineAngle::AddPropItem("wingNormInfo",PROPLIST_ITEM(id++,"两肢法线信息"));
	CLDSLineAngle::AddPropItem("norm_x_wing",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("norm_x_wing.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("norm_x_wing.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("norm_x_wing.z",PROPLIST_ITEM(id++,"Z"));
	CLDSLineAngle::AddPropItem("norm_x_wing.hViceJg",PROPLIST_ITEM(id++,"X肢交叉参照杆件"));
	CLDSLineAngle::AddPropItem("norm_y_wing",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("norm_y_wing.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("norm_y_wing.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("norm_y_wing.z",PROPLIST_ITEM(id++,"Z"));
	CLDSLineAngle::AddPropItem("norm_y_wing.hViceJg",PROPLIST_ITEM(id++,"Y肢交叉参照杆件"));
	CLDSLineAngle::AddPropItem("work_wing",PROPLIST_ITEM(id++,"工作肢法线方向"));
	CLDSLineAngle::AddPropItem("work_wing.hViceJg",PROPLIST_ITEM(id++,"交叉参照杆件"));
	CLDSLineAngle::AddPropItem("XorYMirLayStyle",PROPLIST_ITEM(id++,"X/Y轴里外铁对称方式"));
	CLDSLineAngle::AddPropItem("blScatterBoltLocaTolerance",PROPLIST_ITEM(id++,"孔位误差处理","","0.保证间距|1.保证孔位"));
	CLDSLineAngle::AddPropItem("decWingAngle",PROPLIST_ITEM(id++,"两肢夹角"));
	CLDSLineAngle::AddPropItem("start_wing_plane",PROPLIST_ITEM(id++,"角钢肢平面"));
	CLDSLineAngle::AddPropItem("end_wing_plane",PROPLIST_ITEM(id++,"角钢肢平面"));
	CLDSLineAngle::AddPropItem("kaihe_base_wing_x0_y1",PROPLIST_ITEM(id++,"开合肢"));
	CLDSLineAngle::AddPropItem("m_cHeJiaoReservedWidthType",PROPLIST_ITEM(id++,"合角保留宽度类型"));
	CLDSLineAngle::AddPropItem("m_fHeJiaoReservedWidth",PROPLIST_ITEM(id++,"合角保留宽度"));
	CLDSLineAngle::AddPropItem("SetupPosInfo",PROPLIST_ITEM(id++,"安装定位信息"));
	CLDSLineAngle::AddPropItem("wingXG",PROPLIST_ITEM(id++,"X(红)肢执行准距"));
	CLDSLineAngle::AddPropItem("wingYG",PROPLIST_ITEM(id++,"Y(绿)肢执行准距"));
	CLDSLineAngle::AddPropItem("m_bEnableTeG",PROPLIST_ITEM(id++,"启动特殊准距"));
	CLDSLineAngle::AddPropItem("teGTable",PROPLIST_ITEM(id++,"特殊准距表"));
	CLDSLineAngle::AddPropItem("m_cPosCtrlDatumLineType",PROPLIST_ITEM(id++,"搭接定位基准"));
	CLDSLineAngle::AddPropItem("m_cPosCtrlType",PROPLIST_ITEM(id++,"搭接点定位方式"));
	CLDSLineAngle::AddPropItem("startCutWing",PROPLIST_ITEM(id++,"始端切角切肢"));
	CLDSLineAngle::AddPropItem("startCutWingX",PROPLIST_ITEM(id++,"X肢"));
	CLDSLineAngle::AddPropItem("startCutWingY",PROPLIST_ITEM(id++,"Y肢"));
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
	CLDSLineAngle::AddPropItem("endCutWing",PROPLIST_ITEM(id++,"终端切角切肢"));
	CLDSLineAngle::AddPropItem("endCutWingX",PROPLIST_ITEM(id++,"X肢"));
	CLDSLineAngle::AddPropItem("endCutWingY",PROPLIST_ITEM(id++,"Y肢"));
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
	CLDSLineAngle::AddPropItem("start_push_flat_x1_y2",PROPLIST_ITEM(id++,"始端压扁"));
	CLDSLineAngle::AddPropItem("start_push_flat_length",PROPLIST_ITEM(id++,"压扁长度"));
	CLDSLineAngle::AddPropItem("end_push_flat_x1_y2",PROPLIST_ITEM(id++,"终端压扁"));
	CLDSLineAngle::AddPropItem("end_push_flat_length",PROPLIST_ITEM(id++,"压扁长度"));
	CLDSLineAngle::AddPropItem("mid_push_flat_x1_y2",PROPLIST_ITEM(id++,"中间压扁"));
	CLDSLineAngle::AddPropItem("mid_push_flat_length",PROPLIST_ITEM(id++,"压扁长度"));
	CLDSLineAngle::AddPropItem("mid_push_flat_start_offset",PROPLIST_ITEM(id++,"始端偏移量"));
	CLDSLineAngle::AddPropItem("mid_push_flat_end_offset",PROPLIST_ITEM(id++,"终端偏移量"));
	CLDSLineAngle::AddPropItem("mid_push_flat_node_handle",PROPLIST_ITEM(id++,"压扁位置节点"));
	CLDSLineAngle::AddPropItem("m_fCutAngleCoefR",PROPLIST_ITEM(id++,"内侧切角间隙"));
	CLDSLineAngle::AddPropItem("statMatInfo",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("manuProcess",PROPLIST_ITEM(id++,"工艺信息"));
	CLDSLineAngle::AddPropItem("m_bCutRoot",PROPLIST_ITEM(id++,"清根"));
	CLDSLineAngle::AddPropItem("m_bCutBer",PROPLIST_ITEM(id++,"铲背"));
	CLDSLineAngle::AddPropItem("m_bBendPart",PROPLIST_ITEM(id++,"制弯"));
	CLDSLineAngle::AddPropItem("m_bWeldPart",PROPLIST_ITEM(id++,"焊接"));
	CLDSLineAngle::AddPropItem("FilletArcS",PROPLIST_ITEM(id++,"始端倒圆弧"));
	CLDSLineAngle::AddPropItem("FilletROfSX",PROPLIST_ITEM(id++,"X肢半径R"));
	CLDSLineAngle::AddPropItem("FilletROfSY",PROPLIST_ITEM(id++,"Y肢半径R"));
	CLDSLineAngle::AddPropItem("FilletArcE",PROPLIST_ITEM(id++,"终端倒圆弧"));
	CLDSLineAngle::AddPropItem("FilletROfEX",PROPLIST_ITEM(id++,"X肢半径R"));
	CLDSLineAngle::AddPropItem("FilletROfEY",PROPLIST_ITEM(id++,"Y肢半径R"));
	CLDSLineAngle::AddPropItem("OpenSlotGroup",PROPLIST_ITEM(id++,"开槽"));
	CLDSLineAngle::AddPropItem("OpenSlot1",PROPLIST_ITEM(id++,"开槽1"));
	CLDSLineAngle::AddPropItem("OpenSlot2",PROPLIST_ITEM(id++,"开槽2"));
	CLDSLineAngle::AddPropItem("OpenSlot3",PROPLIST_ITEM(id++,"开槽3"));
	CLDSLineAngle::AddPropItem("OpenSlot4",PROPLIST_ITEM(id++,"开槽4"));
	//内力分析所需要的属性
	CLDSLineAngle::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,"统材编号"));	//统材编号
	CLDSLineAngle::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,"重力放大系数"));
	CLDSLineAngle::AddPropItem("size.idClassType",PROPLIST_ITEM(id++,""));
	//设计信息
	CLDSLineAngle::AddPropItem("designInfo",PROPLIST_ITEM(id++,"杆件验算信息"));
	CLDSLineAngle::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,"单元类型"));
	CLDSLineAngle::AddPropItem("sel_mat_method",PROPLIST_ITEM(id++,"辅材验算依据"));
	CLDSLineAngle::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,"(已注释)是否进行材质优选"));
	CLDSLineAngle::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,"是否进行规格优选"));
	CLDSLineAngle::AddPropItem("hole_out",PROPLIST_ITEM(id++,"螺栓减孔数"));
	CLDSLineAngle::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,"自动计算螺栓减孔数"));
	//预拱项
	CLDSLineAngle::AddPropItem("m_hArcLift",PROPLIST_ITEM(id++,"预拱项"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift",PROPLIST_ITEM(id++,"始端预拱前坐标"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("m_xStartBeforeLift.z",PROPLIST_ITEM(id++,"Z"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift",PROPLIST_ITEM(id++,"终端预拱前坐标"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift.x",PROPLIST_ITEM(id++,"X"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift.y",PROPLIST_ITEM(id++,"Y"));
	CLDSLineAngle::AddPropItem("m_xEndBeforeLift.z",PROPLIST_ITEM(id++,"Z"));
		//计算长度信息
	CLDSLineAngle::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,"折算系数"));
	CLDSLineAngle::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineAngle::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,"计算长度类型"));
	CLDSLineAngle::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,"参照横连杆"));
	CLDSLineAngle::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,"同时受压参照杆件"));
	CLDSLineAngle::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,"最小轴长度"));
	CLDSLineAngle::AddPropItem("CalLenCoef.minR.hStartNode",PROPLIST_ITEM(id++,"起始节点"));
	CLDSLineAngle::AddPropItem("CalLenCoef.minR.hEndNode",PROPLIST_ITEM(id++,"终止节点"));
	CLDSLineAngle::AddPropItem("CalLenCoef.minR.coef",PROPLIST_ITEM(id++,"折算系数"));
	CLDSLineAngle::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,"平行轴长度"));
	CLDSLineAngle::AddPropItem("CalLenCoef.paraR.hStartNode",PROPLIST_ITEM(id++,"起始节点"));
	CLDSLineAngle::AddPropItem("CalLenCoef.paraR.hEndNode",PROPLIST_ITEM(id++,"终止节点"));
	CLDSLineAngle::AddPropItem("CalLenCoef.paraR.coef",PROPLIST_ITEM(id++,"折算系数"));
		//始端设计信息
	CLDSLineAngle::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,"始端信息"));
	CLDSLineAngle::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,"始端约束类型"));
	CLDSLineAngle::AddPropItem("start_force_type",PROPLIST_ITEM(id++,"始端受力类型"));
	CLDSLineAngle::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
		//终端设计信息
	CLDSLineAngle::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,"终端信息"));
	CLDSLineAngle::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,"终端约束类型"));
	CLDSLineAngle::AddPropItem("end_force_type",PROPLIST_ITEM(id++,"终端受力类型"));
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
	CLDSGroupLineAngle::AddPropItem("group_style",PROPLIST_ITEM(id++,"组合形式"));
	CLDSGroupLineAngle::AddPropItem("jg_space",PROPLIST_ITEM(id++,"组合间隙"));
	CLDSGroupLineAngle::AddPropItem("m_bUpdateSonAngleOddment",PROPLIST_ITEM(id++,"更新子角钢正负头"));
	CLDSGroupLineAngle::AddPropItem("ciMasterIndex",PROPLIST_ITEM(id++,"同向子角钢"));
	CLDSGroupLineAngle::AddPropItem("ciMasterDirectIndex",PROPLIST_ITEM(id++,"肢朝向关系"));
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
	CLDSLineSlot::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//影射信息
	CLDSLineSlot::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Shadow Mother Object"));//影射母构件
	CLDSLineSlot::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//归属的部件引用
	CLDSLineSlot::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block"));//归属的部件
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
	//内力分析所需要的属性
	CLDSLineSlot::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,""));	//统材编号
	CLDSLineSlot::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,""));
	//设计信息
	CLDSLineSlot::AddPropItem("designInfo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("hole_out",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,""));
	//计算长度信息
	CLDSLineSlot::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,""));
	//始端设计信息
	CLDSLineSlot::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("start_force_type",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
	//终端设计信息
	CLDSLineSlot::AddPropItem("designEndInfo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("end_joint_type",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("end_force_type",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("connectEnd.bearloadcoef",PROPLIST_ITEM(id++,""));
#else
	CLDSLineSlot::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"槽钢基本信息"));
	CLDSLineSlot::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSLineSlot::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSLineSlot::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	CLDSLineSlot::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级",NULL,"默认|A|B|C|D|"));
	CLDSLineSlot::AddPropItem("specification",PROPLIST_ITEM(id++,"规格"));
	CLDSLineSlot::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSLineSlot::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	CLDSLineSlot::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSLineSlot::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"关联构件"));
	CLDSLineSlot::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"影射信息"));//影射信息
	CLDSLineSlot::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"影射母构件"));//影射母构件
	CLDSLineSlot::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"归属的部件引用"));//归属的部件引用
	CLDSLineSlot::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"归属的部件"));//归属的部件
	CLDSLineSlot::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	CLDSLineSlot::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件"));
	CLDSLineSlot::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	CLDSLineSlot::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助构件"));
	CLDSLineSlot::AddPropItem("width",PROPLIST_ITEM(id++,"宽度"));
	CLDSLineSlot::AddPropItem("thick",PROPLIST_ITEM(id++,"厚度"));
	CLDSLineSlot::AddPropItem("height",PROPLIST_ITEM(id++,"高度"));
	CLDSLineSlot::AddPropItem("length",PROPLIST_ITEM(id++,"实际长度"));
	CLDSLineSlot::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色"));
	CLDSLineSlot::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	CLDSLineSlot::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
	CLDSLineSlot::AddPropItem("specialSlot",PROPLIST_ITEM(id++,"特殊槽钢定位参数"));
	CLDSLineSlot::AddPropItem("hAttachTube",PROPLIST_ITEM(id++,"依附钢管"));
	CLDSLineSlot::AddPropItem("fLen",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desDatumPoint",PROPLIST_ITEM(id++,"定位基准点"));
	CLDSLineSlot::AddPropItem("desDatumPoint.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSLineSlot::AddPropItem("desDatumPoint.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSLineSlot::AddPropItem("desDatumPoint.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.hOffsetDatumNode",PROPLIST_ITEM(id++,"偏移起始节点"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.lenOffsetType",PROPLIST_ITEM(id++,"轴向偏移方向"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.lenOffset",PROPLIST_ITEM(id++,"轴向偏移量"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVecStyle",PROPLIST_ITEM(id++,"射线方向"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVecStdStyle",PROPLIST_ITEM(id++,"指定方向"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVec.x",PROPLIST_ITEM(id++,"射线方向X分量"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVec.y",PROPLIST_ITEM(id++,"射线方向Y分量"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVec.z",PROPLIST_ITEM(id++,"射线方向Z分量"));
	CLDSLineSlot::AddPropItem("desDatumPoint.param.RayVecRotateAngle",PROPLIST_ITEM(id++,"旋转角度"));
	CLDSLineSlot::AddPropItem("workPlaneNorm",PROPLIST_ITEM(id++,"基准面法线"));
	CLDSLineSlot::AddPropItem("hWorkNormRefPart",PROPLIST_ITEM(id++,"法线参照杆件"));
	CLDSLineSlot::AddPropItem("workPlaneNorm.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSLineSlot::AddPropItem("workPlaneNorm.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSLineSlot::AddPropItem("workPlaneNorm.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSLineSlot::AddPropItem("nearWorkPlaneNorm",PROPLIST_ITEM(id++,"基准面近似法线"));
	CLDSLineSlot::AddPropItem("m_iNormCalStyle",PROPLIST_ITEM(id++,"计算方式"));
	CLDSLineSlot::AddPropItem("nearWorkPlaneNorm.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSLineSlot::AddPropItem("nearWorkPlaneNorm.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSLineSlot::AddPropItem("nearWorkPlaneNorm.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSLineSlot::AddPropItem("startInfo",PROPLIST_ITEM(id++,"始端信息"));
	CLDSLineSlot::AddPropItem("hStart",PROPLIST_ITEM(id++,"起点句柄"));
	CLDSLineSlot::AddPropItem("oddStart",PROPLIST_ITEM(id++,"正负头"));
	CLDSLineSlot::AddPropItem("startPos",PROPLIST_ITEM(id++,"始端设计坐标"));
	CLDSLineSlot::AddPropItem("actualStartPos",PROPLIST_ITEM(id++,"始端实际坐标"));
	CLDSLineSlot::AddPropItem("startPos.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSLineSlot::AddPropItem("startPos.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSLineSlot::AddPropItem("startPos.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSLineSlot::AddPropItem("desStartPos",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desStartPos.endPosType",PROPLIST_ITEM(id++,"定位方式"));
	CLDSLineSlot::AddPropItem("desStartPos.datum_pos_style",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desStartPos.hDatumPart",PROPLIST_ITEM(id++,"搭接位置基准角钢"));
	CLDSLineSlot::AddPropItem("desStartPos.fNormOffset",PROPLIST_ITEM(id++,"基准面法线偏移量"));
	CLDSLineSlot::AddPropItem("desStartPos.fEccentricDist",PROPLIST_ITEM(id++,"偏心距"));
	CLDSLineSlot::AddPropItem("desStartPos.face_offset_norm",PROPLIST_ITEM(id++,"平推面法线"));
	CLDSLineSlot::AddPropItem("desStartPos.fTransverseOffset",PROPLIST_ITEM(id++,"横向偏移量"));
	CLDSLineSlot::AddPropItem("desStartPos.hDatumStartPart",PROPLIST_ITEM(id++,"投影线始端所在基准杆件"));
	CLDSLineSlot::AddPropItem("desStartPos.bDatumStartStartEnd",PROPLIST_ITEM(id++,"基准扁钢的基准端点"));
	CLDSLineSlot::AddPropItem("desStartPos.hDatumEndPart",PROPLIST_ITEM(id++,"投影线终端所在基准杆件"));
	CLDSLineSlot::AddPropItem("desStartPos.bDatumEndStartEnd", PROPLIST_ITEM(id++, ""));
	CLDSLineSlot::AddPropItem("endInfo",PROPLIST_ITEM(id++,"终端信息"));
	CLDSLineSlot::AddPropItem("hEnd",PROPLIST_ITEM(id++,"终点句柄"));
	CLDSLineSlot::AddPropItem("oddEnd",PROPLIST_ITEM(id++,"正负头"));
	CLDSLineSlot::AddPropItem("endPos",PROPLIST_ITEM(id++,"终端设计坐标"));
	CLDSLineSlot::AddPropItem("actualEndPos",PROPLIST_ITEM(id++,"终端实际坐标"));
	CLDSLineSlot::AddPropItem("endPos.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSLineSlot::AddPropItem("endPos.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSLineSlot::AddPropItem("endPos.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSLineSlot::AddPropItem("desEndPos",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("desEndPos.endPosType",PROPLIST_ITEM(id++,"定位方式"));
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
	//内力分析所需要的属性
	CLDSLineSlot::AddPropItem("m_uStatMatNo",PROPLIST_ITEM(id++,""));	//统材编号
	CLDSLineSlot::AddPropItem("m_fWeightRaiseCoef",PROPLIST_ITEM(id++,""));
	//设计信息
	CLDSLineSlot::AddPropItem("designInfo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_iElemType",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_bNeedSelMat",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_bNeedSelSize",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("hole_out",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("m_bAutoCalHoleOutCoef",PROPLIST_ITEM(id++,""));
	//计算长度信息
	CLDSLineSlot::AddPropItem("CalLenCoef",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.iSelRFlag",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.iTypeNo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.pHorizPole",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.pRefPole",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.spec_callenMinR",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("CalLenCoef.spec_callenParaR",PROPLIST_ITEM(id++,""));
	//始端设计信息
	CLDSLineSlot::AddPropItem("designStartInfo",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("start_joint_type",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("start_force_type",PROPLIST_ITEM(id++,""));
	CLDSLineSlot::AddPropItem("connectStart.bearloadcoef",PROPLIST_ITEM(id++,""));
	//终端设计信息
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
	propMirSync.AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	//propMirSync.AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("specification",PROPLIST_ITEM(id++,"规格"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	propMirSync.AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("Profile",PROPLIST_ITEM(id++,"外形"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("thick",PROPLIST_ITEM(id++,"厚度"));
#endif
}

long CLDSParamPlate::m_idMinPropUdf;						//可使用的最小用户自定义属性Id
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
	CLDSParamPlate::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//影射信息
	CLDSParamPlate::AddPropItem("m_bSyncShadowPartNo",PROPLIST_ITEM(id++,"Synchornize part label"));//影射信息
	CLDSParamPlate::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Project Mother Part"));//影射母构件
	CLDSParamPlate::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//归属的部件引用
	CLDSParamPlate::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//归属的部件
	CLDSParamPlate::AddPropItem("plateDesignParam",PROPLIST_ITEM(id++,"Design Parameter"));
	CLDSParamPlate::AddPropItem("plateAttachBolts",PROPLIST_ITEM(id++,"Connect Bolt"));
	CLDSParamPlate::AddPropItem("plateAttachElbowPlate",PROPLIST_ITEM(id++,"Connect Elbow Plate"));
#else 
	CLDSParamPlate::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"参数化钢板基本信息"));
	CLDSParamPlate::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSParamPlate::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSParamPlate::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	CLDSParamPlate::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级",NULL,"默认|A|B|C|D|"));
	CLDSParamPlate::AddPropItem("thick",PROPLIST_ITEM(id++,"厚度"));
	CLDSParamPlate::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSParamPlate::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	CLDSParamPlate::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSParamPlate::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	CLDSParamPlate::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件"));
	CLDSParamPlate::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	CLDSParamPlate::AddPropItem("m_cPickedDisplayMode",PROPLIST_ITEM(id++,"显示轮廓边"));
	CLDSParamPlate::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助钢板"));
	CLDSParamPlate::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
	CLDSParamPlate::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"装配方位"));
	CLDSParamPlate::AddPropItem("m_fNormOffset",PROPLIST_ITEM(id++,"基点法向偏移量"));
	CLDSParamPlate::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色"));
	CLDSParamPlate::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	CLDSParamPlate::AddPropItem("ucs",PROPLIST_ITEM(id++,""));
	CLDSParamPlate::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"位置"));
	CLDSParamPlate::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSParamPlate::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSParamPlate::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSParamPlate::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X轴方向"));
	CLDSParamPlate::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSParamPlate::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSParamPlate::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSParamPlate::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y轴方向"));
	CLDSParamPlate::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSParamPlate::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSParamPlate::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSParamPlate::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z轴方向"));
	CLDSParamPlate::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSParamPlate::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSParamPlate::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSParamPlate::AddPropItem("plate_work_plane",PROPLIST_ITEM(id++,"钢板工作面"));
	CLDSParamPlate::AddPropItem("plateProfile",PROPLIST_ITEM(id++,"钢板外形"));
	CLDSParamPlate::AddPropItem("designInfo.hNode",PROPLIST_ITEM(id++,"依赖节点句柄"));
	CLDSParamPlate::AddPropItem("designInfo.hRod",PROPLIST_ITEM(id++,"依赖杆件句柄"));
	CLDSParamPlate::AddPropItem("m_iParamType",PROPLIST_ITEM(id++,"钢板类型"));
	CLDSParamPlate::AddPropItem("m_cRollProcessType",PROPLIST_ITEM(id++,"槽型插板卷边处理方式"));
	CLDSParamPlate::AddPropItem("m_cRollOffsetType",PROPLIST_ITEM(id++,"卷边偏移方式","","0.外侧|1.内侧"));
	CLDSParamPlate::AddPropItem("m_cDeformType",PROPLIST_ITEM(id++,"火曲变形处理方式","","0.装配面|1.中性层|2.凸面|3.凹面|4.默认值"));
	CLDSParamPlate::AddPropItem("m_fDeformCoef",PROPLIST_ITEM(id++,"中性层系数",""));
	CLDSParamPlate::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"关联构件"));
	CLDSParamPlate::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"影射信息"));//影射信息
	CLDSParamPlate::AddPropItem("m_bSyncShadowPartNo",PROPLIST_ITEM(id++,"关联更新编号"));//影射信息
	CLDSParamPlate::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"影射母构件"));//影射母构件
	CLDSParamPlate::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"归属部件引用"));//归属的部件引用
	CLDSParamPlate::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"归属部件"));//归属的部件
	CLDSParamPlate::AddPropItem("plateDesignParam",PROPLIST_ITEM(id++,"设计参数"));
	CLDSParamPlate::AddPropItem("plateAttachBolts",PROPLIST_ITEM(id++,"关联螺栓"));
	CLDSParamPlate::AddPropItem("plateAttachElbowPlate",PROPLIST_ITEM(id++,"关联肘板"));
	CLDSParamPlate::AddPropItem("PlateEdit",PROPLIST_ITEM(id++,"编辑操作"));
	CLDSParamPlate::AddPropItem("WorkUcs",PROPLIST_ITEM(id++,"工作坐标系"));
	CLDSParamPlate::AddPropItem("wcs.origin",PROPLIST_ITEM(id++,"原点位置"));
	CLDSParamPlate::AddPropItem("wcs.origin.x",PROPLIST_ITEM(id++,"X坐标","工作坐标系X坐标分量"));
	CLDSParamPlate::AddPropItem("wcs.origin.y",PROPLIST_ITEM(id++,"Y坐标","工作坐标系Y坐标分量"));
	CLDSParamPlate::AddPropItem("wcs.axis_x",PROPLIST_ITEM(id++,"X轴方向"));
	CLDSParamPlate::AddPropItem("wcs.axis_x.a",PROPLIST_ITEM(id++,"极坐标夹角α"));
	CLDSParamPlate::AddPropItem("wcs.axis_y",PROPLIST_ITEM(id++,"Y轴方向"));
	CLDSParamPlate::AddPropItem("InsertProfileVertex",PROPLIST_ITEM(id++,"插入轮廓点","",""));
	CLDSParamPlate::AddPropItem("AddNewHole",PROPLIST_ITEM(id++,"添加孔","",""));
	CLDSParamPlate::AddPropItem("FilletVertex",PROPLIST_ITEM(id++,"倒角","",""));
	CLDSParamPlate::AddPropItem("DeleteFeature",PROPLIST_ITEM(id++,"删除"));
	CLDSParamPlate::AddPropItem("FilletVertex",PROPLIST_ITEM(id++,"倒角"));
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
	CLDSPlate::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//影射信息
	CLDSPlate::AddPropItem("m_bSyncShadowPartNo",PROPLIST_ITEM(id++,"Synchornize part label"));//影射信息
	CLDSPlate::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Project Mother Part"));//影射母构件
	CLDSPlate::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//归属的部件引用
	CLDSPlate::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//归属的部件
	//两节点间比例等分点
	CLDSPlate::AddPropItem("designinfo.des_para",PROPLIST_ITEM(id++,"Plate Locate Parameter"));
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.hLinePart",PROPLIST_ITEM(id++,"Datum Rod Handle"));			//基准杆件句柄
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.hStartNode",PROPLIST_ITEM(id++,"Start Datum Node Handle"));			//始端节点句柄
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.hEndNode",PROPLIST_ITEM(id++,"End Datum Node Handle"));			//终端节点句柄
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.start_offset_dist",PROPLIST_ITEM(id++,"Start Offset"));	//始端偏移量
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.end_offset_dist",PROPLIST_ITEM(id++,"End Offset"));	//终端偏移量
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.offset_scale",PROPLIST_ITEM(id++,"Offset Proportion"));		//偏移比例
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.offset_dist",PROPLIST_ITEM(id++,"Offset Distance"));		//在等分基础上的偏移值

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
	CLDSPlate::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"钢板基本信息"));
	CLDSPlate::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSPlate::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSPlate::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	CLDSPlate::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级",NULL,"默认|A|B|C|D|"));
	CLDSPlate::AddPropItem("thick",PROPLIST_ITEM(id++,"厚度"));
	CLDSPlate::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSPlate::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	CLDSPlate::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSPlate::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	CLDSPlate::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件"));
	CLDSPlate::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	CLDSPlate::AddPropItem("m_cPickedDisplayMode",PROPLIST_ITEM(id++,"显示轮廓边"));
	CLDSPlate::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟辅助钢板"));
	CLDSPlate::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色"));
	CLDSPlate::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	CLDSPlate::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
	CLDSPlate::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"装配方位"));
	CLDSPlate::AddPropItem("m_fNormOffset",PROPLIST_ITEM(id++,"基点法向位置"));
	CLDSPlate::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"关联构件"));
	CLDSPlate::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"影射信息"));//影射信息
	CLDSPlate::AddPropItem("m_bSyncShadowPartNo",PROPLIST_ITEM(id++,"关联更新编号"));//影射信息
	CLDSPlate::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"影射母构件"));//影射母构件
	CLDSPlate::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"归属部件引用"));//归属的部件引用
	CLDSPlate::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"归属部件"));//归属的部件
	//两节点间比例等分点
	CLDSPlate::AddPropItem("designinfo.des_para",PROPLIST_ITEM(id++,"钢板定位参数"));
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.hLinePart",PROPLIST_ITEM(id++,"基准杆件句柄"));			//基准杆件句柄
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.hStartNode",PROPLIST_ITEM(id++,"始端基准节点句柄"));			//始端节点句柄
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.hEndNode",PROPLIST_ITEM(id++,"终端基准节点句柄"));			//终端节点句柄
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.start_offset_dist",PROPLIST_ITEM(id++,"始端偏移量"));	//始端偏移量
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.end_offset_dist",PROPLIST_ITEM(id++,"终端偏移量"));	//终端偏移量
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.offset_scale",PROPLIST_ITEM(id++,"偏移比例"));		//偏移比例
	CLDSPlate::AddPropItem("des_para.SCALE_NODE.offset_dist",PROPLIST_ITEM(id++,"偏移距离"));		//在等分基础上的偏移值

	CLDSPlate::AddPropItem("ucs",PROPLIST_ITEM(id++,""));
	CLDSPlate::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"位置"));
	CLDSPlate::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSPlate::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSPlate::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSPlate::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X轴方向"));
	CLDSPlate::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSPlate::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSPlate::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSPlate::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y轴方向"));
	CLDSPlate::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSPlate::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSPlate::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSPlate::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z轴方向"));
	CLDSPlate::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSPlate::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSPlate::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSPlate::AddPropItem("plate_work_plane",PROPLIST_ITEM(id++,"钢板工作面"));
	CLDSPlate::AddPropItem("plateProfile",PROPLIST_ITEM(id++,"钢板外形"));
	CLDSPlate::AddPropItem("designInfo.hNode",PROPLIST_ITEM(id++,"依赖节点句柄"));
	CLDSPlate::AddPropItem("designInfo.hRod",PROPLIST_ITEM(id++,"依赖杆件句柄"));
	CLDSPlate::AddPropItem("m_fInnerRadius",PROPLIST_ITEM(id++,"基点内环半径"));
	CLDSPlate::AddPropItem("m_tInnerColumnNorm",PROPLIST_ITEM(id++,"内环轴线方向"));
	CLDSPlate::AddPropItem("m_tInnerColumnNorm.x",PROPLIST_ITEM(id++,"内环轴向X分量"));
	CLDSPlate::AddPropItem("m_tInnerColumnNorm.y",PROPLIST_ITEM(id++,"内环轴向Y分量"));
	CLDSPlate::AddPropItem("m_tInnerColumnNorm.z",PROPLIST_ITEM(id++,"内环轴向Z分量"));
	CLDSPlate::AddPropItem("m_tInnerOrigin",PROPLIST_ITEM(id++,"内环位置"));
	CLDSPlate::AddPropItem("m_tInnerOrigin.x",PROPLIST_ITEM(id++,"内环位置X分量"));
	CLDSPlate::AddPropItem("m_tInnerOrigin.y",PROPLIST_ITEM(id++,"内环位置Y分量"));
	CLDSPlate::AddPropItem("m_tInnerOrigin.z",PROPLIST_ITEM(id++,"内环位置Z分量"));
	CLDSPlate::AddPropItem("iProfileStyle0123",PROPLIST_ITEM(id++,"外形生成方式"));
	helpStr.Copy("最佳受力:展开后倒角处为两条直线, 最少刀数1:保证卷边外侧边长度为L-N,最少刀数2:保证卷边内侧边长度为L-N。钢板只有 最少刀数1，最少刀数1=最少刀数2");
	CLDSPlate::AddPropItem("m_cRollProcessType",PROPLIST_ITEM(id++,"卷边处理方式",helpStr,"0.最佳受力|1.最少刀数1|2.最少刀数2"));
	CLDSPlate::AddPropItem("m_cRollOffsetType",PROPLIST_ITEM(id++,"卷边偏移方式","","0.外侧|1.内侧"));
	CLDSPlate::AddPropItem("m_cDeformType",PROPLIST_ITEM(id++,"火曲变形处理方式","","0.装配面|1.中性层|2.凸面|3.凹面|4.默认值"));
	CLDSPlate::AddPropItem("m_fDeformCoef",PROPLIST_ITEM(id++,"中性层系数",""));
	CLDSPlate::AddPropItem("m_fDeformCoef2",PROPLIST_ITEM(id++,"中性层系数2",""));
	CLDSPlate::AddPropItem("m_siProfileAlgor",PROPLIST_ITEM(id++,"外形生成算法"));
	CLDSPlate::AddPropItem("face_N",PROPLIST_ITEM(id++,"钢板类型"));
	CLDSPlate::AddPropItem("basicFace",PROPLIST_ITEM(id++,"(已注释)基准面定义"));
	CLDSPlate::AddPropItem("m_bSelfCorrectBendline",PROPLIST_ITEM(id++,"火曲线计算方式"));
	CLDSPlate::AddPropItem("huoquFace1",PROPLIST_ITEM(id++,"第一火曲面定义"));
	CLDSPlate::AddPropItem("huoquline_start[0]",PROPLIST_ITEM(id++,"火曲线起始点"));
	CLDSPlate::AddPropItem("huoquline_end[0]",PROPLIST_ITEM(id++,"火曲线终止点"));
	CLDSPlate::AddPropItem("huoqufacenorm[0]",PROPLIST_ITEM(id++,"火曲面法线定义"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].norm_style",PROPLIST_ITEM(id++,"肢法线计算方式"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].vector",PROPLIST_ITEM(id++,"火曲法线"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].vector.x",PROPLIST_ITEM(id++,"火曲法线X轴方向坐标"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].vector.y",PROPLIST_ITEM(id++,"火曲法线Y轴方向坐标"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].vector.z",PROPLIST_ITEM(id++,"火曲法线Z轴方向坐标"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].hVicePart",PROPLIST_ITEM(id++,"基准法线构件"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].norm_wing",PROPLIST_ITEM(id++,"基准肢"));	
	CLDSPlate::AddPropItem("huoqufacenorm[0].direction",PROPLIST_ITEM(id++,"法线朝向"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].hCrossPart",PROPLIST_ITEM(id++,"交叉参考杆件"));
	CLDSPlate::AddPropItem("huoqufacenorm[0].angle",PROPLIST_ITEM(id++,"火曲角(°)"));
	CLDSPlate::AddPropItem("huoquFace2",PROPLIST_ITEM(id++,"第二火曲面定义"));
	CLDSPlate::AddPropItem("huoquline_start[1]",PROPLIST_ITEM(id++,"火曲线起始点"));
	CLDSPlate::AddPropItem("huoquline_end[1]",PROPLIST_ITEM(id++,"火曲线终止点"));
	CLDSPlate::AddPropItem("huoqufacenorm[1]",PROPLIST_ITEM(id++,"火曲面法线定义"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].norm_style",PROPLIST_ITEM(id++,"肢法线计算方式"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].vector",PROPLIST_ITEM(id++,"火曲法线"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].vector.x",PROPLIST_ITEM(id++,"火曲法线X轴方向坐标"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].vector.y",PROPLIST_ITEM(id++,"火曲法线Y轴方向坐标"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].vector.z",PROPLIST_ITEM(id++,"火曲法线Z轴方向坐标"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].hVicePart",PROPLIST_ITEM(id++,"基准法线构件"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].norm_wing",PROPLIST_ITEM(id++,"基准肢"));	
	CLDSPlate::AddPropItem("huoqufacenorm[1].direction",PROPLIST_ITEM(id++,"法线朝向"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].hCrossPart",PROPLIST_ITEM(id++,"交叉参考杆件"));
	CLDSPlate::AddPropItem("huoqufacenorm[1].angle",PROPLIST_ITEM(id++,"火曲角（°)"));
	CLDSPlate::AddPropItem("peakVertexOffsetY",PROPLIST_ITEM(id++,"火曲点偏移量"));
	CLDSPlate::AddPropItem("extendVertexOffsetLen",PROPLIST_ITEM(id++,"割口长度"));
	CLDSPlate::AddPropItem("peakPointMargin",PROPLIST_ITEM(id++,"割口间隙值"));
	CLDSPlate::AddPropItem("plateParam",PROPLIST_ITEM(id++,"钢板参数"));
	CLDSPlate::AddPropItem("plateLinkPoles",PROPLIST_ITEM(id++,"连接杆件"));
	CLDSPlate::AddPropItem("plateAttachBolts",PROPLIST_ITEM(id++,"关联螺栓"));
	CLDSPlate::AddPropItem("fixedVertex",PROPLIST_ITEM(id++,"固定顶点"));
	CLDSPlate::AddPropItem("manuProcess",PROPLIST_ITEM(id++,"加工工艺"));
	CLDSPlate::AddPropItem("m_bWeldPart",PROPLIST_ITEM(id++,"焊接工艺","加工过程中需要焊接","是|否"));
	CLDSPlate::AddPropItem("iWeldLineStyle",PROPLIST_ITEM(id++,"焊缝线类型","焊缝线尺寸计算类型","0.自由计算|1.指定焊缝|2.延伸对齐"));
	CLDSPlate::AddPropItem("m_bNeedFillet",PROPLIST_ITEM(id++,"坡口工艺","焊接过程中对于较厚板一般需要提前进行坡口","是|否"));
	CLDSPlate::AddPropItem("designInfo.m_bEnableFlexibleDesign",PROPLIST_ITEM(id++,""));
	CLDSPlate::AddPropItem("designInfo.weld_start_high",PROPLIST_ITEM(id++,"焊缝起始高"));
	CLDSPlate::AddPropItem("designInfo.weld_end_high",PROPLIST_ITEM(id++,"焊缝终止高"));
	CLDSPlate::AddPropItem("designInfo.weld_width",PROPLIST_ITEM(id++,"焊区宽度"));
	CLDSPlate::AddPropItem("m_bAttachPartByWeldRoad",PROPLIST_ITEM(id++,""));
	CLDSPlate::AddPropItem("VertexList",PROPLIST_ITEM(id++,"轮廓点"));
	CLDSPlate::AddPropItem("PlateEdit",PROPLIST_ITEM(id++,"编辑操作"));
	CLDSPlate::AddPropItem("WorkUcs",PROPLIST_ITEM(id++,"工作坐标系"));
	CLDSPlate::AddPropItem("wcs.origin",PROPLIST_ITEM(id++,"原点位置"));
	CLDSPlate::AddPropItem("wcs.origin.x",PROPLIST_ITEM(id++,"X坐标","工作坐标系X坐标分量"));
	CLDSPlate::AddPropItem("wcs.origin.y",PROPLIST_ITEM(id++,"Y坐标","工作坐标系Y坐标分量"));
	CLDSPlate::AddPropItem("wcs.axis_x",PROPLIST_ITEM(id++,"X轴方向"));
	CLDSPlate::AddPropItem("wcs.axis_x.a",PROPLIST_ITEM(id++,"极坐标夹角α"));
	CLDSPlate::AddPropItem("wcs.axis_y",PROPLIST_ITEM(id++,"Y轴方向"));
	CLDSPlate::AddPropItem("InsertProfileVertex",PROPLIST_ITEM(id++,"插入轮廓点","",""));
	CLDSPlate::AddPropItem("AddNewHole",PROPLIST_ITEM(id++,"添加孔","",""));
	CLDSPlate::AddPropItem("FilletVertex",PROPLIST_ITEM(id++,"倒角","",""));
	CLDSPlate::AddPropItem("DeleteFeature",PROPLIST_ITEM(id++,"删除"));
#endif
	for(int i=0;i<26;i++)
	{
		char sName[50]="",sPos[50]="";
		sprintf(sName,"%c",'A'+i);
		CLDSPlate::AddPropItem(sName,PROPLIST_ITEM(id++,CXhChar50("顶点%s",(char*)sName)));
		sprintf(sPos,"%s.x",sName);
		CLDSPlate::AddPropItem(sPos,PROPLIST_ITEM(id++,"X坐标"));
		sprintf(sPos,"%s.y",sName);
		CLDSPlate::AddPropItem(sPos,PROPLIST_ITEM(id++,"Y坐标"));
	}
}

// 螺栓
MEMORY_BLOCK CLDSBolt::MPCL;	//Mirror Property Control List
PROPLIST_TYPE CLDSBolt::propMirSync;
void CLDSBolt::InitPropMirBitsMap()
{
	int id=1;
	propMirSync.InitHashtableSize(32);
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("specification",PROPLIST_ITEM(id++,"规格"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("workNormInfo",PROPLIST_ITEM(id++,"工作法线"));
	MPCL.SetBitState(id-1);
	propMirSync.AddPropItem("Position",PROPLIST_ITEM(id++,"基准位置"));
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
	CLDSBolt::AddPropItem("m_hFamily",PROPLIST_ITEM(id++,"Bolt Specification Serial"));		//螺栓规格系列
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
	CLDSBolt::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"Project Information"));//影射信息
	CLDSBolt::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"Project Mother Part"));//影射母构件
	CLDSBolt::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"Vest In Block Assembly"));//归属的部件引用
	CLDSBolt::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"Vest In Block Handle"));//归属的部件
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
	//简单模式定位参数
	CLDSBolt::AddPropItem("des_base_pos.datum_pos_style",PROPLIST_ITEM(id++,"定位方式","","1.角钢楞点定位|2.角钢节点定位|3.角钢心线交点"));
	CLDSBolt::AddPropItem("des_base_pos.hBasePart",PROPLIST_ITEM(id++,"基准角钢"));
	CLDSBolt::AddPropItem("des_base_pos.hCrossPart",PROPLIST_ITEM(id++,"交叉角钢"));
	CLDSBolt::AddPropItem("des_base_pos.hBaseNode",PROPLIST_ITEM(id++,"基准节点"));
	CLDSBolt::AddPropItem("des_base_pos.iWorkWing",PROPLIST_ITEM(id++,"所在肢","","X肢|Y肢"));
	CLDSBolt::AddPropItem("des_base_pos.fWingOffDist1",PROPLIST_ITEM(id++,"g by angle leg","","g|g1|g2|g3"));
	CLDSBolt::AddPropItem("des_base_pos.fWingOffDist2",PROPLIST_ITEM(id++,"g by cross leg","","g|g1|g2|g3"));
	CLDSBolt::AddPropItem("des_base_pos.fWingOffDist3",PROPLIST_ITEM(id++,"g on plane","","g|g1|g2|g3"));
	CLDSBolt::AddPropItem("des_base_pos.iDirection",PROPLIST_ITEM(id++,"longitude direction","","start->end|end->start"));
	CLDSBolt::AddPropItem("des_base_pos.fLenOffDist",PROPLIST_ITEM(id++,"longitude offset"));
#else 
	CLDSBolt::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"螺栓基本信息"));
	CLDSBolt::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSBolt::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSBolt::AddPropItem("m_hFamily",PROPLIST_ITEM(id++,"螺栓规格系列"));		//螺栓规格系列
	CLDSBolt::AddPropItem("grade",PROPLIST_ITEM(id++,"螺栓级别","","4.8|5.8|6.8|8.8"));
	CLDSBolt::AddPropItem("specification",PROPLIST_ITEM(id++,"螺栓规格"));
	CLDSBolt::AddPropItem("L0",PROPLIST_ITEM(id++,"通过厚度","通过厚度(mm),包括螺栓垫圈的厚度以及通厚字符串计算得到的厚度两部分。",""));
	CLDSBolt::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSBolt::AddPropItem("statSegStr",PROPLIST_ITEM(id++,"段号统计范围","在多接腿换接处，螺栓同时需要在多个段中统计，因此需要特别指定统计段号范围"));
	CLDSBolt::AddPropItem("dwRayNo",PROPLIST_ITEM(id++,"射线号"));
	CLDSBolt::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSBolt::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	CLDSBolt::AddPropItem("hole_d_increment",PROPLIST_ITEM(id++,"螺孔间隙","由于装配需要,一般螺栓孔直径要大于螺栓的直径，这个直径的差值即为螺孔间隙(mm)。",""));
	//孔属性
	CLDSBolt::AddPropItem("xHolePropGroup",PROPLIST_ITEM(id++,"孔属性","",""));
	CLDSBolt::AddPropItem("dfHoleDiameter",PROPLIST_ITEM(id++,"孔径","构件上的孔直径(mm)。",""));
	CLDSBolt::AddPropItem("siWaistLength",PROPLIST_ITEM(id++,"腰圆长度","螺栓孔为腰圆时，的腰圆两端圆心间距离(mm)。",""));
	CLDSBolt::AddPropItem("hHoleResidePart",PROPLIST_ITEM(id++,"归属构件","螺栓孔所在构件的句柄。",""));
	CLDSBolt::AddPropItem("ciHoleFuncType",PROPLIST_ITEM(id++,"功用类型","螺栓孔的功能作用类型。","0.螺栓孔|1.脚钉孔|2.挂线孔|3.接地孔|4.装配孔|5.挂牌孔|6.引流孔|7.代孔脚钉|8.地脚孔"));
	CLDSBolt::AddPropItem("ciManuProcessType",PROPLIST_ITEM(id++,"制孔工艺","","冲孔|钻孔"));
	CLDSBolt::AddPropItem("blEnlargeHole",PROPLIST_ITEM(id++,"需要扩孔","在原有孔径基础上施加扩孔工艺","是|否"));
	//
	CLDSBolt::AddPropItem("gasketInfo",PROPLIST_ITEM(id++,"垫圈信息"));
	CLDSBolt::AddPropItem("gasketThick",PROPLIST_ITEM(id++,"垫圈厚度","","自动匹配|指定厚度"));
	CLDSBolt::AddPropItem("gasketN",PROPLIST_ITEM(id++,"垫圈数量"));
	CLDSBolt::AddPropItem("gasketOffset",PROPLIST_ITEM(id++,"垫圈位移"));
	CLDSBolt::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,"虚拟螺栓","","是|否"));
	CLDSBolt::AddPropItem("m_bAntiLooseWasher",PROPLIST_ITEM(id++,"统计防松垫圈","","是|否"));
	CLDSBolt::AddPropItem("m_bAntiTheft",PROPLIST_ITEM(id++,"统计为防盗螺栓","","是|否"));
	CLDSBolt::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色","","是|否"));
	CLDSBolt::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	CLDSBolt::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
	CLDSBolt::AddPropItem("m_bCalL0IncGapByInters", PROPLIST_ITEM(id++, "通厚计算执行规则", "0.累加通厚：累加穿过构件厚度计算通厚；1.计入缺失垫板间隙：累加穿过构件厚度及相邻构件间>2mm的间隙值计算通厚", "0.累加通厚|1.计入缺失垫板间隙"));
	CLDSBolt::AddPropItem("relativeObjs",PROPLIST_ITEM(id++,"关联构件"));
	CLDSBolt::AddPropItem("shadowInfo",PROPLIST_ITEM(id++,"影射信息"));//影射信息
	CLDSBolt::AddPropItem("m_hMotherObj",PROPLIST_ITEM(id++,"影射母构件","当前构件对应的影射母构件句柄，该句柄为0x0时表示当前构件为原始构件。"));//影射母构件
	CLDSBolt::AddPropItem("m_hBlockRef",PROPLIST_ITEM(id++,"归属部件引用"));//归属的部件引用
	CLDSBolt::AddPropItem("m_hBlock",PROPLIST_ITEM(id++,"归属部件"));//归属的部件
	CLDSBolt::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"装配方位"));
	CLDSBolt::AddPropItem("des_base_pos",PROPLIST_ITEM(id++,"装配位置设计参数","","简单模式|高级模式"));
	CLDSBolt::AddPropItem("des_base_pos.cLocationStyle",PROPLIST_ITEM(id++,"基点坐标类型","螺栓定位基点坐标类型","模型坐标|相对坐标"));
	CLDSBolt::AddPropItem("des_base_pos.cResidePlane",PROPLIST_ITEM(id++,"归属平面"));
	CLDSBolt::AddPropItem("des_base_pos.datumPoint",PROPLIST_ITEM(id++,"定位基点"));
	CLDSBolt::AddPropItem("des_base_pos.datumPoint.x",PROPLIST_ITEM(id++,"基点X坐标"));
	CLDSBolt::AddPropItem("des_base_pos.datumPoint.y",PROPLIST_ITEM(id++,"基点Y坐标"));
	CLDSBolt::AddPropItem("des_base_pos.datumPoint.z",PROPLIST_ITEM(id++,"基点Z坐标"));
	CLDSBolt::AddPropItem("des_base_pos.hPart",PROPLIST_ITEM(id++,"定位基准构件"));
	CLDSBolt::AddPropItem("des_base_pos.m_biWingOffsetStyle",PROPLIST_ITEM(id++,"横向偏移方式","","g|g1|g2|g3|自定义"));
	CLDSBolt::AddPropItem("des_base_pos.wing_offset_dist",PROPLIST_ITEM(id++,"横向偏移量"));
	CLDSBolt::AddPropItem("des_base_pos.len_offset_dist",PROPLIST_ITEM(id++,"纵向偏移量"));
	CLDSBolt::AddPropItem("RotationStartEdge",PROPLIST_ITEM(id++,"旋转起始边","","0.基准边|1.焊道线"));
	CLDSBolt::AddPropItem("des_base_pos.offset_angle",PROPLIST_ITEM(id++,"偏移角度"));
	CLDSBolt::AddPropItem("des_base_pos.R",PROPLIST_ITEM(id++,"偏移距离"));
	CLDSBolt::AddPropItem("des_base_pos.offset_wing",PROPLIST_ITEM(id++,"偏移肢","","X肢|Y肢"));
	CLDSBolt::AddPropItem("des_base_pos.direction",PROPLIST_ITEM(id++,"纵向偏移方向","","始->终|终->始"));
	CLDSBolt::AddPropItem("des_base_pos.norm_offset",PROPLIST_ITEM(id++,"Z向偏移","螺栓自定位基点沿装配坐标系Ｚ轴方向（自身工作法线方向）的偏移量。"));
	CLDSBolt::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"位置"));
	CLDSBolt::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSBolt::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSBolt::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSBolt::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X轴方向"));
	CLDSBolt::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSBolt::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSBolt::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSBolt::AddPropItem("des_work_norm",PROPLIST_ITEM(id++,"工作法线设计参数"));
	CLDSBolt::AddPropItem("des_work_norm.norm_style",PROPLIST_ITEM(id++,"工作法线计算方式","",CLDSVector::GetNormStyleCmbItems()));
	CLDSBolt::AddPropItem("des_work_norm.hVicePart",PROPLIST_ITEM(id++,"基准法线构件"));
	CLDSBolt::AddPropItem("des_work_norm.norm_wing",PROPLIST_ITEM(id++,"基准肢","","X肢法线|Y肢法线"));
	CLDSBolt::AddPropItem("des_work_norm.reside_plane",PROPLIST_ITEM(id++,"连接面"));
	CLDSBolt::AddPropItem("des_work_norm.direction",PROPLIST_ITEM(id++,"方向类型|法线朝向"));
	CLDSBolt::AddPropItem("des_work_norm.hCrossPart",PROPLIST_ITEM(id++,"交叉参考杆件"));
	CLDSBolt::AddPropItem("des_work_norm.nearVector",PROPLIST_ITEM(id++,"(近似|相对)工作法线"));
	CLDSBolt::AddPropItem("des_work_norm.nearVector.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSBolt::AddPropItem("des_work_norm.nearVector.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSBolt::AddPropItem("des_work_norm.nearVector.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSBolt::AddPropItem("des_work_norm.rot_ang",PROPLIST_ITEM(id++,"旋转角度","","0|90|180|270"));
	CLDSBolt::AddPropItem("work_norm",PROPLIST_ITEM(id++,"工作法线"));
	CLDSBolt::AddPropItem("work_norm.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSBolt::AddPropItem("work_norm.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSBolt::AddPropItem("work_norm.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	//简单模式定位参数
	CLDSBolt::AddPropItem("des_base_pos.datum_pos_style",PROPLIST_ITEM(id++,"定位方式","","1.角钢楞点定位|2.角钢节点定位|3.角钢心线交点"));
	CLDSBolt::AddPropItem("des_base_pos.hBasePart",PROPLIST_ITEM(id++,"基准角钢"));
	CLDSBolt::AddPropItem("des_base_pos.hCrossPart",PROPLIST_ITEM(id++,"交叉角钢"));
	CLDSBolt::AddPropItem("des_base_pos.hBaseNode",PROPLIST_ITEM(id++,"基准节点"));
	CLDSBolt::AddPropItem("des_base_pos.iWorkWing",PROPLIST_ITEM(id++,"所在肢","","X肢|Y肢"));
	CLDSBolt::AddPropItem("des_base_pos.fWingOffDist1",PROPLIST_ITEM(id++,"横向心距","螺栓在基准角钢上的实际心距","g|g1|g2|g3"));
	CLDSBolt::AddPropItem("des_base_pos.fWingOffDist2",PROPLIST_ITEM(id++,"横向心距2","螺栓在交叉角钢上的心距","g|g1|g2|g3"));
	CLDSBolt::AddPropItem("des_base_pos.fWingOffDist3",PROPLIST_ITEM(id++,"平推心距","定位的平推基点在基准角钢上的心距","g|g1|g2|g3"));
	CLDSBolt::AddPropItem("des_base_pos.iDirection",PROPLIST_ITEM(id++,"纵向偏移方向","","始->终|终->始"));
	CLDSBolt::AddPropItem("des_base_pos.fLenOffDist",PROPLIST_ITEM(id++,"纵向偏移距离","自定位基点沿基准角钢纵向的偏移量"));
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
	CLDSSphere::AddPropItem("basicInfo",PROPLIST_ITEM(id++,"球体基本信息"));
	CLDSSphere::AddPropItem("handle",PROPLIST_ITEM(id++,"句柄"));
	CLDSSphere::AddPropItem("layer",PROPLIST_ITEM(id++,"图层"));
	CLDSSphere::AddPropItem("cMaterial",PROPLIST_ITEM(id++,"材质"));
	CLDSSphere::AddPropItem("m_cQualityLevel",PROPLIST_ITEM(id++,"质量等级",NULL,"默认|A|B|C|D|"));
	CLDSSphere::AddPropItem("specification",PROPLIST_ITEM(id++,"规格"));
	CLDSSphere::AddPropItem("iSeg",PROPLIST_ITEM(id++,"分段号"));
	CLDSSphere::AddPropItem("sPartNo",PROPLIST_ITEM(id++,"构件编号"));
	CLDSSphere::AddPropItem("cfgword",PROPLIST_ITEM(id++,"配材号"));
	CLDSSphere::AddPropItem("m_uStatMatCoef",PROPLIST_ITEM(id++,"统材系数"));
	CLDSSphere::AddPropItem("m_hPartWeldParent",PROPLIST_ITEM(id++,"焊接父构件"));
	CLDSSphere::AddPropItem("m_bDisplayHole",PROPLIST_ITEM(id++,"显示孔"));
	CLDSSphere::AddPropItem("m_bVirtualPart",PROPLIST_ITEM(id++,""));
	CLDSSphere::AddPropItem("D",PROPLIST_ITEM(id++,"外直径"));
	CLDSSphere::AddPropItem("d",PROPLIST_ITEM(id++,"内直径"));
	CLDSSphere::AddPropItem("m_bUserSpecColor",PROPLIST_ITEM(id++,"指定构件颜色"));
	CLDSSphere::AddPropItem("crMaterial",PROPLIST_ITEM(id++,"构件颜色"));
	CLDSSphere::AddPropItem("sNotes",PROPLIST_ITEM(id++,"备注"));
	CLDSSphere::AddPropItem("assemblePos",PROPLIST_ITEM(id++,"装配方位"));
	CLDSSphere::AddPropItem("ucs",PROPLIST_ITEM(id++,"装配坐标系"));
	CLDSSphere::AddPropItem("ucs.origin",PROPLIST_ITEM(id++,"位置"));
	CLDSSphere::AddPropItem("hDatumNode",PROPLIST_ITEM(id++,"基准节点"));
	CLDSSphere::AddPropItem("ucs.origin.x",PROPLIST_ITEM(id++,"X坐标"));
	CLDSSphere::AddPropItem("ucs.origin.y",PROPLIST_ITEM(id++,"Y坐标"));
	CLDSSphere::AddPropItem("ucs.origin.z",PROPLIST_ITEM(id++,"Z坐标"));
	CLDSSphere::AddPropItem("ucs.axis_x",PROPLIST_ITEM(id++,"X轴方向"));
	CLDSSphere::AddPropItem("ucs.axis_x.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSSphere::AddPropItem("ucs.axis_x.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSSphere::AddPropItem("ucs.axis_x.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSSphere::AddPropItem("ucs.axis_y",PROPLIST_ITEM(id++,"Y轴方向"));
	CLDSSphere::AddPropItem("ucs.axis_y.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSSphere::AddPropItem("ucs.axis_y.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSSphere::AddPropItem("ucs.axis_y.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSSphere::AddPropItem("ucs.axis_z",PROPLIST_ITEM(id++,"Z轴方向"));
	CLDSSphere::AddPropItem("ucs.axis_z.x",PROPLIST_ITEM(id++,"X坐标分量"));
	CLDSSphere::AddPropItem("ucs.axis_z.y",PROPLIST_ITEM(id++,"Y坐标分量"));
	CLDSSphere::AddPropItem("ucs.axis_z.z",PROPLIST_ITEM(id++,"Z坐标分量"));
	CLDSSphere::AddPropItem("sphereLinkPoles",PROPLIST_ITEM(id++,"连接杆件"));
#endif
}
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef __PART_DESIGN_INC_

void WEATHER::FromBuffer(CBuffer &buffer,long version/*=NULL*/)
{
	buffer.ReadInteger(&m_nHighestT);	//最高气温
	buffer.ReadInteger(&m_nLowestT);	//最低气温
	buffer.ReadInteger(&m_nAverageT);	//平均气温
	buffer.ReadDouble(&m_fMaxWindV);		//最大风速
	buffer.ReadInteger(&m_nMaxWindT);	//最大风速时温度
	buffer.ReadDouble(&m_fMaxIceThick);			//最大覆冰厚(mm)
	buffer.ReadInteger(&m_nMaxIceT);			//最大覆冰时温度
	buffer.ReadDouble(&m_fMaxIceWindV);			//最大覆冰时风速
	buffer.ReadInteger(&m_nSetupT);				//安装时温度
	buffer.ReadDouble(&m_fSetupWindV);	//安装时风速
}
void WEATHER::ToBuffer(CBuffer &buffer)
{
	buffer.WriteInteger(m_nHighestT);	//最高气温
	buffer.WriteInteger(m_nLowestT);	//最低气温
	buffer.WriteInteger(m_nAverageT);	//平均气温
	buffer.WriteDouble(m_fMaxWindV);	//最大风速
	buffer.WriteInteger(m_nMaxWindT);	//最大风速时温度
	buffer.WriteDouble(m_fMaxIceThick);	//最大覆冰厚(mm)
	buffer.WriteInteger(m_nMaxIceT);	//最大覆冰时温度
	buffer.WriteDouble(m_fMaxIceWindV);	//最大覆冰时风速
	buffer.WriteInteger(m_nSetupT);		//安装时温度
	buffer.WriteDouble(m_fSetupWindV);	//安装时风速
}
void WEATHER::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[50]="";
	fprintf(fp,"<天气信息>\n");
	fprintf(fp,"<最高气温 HighestT=\"%d\"/>\n",m_nHighestT);
	fprintf(fp,"<最低气温 LowestT=\"%d\"/>\n",m_nLowestT);
	fprintf(fp,"<平均气温 AvergeT=\"%d\"/>\n",m_nAverageT);
	sprintf(sText,"%f",m_fMaxWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<最大风速 MaxWindV=\"%s\"/>\n",sText);
	fprintf(fp,"<最大风速时温度 MaxWindT=\"%d\"/>\n",m_nMaxWindT);
	sprintf(sText,"%f",m_fMaxIceThick);
	SimplifiedNumString(sText);
	fprintf(fp,"<最大覆冰厚 MaxIceThick=\"%s\"/>\n",sText);
	fprintf(fp,"<最大覆冰时温度 MaxIceT=\"%d\"/>\n",m_nMaxIceT);
	sprintf(sText,"%f",m_fMaxIceWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<最大覆冰时风速 MaxIceWindV=\"%s\"/>\n",sText);
	fprintf(fp,"<安装时温度 SetupT=\"%d\"/>\n",m_nSetupT);
	sprintf(sText,"%f",m_fSetupWindV);
	SimplifiedNumString(sText);
	fprintf(fp,"<安装时风速 SetupWindV=\"%s\"/>\n",sText);
	fprintf(fp,"</天气信息>\n");
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
	fprintf(fp,"<气候区域 weatherCode=\"%s\">\n",weatherCode);
	WEATHER::ToXmlFile(fp,schema);
	fprintf(fp,"</气候区域>\n");
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
	{	//存储单元的关联对称信息，否则无法在单元模式下正常预览对称单元的设计效率
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
			cCalLenType=5;	//5.指定平行轴和最小轴验算长度
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
		return 0;	//单元长度
	else if(LoInfo.bCalByElemLen)
		return 0;	//单元长度
	else if(LoInfo.hStartNode<=0||LoInfo.hEndNode<=0)
		return 2;	//不予考虑
	else
		return 1;	//指定节间
}
int CElemInfo::GetSmcItemsNum(int *scmItemsTag/* =NULL */)
{	//得到同时受压考虑的项数并标记具体的项
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
BYTE SMC_ITEM::RetrieveSmcType(long hElemStart,long hElemEnd,IModel* pModel)	//交叉斜材同时受压类型
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
		//不采用小号－大号形式显示，是为了避免与单元校审窗口中的始末端产生冲突　wjh-2014.1.14
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
		if(pParentRod->layer(0)=='L')	//接腿构件
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
			strcpy(sText, "等肢角钢");
		else if(pParentRod->size_idClassType==CLS_GROUPLINEANGLE)
		{
			if(pParentRod->size_cSubClassType=='T')
				strcpy(sText, "T型组合");
			else if(pParentRod->size_cSubClassType=='D')
				strcpy(sText, "对角组合");
			else if(pParentRod->size_cSubClassType=='X')
				strcpy(sText, "十字组合");
		}
		else if(pParentRod->size_idClassType==CLS_LINETUBE)
			strcpy(sText, "钢管");
		else if(pParentRod->size_idClassType==CLS_LINEFLAT)
			strcpy(sText, "扁钢");
		else if(pParentRod->size_idClassType==CLS_LINESLOT)
			strcpy(sText, "槽钢");
		else if(pParentRod->size_idClassType==CLS_CABLE)
			strcpy(sText, "线缆");
		else if(pParentRod->size_idClassType==0)
			strcpy(sText, "用户定制");
#endif
	}
	else if(GetPropID("specification")==id)
	{
		if(pParentRod->GetClassTypeId()==CLS_LINEPART&&pParentRod->size_idClassType==0)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(sText,"Cross-section Constant");
#else 
			strcpy(sText,"截面常数");
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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
			strcpy(sText,"1.二力杆");
		else if(ciFreedomStart>3||ciFreedomEnd>3)
			strcpy(sText,"2.经典梁");
		else if(pParentRod->m_iElemType==0)
			strcpy(sText,"0.自动判断");
		else if(pParentRod->m_iElemType==1)
			strcpy(sText,"1.二力杆");
		else if(pParentRod->m_iElemType==2)
			strcpy(sText,"2.经典梁");
		else if(pParentRod->m_iElemType==3)
			strcpy(sText,"3.柔索单元");
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
			strcpy(sText,"铰接");
		else if(jointType==JOINT_RIGID||ciFreedomStart>3)
			strcpy(sText,"刚接");
		else //if(jointType==JOINT_AUTOJUSTIFY)
			strcpy(sText,"自动判断");
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
			strcpy(sText,"中心受力");
		else //if(forceType==ECCENTRIC_FORCE)
			strcpy(sText,"偏心受力");
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
			strcpy(sText,"铰接");
		else if(jointType==JOINT_RIGID||ciFreedomEnd>3)
			strcpy(sText,"刚接");
		else //if(jointType==JOINT_AUTOJUSTIFY)
			strcpy(sText,"自动判断");
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
			strcpy(sText,"中心受力");
		else //if(forceType==ECCENTRIC_FORCE)
			strcpy(sText,"偏心受力");
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
			strcpy(sText,"0.智能计算");
		else if(cCalLenType==1)
			strcpy(sText,"1.V斜材(跨脚材连接)");
		else if(cCalLenType==2)
			strcpy(sText,"2.V斜材(V面连接)");
		else if(cCalLenType==3)
			strcpy(sText,"3.V斜材3节间带横连杆");
		else if(cCalLenType==4)
			strcpy(sText,"4.V斜材4节间带横连杆");
		else if(cCalLenType==6)
			strcpy(sText,"6.指定交叉材计算长度");
		else //if(cCalLenType==5)
			strcpy(sText,"5.指定单元计算长度");
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
			strcpy(sText,"0.不考虑");
		else			  
			strcpy(sText,"1.考虑");
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
			strcpy(sText,"0.不考虑");
		else			  
			strcpy(sText,"1.考虑");
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
			strcpy(sText,"0.不考虑");
		else if(iNum==1)
			strcpy(sText,"1.单项");
		else if(iNum==2)
			strcpy(sText,"2.两项");
		else
			strcpy(sText,"3.三项");
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
			strcpy(sText,"0.单元长度");
		else if(type==1)
			strcpy(sText,"1.指定节间");
		else
			strcpy(sText,"2.不考虑");
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
			strcpy(sText,"0.单元长度");
		else if(type==1)
			strcpy(sText,"1.指定节间");
		else
			strcpy(sText,"2.不考虑");
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
			strcpy(sText,"0.单元长度");
		else if(type==1)
			strcpy(sText,"1.指定节间");
		else
			strcpy(sText,"2.不考虑");
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
			strcpy(sText,"0.单元长度");
		else if(type==1)
			strcpy(sText,"1.指定节间");
		else
			strcpy(sText,"2.不考虑");
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
			strcpy(sText,"0.单元长度");
		else if(type==1)
			strcpy(sText,"1.指定节间");
		else
			strcpy(sText,"2.不考虑");
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
		if(LoRyoCalType()==0)	//指定单元长度
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
		if(LoRxCalType()==0)	//指定单元长度
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
		if(LoCalType(smcItems[0].LoRc)==0)	//指定单元长度
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
		if(LoCalType(smcItems[1].LoRc)==0)	//指定单元长度
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
		if(LoCalType(smcItems[2].LoRc)==0)	//指定单元长度
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
	//受压类型
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
			strcpy(sText,"0.X轴对称");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_MIRY)	
			strcpy(sText,"1.Y轴对称");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_START)
			strcpy(sText,"2.始端同端点");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_END)
			strcpy(sText,"3.终端同端点");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_2ENDS)
			strcpy(sText,"4.两端同端点");
		else if(smcItems[0].cSMCType==SMC_ITEM::SMC_REFER_UDF)	
			strcpy(sText,"5.指定同时受压单元");
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
			strcpy(sText,"0.X轴对称");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_MIRY)	
			strcpy(sText,"1.Y轴对称");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_START)
			strcpy(sText,"2.始端同端点");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_END)
			strcpy(sText,"3.终端同端点");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_2ENDS)
			strcpy(sText,"4.两端同端点");
		else if(smcItems[1].cSMCType==SMC_ITEM::SMC_REFER_UDF)	
			strcpy(sText,"5.指定同时受压单元");
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
			strcpy(sText,"0.X轴对称");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_MIRY)	
			strcpy(sText,"1.Y轴对称");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_START)
			strcpy(sText,"2.始端同端点");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_END)	
			strcpy(sText,"3.终端同端点");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_2ENDS)
			strcpy(sText,"4.两端同端点");
		else if(smcItems[2].cSMCType==SMC_ITEM::SMC_REFER_UDF)
			strcpy(sText,"5.指定同时受压单元");
#endif
	}
	//回转半径
	else if(GetPropID("smcItem1.gyreRadius")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(smcItems[0].cSMCType&0x80)	//最小轴
			strcpy(sText,"1.minimum axial");
		else 
			strcpy(sText,"0.parallel axial");
#else 
		if(smcItems[0].cSMCType&0x80)	//最小轴
			strcpy(sText,"1.最小轴");
		else 
			strcpy(sText,"0.平行轴");
#endif
	}
	else if(GetPropID("smcItem2.gyreRadius")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(smcItems[1].cSMCType&0x80)	//最小轴
			strcpy(sText,"1.minimum axial");
		else 
			strcpy(sText,"0.parallel axial");
#else 
		if(smcItems[1].cSMCType&0x80)	//最小轴
			strcpy(sText,"1.最小轴");
		else 
			strcpy(sText,"0.平行轴");
#endif
	}
	else if(GetPropID("smcItem3.gyreRadius")==id)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(smcItems[1].cSMCType&0x80)	//最小轴
			strcpy(sText,"1.minimum axial");
		else 
			strcpy(sText,"0.parallel axial");
#else 
		if(smcItems[1].cSMCType&0x80)	//最小轴
			strcpy(sText,"1.最小轴");
		else 
			strcpy(sText,"0.平行轴");
#endif
	}
#ifdef AFX_TARG_ENU_ENGLISH
	if(strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("0X%X-0X%X Element's property##d string value out of length",m_hStartNode,m_hEndNode,id);
#else
	if(strlen(sText)>=nMaxStrBufLen)
		Log2File()->Log("单元0X%X-0X%X属性#%d字符串超出长度限制",m_hStartNode,m_hEndNode,id);
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
		strcpy(prop_str,"没找到名为\"");
		strcat(prop_str,propStr);
		strcat(prop_str,"\"的属性ID！");
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
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
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
short CElemInfo::ElemType(CLDSLinePart* pRod)	//0:二力杆或经典梁；1:柔索
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
	if(pRod&&pRod->pStart&&pRod->pStart->handle==m_hStartNode)	//杆件端节点，与杆件端节点受力类型一致
	{
		if(pRod->start_joint_type==JOINT_HINGE)
			return 2;	//强制铰接
		else if(pRod->start_joint_type==JOINT_RIGID)
			return 1;	//刚接
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
	if(pRod&&pRod->pEnd&&pRod->pEnd->handle==m_hEndNode)	//杆件端节点，与杆件端节点受力类型一致
	{
		if(pRod->end_joint_type==JOINT_HINGE)
			return 2;	//强制铰接
		else if(pRod->end_joint_type==JOINT_RIGID)
			return 1;	//刚接
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
	if(pRod&&pRod->pStart&&pRod->pStart->handle==m_hStartNode)	//杆件端节点，与杆件端节点受力类型一致
		return pRod->start_force_type;
	if(pRod&&pRod->pStart&&pRod->pStart->handle==m_hEndNode)	//杆件端节点，与杆件端节点受力类型一致
		return pRod->end_force_type;
	else						//杆件中间节点，肯定是中心受力
		return CENTRIC_FORCE;
}
int CElemInfo::EndForceType(CLDSLinePart* pRod)
{
	if(m_pModel&&pRod==NULL)
		pRod=(CLDSLinePart*)m_pModel->FromPartHandle(m_hParentRod,CLS_LINEPART);
	if(pRod&&pRod->pEnd&&pRod->pEnd->handle==m_hEndNode)	//杆件端节点，与杆件端节点受力类型一致
		return pRod->end_force_type;
	if(pRod&&pRod->pEnd&&pRod->pEnd->handle==m_hStartNode)	//杆件端节点，与杆件端节点受力类型一致
		return pRod->start_force_type;
	else						//杆件中间节点，肯定是中心受力
		return CENTRIC_FORCE;
}
/////////////////////////////////////////////////////////////////////////////
//与构件直径有关的裹冰厚度修正系数alfa1
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
//high_pos(m)高度位置,裹冰厚度的高度递增系数alfa2
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
	double Af=0.0;	//净截面积
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
			if(fabs(cs.axis_y*cs.axis_x)>EPS_COS)	//与X轴共线为无效Y轴方向,清空继续寻找
				cs.axis_y.Set();
			else
			{	//找到合理坐标系
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
	//计算后侧挡风面
	backcs=cs;
	//计算风压投影面
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
	//削除多边形凹角
	int n=vertexList.GetSize();
	for(i=0;i<n;i++)
	{
		double result=DistOf2dPtLine(vertexList[(i+2)%n],vertexList[i],vertexList[(i+1)%n]);
		if(result<0)	// 后点在线右侧，有凹角出现
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
		return false;	//不在投影多边形区域内
	GEPOINT frontpick=cs.TransPToCS(point3d);
	GEPOINT justify_norm_vec(cs.origin.x-prjucs.origin.x,cs.origin.y-prjucs.origin.y);
	double justify=justify_norm_vec.x*cs.axis_z.x+justify_norm_vec.y*cs.axis_z.y;
	if((justify>0&&frontpick.z>maxZ+EPS)||(justify<0&&frontpick.z<minZ-EPS))
		return false;	//位于前侧挡风面包容盒更外侧
	GEPOINT backpick=backcs.TransPToCS(point3d);
	if((justify>0&&backpick.z>maxZ+EPS)||(justify<0&&backpick.z<minZ-EPS))
		return false;	//位于后侧挡风面包容盒更外侧
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
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "%d号风压段对称过程中发现，缺少合适的对称类型属性", iNo);
			return false;
		}
		MIRMSG mirmsg(3 - ciMirY1X2);
		if ((pMirNode = pNode->GetMirNode(mirmsg)) != NULL)
			pMirWindSeg->nodeArr[i] = pMirNode->handle;
		if ((pMirWindSeg->nodePtrArr[i] = pMirNode) == NULL)
			logerr.LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT, "%d号风压段，轮廓节点0x%X对称失败", iNo, nodeArr[i]);
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
	AddPropItem("attribute",PROPLIST_ITEM(id++,"风压段属性","风压段属性"));
	AddPropItem("type",PROPLIST_ITEM(id++,"风压段类型","风压段类型","0.八点分配矩形塔身段|1.自定义多节点轮廓|2.六点分配三角塔身段"));
	AddPropItem("cMirType",PROPLIST_ITEM(id++,"对称方式","对称方式","无对称|X轴对称|Y轴对称"));
	AddPropItem("cCalStyle",PROPLIST_ITEM(id++,"计算方式","计算方式","塔身|X向横担|Y向横担"));
	AddPropItem("BetaZ",PROPLIST_ITEM(id++,"风振系数(杆塔)","风振系数(杆塔)"));
	AddPropItem("BetaZForFoundation",PROPLIST_ITEM(id++,"风振系数(基础)","风振系数(基础)"));
	AddPropItem("CqiFront",PROPLIST_ITEM(id++,"正面效应系数","正面效应系数"));
	AddPropItem("CqiSide",PROPLIST_ITEM(id++,"侧面效应系数","侧面效应系数"));
	AddPropItem("m_bAutoCalProfileCoef",PROPLIST_ITEM(id++,"塔架体型系数","是否自动计算塔架体型系数","自动计算|手动指定"));
	AddPropItem("m_fProfileModeCoef",PROPLIST_ITEM(id++,"系数","塔架体型系数"));
	AddPropItem("m_bSpecB_A",PROPLIST_ITEM(id++,"正面深度/宽度","用户指定'正面深度/宽度'值","自动计算|手动指定"));
	AddPropItem("B_AFront",PROPLIST_ITEM(id++,"系数","正面深度/宽度"));
	AddPropItem("K",PROPLIST_ITEM(id++,"塔身断面形状系数","塔身断面形状系数"));
	AddPropItem("basis_points",PROPLIST_ITEM(id++,"投影面基点","投影面基点"));
	AddPropItem("X",PROPLIST_ITEM(id++,"X","投影面基点X坐标"));
	AddPropItem("Y",PROPLIST_ITEM(id++,"Y","投影面基点Y坐标"));
	AddPropItem("Z",PROPLIST_ITEM(id++,"Z","投影面基点Z坐标"));
#endif
}
int CWindSegment::GetPropValueStr(long id,char *valueStr,UINT nMaxStrBufLen/*=100*/)
{
	CXhChar100 sText;
	if(GetPropID("type")==id)
	{
		if(cType==0)
			sText.Printf("0.八点分配矩形塔身段");
		else if(cType==1)
			sText.Printf("1.自定义多节点轮廓");
		else if(cType==2)
			sText.Printf("2.六点分配三角塔身段");
	}
	else if(GetPropID("cMirType")==id)
	{
		if(cType==0||cType==2||cMirType==0)
			sText.Printf("无对称");
		else if(cMirType==1)
			sText.Printf("X轴对称");
		else if(cMirType==2)
			sText.Printf("Y轴对称");
	}
	else if(GetPropID("cCalStyle")==id)
	{
		if(cType==0||cType==2||cCalStyle==0)
			sText.Printf("塔身");
		else if(cCalStyle==1)
			sText.Printf("X向横担");
		else if(cCalStyle==2)
			sText.Printf("Y向横担");
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
			sText.Copy("自动计算");
		else
			sText.Copy("手动指定");
	}
	else if(GetPropID("B_AFront")==id)
		sText.Printf("%lg",B_AFront);
	else if(GetPropID("m_bSpecB_A")==id)
	{
		if(m_bSpecB_A)
			sText.Copy("手动指定");
		else
			sText.Copy("自动计算");
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
	buffer.WritePoint(xProjPlaneOrg);			//投影面基点
	buffer.WriteInteger(m_bSpecB_A);			//由用户指定迎风面深度与宽度比值
	buffer.WriteInteger(m_bAutoCalProfileCoef);	//自动计算塔架整体构件体型系数
	buffer.WriteDouble(m_fProfileModeCoef);		//塔架整体构件体型系数
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
	fprintf(fp,"<风压段信息>\n");
	fprintf(fp,"<编号 iNo=\"%d\"/>\n",iNo);
	if (cType==0)
		fprintf(fp,"<标准段号 cType=\"0.八点分配塔身段\"/>\n");
	else if (cType==1)
		fprintf(fp,"<标准段号 cType=\"1.自定义多节点轮廓\"/>\n");
	else if (cType==2)
		fprintf(fp,"<标准段号 cType=\"2.六点分配三角塔身\"/>\n");
	if (cMirType==0)
		fprintf(fp,"<对称信息 cMirType=\"0.无对称\"/>\n");
	else if (cMirType==1)
		fprintf(fp,"<对称信息 cMirType=\"1.Y轴对称\"/>\n");
	else if (cMirType==2)
		fprintf(fp,"<对称信息 cMirType=\"2.X轴对称\"/>\n");
	if (cCalStyle==0)
		fprintf(fp,"<计算方式 cCalStyle=\"0.按塔身方式计算风压\"/>\n");
	else if (cCalStyle==1)
		fprintf(fp,"<计算方式 cCalStyle=\"1.按X方向水平横担方式计算风压\"/>\n");
	else if (cCalStyle==2)
		fprintf(fp,"<计算方式 cCalStyle=\"2.按Y方向水平横担方式计算风压\"/>\n");

	//buffer.LogPosition();
	//buffer.WriteInteger(0);
	int i,n=GetMaxNodeCount();
	fprintf(fp,"<八个轮廓节点的句柄 nodeArr=\"[");
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
	fprintf(fp,"<i值 i=\"%d\"/>\n",i);
	//buffer.RecallPosition();
	sprintf(sText,"%f",CqiFront);
	SimplifiedNumString(sText);
	fprintf(fp,"<正面增大系数 CqiFront=\"%s\"/>\n",sText);
	sprintf(sText,"%f",CqiSide);
	SimplifiedNumString(sText);
	fprintf(fp,"<侧面增大系数 CqiSide=\"%s\"/>\n",sText);
	sprintf(sText,"%f",K);
	SimplifiedNumString(sText);
	fprintf(fp,"<塔身风荷载断面形状系数 K=\"%s\"/>\n",sText);
	sprintf(sText,"%f",AfFront);
	SimplifiedNumString(sText);
	fprintf(fp,"<m2构件正面承受风压净投影面积 AfFront=\"%s\"/>\n",sText);
	sprintf(sText,"%f",AfSide);
	SimplifiedNumString(sText);
	fprintf(fp,"<m2构件侧面承受风压净投影面积 AfSide=\"%s\"/>\n",sText);
	sprintf(sText,"%f",xProjPlaneOrg.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",xProjPlaneOrg.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",xProjPlaneOrg.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<投影面基点 xProjPlaneOrg=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",m_bSpecB_A);
	SimplifiedNumString(sText);
	fprintf(fp,"<由用户指定迎风面深度与宽度比值 m_bSpecB_A=\"%s\"/>\n",sText);
	sprintf(sText,"%f",m_bAutoCalProfileCoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<自动计算塔架整体构件体型系数 m_bAutoCalProfileCoef=\"%s\"/>\n",sText);
	sprintf(sText,"%f",m_fProfileModeCoef);
	SimplifiedNumString(sText);
	fprintf(fp,"<塔架整体构件体型系数 m_fProfileModeCoef=\"%s\"/>\n",sText);
	sprintf(sText,"%f",FaiFront);
	SimplifiedNumString(sText);
	fprintf(fp,"<正面挡风系数 FaiFront=\"%s\"/>\n",sText);
	sprintf(sText,"%f",B_AFront);
	SimplifiedNumString(sText);
	fprintf(fp,"<正面塔架迎风面与背风面之间距离或塔架迎风面宽度 B_AFront=\"%s\"/>\n",sText);
	sprintf(sText,"%f",FaiSide);
	SimplifiedNumString(sText);
	fprintf(fp,"<侧面挡风系数 FaiSide=\"%s\"/>\n",sText);
	sprintf(sText,"%f",B_ASide);
	SimplifiedNumString(sText);
	fprintf(fp,"<侧面塔架迎风面与背风面之间距离或塔架迎风面宽度 B_ASide=\"%s\"/>\n",sText);
	sprintf(sText,"%f",BetaZ);
	SimplifiedNumString(sText);
	fprintf(fp,"<风振系数或称风荷载调整系数 BetaZ=\"%s\"/>\n",sText);
	sprintf(sText,"%f",BetaZForFoundation);
	SimplifiedNumString(sText);
	fprintf(fp,"<用于基础设计的风振系数或称风荷载调整系数 BetaZForFoundation=\"%s\"/>\n",sText);
	fprintf(fp,"</风压段信息>\n");
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
			throw "The node number of wind pressure is more than 8,so the data file is wrong！";
#else 
			throw "风压段节点数超过８个，数据文件有误！";
#endif
		for(int i=0;i<n;i++)
			buffer.ReadInteger(&nodeArr[i]);
	}
	else
	{
		long iType;
		buffer.ReadInteger(&iType);
		cType=(BYTE)iType%100;
		if(cType==1)	//0.八节点矩形塔身段
			cType=0;
		else if(cType<8)
			cType=1;	//1.自定义多节点轮廓
		else
			cType=2;	//2.六节点三角塔身段
		cCalStyle=(BYTE)(iType/1000);	//0.塔身方式计算1.X方向水平横担2.Y方向水平横担
		cMirType=(BYTE)((iType%1000)/100);	//对称信息0.无对称1.Y轴对称2.X轴对称
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
		buffer.ReadPoint(xProjPlaneOrg);			//投影面基点
		buffer.ReadInteger(&m_bSpecB_A);			//由用户指定迎风面深度与宽度比值
		buffer.ReadInteger(&m_bAutoCalProfileCoef);	//自动计算塔架整体构件体型系数
		buffer.ReadDouble(&m_fProfileModeCoef);		//塔架整体构件体型系数
	}
	buffer.ReadDouble(&FaiFront);
	buffer.ReadDouble(&B_AFront);
	if(B_AFront<=0)	//0肯定不合理，而且会导致风压段对话框报错，所以暂赋值1.0，具体可自动计算
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
	// 由于Ta.Node已包含哈希表，上述代码下面简化为如下四行　wjh-2012.8.11
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
	if(cType==0)	//八点分配塔身段
	{
		if(nodePtrArr[0]==NULL||nodePtrArr[1]==NULL)
			return false;
		nodeset.append(nodePtrArr[0]);
		nodeset.append(nodePtrArr[1]);
		if(bFront)
		{	//正面Y轴对称轮廓点
			nodePtrArr[2]=nodePtrArr[1]->GetMirNode(MIRMSG(2));
			nodePtrArr[3]=nodePtrArr[0]->GetMirNode(MIRMSG(2));
		}
		else
		{	//侧面X轴对称轮廓点
			nodePtrArr[2]=nodePtrArr[1]->GetMirNode(MIRMSG(1));
			nodePtrArr[3]=nodePtrArr[0]->GetMirNode(MIRMSG(1));
		}
		if(nodePtrArr[2]==NULL||nodePtrArr[3]==NULL)
			return false;
		nodeset.append(nodePtrArr[2]);
		nodeset.append(nodePtrArr[3]);
	}
	else if(cType==1)
	{	//横担多节点轮廓面
		if(nValidNode<3)
			return false;
		for(i=0;i<nValidNode;i++)
			nodeset.append(nodePtrArr[i]);
	}
	else if(cType==2)
	{	//三角塔身段
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
	if (cType == 0)	//八点分配塔身段
	{
		if (nodePtrArr[0] == NULL || nodePtrArr[1] == NULL)
			return false;
		//正面Y轴对称轮廓点
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
	{	//横担多节点轮廓面
		if (nValidNode < 3)
			return false;
		GEPOINT _arrPoints[8];
		ARRAY_LIST<GEPOINT> xarrPolyVertexes(_arrPoints, 8);
		for (i = 0; i < nValidNode; i++)
			xarrPolyVertexes.append(nodePtrArr[i]->xOriginalPos);
		GEPOINT vBasePlaneNormal(0, 1, 0);
		if (this->cCalStyle == 2)	//Y向水平横担
			vBasePlaneNormal.Set(1, 0, 0);
		xTaperScope.InitPolyVertexes(this->xProjPlaneOrg, vBasePlaneNormal, 
			xarrPolyVertexes.m_pData, xarrPolyVertexes.GetSize());
		return xTaperScope.IsIncludePoint(pxPoint, tolerance);
	}
	else if (cType == 2)
	{	//三角塔身段
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
	if (cType == 0)	//八点分配塔身段
	{
		if (nodePtrArr[0] == NULL || nodePtrArr[1] == NULL)
			return false;
		//正面Y轴对称轮廓点
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
	{	//横担多节点轮廓面
		if (nValidNode < 3)
			return false;
		GEPOINT _arrPoints[8];
		ARRAY_LIST<GEPOINT> xarrPolyVertexes(_arrPoints, 8);
		for (i = 0; i < nValidNode; i++)
			xarrPolyVertexes.append(nodePtrArr[i]->xOriginalPos);
		GEPOINT vBasePlaneNormal(0, 1, 0);
		if (this->cCalStyle == 2)	//Y向水平横担
			vBasePlaneNormal.Set(1, 0, 0);
		xTaperScope.InitPolyVertexes(this->xProjPlaneOrg, vBasePlaneNormal,
			xarrPolyVertexes.m_pData, xarrPolyVertexes.GetSize());
		return xTaperScope.IsIncludeLine(pxElemStart, pxElemEnd, tolerance);
	}
	else if (cType == 2)
	{	//三角塔身段
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
	strcpy(grade,"6.8");//螺栓级别4.8;5.8;6.8;8.8;10.9
	d=20;				//螺栓直径（规格）
	uiLegacyN=1;	//螺栓组中的螺栓数量
	rows=1;				//螺栓布置排数
	bearloadcoef=0;		//螺栓承载系数
	m_iConnectWing=0;	//角钢连接肢（如角钢双肢:0、X肢:1或Y肢:2连接）
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
		return wnConnBoltN;	//未输入计算螺栓数，表示可能是放样模型手工输入的实际单面连接螺栓数
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
	buffer.ReadInteger(&d);//螺栓组内大多数螺栓的规格
	buffer.Read(&uiLegacyN,4);			//螺栓组内螺栓的数量
	buffer.ReadInteger(&rows);		//螺栓组内螺栓的排数
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
		buffer.ReadInteger(&layout_style);		//螺栓排列方式(在角钢上的相对位置)
		buffer.ReadInteger(&jgzj.g);		//螺栓组中的螺栓心线(角钢准距)表
		buffer.ReadInteger(&jgzj.g1);		//螺栓组中的螺栓心线(角钢准距)表
		buffer.ReadInteger(&jgzj.g2);		//螺栓组中的螺栓心线(角钢准距)表
		buffer.ReadInteger(&jgzj.g3);		//螺栓组中的螺栓心线(角钢准距)表
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
	buffer.WriteInteger(d);//螺栓组内大多数螺栓的规格
	buffer.Write(&uiLegacyN,4);//WriteInteger(N);	//螺栓组内螺栓的数量
	buffer.WriteInteger(rows);		//螺栓组内螺栓的排数
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
CLDSObject::CLDSObject(){	// 构造函数
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
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
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
		idname.Copy("无");
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
		return true;	//完全一致或为最基本对象类型
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
	//hObj=0时表示换腿处原位影射拷贝出的构件 wjh-2015.7.23
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
			//多段整体部件装配时，无法输入统一段号就默认为0段，此时不应更改子段段号 wjh-2015.8.4
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
	if(  version>0&&(	//由于之前错误的设定宏__COMPLEX_PART_INC_致使T/MA无法记录影射参数　故版本号改为5.0.0.1 wjh-2015.3.9
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
	else //构件从属部件,与影射参数同时添加为方便管理，在此处保存
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
	//实际在LDS V1.2.0.4之前版本中并未存此信息,而ShadowParaAndBlockIdFromBuffer中则判断的是LDS V1.2.0.3
	//鉴于LDS V1.2.0.3版本使用极少,且更少会使用到环型构件,因此出于简化代码未进行此版本判断
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
	if( version==0||	//由于之前错误的设定宏__COMPLEX_PART_INC_致使T/MA无法记录影射参数　故版本号改为5.0.0.1 wjh-2015.3.9
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
	wSize=0;	//必须初始化清零，否则在赋值是会出错　wjh-2015.3.6
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
/* 这部分代码目前无用，但将来还是有可能用到的，暂注释掉以备后用 wjh-2015.3.7
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
	{	//参照X轴对称
		start.y*=-1;
		end_x.y*=-1;
		end_y.y*=-1;
	}
	else if(hRefMirAcs==2)
	{	//参照Y轴对称
		start.x*=-1;
		end_x.x*=-1;
		end_y.x*=-1;
	}
	else if(hRefMirAcs==3)
	{	//参照Z轴对称
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
	{	//参照X轴对称
		start.y*=-1;
		end_x.y*=-1;
		end_y.y*=-1;
	}
	else if(hMirAcs==2)
	{	//参照Y轴对称
		start.x*=-1;
		end_x.x*=-1;
		end_y.x*=-1;
	}
	else if(hMirAcs==3)
	{	//参照Z轴对称
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
	//象限定义
	// Ⅳ | Ⅲ
	//---------->X(轴)
	// Ⅱ | Ⅰ
	//    V(Y轴)
	if(wRefRotAngle%90==0 && wRotAngle%90==0)
	{	//典型对称
		if(fabs(vec_x.x)<EPS)	//矢量为Y轴向,
		{
			if(start.x>0&&start.y>0)
			{	//旋转对称到侧面1象限
				mir.hMirAcs=2;		//先Y轴对称
				mir.wRotAngle=-90;	//再绕Z轴反向旋转90
			}
			else if(start.x<0&&start.y<0)
			{	//旋转对称到侧面4象限
				mir.hMirAcs=2;		//先Y轴对称
				mir.wRotAngle=90;	//再绕Z轴正向旋转90
			}
			else if(start.x<0&&start.y>0)
			{	//旋转对称到侧面2象限
				mir.hMirAcs=0;		//无轴对称
				mir.wRotAngle=90;	//再绕Z轴正向旋转90
			}
			else //if(start.x>0&&start.y<0)
			{	//旋转对称到侧面3象限
				mir.hMirAcs=0;		//无轴对称
				mir.wRotAngle=-90;	//再绕Z轴反向旋转90
			}
		}
		else
		{
			if(start.x<0&&start.y<0)
				mir.hMirAcs=3;	//Z轴对称到正面4象限
			else if(start.x<0&&start.y>0)
				mir.hMirAcs=2;	//Y轴对称到正面2象限
			else if(start.x>0&&start.y<0)
				mir.hMirAcs=1;	//X轴对称到正面2象限
			else //if(start.x>0&&start.y>0)
				mir.hMirAcs=0;	//方位未变无需对称
			mir.wRotAngle=0;	//不需要旋转可直接对称到位
		}
	}
	else
	{
		//计算对称(旋转)变换后的X轴标识线的方位射线角
		mir.wRotAngle=ftoi(Cal2dLineAng(start.x,start.y,end_x.x,end_x.y)*DEGTORAD_COEF);
		GEPOINT vec_z=vec_x^vec_y;
		if(vec_z.z>0)	//无X/Y轴向对称
		{
			mir.hMirAcs=0;
			if(mir.wRotAngle==180)
			{	//旋转180°相当于Z轴对称
				mir.hMirAcs=3;
				mir.wRotAngle=0;
			}
		}
		else
		{	//统一为Y轴对称后再旋转
			mir.hMirAcs=2;
			mir.wRotAngle-=180;	//Y轴对称后X轴标识线的方位射线角为180°
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
//移植自GlobalFunc.cpp中的CalMirAtomLayer函数
void CLDSDbObject::MirObjLayer(const char* sOrgLayer,char* sNewLayer,MIRMSG mirmsg)
{
	CXhChar16 baklayer(sNewLayer);
	if(sNewLayer!=sOrgLayer)
		strcpy(sNewLayer,sOrgLayer);

	if(mirmsg.axis_flag&1)
	{	//X轴对称
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
	{	//Y轴对称
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
	{	//Z轴对称
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
			sNewLayer[2]='Y';	//前面45°对称到右面
		else if(sOrgLayer[2]=='Y'&&(mirmsg.mir_norm.IsEqual(1,-1,0)||mirmsg.mir_norm.IsEqual(-1,1,0)))
			sNewLayer[2]='Q';	//右面45°对称到前面
		else if(sOrgLayer[2]=='H'&&(mirmsg.mir_norm.IsEqual(1,-1,0)||mirmsg.mir_norm.IsEqual(-1,1,0)))
			sNewLayer[2]='Z';	//后面45°对称到左面
		else if(sOrgLayer[2]=='Z'&&(mirmsg.mir_norm.IsEqual(1,-1,0)||mirmsg.mir_norm.IsEqual(-1,1,0)))
			sNewLayer[2]='H';	//左面45°对称到后面
		else if(sOrgLayer[2]=='3'&&(mirmsg.mir_norm.IsEqual(1,-1,0)||mirmsg.mir_norm.IsEqual(-1,1,0)))
			sNewLayer[2]='2';	//3象限45°对称到2象限
		else if(sOrgLayer[2]=='2'&&(mirmsg.mir_norm.IsEqual(1,-1,0)||mirmsg.mir_norm.IsEqual(-1,1,0)))
			sNewLayer[2]='3';	//2象限45°对称到3象限

		else if(sOrgLayer[2]=='Q'&&(mirmsg.mir_norm.IsEqual(1,1,0)||mirmsg.mir_norm.IsEqual(-1,-1,0)))
			sNewLayer[2]='Z';	//前面135°对称到左面
		else if(sOrgLayer[2]=='Z'&&(mirmsg.mir_norm.IsEqual(1,1,0)||mirmsg.mir_norm.IsEqual(-1,-1,0)))
			sNewLayer[2]='Q';	//左面135°对称到前面
		else if(sOrgLayer[2]=='H'&&(mirmsg.mir_norm.IsEqual(1,1,0)||mirmsg.mir_norm.IsEqual(-1,-1,0)))
			sNewLayer[2]='Y';	//后面135°对称到右面
		else if(sOrgLayer[2]=='Y'&&(mirmsg.mir_norm.IsEqual(1,1,0)||mirmsg.mir_norm.IsEqual(-1,-1,0)))
			sNewLayer[2]='H';	//右面135°对称到后面
		else if(sOrgLayer[2]=='1'&&(mirmsg.mir_norm.IsEqual(1,1,0)||mirmsg.mir_norm.IsEqual(-1,-1,0)))
			sNewLayer[2]='4';	//1象限135°对称到4象限
		else if(sOrgLayer[2]=='4'&&(mirmsg.mir_norm.IsEqual(1,1,0)||mirmsg.mir_norm.IsEqual(-1,-1,0)))
			sNewLayer[2]='1';	//4象限135°对称到1象限

		else	//复原原图层
			strncpy(sNewLayer,baklayer,3);
	}
	if(mirmsg.axis_flag&8)	//旋转对称
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
//若为本体对象，则返回补齐全部接腿占位后的配材号
CFGWORD CLDSDbObject::UnifiedCfgword()
{
	if(IsLegObj())
		return cfgword;
	else
		return cfgword=cfgword.ToBodyWord();
}
CFGWORD CLDSDbObject::GetUnifiedCfgword() const//若为本体对象，则返回补齐全部接腿占位后的配材号
{
	if(IsLegObj())
		return cfgword;
	else
		return cfgword.ToBodyWord();
}
bool CLDSDbObject::ModuleTogetherWith(const CFGINFO& cfginfo) const//判断两对象是否共存于同一呼高实例
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
			if(pMirObj->mirInfo.axis_flag&0x10)	//镜像
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
		if(pMirObj->mirInfo.axis_flag&0x10)	//镜像
			buffer.WritePoint(pMirObj->mirInfo.mir_norm);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of relative object is wrong!");
#else
		CLDSObject::Log2File()->Log("关联构件记录数出现错误!");
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
		if(layer(0)=='L')	//接腿构件
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
			strcpy(sText,"无预拱");
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
		strcpy(prop_str,"没找到名为\"");
		strcat(prop_str,propStr);
		strcat(prop_str,"\"的属性ID！";
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
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
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
//双向同步
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
//已从Master->Accept进行了同步，避免死循环
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
//双向同步已从Accept->Master进行了同步，避免死循环
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
	wiAcceptPropId=pCopyFrom->wiAcceptPropId;//被动受控的属性或参数
	wiMasterPropId=pCopyFrom->wiMasterPropId;//主控构件的控制属性或参数
	hAcceptObj=pCopyFrom->hAcceptObj;	//被动受控构件对象
	hMasterObj=pCopyFrom->hMasterObj;	//主控构件对象
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
	//更新相关的约束控制对象
	IXhTemplEnumerator<CONSTRAINT>* pList=GetConstraintEnumerator();
	for(CONSTRAINT* pConstraint=pList->EnumFirst();pConstraint;pConstraint=pList->EnumNext())
	{
		if( (pConstraint->hMasterObj==handle&&pConstraint->wiMasterPropId==idMasterObjProp)||
			(pConstraint->hAcceptObj==handle&&pConstraint->wiAcceptPropId==idMasterObjProp&&pConstraint->dual))
		{
			if(pConstraint->hMasterObj==handle)
				pConstraint->came=true;	//阻止反向关联更新
			else if(pConstraint->hAcceptObj==handle)
				pConstraint->gone=true;	//阻止反向关联更新
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
	{	//赋值约束
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
int CLDSPart::CustomerSerial=0;	//客户定制标识序号
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
	m_cFuncType=CLDSPart::FUNC_COMMON;	//构件功用类型
	_cQualityLevel=-1;//0表示默认不显示材料质量等级,0xff(-1)表示随材料表中的默认质量等级而变
}
char CLDSPart::set_cMaterial(char cBriefMaterialMark)
{
	CLDSPart* pMotherPart=MotherPart();
	if(library!=NULL&&_material!=cBriefMaterialMark)
	{	//变换材质时，智能更新默认的材料质量等级
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
}	//在GetHoleSet内调用,以添加除典型螺栓孔外的杂项孔

UINT CLDSPart::GetHoleSet(IXhList<HOLE>* pListHoles,bool bOnlyBoltHole/*=false*/)
{
	return 0;	//返回值为孔总数
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
			if(pSolidBody==NULL)	//第一次生成影射构件时需要调用ShadowDesign()函数
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
				f3dPoint org=ucs.origin;	//将法向偏移考虑到坐标系转中
				if(GetClassTypeId()==CLS_PLATE)
					ucs.origin+=ucs.axis_z*((CLDSPlate*)this)->m_fNormOffset;
				else if(GetClassTypeId()==CLS_PARAMPLATE)
				{

				} 
				CloneBodyFrom(pMotherPart->pSolidBody);
				ucs.origin=org;				//恢复原原点坐标
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
			//影像构件降低色彩强度
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
//对于杆件0.完全对称一致;1.除实体坐标外对称一致;2.拾取点处射线同向对称;3.拾取点处射线异向对称
BYTE CLDSPart::MirSimilarGradeWith(CLDSPart* pPart,const MIRMSG& msg,f3dPoint *pPickPos/*=NULL*/)
{
	return 255;	//未实现
}
CLDSPart *CLDSPart::GetMirPart(MIRMSG mirmsg,const double *pPickPos/*=NULL*/,BOOL bCheckProfile/*=TRUE*/)
{
	//try{
		int cls_typeid=GetClassTypeId();
		for(RELATIVE_OBJECT *pItem=relativeObjs.GetFirst();pItem;pItem=relativeObjs.GetNext())
		{	//先在关联构件列表中查找
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
		BOOL pushed=m_pModel->PushPartStack(); //遍历构件列表前应先压栈 wht 12-03-05
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
			strcpy(sText,"默认");
#endif
	}
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("sPartNo")==id)
		strcpy(sText,GetPartNo());
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(cfgword,valueStr);
	{
		if(layer(0)=='L')	//接腿构件
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");	
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
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
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
			log2file->Log("没找到名为'%s'的属性Id",propStr);
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
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
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
	ciEqualState=peerpart.ciEqualState;	//0.完全一致可工艺互换;1.
	cbDetailFlag=peerpart.cbDetailFlag;	//0.不输出任何细节;0x01:输出hashBoltsByBase; 0x02:输出hashBoltsByPeer;
	hPeerPart=peerpart.hPeerPart;		//伙伴构件的句柄
	m_hBlock=peerpart.m_hBlock;
	csBase=peerpart.csBase;	//基准构件对比一致或近似时的局部（装配）坐标系
	csPeer=peerpart.csPeer;	//伙伴构件对比一致或近似时的局部（装配）坐标系
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
		return "角钢";
	else if(GetClassTypeId()==CLS_GROUPLINEANGLE)
		return "组合角钢";
	else if(GetClassTypeId()==CLS_LINETUBE)
		return "钢管";
	else if(GetClassTypeId()==CLS_LINESLOT)
		return "槽钢";
	else if(GetClassTypeId()==CLS_LINEFLAT)
		return "扁铁";
	else if(GetClassTypeId()==CLS_PLATE)
		return "钢板";
	else if(GetClassTypeId()==CLS_PARAMPLATE)
		return "参数化钢板";
	else if(GetClassTypeId()==CLS_ARCANGLE)
		return "环形角钢";
	else if(GetClassTypeId()==CLS_ARCFLAT)
		return "环形扁铁";
	else if(GetClassTypeId()==CLS_ARCSLOT)
		return "环形槽钢";
	else if(GetClassTypeId()==CLS_SPHERE)
		return "球体";
	else if(GetClassTypeId()==CLS_LINEPART)
		return "杆件";
	else if(GetClassTypeId()==CLS_POLYPART)
		return "制弯杆件";
	else if(GetClassTypeId()==CLS_BOLT)
		return "螺栓";
	else if(GetClassTypeId()==CLS_SPHERE)
		return "球体";
#endif
	else
		return "";
}
//bAnalysisMode=true时根据size.idClassType判断，否则根据GetClassTypeId判断
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
//简化构件类型判断 wht 11-05-19
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
	//TODO:未完成
	CLDSPart *pMotherPart=MotherPart(false);
	CHashTable<f3dPoint*> hashVertexPtr;
	CBuffer buf(pMotherBody->BufferPtr(),pMotherBody->BufferLength());
	pSolidBody->ReadFrom(&buf,buf.GetLength());
	pSolidBody->TransACS(pMotherPart->ucs,ucs);
	/*CSolidBodyBuffer solidbuf(pSolidBody->BufferPtr(),pSolidBody->BufferLength());
	//全部顶点坐标转换
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
	//全部实体边中圆弧矢量参数转换
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
	//全部实体面中指定面法线参数转换
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
{	//基本属性
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
	if(IsAngle())	//存在合法焊接父构件或焊接属性为TRUE
		pBomPart->bWeldPart=((CLDSLineAngle*)this)->IsWeldPart();
	else 
		pBomPart->bWeldPart=(BelongModel()->FromPartHandle(m_hPartWeldParent)!=NULL);
	pBomPart->bLegPart=IsLegObj();
	if(strlen(sNotes)>0)
		strcpy(pBomPart->sNotes,sNotes);
	//螺栓数
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
	COLORREF crLsHole=CLDSPart::GetColor(CLS_BOLT,ftoi(pBolt->get_d()),' ');	//根据螺栓指定得到螺栓孔颜色
	DYN_ARRAY<f3dPoint> scan_point_arr(slices);
	double radius=0.5*(pBolt->get_d()+pBolt->hole_d_increment);
	CLDSPart::GetCircleSimuPolyVertex(radius,scan_point_arr,slices);
	double inner_coef=(radius-0.25)/radius;
	fBody body;
	for(int index=0;index<4;index++)
	{	//分别绘制每个构件上的螺栓孔
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
	solidPart.hiberid.SetHiberarchy(1,items[0].hRelaPart);	//设定关联构件，即孔壁宿主构件
	solidPart.m_idClassType = CLS_HOLEWALL;
	solidPart.acs = pBolt->ucs;
	solidPart.m_iLineType = 0;
	solidPart.pBody = pSolidBody;
	solidPart.dfSphereRadius=0;
	return solidPart;
}
//此函数目的是减少以下函数的参数过长问题，暂未启用 wjh-2011.7.18
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
			solidPart.m_bDisplaySingle=FALSE;	//不显示单线
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
			strcpy(error_info,"构件类型不同");
#endif
		return false;	//构件类型不一致
	}
	if(pPart==this)
		return true;	//pPart为当前构件不用比较
	if(pPart->MotherPart()==MotherPart())
		return true;	//影射母构件一致时不用比较
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
		if(pMirPart==NULL)	//删除不可用的关联对象
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
	//if(bNearLsCheck)	//检查附近螺栓
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
	{	//判断构件是否有脚钉不应考虑get_oldL()有值，old_L用来记录原螺栓的有效长度 wxc-2019.6.13
		if (!pLsRef->GetLsPtr()->IsFootNail())
			continue;
		//主材上有脚钉（含代孔脚钉，主要考虑插入基础的基础短主材）或
		//其余材上有新增独立脚钉（剔除斜材代孔情况）时识别为“带脚钉” wjh-2019.8.14
		if(this->chLayer2nd=='Z'||pLsRef->GetLsPtr()->get_oldL()<EPS)
			return TRUE;
	}
	return FALSE;
}
//根据构件上螺栓孔的先后顺序生螺栓句柄->孔索引标识(以1为基数)映射，返回螺栓孔数量 wjh-2016.1.05
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
	ATOM_LIST<RAYNO_RECORD>rayNoList;	//射线号记录列表
	CLsRef *pLsRef;
	CLDSBolt *pBolt;
	CLsRefList *pLsRefList=NULL;
	GEPOINT rayVec;
	if(GetClassTypeId()==CLS_LINEANGLE)
	{	//设置螺栓射线号时不需要考虑短角钢，否则容易导致螺栓射线数过多，
		//进而导致计算关联板螺栓引用射线号时超出32条射线 wht 15-04-21
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
			//	continue;	//不是以当前角钢为基准定位的螺栓
			double yCoord=0;
			f3dPoint prjBoltPos;	//螺栓在角钢肢上的投影位置
			if(GetClassTypeId()==CLS_LINEANGLE)
			{
				if(fabs(pBolt->ucs.axis_z*((CLDSLineAngle*)this)->get_norm_x_wing())>fabs(pBolt->ucs.axis_z*((CLDSLineAngle*)this)->get_norm_y_wing()))
				{	//X肢上的螺栓，Y值取负
					if(Int3dlf(prjBoltPos,pBolt->ucs.origin,pBolt->ucs.axis_z,((CLDSLineAngle*)this)->Start(),((CLDSLineAngle*)this)->get_norm_x_wing())>0)
						yCoord=-ftoi(wing_x_vec*(prjBoltPos-((CLDSLineAngle*)this)->Start()));
				}
				else
				{	//Y肢上的螺栓，Y值取正
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
					if(fabs(pRayNo->yCoord-yCoord)<0.5)	//0.5是根据四舍五入的原理给出的经验误差系数
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
					pBolt->SetRelaPartRayNo(handle,pRayNo->dwRayNo,rayVec);	//螺栓支持同时存储多个射线号 wjh 2011.8.11
					//pBolt->dwRayNo=pRayNo->dwRayNo;	//此处应进行螺栓射线号赋值 wht 11-07-28
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
		return false;	//非杆件构件且不在过滤线段区间
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
			{	//调整基点定位方式
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
			{	//以中点为拾取点判断该子段所在区段，否则在打断组合角钢时会导致父角钢有错误 wjh-2014.5.28
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
			{	//调换肢翼侧楞线侧 wjh-2016.5.31
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
	//hDatumLine  //基准线，仅在spatiaOperationStyle=16,17时有效，hDatumLine=0时表示定义基准点
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
		if(GetDesignItemValue('B',&handle))        //基准节点句柄 
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('C',&handle))
			pModel->ValidateObjRefH(&handle);       //基准钢管句柄
	}
	else if (m_iParamType==TYPE_CIRRIBPLATE||m_iParamType==TYPE_TUBERIBPLATE)//环向肋板, 钢管肋板
	{
		if(GetDesignItemValue(KEY2C('C'),&handle)) //C:定位基准钢管(球体)句柄(定位参数)
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('E'),&handle)) //E:第一块关联基准钢板句柄
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('F'),&handle)) //F:第二块关联基准钢板句柄，或焊接关联钢管句柄(关联钢管是指与环向板有焊缝的钢管，不含上面的插板螺栓连接钢管，因此不可能出现两根)
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('I',&handle))		   //I:第一根连接方向钢管句柄(指明有此钢管方向的连接意图，即插板螺栓连接或无连接制弯)
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('J',&handle))		   //J:第二根连接方向钢管句柄(指明有此钢管方向的连接意图，即插板螺栓连接或无连接制弯)
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('P',&handle))		   //P:定位基准节点句柄
			pModel->ValidateObjRefH(&handle);
	}
	else if (m_iParamType==TYPE_CIRCOVERPLATE) //环向封板
	{
		if(GetDesignItemValue(KEY2C('E'),&handle))	//定位基准节点
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('F'),&handle))	//第一块定位基准钢板
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('G'),&handle))	//第二块定位基准钢板
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('C'),&handle))	//第二块定位基准钢板
			pModel->ValidateObjRefH(&handle);
	}
	else if(m_iParamType==TYPE_CIRRIBPLATE)   //环向封板
	{
		if(GetDesignItemValue(KEY2C('A'),&handle))	//定位基准节点
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('B'),&handle))	//第一块定位基准钢板
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('C'),&handle))	//第二块定位基准钢板
			pModel->ValidateObjRefH(&handle);
	}
	else if(m_iParamType==TYPE_SHOEPLATE)      //塔脚靴板
	{
		if(GetDesignItemValue(KEY2C("LX"),&handle))	//连接角钢
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C("LH"),&handle))	//连接角钢
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('C',&handle))	        //连接底板
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('D',&handle))	        //依赖角钢
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('G',&handle))         //连接靴板
			pModel->ValidateObjRefH(&handle);
	}
	else if (m_iParamType==TYPE_TUBERIBPLATE)//钢管肋板
	{
		if(GetDesignItemValue('S',&handle))		    //Ｘ轴定向基准钢管
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('P',&handle))	        //基准节点
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('B',&handle))		    //第一块定位基准钢板
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('C',&handle))         //第二块定位基准钢板
			pModel->ValidateObjRefH(&handle);
	}
	else if (m_iParamType==TYPE_BASEPLATE)//塔脚底板
	{
		if(GetDesignItemValue('A',&handle))	//定位基准节点
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue(KEY2C('L'),&handle)) // hBaseAngle 基准角钢
			pModel->ValidateObjRefH(&handle);
	}
	else if(m_iParamType==TYPE_ELBOWPLATE)
	{
		if(GetDesignItemValue('L',&handle))
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('K',&handle))
			pModel->ValidateObjRefH(&handle);
	}
	else if(m_iParamType==TYPE_RIBPLATE)//普通角肋板
	{
		if(GetDesignItemValue('A',&handle))	    //基准构件
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('B',&handle))		//基准节点
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('C',&handle))		//辅助构件1
			pModel->ValidateObjRefH(&handle);
		if(GetDesignItemValue('D',&handle))		//辅助构件2
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
	//pModel->ValidateObjRefH();没找到
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
bool CLDSPart::ModifyPartNoSegI(SEGI newSegI)	//更新构件编号中的段号部分
{
	BOOL bStdPart=FALSE;
	if(GetClassTypeId()==CLS_PARAMPLATE)
		bStdPart=((CLDSParamPlate*)this)->m_bStdPart;
	CXhChar16 sSegNo = _iSeg.ToString();
	iSeg=newSegI;
	if(bStdPart||newSegI.iSeg==0)	//不能根据段号更新标准构件的件号 wht 15-07-13
		return false;	//新段号为0时保持原有件号 wht 16-04-07
	if(strstr(sPartNo,sSegNo)==(char*)sPartNo)
	{
		CXhChar16 serialStr("%s",&sPartNo[sSegNo.GetLength()]);
		sPartNo.Printf("%s%s",(char*)newSegI.ToString(),(char*)serialStr);
		return true;
	}
	return false;
}

//ancestor=true时返回始祖级构件，否则返回直接父构件（如对称时）
CLDSPart* CLDSPart::MotherPart(bool ancestor/*=true*/)
{
	CLDSObject *pMotherObj=MotherObject(ancestor);
	if(pMotherObj==NULL||pMotherObj->GetClassTypeId()!=m_nClassTypeId)
#ifdef AFX_TARG_ENU_ENGLISH
		throw "The part type of projective mother part is wrong！";
#else 
		throw "影射构件母本构件类型错误！";
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
			if(pMotherPart)	//用以自理换腿处同位不同腿的连接构件复用 wjh-2015.7.23
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
// CLDSCable 类
CLDSCable::CLDSCable()
{
}
BOOL CLDSCable::Create3dSolidModel(BOOL bDisplayHole/*=FALSE*/,double scale_of_user2scr/*=1.0*/, double sample_len/*=5.0*/,int smoothness/*=36*/)
{
	return TRUE;
}
//////////////////////////////////////////////////////////////
// CLDSDelegateRod 类
CLDSDelegateRod::CLDSDelegateRod()
{
	//strcpy(m_sClassName,"");
	m_nClassTypeId=CLS_DELEGATEROD;
}
//////////////////////////////////////////////////////////////
// CLDSLinePart 类
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
	dwPermission=dwStartPermission=dwEndPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
	_material=CSteelMatLibrary::Q235BriefMark();//'S';
	crMaterial=GetColor(CLS_LINEPART,3,'S');	//构件颜色
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
	//此处不能用memset,因为其内有虚函数,否则中会冲掉此虚函数指针
	//memset(&CalLenCoef,0,sizeof(POLE_CALLEN_COEF));
	CalLenCoef.minR.coef=CalLenCoef.paraR.coef=1.0;
	CalLenCoef.hHorizPole=CalLenCoef.hRefPole=0;
	CalLenCoef.iTypeNo=CalLenCoef.iSelRFlag=0;
	m_uStatMatNo = 0;
	m_fWeightRaiseCoef=1.2;
	m_iElemType=0;	//自动判断单元类型
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
	//Ahf=;		//螺栓有效承剪面积,孔壁承压面积
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
bool CLDSLinePart::IntelliUpdateConnBoltsBySummBolts()	//根据螺栓计算数智能更新(单肢)连接数
{
	WORD* pwnSummBoltCount=((WORD*)&connectStart.uiLegacyN)+1;
	if(*pwnSummBoltCount!=0)
	{
		WORD odd=0;
		if(this->GetClassTypeId()==CLS_GROUPLINEANGLE)
		{
			CLDSGroupLineAngle* pGroupAngle=(CLDSGroupLineAngle*)this;
			if(pGroupAngle->wSectStyle<2)	//对角或T型组合角钢
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
			{	//T型组合角钢一般认为是单肢连接，其余认为是双肢连接
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
			if(pGroupAngle->wSectStyle<2)	//对角或T型组合角钢
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
			{	//T型组合角钢一般认为是单肢连接，其余认为是双肢连接
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
BOOL CLDSLinePart::IsCfgWordCompatible(CFGWORD filter)	//判断端节点与杆件本身配材号是否一致
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
{	//杆件借用角钢类型的默认同步设定
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
		Log2File()->Log("%s同步对称属性项缺失",propKeyStr);
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
	{	//材质
		pRod->cMaterial=cMaterial;
		pRod->cbVirginQualityLevel=cbVirginQualityLevel;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"specification")==0)&&IsSyncMirPropWith(pDestObj,"specification"))
	{	//规格
		pRod->size_wide=size_wide;
		pRod->size_thick=size_thick;
		pRod->size_idClassType=size_idClassType;
		pRod->size_cSubClassType=size_cSubClassType;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"iSeg")==0)&&IsSyncMirPropWith(pDestObj,"iSeg"))
	{	//分段号
		pRod->iSeg=iSeg;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cfgword")==0)&&IsSyncMirPropWith(pDestObj,"cfgword"))
	{	//配材号
		pRod->cfgword=cfgword;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"crMaterial")==0)&&IsSyncMirPropWith(pDestObj,"crMaterial"))
	{	//构件颜色
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
	{	//连接螺栓信息
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
	{	//图层名同步对称
		MirObjLayer(layer(),pRod->layer(),mirmsg);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cMaterial")==0)&&IsSyncMirPropWith(pDestObj,"cMaterial"))
	{	//材质
		pRod->cMaterial=cMaterial;
		pRod->cbVirginQualityLevel=cbVirginQualityLevel;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"specification")==0)&&IsSyncMirPropWith(pDestObj,"specification"))
	{	//规格
		pRod->size_wide=size_wide;
		pRod->size_thick=size_thick;
		pRod->size_idClassType=size_idClassType;
		pRod->size_cSubClassType=size_cSubClassType;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"iSeg")==0)&&IsSyncMirPropWith(pDestObj,"iSeg"))
	{	//分段号
		pRod->iSeg=iSeg;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"sPartNo")==0)&&IsSyncMirPropWith(pDestObj,"sPartNo"))
	{	//构件编号
		pRod->SetPartNo(sPartNo);
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"cfgword")==0)&&IsSyncMirPropWith(pDestObj,"cfgword"))
	{	//配材号
		pRod->cfgword=cfgword;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"m_bDisplayHole")==0)&&IsSyncMirPropWith(pDestObj,"m_bDisplayHole"))
	{	//显示孔
		pRod->m_bDisplayHole=m_bDisplayHole;
	}
	if((propKeyStr==NULL||strcmp(propKeyStr,"crMaterial")==0)&&IsSyncMirPropWith(pDestObj,"crMaterial"))
	{	//构件颜色
		pRod->crMaterial=crMaterial;
	}
	bool reverseStartEnd=false;
	if(pStart && pRod->pEnd)
	{
		f3dPoint pos=pStart->Position(false);
		f3dPoint mirpos=pRod->pEnd->Position(false);
		f3dPoint destpos=mirmsg.MirPoint(pos);
		if(mirpos.IsEqual(destpos))
			reverseStartEnd=true;	//对称角钢需要颠倒始末端
	}
	else if(pEnd && pRod->pStart)
	{
		f3dPoint pos=pEnd->Position(false);
		f3dPoint mirpos=pRod->pStart->Position(false);
		f3dPoint destpos=mirmsg.MirPoint(pos);
		if(mirpos.IsEqual(destpos))
			reverseStartEnd=true;	//对称角钢需要颠倒始末端
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
	{	//连接螺栓信息
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
bool CLDSLinePart::SyncMirProp(const char* propKeyStr/*=NULL*/,BYTE ciMirAll0X1Y2Z3/*=0*/,DWORD dwPropFilterFlag/*=0xffffffff*/)//propKeyStr表示同步对称全部属性
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
			strcpy(sText,"默认");
#endif
	}
	else if(GetPropID("iSeg")==id)
		strcpy(sText,(char*)iSeg.ToString());
	else if(GetPropID("sPartNo")==id)
		strcpy(sText,GetPartNo());
	else if(GetPropID("cfgword")==id)
		//return MakeWordToString(cfgword,valueStr);
	{
		if(layer(0)=='L')	//接腿构件
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
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
	else if(GetPropID("m_iElemType")==id)	//自动判断单元类型
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(size_idClassType==CLS_CABLE)
			strcpy(sText,"3.Wire Cable");
		else if(m_iElemType==0)
			strcpy(sText,"0.Default");
		else if(m_iElemType==1)
			strcpy(sText,"1.二力杆");
		else if(m_iElemType==2)
			strcpy(sText,"2.Classical Beam");
		else
			strcpy(sText,"1.二力杆");
#else 
		if(size_idClassType==CLS_CABLE)
			strcpy(sText,"3.线缆");
		else if(m_iElemType==0)
			strcpy(sText,"0.自动判断");
		else if(m_iElemType==1)
			strcpy(sText,"1.二力杆");
		else if(m_iElemType==2)
			strcpy(sText,"2.经典梁");
		else
			strcpy(sText,"1.二力杆");
#endif
	}
	else if(GetPropID("sel_mat_method")==id)	//自动判断单元类型
	{
		if(!IsAuxPole())
			strcpy(sText,"");
		else if(sel_mat_method==AUX_AUTODESIGN)
			strcpy(sText,"自动设计");
		else if(sel_mat_method==AUX_BY_START)
			strcpy(sText,"依据始端");
		else if(sel_mat_method==AUX_BY_END)
			strcpy(sText,"依据终端");
		else if(sel_mat_method==AUX_ONLY_PRIMARY)
			strcpy(sText,"依据斜材");
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
			strcpy(sText, "等肢角钢");
		else if(size_idClassType==CLS_GROUPLINEANGLE)
		{
			if(size_cSubClassType=='T')
				strcpy(sText, "T型组合");
			else if(size_cSubClassType=='D')
				strcpy(sText, "对角组合");
			else if(size_cSubClassType=='X')
				strcpy(sText, "十字组合");
		}
		else if(size_idClassType==CLS_LINETUBE)
			strcpy(sText, "钢管");
		else if(size_idClassType==CLS_LINEFLAT)
			strcpy(sText, "扁钢");
		else if(size_idClassType==CLS_LINESLOT)
			strcpy(sText, "槽钢");
		else if(size_idClassType==CLS_CABLE)
			strcpy(sText, "线缆");
		else if(size_idClassType==0)
			strcpy(sText, "用户定制");
#endif
	}
	else if(GetPropID("m_sWireType")==id)
		strcpy(sText,GetWireType());
	else if(GetPropID("m_sWireSpec")==id)
		strcpy(sText,GetSpec());
	else if(GetPropID("m_fWireArea")==id)	//线缆截面积
	{
		if(library)
			sprintf(sText,"%f",GetWireArea());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fUnitWeight")==id)	//单位长重量
	{
		if(library)
			sprintf(sText,"%f",GetWireUnitWeight());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fForcePermit")==id)	//额定抗拉力
	{
		if(library)
			sprintf(sText,"%f",GetWireForcePermit()/1000);	//N->kN
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fPreTension")==id)//拉线锚固端安装预张力
	{
		sprintf(sText,"%f",m_fPreTension/1000);	//N->kN
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fElasticCoef")==id)	//弹性系数(模量)
	{
		sprintf(sText,"%f",GetWireElasticCoef());
		SimplifiedNumString(sText);
	}
	else if(GetPropID("m_fExpansionCoef")==id)//温度线膨胀系数
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
			strcpy(sText,"截面常数");
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
			strcpy(sText,"1.二力杆");
		else if(m_iElemType==2)
			strcpy(sText,"2.Classical Beam");
		else if(m_iElemType==3)
			strcpy(sText,"3.柔索单元");
#else 
		if(m_iElemType==0)
			strcpy(sText,"0.自动判断");
		else if(m_iElemType==1)
			strcpy(sText,"1.二力杆");
		else if(m_iElemType==2)
			strcpy(sText,"2.经典梁");
		else if(m_iElemType==3)
			strcpy(sText,"3.柔索单元");
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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
			strcpy(sText,"0.智能判断");
		else if(CalLenCoef.iTypeNo==1)
			strcpy(sText,"1.V斜材(跨脚材连接)");
		else if(CalLenCoef.iTypeNo==2)
			strcpy(sText,"2.V斜材(V面连接)");
		else if(CalLenCoef.iTypeNo==3)
			strcpy(sText,"3.V斜材3节间带横连杆");
		else if(CalLenCoef.iTypeNo==4)
			strcpy(sText,"4.V斜材4节间带横连杆");
		else if(CalLenCoef.iTypeNo==6)
			strcpy(sText,"6.指定交叉材计算长度");
		else //if(CalLenCoef.iTypeNo==5)
			strcpy(sText,"5.指定杆件计算长度");
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
			strcpy(sText,"铰接");
		else if(start_joint_type==JOINT_RIGID)
			strcpy(sText,"刚接");
		else //if(start_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"自动判断");
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
			strcpy(sText,"中心受力");
		else //if(start_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"偏心受力");
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
			strcpy(sText,"铰接");
		else if(end_joint_type==JOINT_RIGID)
			strcpy(sText,"刚接");
		else //if(end_joint_type==JOINT_AUTOJUSTIFY)
			strcpy(sText,"自动判断");
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
			strcpy(sText,"中心受力");
		else //if(end_force_type==ECCENTRIC_FORCE)
			strcpy(sText,"偏心受力");
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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
		Log2File()->Log("杆件0X%X属性#%d字符串超出长度限制",handle,id);
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
		/*CString prompt="没找到名为\"";
		prompt+=propStr;
		prompt+="\"的属性ID！";
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
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
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
	if(IsStartPosLocked())	//终端位置已被锁定
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
	if(IsEndPosLocked())	//始端位置已被锁定
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

//0.完全对称一致;1.除角钢肢朝向外位置对称一致;2.拾取点处射线同向对称;3.拾取点处射线异向对称
BYTE CLDSLinePart::MirSimilarGradeWith(CLDSPart *pPart,const MIRMSG& mirmsg,const double *pPickPos/*=NULL*/)
{
	BOOL bSingleLineCoord=FALSE;
	if(pStart&&pEnd)
		bSingleLineCoord=TRUE;
	if(pPart==NULL || GetClassTypeId()!=pPart->GetClassTypeId())
		return 255;	//类型不同
	CLDSLinePart *pLinePart=(CLDSLinePart*)pPart;
	f3dLine line,mirLine;
	double INTER_EPS_COS=0.99999,internal_eps=0.00001;	//有些情况单线点也不是非常严格对称，尤其是一些计算模型 wjh-2019.11.23
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
		internal_eps=0.5;	//实体坐标精确到0.5mm就足够了
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
		return 255;	//矢量方向不对称
	else if(!bSingleLineCoord&&(fabs(justify)<EPS_COS2||mirLine.PtInLine(mirStartPoint,EPS2)==0))
		return 255;	//矢量方向不对称
	BYTE similarGrade=255;
	if(pPickPos)	//指定对称拾取点
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
			{	//sectlen_start与sectlen_end同号且最小长度大于1米时，pickPos取值不合理
				//（如：横担除挂线短角钢,pickpos一般默认取值（0，0，0）,两角钢基本平行时会放大误差）。
				//此时pickPos需要重新赋值（取杆件中点为拾取点）再次计算 wht 19-09-18
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
		{	//对称方向相同
			mirsectlen_start=(mirLine.startPt-perp)*line_vec;
			mirsectlen_end  =(mirLine.endPt  -perp)*line_vec;
		}
		else
		{	//对称方向相反
			mirsectlen_start=((mirLine.endPt  -perp)*line_vec);
			mirsectlen_end  =((mirLine.startPt-perp)*line_vec);
		}
		if(mirStartPoint.IsEqual(mirPickPos)&&(mirsectlen_start>internal_eps||mirsectlen_end<-internal_eps))
			return 255;	//始端点当前连接端，对称线与被验证杆件在拾取点处无重合
		else if(mirEndPoint.IsEqual(mirPickPos)&&(mirsectlen_start>internal_eps||mirsectlen_end<-internal_eps))
			return 255;	//终端点当前连接端，对称线与被验证杆件在拾取点处无重合
		else if(fabs(sectlen_end-mirsectlen_start)<internal_eps||fabs(sectlen_start-mirsectlen_end)<internal_eps)
			return 3;	//端点反向对接
		else if(sectlen_end<mirsectlen_start||sectlen_start>mirsectlen_end)
			return 255;	//无任何交集
		else	//有交集，且拾取点不是端点时按射线同向对称处理
			similarGrade=2;
	}
	//进行始终端比对
	f3dPoint mirStartPos=GetMirPos(line.startPt,mirmsg);
	f3dPoint mirEndPos=GetMirPos(line.endPt,mirmsg);
	if( (mirStartPos.IsEqual(mirLine.startPt,internal_eps)&&mirEndPos.IsEqual(mirLine.endPt,internal_eps))||
		(mirStartPos.IsEqual(mirLine.endPt,internal_eps)&&mirEndPos.IsEqual(mirLine.startPt,internal_eps)))
	{
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		if(GetClassTypeId()!=CLS_LINEANGLE)
			similarGrade=0;	//其余杆件不存在肢朝向的问题
		else
		{
			similarGrade=1;	//位置坐标对称
			//对组合角钢子角钢应根据法线对称判断
			CLDSLineAngle* pLineAngle=(CLDSLineAngle*)this;
			CLDSLineAngle* pMirLineAngle=(CLDSLineAngle*)pPart;
			f3dPoint norm_x_wing=pLineAngle->get_norm_x_wing();
			f3dPoint norm_y_wing=pLineAngle->get_norm_y_wing();
			f3dPoint mir_norm_x_wing=GetMirVector(norm_x_wing,mirmsg);
			f3dPoint mir_norm_y_wing=GetMirVector(norm_y_wing,mirmsg);
			if( (pMirLineAngle->get_norm_x_wing()*mir_norm_x_wing>EPS_COS2&&pMirLineAngle->get_norm_y_wing()*mir_norm_y_wing>EPS_COS2)||
				(pMirLineAngle->get_norm_x_wing()*mir_norm_y_wing>EPS_COS2&&pMirLineAngle->get_norm_y_wing()*mir_norm_x_wing>EPS_COS2))
				similarGrade=0;		//是完全对称的组合子角钢学习
			else if(GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLineAngle)->group_father_jg_h>0x20)
				similarGrade=255;	//组合角钢子角钢要么完全对称,要么不对称,不存在只位置对称情况
			else if(!bSingleLineCoord)
				similarGrade=255;	//实体角钢对称也不存在只位置对称的情况
		}
		return similarGrade;
#else
		return 0;
#endif
	}
	else if(!pPickPos)	//未指定对称拾取点时，杆件必须完全对称才认为具有对称关联 wjh-2015.6.07
		return 255;
	else if(GetClassTypeId()!=CLS_LINEANGLE)
		return similarGrade;	//其余杆件不存在肢朝向的问题
	else
	{
		//对组合角钢子角钢应根据法线对称判断
		CLDSLineAngle* pLineAngle=(CLDSLineAngle*)this;
		CLDSLineAngle* pMirLineAngle=(CLDSLineAngle*)pPart;
		f3dPoint norm_x_wing=pLineAngle->get_norm_x_wing();
		f3dPoint norm_y_wing=pLineAngle->get_norm_y_wing();
		f3dPoint mir_norm_x_wing=GetMirVector(norm_x_wing,mirmsg);
		f3dPoint mir_norm_y_wing=GetMirVector(norm_y_wing,mirmsg);
		if( (pMirLineAngle->get_norm_x_wing()*mir_norm_x_wing>EPS_COS2&&pMirLineAngle->get_norm_y_wing()*mir_norm_y_wing>EPS_COS2)||
			(pMirLineAngle->get_norm_x_wing()*mir_norm_y_wing>EPS_COS2&&pMirLineAngle->get_norm_y_wing()*mir_norm_x_wing>EPS_COS2))
			similarGrade=1;		//肢朝向对称
		else if(GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pLineAngle)->group_father_jg_h>0x20)
			similarGrade=255;	//组合角钢子角钢要么完全对称,要么不对称,不存在只位置对称情况
		else if(!bSingleLineCoord)
			similarGrade=255;	//实体角钢对称也不存在只位置对称的情况
		return similarGrade;
	}
}

CLDSLinePart *CLDSLinePart::GetMirRod(MIRMSG mirmsg,double epsilong/*=EPS*/)
{
	CLDSLinePart *pMirRod=NULL;
	if(pStart==NULL||pEnd==NULL)	//只有始终关联节点都存在时才采用单线坐标
		return NULL;
	f3dPoint mir_start=mirmsg.MirPoint(pStart->Position(false));
	f3dPoint mir_end=mirmsg.MirPoint(pEnd->Position(false));
	if(mir_start.IsEqual(pEnd->Position(false),epsilong)&&mir_end.IsEqual(pStart->Position(false),epsilong))
		return this;	//杆件始终端节点互相对称,对称的杆件就是自己
	CLDSLinePart *pLastMirLinePart=NULL;
	for(RELATIVE_OBJECT *pItem=relativeObjs.GetFirst();pItem;pItem=relativeObjs.GetNext())
	{	//优先在关联构件列表中查找对称度小于等于3的关联构件返回
		pMirRod=NULL;
		if(pItem->mirInfo.IsSameMirMsg(mirmsg))
		{	//判断根据关联构件列表找到的对称杆件是否合法(主要时在启用一端对称时可能出错) wht 09-12-06
			pMirRod=(CLDSLinePart*)BelongModel()->FromPartHandle(pItem->hObj,GetClassTypeId());
			if(pMirRod==NULL||pMirRod->pStart==NULL||pMirRod->pEnd==NULL)
				continue;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			if( pMirRod->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pMirRod)->group_father_jg_h>0x20)
				continue;	//跳过组合角钢的子角钢
			if( pMirRod->GetClassTypeId()!=this->m_nClassTypeId)
				continue;	//构件类型不一致，防止组合角钢的子角钢与组合角钢间产生对称关联 wjh-2019.1.16
#endif
			if( (mir_start.IsEqual(pMirRod->pStart->Position(false),epsilong)&&mir_end.IsEqual(pMirRod->pEnd->Position(false),epsilong))||
				(mir_start.IsEqual(pMirRod->pEnd->Position(false),epsilong)&&mir_end.IsEqual(pMirRod->pStart->Position(false),epsilong)))
				return pMirRod;
		}
	}

	CLdsPartListStack stack(m_pModel);	//原代码在for循环中退出时易疏乎弹出栈　wjh-2017.1.22
	for(pMirRod=m_pModel->EnumRodFirst();pMirRod;pMirRod=m_pModel->EnumRodNext())
	{
		if(!pMirRod->cfgword.And(cfgword))
			continue;
		if(pMirRod==NULL||pMirRod->pStart==NULL||pMirRod->pEnd==NULL)
			continue;
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
		if( pMirRod->GetClassTypeId()==CLS_LINEANGLE&&((CLDSLineAngle*)pMirRod)->group_father_jg_h>0x20)
			continue;	//跳过组合角钢的子角钢
		if( pMirRod->GetClassTypeId()!=this->m_nClassTypeId)
			continue;	//构件类型不一致，防止组合角钢的子角钢与组合角钢间产生对称关联 wjh-2019.1.16
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
	//0.完全对称一致;1.除肢朝向外对称一致;2.拾取点处射线同向对称;3.拾取点处射线异向对称
	BYTE similarGrade=255;
	BOOL bSingleLineCoord=TRUE;
	if(pStart==NULL||pEnd==NULL)	//只有始终关联节点都存在时才采用单线坐标
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
			return this;	//杆件始终端节点互相对称,对称的杆件就是自己
		//else	组合角钢的子角钢间也有可能存在对称关系
	}
	else if((pStart==NULL||pEnd==NULL)&&MirSimilarGradeWith(this,mirmsg,pPickPos)==0)
		return this;	//处理短角钢自对称 wht 16-10-31
	CLDSLinePart *pLastMirLinePart=NULL;
	for(RELATIVE_OBJECT *pItem=relativeObjs.GetFirst();pItem;pItem=relativeObjs.GetNext())
	{	//优先在关联构件列表中查找对称度小于等于3的关联构件返回
		pMirLinePart=NULL;
		if(pItem->mirInfo.IsSameMirMsg(mirmsg))
		{	//判断根据关联构件列表找到的对称杆件是否合法(主要时在启用一端对称时可能出错) wht 09-12-06
			pMirLinePart=(CLDSLinePart*)BelongModel()->FromPartHandle(pItem->hObj,GetClassTypeId());
			BYTE similar=MirSimilarGradeWith(pMirLinePart,mirmsg,pPickPos);
			if(similar<similarGrade)
			{
				pLastMirLinePart=pMirLinePart;
				similarGrade=similar;
			}
			if(similar>1)
				continue;	//完全不对称
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
			//组合角钢子角钢必须含实体在内完全一致 wjh-2011.8.9
			if( pMirLinePart&&GetClassTypeId()==CLS_LINEANGLE&&//pMirLinePart->m_bVirtualPart&&
				((CLDSLineAngle*)pMirLinePart)->group_father_jg_h>0x20&&similar>0)
				continue;	//通过关联构件列表得到的对称构件为虚拟子角钢时应通过对称查找对称构件 wht 11-08-04
#endif
			return pMirLinePart;
		}
	}

	CLdsPartListStack stack(m_pModel);	//原代码在for循环中退出时易疏乎弹出栈　wjh-2017.1.22
	int cls_typeid=GetClassTypeId();
	for(pMirLinePart=(CLDSLinePart*)m_pModel->EnumPartFirst(cls_typeid);pMirLinePart;pMirLinePart=(CLDSLinePart*)m_pModel->EnumPartNext(cls_typeid))
	{
		if(!ModuleTogetherWith(pMirLinePart))
			continue;
		//pAngleSelf!=NULL表示组合角钢子角钢自对称的情况，需根据位置进行对称,
		//如:包含横担挂点组合角钢的节点板对称时子角钢对称杆件为自己 wht 15-08-11
		if(pMirLinePart==this&&pAngleSelf==NULL)
			continue;	//对称杆件为自己的情况已在上边处理过
		BYTE similar=MirSimilarGradeWith(pMirLinePart,mirmsg,pPickPos);
		if(similar<similarGrade)
		{
			similarGrade=similar;
			if(similar==0)	//已找到完全对称一致的杆件,就不在继续查找
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
	{	//比较始端
		if(pStart)
		{	//原始杆件始节点不为空
			if(pStartMirNode.IsNULL())
				continue;	//为空
			else if(!pStartMirNode.IsEqualPtr(pMirLinePart->pStart)&&!pStartMirNode.IsEqualPtr(pMirLinePart->pEnd))
				continue;	//不为空但不一致
		}
		else
		{	//原始杆件始节点即为空,比较起点坐标是否一致
			if(mir_start!=pMirLinePart->Start()&&mir_start!=pMirLinePart->End())
				continue;
		}
	}
	if(scope_start0_end1_elseall!=0)
	{	//比较终端
		if(pEnd)
		{	//原始杆件始节点不为空
			if(pEndMirNode.IsNULL())
				continue;	//为空
			else if(!pEndMirNode.IsEqualPtr(pMirLinePart->pEnd)&&!pEndMirNode.IsEqualPtr(pMirLinePart->pStart))
				continue;	//为空但不一致
		}
		else
		{	//原始杆件始节点即为空,比较起点坐标是否一致
			if(mir_end!=pMirLinePart->End()&&mir_end!=pMirLinePart->Start())
				continue;
		}
	}
	if(scope_start0_end1_elseall==0||scope_start0_end1_elseall==1)
	{	//只比较始端或终端
		f3dPoint new_line_vec,mir_line_vec;
		//对称杆件始端对应源杆件始端或终端对应终端 杆件延伸方向应与源杆件对称方向相同,反之相反 wht 09-12-22
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
		//应该将由原杆件方向求得的对称方向保存到临时变量中，不应保存到原杆件方向中
		mir_line_vec=GetMirVector(line_vec,mirmsg);
		if(mir_line_vec*new_line_vec<EPS_COS)
			continue;
	}*/
}

void CLDSLinePart::GetMirLinePartsByPos(MIRMSG mirmsg,LINEPARTSET &mirRodSet)
{
	if(pStart==NULL||pEnd==NULL)
		return;
	BOOL pushed=m_pModel->PushPartStack(); //遍历构件列表前应先压栈 wht 12-03-05
	int cls_typeid=GetClassTypeId();
	CLDSLinePart *pMirLinePart=NULL;
	for(pMirLinePart=(CLDSLinePart*)m_pModel->EnumPartFirst(cls_typeid);pMirLinePart;pMirLinePart=(CLDSLinePart*)m_pModel->EnumPartNext(cls_typeid))
	{
		if(!pMirLinePart->cfgword.And(cfgword))
			continue;
		if(pMirLinePart==this)
			continue;	//对称杆件为自己的情况已在上边处理过
		if(pMirLinePart->pStart==NULL||pMirLinePart->pEnd==NULL)
			continue;
		f3dPoint line_vec=pMirLinePart->pEnd->Position(false)-pMirLinePart->pStart->Position(false);
		normalize(line_vec);
		int n=3;
		if(mirmsg.axis_flag&0x08)
			n=7;	//查找旋转对称及XYZ旋转对称杆件
		for(int i=1;i<=n;i++)
		{
			MIRMSG msg;
			f3dPoint mir_start=pStart->Position(false),mir_end=pEnd->Position(false);
			if(i>3)
			{	//旋转对称及XYZ旋转对称
				if(i>4)
				{	//先计算关于XYZ对称以后的坐标，然后再进行旋转对称
					msg=mirmsg.GetSubMirItem(i-4);
					mir_start=GetMirPos(mir_start,msg);
					mir_end=GetMirPos(mir_end,msg);
				}
				msg.axis_flag=0x08;
				msg.rotate_angle=90;
			}
			else //XYZ对称
				msg=mirmsg.GetSubMirItem(i);
			mir_start=GetMirPos(mir_start,msg);
			mir_end=GetMirPos(mir_end,msg);
			f3dPoint mir_line_vec=mir_end-mir_start;
			normalize(mir_line_vec);
			double justify=line_vec*mir_line_vec;
			if(fabs(justify)<EPS_COS)
				continue;	//矢量方向不对称
			//进行始终端比对
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
	{	//角钢
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
				if(pDesStartPos->endPosType==0		//端点即节点
					||pDesStartPos->endPosType==1	//搭接在节点父钢管杆件上
					||pDesStartPos->endPosType==3)	//杆件心线交点
					start -= workNorm*pDesStartPos->fNormOffset;
			}
			end=baseline.End();
			if(fabs(pDesEndPos->fNormOffset)>0)
			{	
				if(pDesEndPos->endPosType==0		//端点即节点
					||pDesEndPos->endPosType==1		//搭接在节点父钢管杆件上
					||pDesEndPos->endPosType==3)	//杆件心线交点
					end -= workNorm*pDesEndPos->fNormOffset;
			}
		}
	}
	f3dPoint vec=end-start;
	return vec.mod();*/
}
//对称杆件规格、材质、基准长度
bool CLDSLinePart::IsEqualWith(CLDSLinePart *pLinePart,double epsilon/*=1.0*/,char *error_info/*=NULL*/,
							   int *pErrorInfo/*=NULL*/,DWORD *pChkStyleFlag/*=NULL*/)
{	
	if(pChkStyleFlag==NULL||(*pChkStyleFlag)&CHK_MIRROD_BASICINFO)
	{	
		bool bRet=true;
		//比较杆件类型
		if(pLinePart->GetClassTypeId()!=GetClassTypeId())
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"The rod type is different between 0x%X and 0x%X！",handle,pLinePart->handle);
#else 
				sprintf(error_info,"0x%X与0x%X杆件类型不同！",handle,pLinePart->handle);
#endif
			bRet=false;
		}
		//比较杆件规格
		else if(fabs(pLinePart->GetWidth()-GetWidth())>eps||fabs(pLinePart->GetThick()-GetThick())>eps)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"The rod specification is different between 0x%X and 0x%X！",handle,pLinePart->handle);
#else 
				sprintf(error_info,"0x%X与0x%X杆件规格不同！",handle,pLinePart->handle);
#endif
			bRet=false;
		}
		//比较杆件材质
		else if(pLinePart->cMaterial!=cMaterial)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"The rod material is different between 0x%X and 0x%X！",handle,pLinePart->handle);
#else 
				sprintf(error_info,"0x%X与0x%X杆件材质不同！",handle,pLinePart->handle);
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
		//比较杆件基准长度，暂时没有实现
		if(fabs(pLinePart->GetDatumLength()-GetDatumLength())>epsilon)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"The rod length is different between 0x%X and 0x%X！",handle,pLinePart->handle);
#else 
				sprintf(error_info,"0x%X与0x%X杆件基准长度不同！",handle,pLinePart->handle);
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
{	//替换法向偏移量字符串中的句柄
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
	else if(datum_pos_style==3)	//角钢心线交点
	{
		if(idClassType==0||idClassType==CLS_LINEANGLE||idClassType==CLS_LINETUBE
			||idClassType==CLS_LINEFLAT||idClassType==CLS_LINESLOT)
		{
			replaced|=replacer.VerifyAndReplace(&des_para.AXIS_INTERS.hDatum1);
			replaced|=replacer.VerifyAndReplace(&des_para.AXIS_INTERS.hDatum2);
			replaced|=ReplaceReferenceHandleOfNormOffset(des_para.AXIS_INTERS.norm_offset,replacer);
		}
	}
	else if(datum_pos_style==4)	//基准线空间交点
	{
		if(idClassType==0||idClassType==CLS_GELINE)
		{
			replaced|=replacer.VerifyAndReplace(&des_para.AXIS_INTERS.hDatum1);
			replaced|=replacer.VerifyAndReplace(&des_para.AXIS_INTERS.hDatum2);
			replaced|=ReplaceReferenceHandleOfNormOffset(des_para.AXIS_INTERS.norm_offset,replacer);
		}
	}
	else if(datum_pos_style==5)	//基准线上轴向坐标不变点
	{
		if(idClassType==0||idClassType==CLS_GELINE)
			replaced|=replacer.VerifyAndReplace(&des_para.DATUMLINE.hDatumLine);
	}
	else if(datum_pos_style==6)	//基准面上指定轴向点
	{
		if(idClassType==0||idClassType==CLS_GELINE)
			replaced|=replacer.VerifyAndReplace(&des_para.DATUMPLANE.hDatumPlane);
	}
	else if(datum_pos_style==7)	//角钢内外包铁楞点
	{
		if(idClassType==0||idClassType==CLS_LINEANGLE)
		{
			replaced|=replacer.VerifyAndReplace(&des_para.RODEND.hRod);
			replaced|=ReplaceReferenceHandleOfNormOffset(des_para.RODEND.norm_offset,replacer);
		}
	}
	else if(datum_pos_style==8)	//与节点同位
	{
		if(idClassType==0||idClassType==CLS_NODE)
			replaced|=replacer.VerifyAndReplace(&des_para.hNode);
	}
	else if(datum_pos_style==9)	//指定构件上的相对坐标
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
				return false;	//地址重复，不能再添加
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
	else if (datum_pos_style == 3|| datum_pos_style == 4)	//角钢心线(或空间基准线)交点
	{
		uniaddr.SafeAdd(&des_para.AXIS_INTERS.hDatum1);
		uniaddr.SafeAdd(&des_para.AXIS_INTERS.hDatum2);
		uniaddr.SafeAdd(des_para.AXIS_INTERS.norm_offset);
	}
	else if (datum_pos_style == 5)	//基准线上轴向坐标不变点
		uniaddr.SafeAdd(&des_para.DATUMLINE.hDatumLine);
	else if (datum_pos_style == 6)	//基准面上指定轴向点
	{
		uniaddr.SafeAdd(&des_para.DATUMPLANE.hDatumPlane);
	}
	else if (datum_pos_style == 7)	//角钢内外包铁楞点
	{
		uniaddr.SafeAdd(&des_para.RODEND.hRod);
		uniaddr.SafeAdd(des_para.RODEND.norm_offset);
	}
	else if (datum_pos_style == 8)	//与节点同位
	{
		uniaddr.SafeAdd(&des_para.hNode);
		uniaddr.SafeAdd(des_para.RODEND.norm_offset);
	}
	else if (datum_pos_style == 9)	//指定构件上的相对坐标
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
	else if(datum_pos_style==3||datum_pos_style==4)	//角钢心线交点/基准线空间交点
	{
		pModel->ValidateObjRefH(&des_para.AXIS_INTERS.hDatum1);
		pModel->ValidateObjRefH(&des_para.AXIS_INTERS.hDatum2);
		//replaced|=ReplaceReferenceHandleOfNormOffset(des_para.AXIS_INTERS.norm_offset,replacer);
	}
	else if(datum_pos_style==5)	//基准线上轴向坐标不变点
	{
		pModel->ValidateObjRefH(&des_para.DATUMLINE.hDatumLine);
	}
	else if(datum_pos_style==6)	//基准面上指定轴向点
	{
		pModel->ValidateObjRefH(&des_para.DATUMPLANE.hDatumPlane);
	}
	else if(datum_pos_style==7)	//角钢内外包铁楞点
	{
		pModel->ValidateObjRefH(&des_para.RODEND.hRod);
		//replaced|=ReplaceReferenceHandleOfNormOffset(des_para.RODEND.norm_offset,replacer);
	}
	else if(datum_pos_style==8)	//与节点同位
	{
		pModel->ValidateObjRefH(&des_para.hNode);
	}
	else if(datum_pos_style==9)	//指定构件上的相对坐标
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
	cPointAndVectorFlag&=0XFE;	//清空标识位 0xFE=0xFF^0x01
	AxisVector.axis_vec[0]=vx;
	AxisVector.axis_vec[1]=vy;
	AxisVector.axis_vec[2]=vz;
}
void CLDSPoint::DES_PARA::tagCOLUMN_INTERS::SetSectCenter(double cx,double cy,double cz)
{
	cPointAndVectorFlag&=0XFD;	//清空标识位 0xFE=0xFF^0x02
	SectCenter.sect_center[0]=cx;
	SectCenter.sect_center[1]=cy;
	SectCenter.sect_center[2]=cz;
}
void CLDSPoint::DES_PARA::tagCOLUMN_INTERS::SetSectNorm(double vx,double vy,double vz)
{
	cPointAndVectorFlag&=0XFB;	//清空标识位 0xFE=0xFF^0x04
	SectNorm.sect_norm[0]=vx;
	SectNorm.sect_norm[1]=vy;
	SectNorm.sect_norm[2]=vz;
}
void CLDSPoint::DES_PARA::tagCOLUMN_INTERS::SetRayVector(double vx,double vy,double vz)
{
	cPointAndVectorFlag&=0XF7;	//清空标识位 0xFE=0xFF^0x08
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
	else if(datum_pos_style==1)	//角钢楞点定位点
	{
		CLDSLinePart* pLinePart=(CLDSLinePart*)m_pModel->FromPartHandle(des_para.RODEND.hRod,CLS_LINEPART);
		if(pLinePart==NULL||!pLinePart->IsAngle())
			return false; //throw "基点定位数据缺少基准角钢信息";
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
	else if(datum_pos_style==2)	//杆件上节点定位点
	{
		f3dPoint offset_vec;
		SmartRodPtr pBaseRod=m_pModel->FromRodHandle(des_para.RODNODE.hRod);
		if(pBaseRod.IsNULL())
			return FALSE;	//throw "基点定位数据缺少杆件信息";
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
		return des_para.hNode;	//与节点同位
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
bool CParaPointLifeObj::LockCall(long hAngle)	//指定角钢未锁定时，锁定返回true,否则返回false wjh-2018.1.5
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
	else if(datum_pos_style==1)	//角钢楞点定位点
	{
		f3dPoint offset_vec;
		CLDSLinePart* pLinePart=(CLDSLinePart*)pTower->FromPartHandle(des_para.RODEND.hRod,CLS_LINEPART);
		if(pLinePart==NULL)
			return FALSE; //throw "基点定位数据缺少基准角钢信息";
		if(pLinePart->IsAngle())
		{
			CSmartPtr<CLDSLineAngle> pJg=(CLDSLineAngle*)pLinePart;
			if(des_para.RODEND.wing_offset_style==0)
			{//标准准距
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
			if(des_para.RODEND.direction==0)		//始->终
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
			if(des_para.RODEND.offset_wing==0)	//X肢
			{
				pos += pJg->CalWingOffsetVec('X',des_para.RODEND.wing_offset_dist);//*pJg->GetWingVecX();
				pos += pJg->get_norm_x_wing()*norm_offset_dist;
			}
			else							//Y肢
			{
				pos += pJg->CalWingOffsetVec('Y',des_para.RODEND.wing_offset_dist);//*pJg->GetWingVecY();
				pos += pJg->get_norm_y_wing()*norm_offset_dist;
			}
		}
		else 
		{	//除角钢外的其它杆件
			if(des_para.RODEND.direction==0)		//始->终
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
			{	//处理基点位于槽钢两肢的情况  wxc-2019.7.26
				if (des_para.RODEND.offset_wing == 1)			//X+肢
					pos += pLinePart->ucs.axis_x*pLinePart->size_height*0.5;
				else if (des_para.RODEND.offset_wing == 2)	//X-肢
					pos -= pLinePart->ucs.axis_x*pLinePart->size_height*0.5;
				if (des_para.RODEND.offset_wing > 0 && des_para.RODEND.wing_offset_dist > 0)
					pos += pLinePart->ucs.axis_y*des_para.RODEND.wing_offset_dist;
			}
		}
	}
	else if(datum_pos_style==2)	//杆件上节点定位点
	{
		f3dPoint offset_vec;
		SmartPartPtr pBasePart=pTower->FromPartHandle(des_para.RODNODE.hRod);
		CSmartPtr<CLDSNode>pNode=pTower->FromNodeHandle(des_para.RODNODE.hNode);
		if(pNode.IsNULL())
			return FALSE;	//throw "基点定位数据缺少节点信息";
		if(pBasePart.IsNULL())
			return FALSE;	//throw "基点定位数据缺少杆件信息";
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
			{//标准准距
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
			if(des_para.RODNODE.direction==0)		//始->终
				offset_vec = pLinePart->End()-pLinePart->Start();
			else
				offset_vec = pLinePart->Start()-pLinePart->End();
			normalize(offset_vec);
			CDesignThickPara thick(des_para.RODNODE.norm_offset);
			double norm_offset_dist=thick.Thick(pTower);
			f3dPoint face_norm(des_para.RODNODE.vPlaneNormal);
			double dfErrorIntersPlaneNormalDetect = fabs(offset_vec*face_norm);
			if (des_para.RODNODE.bFaceOffset&&dfErrorIntersPlaneNormalDetect < 0.2)
				logerr.LevelLog(CLogFile::WARNING_LEVEL3_UNIMPORTANT, "基准点(杆件0x%x@节点0x%x)平推面法向数据错误", pLinePart->handle, des_para.RODNODE.hNode);
			if(des_para.RODNODE.bFaceOffset&&!face_norm.IsZero()&& dfErrorIntersPlaneNormalDetect >0.2)
			{
				f3dLine axisLine;
				if(des_para.RODNODE.offset_wing==0)	//X肢
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
				if(des_para.RODNODE.offset_wing==0)	//X肢
					pos +=pLinePart.LineAnglePointer()->get_norm_x_wing()*norm_offset_dist;
				else							//Y肢
					pos +=pLinePart.LineAnglePointer()->get_norm_y_wing()*norm_offset_dist;
				//平推时的纵向偏移 
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
					//只有当节点非当前杆件端节点时才考虑调用GetDatumPosBer，否则可能会由于固定正负头计算时涉及CLDSPoint::UpdatePos->CLDSLineAngle::GetDatumPosBer陷入死循环　wjh-2018.1.5
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
				if(des_para.RODNODE.offset_wing==0)	//X肢
				{
					pos += pLinePart.LineAnglePointer()->CalWingOffsetVec('X',des_para.RODNODE.wing_offset_dist);//GetWingVecX();
					pos +=pLinePart.LineAnglePointer()->get_norm_x_wing()*norm_offset_dist;
				}
				else							//Y肢
				{
					pos += pLinePart.LineAnglePointer()->CalWingOffsetVec('Y',des_para.RODNODE.wing_offset_dist);//GetWingVecY();
					pos +=pLinePart.LineAnglePointer()->get_norm_y_wing()*norm_offset_dist;
				}
			}
		}
		else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
		{
			if(des_para.RODNODE.direction==0)		//始->终
				offset_vec=(pLinePart->End()-pLinePart->Start()).normalized();
			else
				offset_vec=(pLinePart->Start()-pLinePart->End()).normalized();
			f3dPoint perp=pos;
			if(pNode->m_cPosCalType==4)	//交叉点应取交叉关联杆件的实际交点（角钢为心线交点，钢管为轴线交点）
				pNode->GetIntersZhun(&perp);
			else
				SnapPerp(&perp,pLinePart->Start(),pLinePart->End(),pNode->Position(true));
			pos=perp;
			pos+=offset_vec*des_para.RODNODE.len_offset_dist;
			//
			if(des_para.RODNODE.offset_wing==1)			//X+肢
				pos+=pLinePart->ucs.axis_x*pLinePart->size_height*0.5;
			else if(des_para.RODNODE.offset_wing==2)	//X-肢
				pos-=pLinePart->ucs.axis_x*pLinePart->size_height*0.5;
			if(des_para.RODNODE.offset_wing>0&&des_para.RODNODE.wing_offset_dist>0)
				pos+=pLinePart->ucs.axis_y*des_para.RODNODE.wing_offset_dist;
		}
		else if(pLinePart->GetClassTypeId()==CLS_LINETUBE||pLinePart->GetClassTypeId()==CLS_LINEFLAT)
		{
			if(des_para.RODNODE.direction==0)		//始->终
				offset_vec = pLinePart->End()-pLinePart->Start();
			else
				offset_vec = pLinePart->Start()-pLinePart->End();
			normalize(offset_vec);
			f3dPoint perp=pos;
			if(pNode->m_cPosCalType==4)	//交叉点应取交叉关联杆件的实际交点（角钢为心线交点，钢管为轴线交点）
				pNode->GetIntersZhun(&perp);
			else
				SnapPerp(&perp,pLinePart->Start(),pLinePart->End(),pNode->Position(true));
			pos=perp;
			pos+=offset_vec*des_para.RODNODE.len_offset_dist;
		}
		else
			pos=pNode->Position(true);
	} 
	else if(datum_pos_style==3)	//杆件轴线(角钢心线)交点
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
		
		//此处不能使用节点位置计算面法线，预拱后计算不准确 wht 13-04-18
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
		if(norm*justify<0)	//如果法线预定近似方向相反则需要反号
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
			pCrossJg=datumPartPtr[1].pAngle;	//以肢宽较大的角钢为心线交点的投影基准
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
		{	//只有计算两角钢心线交点时才需要以其中一根为基准(但出于旧版兼容性考虑必须以第一根为基点)进行心线投影计算 wjh-2016.3.26
			project_point(zhun_line[1-baseAngleI].startPt,zhun_line[0].startPt,vWorkWingNorm);
			project_point(zhun_line[1-baseAngleI].endPt,zhun_line[0].startPt,vWorkWingNorm);
		}
		Int3dll(zhun_line[0],zhun_line[1],pt);
		if(datumPartPtr[0]->IsAngle()&&datumPartPtr[1]->IsAngle()&&!gbLegacyMode&&pTower->RepositionAlgor()!=1)
		{	//为减少计算误差，取更靠近交点的1号心线拾取点为平面基点　wjh-2016.4.22
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
	else if(datum_pos_style==4)	//基准线空间交点
	{
		CSmartPtr<CLDSLine>pLineFir=pTower->FromLineHandle(des_para.AXIS_INTERS.hDatum1);
		CSmartPtr<CLDSLine>pLineSec=pTower->FromLineHandle(des_para.AXIS_INTERS.hDatum2);
		if(pLineFir==NULL||pLineSec==NULL)
			return FALSE;	//throw "基点定位数据缺少基准线信息";
		pLineFir->UpdateLine(pTower);
		pLineSec->UpdateLine(pTower);
		f3dLine L1(pLineFir->start_pos,pLineFir->end_pos),L2(pLineSec->start_pos,pLineSec->end_pos);
		f3dPoint line_vec1=L1.endPt-L1.startPt;
		f3dPoint line_vec2=L2.endPt-L2.startPt;
		if(des_para.AXIS_INTERS.cProjectPlane<=0||des_para.AXIS_INTERS.cProjectPlane>3)
		{	//本次改动仅限,退格缩进 -wjh 2013.8.3
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
				//AfxMessageBox("两基准线平行重合或异面!");
				return FALSE;
			}
		}
		else
		{
			f3dPoint face_norm,inters;
			if(des_para.AXIS_INTERS.cProjectPlane==1)	//X-Z平面
				face_norm=line_vec2^f3dPoint(0,1,0);
			else if(des_para.AXIS_INTERS.cProjectPlane==2)	//Y-Z平面
				face_norm=line_vec2^f3dPoint(1,0,0);
			else if(des_para.AXIS_INTERS.cProjectPlane==3)	//X-Y平面
				face_norm=line_vec2^f3dPoint(0,0,1);
			int retCode=Int3dlf(inters,L1,L2.startPt,face_norm);
			if(retCode==-1||retCode==0)
				return FALSE;//AfxMessageBox("投影面与基准线平行或重合!");
		}
	}
	else if(datum_pos_style==5)	//基准线上轴向坐标不变点
	{
		CSmartPtr<CLDSLine>pLineFir=pTower->FromLineHandle(des_para.DATUMLINE.hDatumLine);
		if(pLineFir==NULL)
			return FALSE;	//throw "基点定位数据缺少基准线信息";
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
				//AfxMessageBox("基准线与基准轴向平行!");
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
				//AfxMessageBox("基准线与基准轴向平行!");
				return FALSE;
			}
		}
	}
	else if(datum_pos_style==6)	//基准面上指定轴向点
	{
		CSmartPtr<CLDSPlane>pPlane=pTower->FromPlaneHandle(des_para.DATUMPLANE.hDatumPlane);
		if(pPlane==NULL)
			return FALSE;	//throw "基点定位数据缺少基准面信息";
		if(!pPlane->UpdatePlane(pTower))
		{
			//AfxMessageBox("基准面定义有误!");
			return FALSE;
		}
		f3dPoint temporary;
		if(pLift)	//圆弧预拱基准点初始位置
		{
			if(!bReverseLift)
				temporary=pos=pLift->LiftPosTrans(datum_pos);
			else
				temporary=pos=pLift->ReverseLiftPosTrans(datum_pos);
		}
		else
			temporary=pos=datum_pos;
		//处理参照对称
		if(des_para.DATUMPLANE.cMirTransType=='X')	//X轴向坐标对称变换
			temporary.y=-temporary.y;
		else if(des_para.DATUMPLANE.cMirTransType=='Y')	//Y轴向坐标对称变换
			temporary.x=-temporary.x;
		else if(des_para.DATUMPLANE.cMirTransType=='Z')	//Z轴向坐标对称变换
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
			Log2File()->Log("0X%X基准面与基准轴向平行，也可能是基准面法线为空值!",pPlane->handle);
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
			Log2File()->Log("0X%X基准面与基准轴向平行，也可能是基准面法线为空值!");
			return FALSE;
		}
		if(des_para.DATUMPLANE.cMirTransType=='X')	//X轴向坐标对称变换
			temporary.y=-temporary.y;
		else if(des_para.DATUMPLANE.cMirTransType=='Y')	//Y轴向坐标对称变换
			temporary.x=-temporary.x;
		else if(des_para.DATUMPLANE.cMirTransType=='Z')	//Z轴向坐标对称变换
		{
			temporary.x=-temporary.x;
			temporary.y=-temporary.y;
		}
		pos=temporary;
	}
#if !defined(__TSA_)&&!defined(__TSA_FILE_)
	else if(datum_pos_style==7)	//角钢内外包铁楞点
	{
		f3dPoint offset_vec;
		CSmartPtr<CLDSLineAngle> pJg=(CLDSLineAngle*)pTower->FromPartHandle(des_para.RODEND.hRod,CLS_LINEANGLE,CLS_GROUPLINEANGLE);
		if(pJg.IsNULL())
			return FALSE;	//throw "基点定位数据缺少基准角钢信息";
		//if(pLift)
		//	pos=pLift->LiftPosTrans(datum_pos);
		//else
		//	pos=datum_pos;
		if(des_para.RODEND.direction==0)		//始->终
		{
			offset_vec = pJg->End()-pJg->Start();
			normalize(offset_vec);
			//默认计入正负头影响
			pos = pJg->Start()+offset_vec*(des_para.RODEND.len_offset_dist-pJg->startOdd());
		}
		else
		{
			offset_vec = pJg->Start()-pJg->End();
			normalize(offset_vec);
			//默认计入正负头影响
			pos = pJg->End()+offset_vec*(des_para.RODEND.len_offset_dist-pJg->endOdd());
		}
		f3dPoint norm_x_wing=pJg->get_norm_x_wing();
		f3dPoint norm_y_wing=pJg->get_norm_y_wing();
		f3dPoint bi_vec=norm_x_wing+norm_y_wing;
		normalize(bi_vec);
		CDesignThickPara thick(des_para.RODEND.norm_offset);
		double norm_offset_dist=thick.Thick(pTower);
		if(pJg->IsReservedHeJiao())	//保留一定肢宽度的特殊合角时楞点处是直角不用计算等边线
			pos+=(norm_offset_dist*1.4142135623731)*bi_vec;//sqrt(2)=1.4142135623731
		else
		{
			double cosa=bi_vec*norm_x_wing;
			pos+=(norm_offset_dist/cosa)*bi_vec;
		}
	}
	else if(datum_pos_style==10)
	{	//两节点间的比例等分点 wht 11-01-23
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
	else if(datum_pos_style==8)	//与节点同位
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
			//AfxMessageBox("找不到指定的基准节点!");
			return FALSE;
		}
	}
	else if(datum_pos_style==9)	//指定构件上的相对坐标
	{
		SmartPartPtr pSmartPart=pTower->FromPartHandle(des_para.hPart);
		if(pSmartPart.blValid)
		{
			pos=datum_pos;
			if(pSmartPart->IsAngle())//角钢的装配坐标系不存储在文件里，需要实时生成 wjh-2019.5.18
				pSmartPart.pAngle->getUCS(pSmartPart->ucs);
			coord_trans(pos,pSmartPart->ucs,TRUE);
		}
		else
			return FALSE;	//找不到指定构件
	}
#ifdef __COMPLEX_PART_INC_
	else if(datum_pos_style==11)
	{	//柱面与射线交点
		CLDSLineTube* pTube=NULL;
		if(des_para.COLUMN_INTERS.hLineTube>0)
			pTube=(CLDSLineTube*)pTower->FromPartHandle(des_para.COLUMN_INTERS.hLineTube,CLS_LINETUBE);
		f3dPoint axis_vec,sect_norm;
		f3dPoint sect_center(des_para.COLUMN_INTERS.SectCenter.sect_center[0],des_para.COLUMN_INTERS.SectCenter.sect_center[1],des_para.COLUMN_INTERS.SectCenter.sect_center[2]);
		//1.确定（椭）圆截面圆心点及柱轴线方向
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
			{	//参数化定义截面圆心点坐标
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
				logerr.Log("在计算柱面参数化定位点时，柱心线与截面求交失败！");
#else 
				logerr.Log("在计算柱面参数化定位点时，柱心线与截面求交失败！");
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
		//2.规范化柱面轴线与截面法线
		normalize(sect_norm);
		//3.确定射线扇形旋转定向角
		f3dPoint ray_vec(des_para.COLUMN_INTERS.RayVector.ray_vec[0],des_para.COLUMN_INTERS.RayVector.ray_vec[1],des_para.COLUMN_INTERS.RayVector.ray_vec[2]);
		if(des_para.COLUMN_INTERS.ray_vec_style==0||pTube==NULL)//ray_vec为直接定义的射线绝对方向
		{
			if(!project_vector(ray_vec,sect_norm))
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("在进行射线向截面投影时，计算失败");
#else 
				logerr.Log("在进行射线向截面投影时，计算失败");
#endif
				return FALSE;
			}
		}
		else if(des_para.COLUMN_INTERS.ray_vec_style>0)
		{	// 指定柱面起始边，并根据ray_angle确定射线方向
			double rot_ang=des_para.COLUMN_INTERS.ray_angle*RADTODEG_COEF;
			if(des_para.COLUMN_INTERS.ray_vec_style==4)
			{	//常用于钢管倾斜材上的水平槽钢底座脚钉 wjh-2015.10.10
				GEPOINT horiRayVec=sect_norm^f3dPoint(0,0,1);
				if(horiRayVec*ray_vec<0)
					horiRayVec*=-1.0;
				if(normalize(horiRayVec))
					des_para.COLUMN_INTERS.RayVector.ray_vec=ray_vec=horiRayVec;
			}
			else
			{
				if(des_para.COLUMN_INTERS.ray_vec_style>1)
				{	//以钢管基准边或焊道线为柱面起始边，经旋转确定射线方向
					pTube->BuildUCS();
					ray_vec=pTube->ucs.axis_x;
					if(pTube&&des_para.COLUMN_INTERS.ray_vec_style==3)	//钢管焊道线为柱面起始边
						rot_ang+=pTube->CalWeldRoadAngle();
				}
				//else if(pTube&&ray_vec_style==2)	//钢管基准边为柱面起始边
				RotateVectorAroundVector(ray_vec,rot_ang,axis_vec);
				//向斜截面影射
				if(fabs(sect_norm*axis_vec)<EPS_COS)
				{
					f3dPoint ray_pick=sect_center+ray_vec;
					if(Int3dlf(ray_pick,ray_pick,axis_vec,sect_center,sect_norm)<=0)
					{
#ifdef AFX_TARG_ENU_ENGLISH
						logerr.Log("在计算柱面参数化定位点时，射线拾取点与截面求交失败！");
#else 
						logerr.Log("在计算柱面参数化定位点时，射线拾取点与截面求交失败！");
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
		//4.计算最终柱面参数化定位点
		f3dPoint ray_vert_vec=ray_vec;
		if(fabs(des_para.COLUMN_INTERS.ray_para_offset)>EPS)	//需要对射线进行平行移动
			RotateVectorAroundVector(ray_vert_vec,0.5*Pi,sect_norm);
		if(!des_para.COLUMN_INTERS.bSpecRo)	//辅射半径以柱面射线交点向外的延伸偏量确定
		{	//4.1计算柱面与射线交点
			f3dPoint inters,inters1,inters2;
			f3dPoint line_pick=sect_center;
			if(fabs(des_para.COLUMN_INTERS.ray_para_offset)>EPS)	//需要对射线进行平行移动
				line_pick+=ray_vert_vec*des_para.COLUMN_INTERS.ray_para_offset;
			int inters_n=Int3dlc(&inters1,&inters2,line_pick,ray_vec,
				sect_center,sect_center+axis_vec,des_para.COLUMN_INTERS.column_r,des_para.COLUMN_INTERS.column_r);
			if(inters_n<=0)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("在计算柱面参数化定位点时，射线与圆柱面求交失败");
#else 
				logerr.Log("在计算柱面参数化定位点时，射线与圆柱面求交失败");
#endif
				return FALSE;
			}
			if(inters_n==2 && ((inters1-sect_center)*ray_vec<0))
				inters=inters2;
			else
				inters=inters1;
			//4.2根据柱面与射线交点进行射线方向偏移确定最终定位点
			pos=inters+des_para.COLUMN_INTERS.ray_offset_r*ray_vec;
		}
		else
		{	//4.3根据指定的到截面圆心点的投影距离绝对值计算最终定位点
			pos=sect_center+ray_vec*des_para.COLUMN_INTERS.ray_offset_r;
			if(fabs(des_para.COLUMN_INTERS.ray_para_offset)>EPS)	//需要对射线进行平行移动
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
	if(datum_pos_style==0)	//直接指定
		mirPoint.datum_pos=GetMirPos(datum_pos,mirinfo);
	else if(datum_pos_style==1||datum_pos_style==7)
	{	//角钢两端楞点为基准或角钢包铁楞点
		CLDSLinePart *pRod=pModel->FromRodHandle(des_para.RODEND.hRod);
		CLDSLinePart *pMirRod=NULL;
		if(pRod)	//由于存在好多横材及塔身主材非对称打断情况，此时螺栓依赖杆件对称没办法遵循严格对称原则 wjh-2015.11.26
			pMirRod=(CLDSLinePart*)pRod->GetMirPart(mirinfo,pos);
		//在高低腿结构塔身主材点线投影(平推)中很有可能找不到对称材，此是不能直接粗暴清零而是应该保持原值
		long hOldRod=mirPoint.des_para.RODEND.hRod;
		mirPoint.des_para.RODEND=des_para.RODEND;
		mirPoint.des_para.RODEND.hRod=hOldRod;//0;
		if (pMirRod)
		{
			mirPoint.des_para.RODEND.hRod = pMirRod->handle;
			GEPOINT line_vec,mir_line_vec;
			if (pRod->pStart&&pRod->pEnd&&pMirRod->pStart&&pMirRod->pEnd)
			{	//杆件初始端点位置若与正确位置存在较大偏差时，会出现判断错误故优先选用单线坐标 wjh-2020.2.11
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
			{	//角钢两端楞点定位
				f3dPoint mir_wing_norm = mirinfo.MirVector(((CLDSLineAngle*)pRod)->get_norm_x_wing());
				if (fabs(mir_wing_norm*((CLDSLineAngle*)pMirRod)->get_norm_x_wing()) <
					fabs(mir_wing_norm*((CLDSLineAngle*)pMirRod)->get_norm_y_wing()))	//两肢颠倒对称
					mirPoint.des_para.RODEND.offset_wing = 1 - mirPoint.des_para.RODEND.offset_wing;
			}
			if (datum_pos_style == 1 && pRod->IsAngle() && des_para.RODEND.wing_offset_style == 4 && des_para.RODEND.wing_offset_dist != 0)
			{	//自动识别出自定义肢偏移量为标准准距的情况,并进行对称
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
	{	//杆件上节点为基准
		//在高低腿结构塔身主材点线投影(平推)中很有可能找不到对称材，此是不能直接粗暴清零而是应该保持原值 wjh-2015.10.09
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
			Log2File()->Log("未找到参数化点(0x%X)定位节点0x%X的匹配对称节点",handle,des_para.RODNODE.hNode);
			errorstatus=-1;	//丢失基准杆件
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
					fabs(mir_wing_norm*((CLDSLineAngle*)pMirRod)->get_norm_y_wing()))	//两肢颠倒对称
					mirPoint.des_para.RODNODE.offset_wing=1-mirPoint.des_para.RODNODE.offset_wing;
			}
		}
		if(pRod!=NULL&&pRod->IsAngle()&&des_para.RODNODE.wing_offset_style==4&&des_para.RODNODE.wing_offset_dist!=0)
		{	//自动识别出自定义肢偏移量为标准准距的情况,并进行对称
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
	{	//角钢心线交点
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
	else if(mirPoint.datum_pos_style==6)	//基准面上指定坐标分量节点
	{
		if(msg.axis_flag==0x01||msg.axis_flag==0x02||msg.axis_flag==0x04)
		{
			mirPoint.datum_pos=GetMirPos(datum_pos,msg);
			mirPoint.datum_pos_style=datum_pos_style;
			mirPoint.des_para.DATUMPLANE.hDatumPlane=des_para.DATUMPLANE.hDatumPlane;
			mirPoint.des_para.DATUMPLANE.fix_axis_style_x0_y1_z2=des_para.DATUMPLANE.fix_axis_style_x0_y1_z2;
			//对称基准面定位的定位参数信息
			if(mirinfo.axis_flag==0x01)	//X轴对称
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
			else if(mirinfo.axis_flag==0x02)	//Y轴对称
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
			else if(mirinfo.axis_flag==0x04)	//Z轴对称
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
	else if(mirPoint.datum_pos_style==8)	//节点与基点同位
	{
		MirPartHandle(pModel,&mirPoint.des_para.hNode,des_para.hNode,msg);
	}
	else if(mirPoint.datum_pos_style==9)	//基准构件上的相对坐标
	{	
		mirPoint.datum_pos=datum_pos;
		MirPartHandle(pModel,&mirPoint.des_para.hPart,des_para.hPart,msg);
		if(pModel)
		{
			CLDSPart *pDatumPart=pModel->FromPartHandle(des_para.hPart);
			CLDSPart *pMirDatumPart=pModel->FromPartHandle(mirPoint.des_para.hPart);
			if(pDatumPart==NULL||pMirDatumPart==NULL)
				return -1;	//丢失基准杆件
			f3dPoint datumpos=datum_pos;
			coord_trans(datumpos,pDatumPart->ucs,TRUE);
			mirPoint.datum_pos=GetMirPos(datumpos,msg);
			coord_trans(mirPoint.datum_pos,pMirDatumPart->ucs,FALSE);
		}
	}
	else if(mirPoint.datum_pos_style==10)	//两节点间的比例等分点 wht 12-03-19
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
CHashStrList<PROPLIST_ITEM>CLDSPoint::propHashtable;	//属性字符串哈希表
CXhChar100 CLDSPoint::HELPLINK_URL;
CHashTable<DWORD>CLDSPoint::propStatusHashtable;	//属性状态哈希表
CHashSet<PROPLIST_ITEM*>CLDSPoint::hashPropItemById;
void CLDSPoint::InitPropHashtable()						//初始化属性哈希表
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
	//与节点同位
	CLDSPoint::AddPropItem("des_para.hNode",PROPLIST_ITEM(id++,"Datum Node Handle"));
	//指定构件上的相对坐标
	CLDSPoint::AddPropItem("des_para.hPart",PROPLIST_ITEM(id++,"Datum Part Handle"));
	CLDSPoint::AddPropItem("datum_pos",PROPLIST_ITEM(id++,"Relative Coordinates"));
	CLDSPoint::AddPropItem("datum_pos.x",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("datum_pos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("datum_pos.z",PROPLIST_ITEM(id++,"Z"));
	//心线交点
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.hDatum1",PROPLIST_ITEM(id++,"Datum Line 1 Handle"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.hDatum2",PROPLIST_ITEM(id++,"Datum Line 2 Handle"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_style1",PROPLIST_ITEM(id++,"Leg Offset Type 1"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_style2",PROPLIST_ITEM(id++,"Leg Offset Type 2"));//0.g;1.g1;2.g2;3.g3;4.自定义
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_dist1",PROPLIST_ITEM(id++,"Leg Offset 1"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_dist2",PROPLIST_ITEM(id++,"Leg Offset 2"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector",PROPLIST_ITEM(id++,"Near Normal"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector[0]",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector[1]",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector[2]",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.norm_offset",PROPLIST_ITEM(id++,"Normal Offset"));	//法向偏移量;
	//角钢楞点
	CLDSPoint::AddPropItem("des_para.RODEND.hRod",PROPLIST_ITEM(id++,"Datum Rod Handle"));				//角钢句柄
	CLDSPoint::AddPropItem("des_para.RODEND.direction",PROPLIST_ITEM(id++,"Vertical Offset Direction"));			//0.基准角钢方向:始->终;1.终->始
	CLDSPoint::AddPropItem("des_para.RODEND.offset_wing",PROPLIST_ITEM(id++,"Offset Leg"));		//0.X肢;1.Y肢
	CLDSPoint::AddPropItem("des_para.RODEND.wing_offset_style",PROPLIST_ITEM(id++,"Leg Offset Type "));	//0.g;1.g1;2.g2;3.g3;4.自定义
	CLDSPoint::AddPropItem("des_para.RODEND.wing_offset_dist",PROPLIST_ITEM(id++,"Leg Offset "));
	CLDSPoint::AddPropItem("des_para.RODEND.len_offset_dist",PROPLIST_ITEM(id++,"Vertical Direction Offset"));	//肢向偏移值;纵向偏移值;
	CLDSPoint::AddPropItem("des_para.RODEND.bIncOddEffect",PROPLIST_ITEM(id++,"Odd Length Affect"));		//纵向偏移值计入正负头影响
	CLDSPoint::AddPropItem("des_para.RODEND.norm_offset",PROPLIST_ITEM(id++,"Normal Offset"));		//法向偏移量
	//杆件上节点为基准
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.hLinePart",PROPLIST_ITEM(id++,"Datum Line Part Handle"));	//角钢句柄
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.hNode",PROPLIST_ITEM(id++,"Datum Node Handle"));		//基准节点句柄
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.bFaceOffset",PROPLIST_ITEM(id++,"Offset Flatly"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.direction",PROPLIST_ITEM(id++,"Vertical Offset Direction"));	//0.基准角钢方向:始->终;1.终->始
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.offset_wing",PROPLIST_ITEM(id++,"Offset Leg"));	//0.X肢;1.Y肢
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.wing_offset_style",PROPLIST_ITEM(id++,"Leg Offset Type "));//0.g;1.g1;2.g2;3.g3;4.自定义
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.wing_offset_dist",PROPLIST_ITEM(id++,"Leg Offset "));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.len_offset_dist",PROPLIST_ITEM(id++,"Vertical Direction Offset"));	//肢向偏移值;纵向偏移值;
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.norm_offset",PROPLIST_ITEM(id++,"Normal Offset"));		//法向偏移量;
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset",PROPLIST_ITEM(id++,"Offset Plane Normal "));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset[0]",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset[1]",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset[2]",PROPLIST_ITEM(id++,"Z"));
	//基准线上轴向坐标不变点
	CLDSPoint::AddPropItem("des_para.DATUMLINE.hDatumLine",PROPLIST_ITEM(id++,"Datum Line Handle"));
	CLDSPoint::AddPropItem("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2",PROPLIST_ITEM(id++,"Specify Axial"));
	//基准面上轴向坐标不变点
	CLDSPoint::AddPropItem("des_para.DATUMPLANE.hDatumPlane",PROPLIST_ITEM(id++,"Datum Plane Handle"));
	CLDSPoint::AddPropItem("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2",PROPLIST_ITEM(id++,"Specify Axial"));
	//两节点间比例等分点
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.hLinePart",PROPLIST_ITEM(id++,"Datum Rod Handle"));			//基准杆件句柄
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.hStartNode",PROPLIST_ITEM(id++,"Start Datum Node Handle"));		//始端节点句柄
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.hEndNode",PROPLIST_ITEM(id++,"End Datum Node Handle"));			//终端节点句柄
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.start_offset_dist",PROPLIST_ITEM(id++,"Start Offset"));	//始端偏移量
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.end_offset_dist",PROPLIST_ITEM(id++,"End Offset"));	//终端偏移量
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.offset_scale",PROPLIST_ITEM(id++,"Offset Proportion"));		//偏移比例
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.offset_dist",PROPLIST_ITEM(id++,"Offset Distance"));		//在等分基础上的偏移值
	//柱面定位点
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.column",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.column_r",PROPLIST_ITEM(id++,"Column Radius"));		//柱面半径
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.hLineTube",PROPLIST_ITEM(id++,"Datum Tube Handle"));		//钢管句柄
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec",PROPLIST_ITEM(id++,"Column Axis Positive Direction"));		//柱面轴线正方向
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec.x",PROPLIST_ITEM(id++,"X"));		
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_plane",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center",PROPLIST_ITEM(id++,"Section Center"));	//截面圆心点
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center.x",PROPLIST_ITEM(id++,"X"));	
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm",PROPLIST_ITEM(id++,"Section Normal"));		//截面法向，NULL表示柱面的垂直截面
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm_std_style",PROPLIST_ITEM(id++,"Specify Direction"));//射线方向类型
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm.x",PROPLIST_ITEM(id++,"X"));	
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec_style",PROPLIST_ITEM(id++,"Projective Direction"));	//射线方向定义方式
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec",PROPLIST_ITEM(id++,""));		//射线方向	
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec_std_style",PROPLIST_ITEM(id++,"Specify Direction"));	//射线方向类型
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec.x",PROPLIST_ITEM(id++,"X"));		
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_angle",PROPLIST_ITEM(id++,"Rotation Angle"));		//偏移角度
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_para_offset",PROPLIST_ITEM(id++,"Ray Line Offset"));//射线平行移动距离
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.bSpecRo",PROPLIST_ITEM(id++,"Offset Datum Point"));		//辅助半径定义方式
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_offset_r",PROPLIST_ITEM(id++,"Offset|Ray Line Radius"));	//辅助半径
#else 
	AddPropItem("datumPoint",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("description",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("pos",PROPLIST_ITEM(id++,"基点坐标"));
	CLDSPoint::AddPropItem("pos.x",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("pos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("pos.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("datum_pos_style",PROPLIST_ITEM(id++,"基点定位方式"));
	//与节点同位
	CLDSPoint::AddPropItem("des_para.hNode",PROPLIST_ITEM(id++,"基准节点句柄"));
	//指定构件上的相对坐标
	CLDSPoint::AddPropItem("des_para.hPart",PROPLIST_ITEM(id++,"基准构件句柄"));
	CLDSPoint::AddPropItem("datum_pos",PROPLIST_ITEM(id++,"相对坐标"));
	CLDSPoint::AddPropItem("datum_pos.x",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("datum_pos.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("datum_pos.z",PROPLIST_ITEM(id++,"Z"));
	//心线交点
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.hDatum1",PROPLIST_ITEM(id++,"基准线1句柄"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.hDatum2",PROPLIST_ITEM(id++,"基准线2句柄"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_style1",PROPLIST_ITEM(id++,"肢向偏移类型1"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_style2",PROPLIST_ITEM(id++,"肢向偏移类型2"));//0.g;1.g1;2.g2;3.g3;4.自定义
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_dist1",PROPLIST_ITEM(id++,"肢向偏移值1"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.wing_offset_dist2",PROPLIST_ITEM(id++,"肢向偏移值2"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector",PROPLIST_ITEM(id++,"近似法线"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector[0]",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector[1]",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.near_vector[2]",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.AXIS_INTERS.norm_offset",PROPLIST_ITEM(id++,"法向偏移量"));	//法向偏移量;
	//角钢楞点
	CLDSPoint::AddPropItem("des_para.RODEND.hRod",PROPLIST_ITEM(id++,"基准杆件句柄"));				//角钢句柄
	CLDSPoint::AddPropItem("des_para.RODEND.direction",PROPLIST_ITEM(id++,"纵向偏移方向"));			//0.基准角钢方向:始->终;1.终->始
	CLDSPoint::AddPropItem("des_para.RODEND.offset_wing",PROPLIST_ITEM(id++,"偏移肢"));		//0.X肢;1.Y肢
	CLDSPoint::AddPropItem("des_para.RODEND.wing_offset_style",PROPLIST_ITEM(id++,"肢向偏移类型"));	//0.g;1.g1;2.g2;3.g3;4.自定义
	CLDSPoint::AddPropItem("des_para.RODEND.wing_offset_dist",PROPLIST_ITEM(id++,"肢向偏移值"));
	CLDSPoint::AddPropItem("des_para.RODEND.len_offset_dist",PROPLIST_ITEM(id++,"纵向偏移值"));	//肢向偏移值;纵向偏移值;
	CLDSPoint::AddPropItem("des_para.RODEND.bIncOddEffect",PROPLIST_ITEM(id++,"纵向偏移计入正负头影响"));		//纵向偏移值计入正负头影响
	CLDSPoint::AddPropItem("des_para.RODEND.norm_offset",PROPLIST_ITEM(id++,"法向偏移量"));		//法向偏移量
	//杆件上节点为基准
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.hLinePart",PROPLIST_ITEM(id++,"基准杆件句柄"));	//角钢句柄
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.hNode",PROPLIST_ITEM(id++,"基准节点句柄"));		//基准节点句柄
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.bFaceOffset",PROPLIST_ITEM(id++,"平推"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.direction",PROPLIST_ITEM(id++,"纵向偏移方向"));	//0.基准角钢方向:始->终;1.终->始
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.offset_wing",PROPLIST_ITEM(id++,"偏移肢"));	//0.X肢;1.Y肢
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.wing_offset_style",PROPLIST_ITEM(id++,"肢向偏移类型"));//0.g;1.g1;2.g2;3.g3;4.自定义
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.wing_offset_dist",PROPLIST_ITEM(id++,"肢向偏移值"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.len_offset_dist",PROPLIST_ITEM(id++,"纵向偏移值"));	//肢向偏移值;纵向偏移值;
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.norm_offset",PROPLIST_ITEM(id++,"法向偏移量"));		//法向偏移量;
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset",PROPLIST_ITEM(id++,"平推面法线"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset[0]",PROPLIST_ITEM(id++,"X"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset[1]",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.LINEPARTNODE.face_norm_offset[2]",PROPLIST_ITEM(id++,"Z"));
	//基准线上轴向坐标不变点
	CLDSPoint::AddPropItem("des_para.DATUMLINE.hDatumLine",PROPLIST_ITEM(id++,"基准线句柄"));
	CLDSPoint::AddPropItem("des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2",PROPLIST_ITEM(id++,"指定轴向"));
	//基准面上轴向坐标不变点
	CLDSPoint::AddPropItem("des_para.DATUMPLANE.hDatumPlane",PROPLIST_ITEM(id++,"基准面句柄"));
	CLDSPoint::AddPropItem("des_para.DATUMPLANE.fix_axis_style_x0_y1_z2",PROPLIST_ITEM(id++,"指定轴向"));
	//两节点间比例等分点
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.hLinePart",PROPLIST_ITEM(id++,"基准杆件句柄"));			//基准杆件句柄
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.hStartNode",PROPLIST_ITEM(id++,"始端基准节点句柄"));		//始端节点句柄
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.hEndNode",PROPLIST_ITEM(id++,"终端基准节点句柄"));			//终端节点句柄
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.start_offset_dist",PROPLIST_ITEM(id++,"始端偏移量"));	//始端偏移量
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.end_offset_dist",PROPLIST_ITEM(id++,"终端偏移量"));	//终端偏移量
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.offset_scale",PROPLIST_ITEM(id++,"偏移比例"));		//偏移比例
	CLDSPoint::AddPropItem("des_para.SCALE_NODE.offset_dist",PROPLIST_ITEM(id++,"偏移距离"));		//在等分基础上的偏移值
	//柱面定位点
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.column",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.column_r",PROPLIST_ITEM(id++,"柱面半径"));		//柱面半径
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.hLineTube",PROPLIST_ITEM(id++,"基准钢管句柄"));		//钢管句柄
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec",PROPLIST_ITEM(id++,"柱面轴线正方向"));		//柱面轴线正方向
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec.x",PROPLIST_ITEM(id++,"X"));		
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.axis_vec.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_plane",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center",PROPLIST_ITEM(id++,"截面圆心"));	//截面圆心点
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center.x",PROPLIST_ITEM(id++,"X"));	
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_center.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm",PROPLIST_ITEM(id++,"截面法线"));		//截面法向，NULL表示柱面的垂直截面
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm_std_style",PROPLIST_ITEM(id++,"指定方向"));//射线方向类型
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm.x",PROPLIST_ITEM(id++,"X"));	
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.sect_norm.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray",PROPLIST_ITEM(id++,""));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec_style",PROPLIST_ITEM(id++,"射线方向"));	//射线方向定义方式
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec",PROPLIST_ITEM(id++,""));		//射线方向	
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec_std_style",PROPLIST_ITEM(id++,"指定方向"));	//射线方向类型
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec.x",PROPLIST_ITEM(id++,"X"));		
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec.y",PROPLIST_ITEM(id++,"Y"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_vec.z",PROPLIST_ITEM(id++,"Z"));
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_angle",PROPLIST_ITEM(id++,"旋转角度"));		//偏移角度
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_para_offset",PROPLIST_ITEM(id++,"射线平移距离"));//射线平行移动距离
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.bSpecRo",PROPLIST_ITEM(id++,"偏移基准点"));		//辅助半径定义方式
	CLDSPoint::AddPropItem("des_para.COLUMN_INTERS.ray_offset_r",PROPLIST_ITEM(id++,"偏移量|辅助射线半径"));	//辅助半径
#endif
}
long CLDSPoint::GetPropID(char* propStr)			//得到属性ID
{
	PROPLIST_ITEM* pItem=propHashtable.GetValue(propStr);
	if(pItem)
		return pItem->id;
	else
		return 0;
}
int CLDSPoint::GetPropStatus(long id)					//得到属性状态
{
	DWORD dwStatus;
	if(CLDSPoint::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}
BOOL CLDSPoint::SetPropStatus(long id,BOOL bHide)		//设置属性状态
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSPoint::propStatusHashtable.SetValueAt(id,dwStatus);
}
int CLDSPoint::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)	//根据属性ID得到属性值
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
			strcpy(sText,"1.角钢楞点定位点");
		else if(datum_pos_style==2)
			strcpy(sText,"2.杆件节点定位点");
		else if(datum_pos_style==3)
			strcpy(sText,"3.角钢心线交点");
		else if(datum_pos_style==4)
			strcpy(sText,"4.基准线空间交点");
		else if(datum_pos_style==5)
			strcpy(sText,"5.基准线上指定坐标分量点");
		else if(datum_pos_style==6)
			strcpy(sText,"6.基准面上指定坐标分量点");
		else if(datum_pos_style==7)
			strcpy(sText,"7.沿角钢两肢法向偏移的包铁楞点");
		else if(datum_pos_style==8)
			strcpy(sText,"8.与节点同位");
		else if(datum_pos_style==9)
			strcpy(sText,"9.基准构件上的相对坐标");
		else //if(datum_pos_style==0)
			strcpy(sText,"0.手动输入");
#endif
	}
	else if(GetPropID("des_para.RODEND.hRod")==id)	//角钢句柄
		sprintf(sText,"0X%X",des_para.RODEND.hRod);
	else if(GetPropID("des_para.RODEND.direction")==id)	
	{	//0.基准角钢方向:始->终;1.终->始
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.RODEND.direction==0)
			strcpy(sText,"0.Start->End");
		else 
			strcpy(sText,"1.End->Start");
#else 
		if(des_para.RODEND.direction==0)
			strcpy(sText,"0.始->终");
		else 
			strcpy(sText,"1.终->始");
#endif
	}
	else if(GetPropID("des_para.RODEND.offset_wing")==id)//0.X肢;1.Y肢
	{	//0.X肢;1.Y肢
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.RODEND.offset_wing==0)
			strcpy(sText,"0.X Leg");
		else 
			strcpy(sText,"1.Y Leg");
#else 
		if(des_para.RODEND.offset_wing==0)
			strcpy(sText,"0.X肢");
		else 
			strcpy(sText,"1.Y肢");
#endif
	}
	else if(GetPropID("des_para.RODEND.wing_offset_style")==id)	
	{	//0.g;1.g1;2.g2;3.g3;4.自定义
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
			strcpy(sText,"自定义");
#endif
	}
	else if(GetPropID("des_para.RODEND.wing_offset_dist")==id)
	{
		sprintf(sText,"%f",des_para.RODEND.wing_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.RODEND.len_offset_dist")==id)	
	{	//纵向偏移值;
		sprintf(sText,"%f",des_para.RODEND.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.RODEND.bIncOddEffect")==id)		
	{	//纵向偏移值计入正负头影响
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.RODEND.bIncOddEffect)
			strcpy(sText,"Yes");
		else 
			strcpy(sText,"No");
#else 
		if(des_para.RODEND.bIncOddEffect)
			strcpy(sText,"是");
		else 
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("des_para.RODEND.norm_offset")==id)//法向偏移量
	{
		CDesignThickPara thick(des_para.RODEND.norm_offset);
		sprintf(sText,"%d",thick.Thick(NULL));
	}
	else if(GetPropID("des_para.LINEPARTNODE.hLinePart")==id)	//角钢句柄
		sprintf(sText,"0X%X",des_para.RODNODE.hRod);
	else if(GetPropID("des_para.LINEPARTNODE.hNode")==id)	//基准节点句柄
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
			strcpy(sText,"是");
		else 
			strcpy(sText,"否");
#endif
	}
	else if(GetPropID("des_para.LINEPARTNODE.direction")==id)	
	{	//0.基准角钢方向:始->终;1.终->始
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.RODNODE.direction==0)
			strcpy(sText,"0.Start->End");
		else 
			strcpy(sText,"1.End->Start");
#else 
		if(des_para.RODNODE.direction==0)
			strcpy(sText,"0.始->终");
		else 
			strcpy(sText,"1.终->始");
#endif
	}
	else if(GetPropID("des_para.LINEPARTNODE.offset_wing")==id)	
	{	//0.X肢;1.Y肢
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.RODNODE.offset_wing==0)
			strcpy(sText,"0.X Leg");
		else 
			strcpy(sText,"1.Y Leg");
#else 
		if(des_para.RODNODE.offset_wing==0)
			strcpy(sText,"0.X肢");
		else 
			strcpy(sText,"1.Y肢");
#endif
	}
	else if(GetPropID("des_para.LINEPARTNODE.wing_offset_style")==id)
	{	//0.g;1.g1;2.g2;3.g3;4.自定义
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
			strcpy(sText,"自定义");
#endif
	}	
	else if(GetPropID("des_para.LINEPARTNODE.wing_offset_dist")==id)
	{
		sprintf(sText,"%f",des_para.RODNODE.wing_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.LINEPARTNODE.len_offset_dist")==id)	
	{	//纵向偏移值
		sprintf(sText,"%f",des_para.RODNODE.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.LINEPARTNODE.norm_offset")==id)	//法向偏移量
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
	{	//0.g;1.g1;2.g2;3.g3;4.自定义
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
			strcpy(sText,"自定义");
#endif
	}
	else if(GetPropID("des_para.AXIS_INTERS.wing_offset_style2")==id)
	{	//0.g;1.g1;2.g2;3.g3;4.自定义
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
			strcpy(sText,"自定义");
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
	else if(GetPropID("des_para.AXIS_INTERS.norm_offset")==id)	//法向偏移量;
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
			sprintf(sText,"0.X轴");
		else if(des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			sprintf(sText,"1.Y轴");
		else 
			sprintf(sText,"2.Z轴");
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
			sprintf(sText,"0.X轴");
		else if(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2==1)
			sprintf(sText,"1.Y轴");
		else 
			sprintf(sText,"2.Z轴");
#endif
	}
	else if(GetPropID("des_para.RODEND.hRod")==id)	//角钢句柄
		sprintf(sText,"0X%X",des_para.RODEND.hRod);
	else if(GetPropID("des_para.RODEND.direction")==id)	
	{	//0.基准角钢方向:始->终;1.终->始
#ifdef AFX_TARG_ENU_ENGLISH
		if(des_para.RODEND.direction==0)
			strcpy(sText,"0.Start->End");
		else 
			strcpy(sText,"1.End->Start");
#else 
		if(des_para.RODEND.direction==0)
			strcpy(sText,"0.始->终");
		else 
			strcpy(sText,"1.终->始");
#endif
	}
	else if(GetPropID("des_para.RODEND.len_offset_dist")==id)	
	{	//纵向偏移值;
		sprintf(sText,"%f",des_para.RODEND.len_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.RODEND.norm_offset")==id)//法向偏移量
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
	else if(GetPropID("des_para.SCALE_NODE.hLinePart")==id)			//基准杆件句柄
		sprintf(sText,"0X%0X",des_para.SCALE_NODE.hLinePart);
	else if(GetPropID("des_para.SCALE_NODE.hStartNode")==id)		//始端节点句柄
		sprintf(sText,"0X%0X",des_para.SCALE_NODE.hStartNode);
	else if(GetPropID("des_para.SCALE_NODE.hEndNode")==id)			//终端节点句柄
		sprintf(sText,"0X%0X",des_para.SCALE_NODE.hEndNode);
	else if(GetPropID("des_para.SCALE_NODE.start_offset_dist")==id)	//始端偏移量
	{
		sprintf(sText,"%f",des_para.SCALE_NODE.start_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.end_offset_dist")==id)	//终端偏移量
	{
		sprintf(sText,"%f",des_para.SCALE_NODE.end_offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.offset_scale")==id)		//偏移比例
	{
		sprintf(sText,"%f",des_para.SCALE_NODE.offset_scale);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.SCALE_NODE.offset_dist")==id)		//在等分基础上的偏移值
	{
		sprintf(sText,"%f",des_para.SCALE_NODE.offset_dist);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.hLineTube")==id)		//钢管句柄
		sprintf(sText,"0X%X",des_para.COLUMN_INTERS.hLineTube);
	else if(GetPropID("des_para.COLUMN_INTERS.axis_vec")==id)
	{
		f3dPoint axis_vec(des_para.COLUMN_INTERS.AxisVector.axis_vec[0],des_para.COLUMN_INTERS.AxisVector.axis_vec[1],des_para.COLUMN_INTERS.AxisVector.axis_vec[2]);
		sText.ConvertFromPoint(axis_vec);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.axis_vec.x")==id)		//柱面轴向正方向
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
	else if(GetPropID("des_para.COLUMN_INTERS.sect_center.x")==id)	//截面圆心点
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
	else if(GetPropID("des_para.COLUMN_INTERS.sect_norm.x")==id)	//截面法向，NULL表示柱面的垂直截面
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
	else if(GetPropID("des_para.COLUMN_INTERS.ray_vec_style")==id)	//射线方向定义方式
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
			strcpy(sText,"0.直接指定");
		else if(des_para.COLUMN_INTERS.ray_vec_style==1)
			strcpy(sText,"1.指定基准边");
		else if(des_para.COLUMN_INTERS.hLineTube>0x20 && des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"2.以钢管基准边为基准");
		else if(des_para.COLUMN_INTERS.hLineTube>0x20 && des_para.COLUMN_INTERS.ray_vec_style==2)
			strcpy(sText,"3.以钢管焊道线为基准");
#endif
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_vec_std_style")==id)
	{
		f3dPoint rec_vector(des_para.COLUMN_INTERS.RayVector.ray_vec[0],des_para.COLUMN_INTERS.RayVector.ray_vec[1],des_para.COLUMN_INTERS.RayVector.ray_vec[2]);
		sprintf(sText,"%s",CStdVector::GetVecTypeString(rec_vector));
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_vec.x")==id)		//射线方向
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
	else if(GetPropID("des_para.COLUMN_INTERS.ray_angle")==id)		//偏移角度
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.ray_angle);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.column_r")==id)		//柱面半径
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.column_r);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_para_offset")==id)	//射线平行移动距离
	{
		sprintf(sText,"%f",des_para.COLUMN_INTERS.ray_para_offset);
		SimplifiedNumString(sText);
	}
	else if(GetPropID("des_para.COLUMN_INTERS.bSpecRo")==id)			//辅助半径定义方式
	{
#ifdef AFX_TARG_ENU_ENGLISH
		if(!des_para.COLUMN_INTERS.bSpecRo)
			strcpy(sText,"0.Ray Line And Column Intersection");
		else
			strcpy(sText,"1.Column Center");
#else 
		if(!des_para.COLUMN_INTERS.bSpecRo)
			strcpy(sText,"0.射线与柱面交点");
		else
			strcpy(sText,"1.圆柱柱心");
#endif
	}
	else if(GetPropID("des_para.COLUMN_INTERS.ray_offset_r")==id)	//辅助半径
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
	hVicePart=0;				//工作法线基准角钢
	hCrossPart=0;
	norm_wing=direction=0;	//0.X肢;1.Y肢	//方向0.朝外;1.朝里 
	norm_style=0;			//0.指定;1.角钢肢法线;2交叉面法线
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
	else if(CTower::IsPrevDeflateVersion(version,doc_type))	//文件瘦身之前的版本
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
	if(norm_style==2)	//杆件交叉面法线定位
		buffer.WriteInteger(hCrossPart);
	buffer.WriteWord(norm_wing);
	buffer.WritePoint(nearVector);
	if(norm_style==0||norm_style==4)	//指定法线或基准构件上的指定方向
		buffer.WritePoint(vector);
	if(norm_style==5)
		buffer.WriteDouble(rot_ang);
}
void CLDSVector::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<法线设计参数>\n");
	if (norm_style==0)//0.指定;1.角钢肢法线;2.杆件交叉面法线3.构件工作法线4.基准构件上的相对方向5.基准杆件上的特征方向
		fprintf(fp,"<定位方式 norm_style=\"0.指定\"/>\n");
	else if (norm_style==1)
		fprintf(fp,"<定位方式 norm_style=\"1.角钢肢法线\"/>\n");
	else if (norm_style==2)
		fprintf(fp,"<定位方式 norm_style=\"2.杆件交叉面法线\"/>\n");
	else if (norm_style==3)
		fprintf(fp,"<定位方式 norm_style=\"3.构件工作法线\"/>\n");
	else if (norm_style==4)
		fprintf(fp,"<定位方式 norm_style=\"4.基准构件上的相对方向\"/>\n");
	else if (norm_style==5)
		fprintf(fp,"<定位方式 norm_style=\"5.基准杆件上的特征方向\"/>\n");
	if (direction==0)
		fprintf(fp,"<朝向 direction=\"0.朝外\"/>\n");
	else if (direction==1)
		fprintf(fp,"<朝向 direction=\"1.朝里\"/>\n");
	fprintf(fp,"<工作法线基准杆件 hVicePart=\"OX%X\"/>\n",hVicePart);
	if(norm_style==2)	//杆件交叉面法线定位
		fprintf(fp,"<交叉杆件 hCrossPart=\"OX%X\"/>\n",hCrossPart);
	if (norm_wing==0)
		fprintf(fp,"<基准肢 norm_wing=\"0.X肢\"/>\n");
	else if (norm_wing==1)				
		fprintf(fp,"<基准肢 norm_wing=\"1.Y肢\"/>\n");
	sprintf(sText,"%f",nearVector.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",nearVector.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",nearVector.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<近似的方向矢量 nearVector=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	if(norm_style==0||norm_style==4)	//指定法线或基准构件上的指定方向
	{
		sprintf(sText,"%f",vector.x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",vector.y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",vector.z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<实际所代表的方向矢量 vector=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	}
	if(norm_style==5)
	{
		sprintf(sText,"%f",rot_ang);
		SimplifiedNumString(sText);
		fprintf(fp,"<绕钢管逆时针旋转指定的角度 rot_ang=\"%s\"/>\n",sText);
	}
	fprintf(fp,"</法线设计参数>\n");
}
void CLDSVector::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(CLDSObject::_console==NULL||m_pTower==NULL)	//已调用过)
		return;
	CLDSPart* pPart;
	if(norm_style>0)	//角钢肢法线等
	{
		pPart=m_pTower->FromPartHandle(hVicePart);
		if(pPart!=NULL)
			pPart->AddToConsoleObjBuf(bAddRelaObj&&bAddSubLevelRelaObj);
	}
	if(norm_style==2)	//杆件交叉面法线
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
			strcpy(error_info,"The handle of specified angle is illegal！");
#else 
			strcpy(error_info,"指定角钢句柄非法！");
#endif
		return FALSE;
	}
	else if(norm_style==2)
	{
		if(pTower->FromPartHandle(hVicePart,CLS_LINEPART)==NULL)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified rod is illegal！");
#else 
				strcpy(error_info,"指定基准杆件句柄非法！");
#endif
			return FALSE;
		}
		if(pTower->FromPartHandle(hCrossPart,CLS_LINEPART)==NULL)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified cross-bracing rod is illegal！");
#else 
				strcpy(error_info,"指定交叉杆件句柄非法！");
#endif
			return FALSE;
		}
	}
	else if(norm_style==5)
	{	//杆件上的特征方向
		CLDSLinePart *pLinePart=(CLDSLinePart*)pTower->FromPartHandle(hVicePart,CLS_LINEPART);
		if(pLinePart==NULL)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified rod is illegal！");
#else 
				strcpy(error_info,"指定基准杆件句柄非法！");
#endif
			return FALSE;
		}
		if(!pLinePart->IsAngle()&&!pLinePart->IsTube()&&pLinePart->GetClassTypeId()!=CLS_LINESLOT&&direction>2)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The specified datum rod isn't angle, tube or [ steel handle！");
#else 
				strcpy(error_info,"指定的基准杆件不是角钢、钢管或槽钢句柄！");
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
				strcpy(error_info,"The handle of specified part is illegal！");
#else 
				strcpy(error_info,"指定基准构件句柄非法！");
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
			norm_wing=1-norm_wing;	//1.角钢肢法线
		else if(replacer.IsReverseWingXAndWingY()&&norm_style==5&&direction>1)
		{	//基准杆件上的特征方向(hVicePart指向不是角钢时IsReverseWingXAndWingY()!=true) wjh-2016.6.14
			if(direction==2||direction==3)
				direction+=2;
			else if(direction==4||direction==5)
				direction-=2;
			else//direction>5视为5
				direction=3;
		}
		if(replacer.IsReverseStartAndEnd()&&norm_style==5&&direction<=1)
			direction=1-direction;	//调转始末端
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
	if(norm_style==0)	//指定方向矢量
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
	vector.Set();	//非指定方向时在此处初始化vector wht 11-05-24
	//try{
		if(norm_style==2&&pLinePart.IsHasPtr()&&pCrossLinePart.IsHasPtr())
		{	//杆件交叉面法线
			f3dPoint vec=pLinePart.LinePartPointer()->End()-pLinePart.LinePartPointer()->Start();
			f3dPoint cross_vec=pCrossLinePart.LinePartPointer()->End()-pCrossLinePart.LinePartPointer()->Start();
			normalize(vec);
			normalize(cross_vec);
			if(fabs(vec*cross_vec)>EPS_COS)	//两杆件接近共线，此时共面法线应两杆件求叉积
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
		{	//角钢肢法线
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
				logerr.Log("角钢肢法线类型的参数方向指定了不合理的矢量朝向{%d}",direction);
#endif
		}
		else if(norm_style==3&&pDatumPart.IsHasPtr())
		{	//构件工作法线
			if(direction==0)	//朝外
				vector=pDatumPart->ucs.axis_z;
			else
				vector=-pDatumPart->ucs.axis_z;
			if(pDatumPart->GetClassTypeId()==CLS_PARAMPLATE&&norm_wing==10)
			{	//十字插板上与主板垂直的板上的螺栓
				if(direction==0)	//朝外
					vector=pDatumPart->ucs.axis_y;
				else
					vector=-pDatumPart->ucs.axis_y;
			}
			else if(pDatumPart->GetClassTypeId()==CLS_PLATE&&reside_plane>0&&reside_plane<pDatumPart.PlatePointer()->face_N)
			{	//处理火曲面上的挂线孔虚拟螺栓 wjh-2015.2.7
				if(direction==0)	//朝外
					vector=pDatumPart.PlatePointer()->huoqufacenorm[reside_plane-1];
				else
					vector=-pDatumPart.PlatePointer()->huoqufacenorm[reside_plane-1];
			}
			normalize(vector);
		}
		else if(norm_style==4&&pDatumPart.IsHasPtr())
		{	//基准构件坐标系下的相对方向
			if(!nearVector.IsZero())
			{
				vector=nearVector;
				vector_trans(vector,pDatumPart->ucs,TRUE);
				normalize(vector);
			}
		}
		else if(norm_style==5&&pLinePart.IsHasPtr())
		{	//基准杆件上的特征方向
			if(direction<2)
			{
				if(direction==0)
					vector=pLinePart.LinePartPointer()->End()-pLinePart.LinePartPointer()->Start();
				else if(direction==1)
					vector=pLinePart.LinePartPointer()->Start()-pLinePart.LinePartPointer()->End();
				normalize(vector);
			}
			else if(pLinePart->IsAngle())
			{	//基准杆件是角钢
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
			{	//基准杆件是钢管
				if(direction==2||direction==3)
				{	
					double rot_radang=rot_ang*RADTODEG_COEF;
					f3dPoint axis_vec=pLinePart.LineTubePointer()->End()-pLinePart.LineTubePointer()->Start();
					vector=pLinePart.LineTubePointer()->WorkPlaneNorm();
					if(direction==2)
					{	//自基准边平面法向绕钢管逆时针旋转rot_ang角度后的方向
						if(rot_radang!=0)
							RotateVectorAroundVector(vector,rot_radang,axis_vec);
					}
					else if(direction==3)
					{	//自焊道线平面法向绕钢管逆时针旋转rot_ang角度后的方向
						double weld_ang=pLinePart.LineTubePointer()->CalWeldRoadAngle();
						if(weld_ang+rot_radang!=0)
							RotateVectorAroundVector(vector,weld_ang+rot_radang,axis_vec);
					}
				}
				//新增钢管特征方向 4.钢管始端切割面法线方向，其他.钢管终端切割面法线 wht 12-09-05
				else if(direction==4)
					vector=pLinePart.LineTubePointer()->m_tJointStart.cutPlaneNorm;
				else 
					vector=pLinePart.LineTubePointer()->m_tJointEnd.cutPlaneNorm;
			}
			else if(pLinePart->GetClassTypeId()==CLS_LINEFLAT)
			{	//基准杆件是槽钢 2.扁铁法线正向+，3.扁铁法线负向-
				pLinePart.pFlat->BuildUCS();
				if(direction==2)
					vector= pLinePart.pFlat->WorkPlaneNorm();
				else if(direction==3)
					vector=-pLinePart.pFlat->WorkPlaneNorm();
			}
			else if(pLinePart->GetClassTypeId()==CLS_LINESLOT)
			{	//基准杆件是槽钢 2.槽钢肢法线方向(X轴方向)+，3.槽钢肢法线方向(X轴方向)-
				//				 4.槽钢底面法线方向(Y轴方向)+，5.槽钢底面法线方向(Y轴方向)- wht 12-09-26
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
//调转里外方向,对于钢管上基准旋转方向时返回false
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
	if(direction%2==0)	//偶数加1
		direction+=1;
	else				//奇数减1
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
		CLDSObject::Log2File()->Log("未输入当前适用模型,无法进行参数化方向的对称",norm_style);
		return;
	}
	UpdateVector(pModel);		//srcVector未计算前进行对称，可能导致当前肢判断错误 wht 16-09-09
	mirVector.vector=GetMirVector(vector,mirinfo);
	//构件上相对方向时nearVector表示局部坐标系下方向不能反号，应在后续代码进行判断wjh-2016.9.19
	mirVector.nearVector=norm_style!=4 ? GetMirVector(nearVector,mirinfo) : nearVector;
	mirVector.norm_style=norm_style;
	if(norm_style==1)	//角钢肢法线
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
			CLDSObject::Log2File()->Log("参数化方向中的肢法线参照角钢句柄'0X%X'不合法,无法进行参数化方向的对称",hVicePart);
		if(pAngle!=NULL && pMirAngle==NULL)
			CLDSObject::Log2File()->Log("参数化方向中的肢法线参照角钢句柄'0X%X'未找相应的对称角钢,无法进行参数化方向的对称",hVicePart);
#endif
		if(pMirAngle)
		{
			mirVector.hVicePart=pMirAngle->handle;
			f3dPoint datum_wing_norm;
			if(norm_wing==0)	//X肢法线
				datum_wing_norm=pAngle->get_norm_x_wing();
			else
				datum_wing_norm=pAngle->get_norm_y_wing();
			if(direction==1)	//朝里
				datum_wing_norm*=-1;
			f3dPoint dest_wing_norm=mirinfo.MirVector(datum_wing_norm);
			double ddx=dest_wing_norm*pMirAngle->get_norm_x_wing();
			double ddy=dest_wing_norm*pMirAngle->get_norm_y_wing();
			if(fabs(ddx)>fabs(ddy))
			{	//X肢为基准肢
				mirVector.norm_wing=0;
				mirVector.direction= ddx>0?0:1;
			}
			else
			{	//Y肢为基准肢
				mirVector.norm_wing=1;
				mirVector.direction= ddy>0?0:1;
			}
		}
	}
	else if(norm_style==2)//杆件交叉面法线
	{
		MirPartHandle(pModel,&mirVector.hVicePart,hVicePart,mirinfo);
		MirPartHandle(pModel,&mirVector.hCrossPart,hCrossPart,mirinfo);
	}
	else if(norm_style==3)//构件工作法线
	{
		GEPOINT datum_wing_norm,dest_wing_norm;
		CLDSPart* pDatumPart=pModel->FromPartHandle(hVicePart);
		CLDSPart* pMirPart=NULL;
		if(pDatumPart!=NULL)
		{
			pMirPart=pDatumPart->GetMirPart(mirinfo);
			datum_wing_norm=pDatumPart->ucs.axis_z;
			if(direction==1)	//朝里
				datum_wing_norm*=-1;
			dest_wing_norm=mirinfo.MirVector(datum_wing_norm);
			if(dest_wing_norm*mirVector.vector<0)
				direction=1-direction;	//反向
		}
	}
	else if(norm_style==4)//基准构件上的相对方向
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
			mirVector.nearVector*=-1;	//反向
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
				direction%2==0?direction+=1:direction-=1;	//偶数加1, 奇数减1
		}
		else if(norm_style!=0)
		{
			CLDSObject::Log2File()->Log("暂不支持方式%d类型的参数化方向的对称",norm_style);
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
			return "0.朝外";
		else
			return "1.朝里";
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
			return "0.始->终";
		else if(direction==1)
			return "1.终->始";
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
					return "2.X肢法线+";
				else if(direction==3)
					return "3.X肢法线-";
				else if(direction==4)
					return "4.Y肢法线+";
				else 
					return "5.Y肢法线-";
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
					return "2.自基准面旋转";
				else if(direction==3)
					return "3.自焊道面旋转";
				else if(direction==4)
					return "4.始端切割平面法线";
				else
					return "5.终端切割平面法线";
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
					return "2.槽钢肢法线+";
				else if(direction==3)
					return "3.槽钢肢法线-";
				else if(direction==4)
					return "4.槽钢侧法线+";
				else 
					return "5.槽钢侧法线-";
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
			return "0.正向";
		else
			return "1.反向";
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
		return "0.朝外|1.朝里";
	else if(norm_style==5)
	{
		CSuperSmartPtr<CLDSPart>pDatumPart=m_pTower->FromPartHandle(hVicePart);
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsAngle())
			return "0.始->终|1.终->始|2.X肢法线+|3.X肢法线-|4.Y肢法线+|5.Y肢法线-";
		else if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
			return "0.始->终|1.终->始|2.自基准面旋转|3.自焊道面旋转|4.始端切割平面法线|5.终端切割平面法线";
		else if(pDatumPart.IsHasPtr()&&pDatumPart->GetClassTypeId()==CLS_LINESLOT)
			return "0.始->终|1.终->始|2.槽钢肢法线+|3.槽钢肢法线-|4.槽钢侧法线+|5.槽钢侧法线-";
		else 
			return "0.始->终|1.终->始";
	}
	else 
		return "0.正向|1.反向";
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
		return "Forward direction is the plus work normal  of datum part；reverse direction is the minus work normal  of datum part";
#else 
	if(norm_style==1)
		return "0.朝外 1.朝里";
	else if(norm_style==5)
	{
		CSuperSmartPtr<CLDSPart>pDatumPart=m_pTower->FromPartHandle(hVicePart);
		if(pDatumPart.IsHasPtr()&&pDatumPart->IsAngle())
			return "0.杆件始->终方向,1.杆件终->始方向,2.角钢X肢法线方向,3.角钢X肢法线负方向,4.角钢Y肢法线方向,5.角钢Y肢法线负方向";
		else if(pDatumPart.IsHasPtr()&&pDatumPart->IsTube())
			return "0.杆件始->终方向\n 1.杆件终->始方向\n 2.自钢管基准线平面绕轴线旋转指定角度°\n 3.自钢管焊道线平面绕轴线旋转指定角度°\n 4.钢管始端切割平面法线方向\n 5.钢管终端切割平面法线方向\n";
		else if(pDatumPart.IsHasPtr()&&pDatumPart->GetClassTypeId()==CLS_LINESLOT)
			return "0.杆件始->终方向,1.杆件终->始方向,2.槽钢法线方向,3.槽钢法线负方向,4.槽钢肢法线方向,5.槽钢肢法线负方向";
		else 
			return "0.杆件始->终方向,1.杆件终->始方向";
	}
	else 
		return "正向是指基准构件的正工作法线方向；反向是指基准构件的负工作法线方向";
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
		return "1.单角钢肢法线";
	else if(norm_style==2)
		return "2.两杆件交叉面法线";
	else if(norm_style==3)
		return "3.构件工作法线";
	else if(norm_style==4)
		return "4.构件上的相对方向";
	else if(norm_style==5)
		return "5.杆件上的特征方向";
	else if(norm_style==6)
		return "6.由另一肢确定";
	else //if(norm_style==0)
		return "0.指定方向";
#endif

}
char *CLDSVector::GetNormStyleCmbItems()
{
#ifdef AFX_TARG_ENU_ENGLISH
	return "0.Specify Normal|1.Single Angle Leg Normal|2.Two Rod Cross-bracing Normal|3.Part Work Normal|4.Part Relative Direction|5.Rod Characteristic Direction";
#else 
	return "0.指定方向|1.单角钢肢法线|2.两杆件交叉面法线|3.构件工作法线|4.构件上的相对方向|5.杆件上的特征方向";
#endif
}

CHashStrList<PROPLIST_ITEM>CLDSVector::propHashtable;	//属性字符串哈希表
CXhChar100 CLDSVector::HELPLINK_URL;
CHashSet<PROPLIST_ITEM*>CLDSVector::hashPropItemById;
CHashTable<DWORD>CLDSVector::propStatusHashtable;	//属性状态哈希表
void CLDSVector::InitPropHashtable()						//初始化属性哈希表
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
	CLDSVector::AddPropItem("vector",PROPLIST_ITEM(id++,"火曲法线"));
	CLDSVector::AddPropItem("vector.x",PROPLIST_ITEM(id++,"火曲法线X轴方向坐标"));
	CLDSVector::AddPropItem("vector.y",PROPLIST_ITEM(id++,"火曲法线Y轴方向坐标"));
	CLDSVector::AddPropItem("vector.z",PROPLIST_ITEM(id++,"火曲法线Z轴方向坐标"));
	CLDSVector::AddPropItem("norm_style",PROPLIST_ITEM(id++,"法线定位方式"));
	CLDSVector::AddPropItem("hVicePart",PROPLIST_ITEM(id++,"基准杆件"));
	CLDSVector::AddPropItem("hCrossPart",PROPLIST_ITEM(id++,"交叉参照杆件"));
	CLDSVector::AddPropItem("norm_wing",PROPLIST_ITEM(id++,"基准肢"));
	CLDSVector::AddPropItem("direction",PROPLIST_ITEM(id++,"朝向"));
	CLDSVector::AddPropItem("rot_ang",PROPLIST_ITEM(id++,"旋转角度"));
	CLDSVector::AddPropItem("VectorType",PROPLIST_ITEM(id++,"指定方向"));
	CLDSVector::AddPropItem("nearVectorType",PROPLIST_ITEM(id++,"近似方向"));
	CLDSVector::AddPropItem("nearVector.x",PROPLIST_ITEM(id++,"X分量"));
	CLDSVector::AddPropItem("nearVector.y",PROPLIST_ITEM(id++,"Y分量"));
	CLDSVector::AddPropItem("nearVector.z",PROPLIST_ITEM(id++,"Z分量"));
#endif
}
int CLDSVector::GetPropValueStr(long id, char *valueStr,UINT nMaxStrBufLen/*=100*/)	//根据属性ID得到属性值
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
		strcpy(sText,GetNormStyleDes());	//法向属性
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
			strcpy(sText,"0.X肢法线");
		else
			strcpy(sText,"1.Y肢法线");
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
int CLDSVector::GetPropStatus(long id)					//得到属性状态
{
	DWORD dwStatus;
	if(CLDSPoint::propStatusHashtable.GetValueAt(id,dwStatus))
	{
		if(dwStatus==STATUS_EXPAND)
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
}
BOOL CLDSVector::SetPropStatus(long id,BOOL bHide)		//设置属性状态
{
	DWORD dwStatus;
	if(bHide)
		dwStatus = STATUS_COLLAPSE;
	else
		dwStatus = STATUS_EXPAND;
	return CLDSPoint::propStatusHashtable.SetValueAt(id,dwStatus);
}
long CLDSVector::GetPropID(char* propStr)			//得到属性ID
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
	{	//角钢楞点定位
		if(pTower->FromPartHandle(des_para.RODEND.hRod,CLS_LINEPART)==NULL)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified rod is illegal!");
#else 
				strcpy(error_info,"指定杆件句柄非法!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==2)
	{	//杆件节点定位
		if(pTower->FromPartHandle(des_para.RODNODE.hRod,CLS_LINEPART)==NULL)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified rod is illegal!");
#else 
				strcpy(error_info,"指定杆件句柄非法!");
#endif
			return FALSE;
		}
		if(pTower->FromNodeHandle(des_para.RODNODE.hNode)==NULL)
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified datum node is illegal!");
#else 
				strcpy(error_info,"指定基准节点句柄非法!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==3)
	{	//心线交点
		if(!pTower->FromPartHandle(des_para.AXIS_INTERS.hDatum1,CLS_LINEPART) || 
		   !pTower->FromPartHandle(des_para.AXIS_INTERS.hDatum2,CLS_LINEPART))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified rod is illegal!");
#else 
				strcpy(error_info,"指定杆件句柄非法!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==4)	
	{	//基准线空间交点
		if(!pTower->FromLineHandle(des_para.AXIS_INTERS.hDatum1) || 
		   !pTower->FromLineHandle(des_para.AXIS_INTERS.hDatum2))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified datum line is illegal!");
#else 
				strcpy(error_info,"指定基准线句柄非法!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==5)	
	{	//基准线上轴向坐标不变点
		if(!pTower->FromLineHandle(des_para.DATUMLINE.hDatumLine))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified datum line is illegal!");
#else 
				strcpy(error_info,"指定基准线句柄非法!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==6)	
	{	//基准面上指定轴向点
		if(!pTower->FromPlaneHandle(des_para.DATUMPLANE.hDatumPlane))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified datum plane is illegal!");
#else 
				strcpy(error_info,"指定基准面句柄非法!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==7)	
	{	//角钢内外包铁楞点
		if(!pTower->FromPartHandle(des_para.RODEND.hRod,CLS_LINEANGLE))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified angle is illegal!");
#else 
				strcpy(error_info,"指定角钢句柄非法!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==8)	
	{	//与节点同位
		if(!pTower->FromNodeHandle(des_para.hNode))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified node is illegal!");
#else 
				strcpy(error_info,"指定节点句柄非法!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==9)	
	{	//指定构件上的相对坐标
		if(!pTower->FromPartHandle(des_para.hPart))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified part is illegal!");
#else 
				strcpy(error_info,"指定构件句柄非法!");
#endif
			return FALSE;
		}
	}
	else if(datum_pos_style==10) 
	{	//两节点间的比例等分点
		if(!pTower->FromPartHandle(des_para.SCALE_NODE.hLinePart,CLS_LINEPART))
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"The handle of specified rod is illegal!");
#else 
				strcpy(error_info,"指定杆件句柄非法!");
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
				strcpy(error_info,"指定节点句柄非法!");
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
	m_ciLiftType=0;				//默认圆弧预拱
	m_cRotAxis='Y';				//预拱时旋转轴'X':X轴;'Y':Y轴
	m_xRotOrg.Set(0,0,0);		//预拱旋转基点
	m_xDatumLiftPos.Set(0,0,0);	//基准预拱点初始位置
	m_fLiftHeight=0;			//基准预拱点的预拱值,mm
	m_pModel=NULL;
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
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
	{	//圆弧预拱
		double tana=m_fLiftHeight/deita;
		double rot_ang=atan(tana);
		if(!bInRightSide)
			rot_ang=-rot_ang;
		f3dPoint result=initPos;
		rotate_point_around_axis(result,rot_ang,axis);
		return result;
	}
	else
	{	//垂直预拱
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
//进行逆向预拱还原运算（主要用于更改预拱后，指定角钢预拱前楞点坐标）wjh-2015.5.19
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
	{	//圆弧预拱
		double tana=m_fLiftHeight/deita;
		double rot_ang=atan(tana);
		if(!bInRightSide)
			rot_ang=-rot_ang;
		f3dPoint result=arcPos;
		rotate_point_around_axis(result,-rot_ang,axis);
		return result;
	}
	else
	{	//垂直预拱
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
	//TODO:未处理垂直预拱对矢量方向的影响 wjh-2018.5.15
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
		(doc_type==5&&version>=1030401))	//TDA(V1,3,4,1)	吉林骏天在用的V1.3.4.0因临时发布未包含dwPermission属性 wjh-2017.5.18
		buffer.ReadDword(&dwPermission);
	buffer.ReadInteger(&iNo);			//预拱序号
	buffer.ReadString(name,50);
	if( version==0||
		(doc_type==1&&version>=5010700)||	//TMA(V5.1.7.0)
		(doc_type==2&&version>=2000800)||	//LMA(V2.0.8.0)
		(doc_type==4&&version>=1030701)||	//LDS(V1.3.7.1)
		(doc_type==5&&version>=1030701))	//TDA(V1.3.7.1)
		buffer.ReadByte(&m_ciLiftType);
	buffer.ReadByte(&m_cRotAxis);		//预拱时旋转轴'X':X轴;'Y':Y轴
	buffer.ReadPoint(m_xRotOrg);		//预拱旋转基点
	buffer.ReadPoint(m_xDatumLiftPos);	//基准预拱点初始位置
	buffer.ReadDouble(&m_fLiftHeight);	//基准预拱点的预拱值,mm
}
void CLDSArcLift::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if( version==0||
		(doc_type==1&&version>=5000400)||	//TMA(V5,0,4,0)
		(doc_type==2&&version>=2000400)||	//LMA(V2,0,4,0)
		(doc_type==4&&version>=1030400)||	//LDS(V1,3,4,0)
		(doc_type==5&&version>=1030401))	//TDA(V1,3,4,1)
		buffer.WriteDword(dwPermission);
	buffer.WriteInteger(iNo);			//预拱序号
	buffer.WriteString(name);
	if( version==0||
		(doc_type==1&&version>=5010700)||	//TMA(V5.1.7.0)
		(doc_type==2&&version>=2000800)||	//LMA(V2.0.8.0)
		(doc_type==4&&version>=1030701)||	//LDS(V1.3.7.1)
		(doc_type==5&&version>=1030701))	//TDA(V1.3.7.1)
		buffer.WriteByte(m_ciLiftType);
	buffer.WriteByte(m_cRotAxis);		//预拱时旋转轴'X':X轴;'Y':Y轴
	buffer.WritePoint(m_xRotOrg);		//预拱旋转基点
	buffer.WritePoint(m_xDatumLiftPos);	//基准预拱点初始位置
	buffer.WriteDouble(m_fLiftHeight);	//基准预拱点的预拱值,mm
}
void CLDSArcLift::ToXmlFile(FILE* fp,DWORD schema)
{
	char sText[500]="";
	char sPosX[50]="",sPosY[50]="",sPosZ[50]="";
	fprintf(fp,"<圆弧预拱信息>\n");
	fprintf(fp,"<预拱序号 iNo=\"%d\"/>\n",iNo);
	fprintf(fp,"<预拱名称 name=\"%s\"/>\n",name);
	fprintf(fp,"<旋转轴 RotAxis=\"%c\"/>\n",m_cRotAxis);
	sprintf(sPosX,"%f",m_xDatumLiftPos.x);
	SimplifiedNumString(sPosX);
	sprintf(sPosY,"%f",m_xDatumLiftPos.y);
	SimplifiedNumString(sPosY);
	sprintf(sPosZ,"%f",m_xDatumLiftPos.z);
	SimplifiedNumString(sPosZ);
	fprintf(fp,"<预拱点初始位置 DatumListPos=\"(%s,%s,%s)\"/>\n",sPosX,sPosY,sPosZ);
	sprintf(sText,"%f",m_fLiftHeight);
	SimplifiedNumString(sText);
	fprintf(fp,"<预拱点的预拱值 LiftHeight=\"%s\"/>\n",sText);
	fprintf(fp,"</圆弧预拱信息>\n");
}
void POINT_PARA::ZeroClear()
{
	memset(this,0,sizeof(POINT_PARA));
}
void POINT_PARA::ToBuffer(BUFFER_IO* pIO)
{
	bool bHasArcLiftBeforePos=false;
	//if(	version==0||(doc_type==1&&version>=4020001)||((doc_type==2||doc_type==4||doc_type==5)&&version>=1020001))
		bHasArcLiftBeforePos=true;	//已具备自动预拱功能

	int version=0,doc_type=1;
	pIO->WriteByte(ciLocationType);
	if(ciLocationType==0)//&&bHasArcLiftBeforePos)
		pIO->WritePoint(xDatumPos);
	if(ciLocationType==1||ciLocationType==7)		//角钢楞点定位
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
	else if(ciLocationType==2)	//角钢节点定位
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
	else if(ciLocationType==3||ciLocationType==4)	//基准线空间交点
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
	else if(ciLocationType==5)	//基准线上轴向坐标不变点
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
	else if(ciLocationType==6)	//基准面上指定轴向点
	{
		pIO->WriteInteger(DATUMPLANE.hDatumPlane);
		pIO->WriteByte(DATUMPLANE.cMirTransType);
		pIO->WriteWord(DATUMPLANE.fix_axis_style_x0_y1_z2);
		if(bHasArcLiftBeforePos)
			pIO->WritePoint(xDatumPos);
	}
	else if(ciLocationType==8)	//与节点同位
		pIO->WriteInteger(hNode);
	else if(ciLocationType==9)	//指定构件上的相对坐标
	{
		pIO->WriteInteger(hPart);
		pIO->WritePoint(xDatumPos);
	}
	else if(ciLocationType==10)//两节点间的比例等分点 wht 11-01-24
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
		pIO->WriteInteger(COLUMN_INTERS.hLineTube);		//>0时以该钢管定义柱面｛axis_vec,axis_pick,column_r｝
		pIO->WriteByte(COLUMN_INTERS.cPointAndVectorFlag);
		pIO->WritePoint(COLUMN_INTERS.AxisVector.axis_vec);		//柱面的轴线正方向
		pIO->WritePoint(COLUMN_INTERS.SectCenter.sect_center);		//柱面轴线与截面的交点，即圆截面（圆或椭圆）的圆心点
		if(  version==0||
			(doc_type==1&&version>=5000300)||	//TMA V5.0.3.0
			(doc_type==2&&version>=2000300)||	//LMA V2.0.3.0
			(doc_type==3&&version>=2010300)||	//TSA V2.1.3.0
			(doc_type==4&&version>=1030300)||	//LDS V1.3.3.0
			(doc_type==5&&version>=1030300))	//TDA V1.3.3.0
		{
			pIO->WriteInteger(COLUMN_INTERS.SectCenterParam.hNode);		//截面圆心的偏移基准节点
			pIO->WriteDouble(COLUMN_INTERS.SectCenterParam.len_offset);	//截面圆心自基准节点的偏移量
		}
		pIO->WritePoint(COLUMN_INTERS.SectNorm.sect_norm);		//截面法向：为空时表示为柱面的垂直截面
		pIO->WriteByte(COLUMN_INTERS.ray_vec_style);	//0.ray_vec为直接定义的射线绝对方向;>0:指定柱面起始边，并根据ray_angle确定射线方向
		pIO->WritePoint(COLUMN_INTERS.RayVector.ray_vec);			//射线方向
		pIO->WriteDouble(COLUMN_INTERS.ray_angle);		//ray_vec_style==1时射线方向自柱面起始边的逆时针旋转角度,°
		pIO->WriteDouble(COLUMN_INTERS.column_r);		//柱面半径
		pIO->WriteDouble(COLUMN_INTERS.ray_para_offset);//射线平行移动距离（垂直射线方向进行平行线移动,以右手螺旋正向侧移动为正，另一侧为负）
		pIO->WriteBooleanThin(COLUMN_INTERS.bSpecRo);	//true:辅射半径以距柱心投影绝对距离定；false:辅射半径以柱面射线交点向外的延伸偏量确定
		pIO->WriteDouble(COLUMN_INTERS.ray_offset_r);	//bSpecRoByOffset=true:辅射半径; bSpecRoByOffset=false:射线方向延伸偏移量
	}
}
void POINT_PARA::FromBuffer(BUFFER_IO* pIO)
{
	pIO->ReadByte(&ciLocationType);
	if(ciLocationType==0)
		pIO->ReadPoint(this->xDatumPos);	//读取直接指定坐标时预拱前位置
	if(ciLocationType==1||ciLocationType==7)		//角钢楞点定位
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
	else if(ciLocationType==2)	//角钢节点定位
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
	else if(ciLocationType==3||ciLocationType==4)	//基准线空间交点
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
	else if(ciLocationType==5)	//基准线上轴向坐标不变点
	{
		pIO->ReadInteger(&DATUMLINE.hDatumLine);
		pIO->ReadByte(&DATUMLINE.cMirTransType);
		pIO->ReadWord(&DATUMLINE.fix_axis_val_style_x0_y1_z2);
		pIO->ReadPoint(xDatumPos);	//读取直接指定坐标时预拱前位置
	}
	else if(ciLocationType==6)	//基准面上指定轴向点
	{
		pIO->ReadInteger(&DATUMPLANE.hDatumPlane);
		pIO->ReadByte(&DATUMPLANE.cMirTransType);
		pIO->ReadWord(&DATUMPLANE.fix_axis_style_x0_y1_z2);
		pIO->ReadPoint(xDatumPos);	//读取直接指定坐标时预拱前位置
	}
	else if(ciLocationType==8)	//与节点同位
		pIO->ReadInteger(&hNode);
	else if(ciLocationType==9)	//指定构件上的相对坐标
	{
		pIO->ReadInteger(&hPart);
		pIO->ReadPoint(xDatumPos);
	}
	else if(ciLocationType==10)
	{	//两节点间的比例等分点 wht 11-01-24
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
		pIO->ReadInteger(&COLUMN_INTERS.hLineTube);		//>0时以该钢管定义柱面｛axis_vec,axis_pick,column_r｝
		pIO->ReadByte(&COLUMN_INTERS.cPointAndVectorFlag);
		pIO->ReadPoint(COLUMN_INTERS.AxisVector.axis_vec);		//柱面的轴线正方向
		pIO->ReadPoint(COLUMN_INTERS.SectCenter.sect_center);		//柱面轴线与截面的交点，即圆截面（圆或椭圆）的圆心点
		pIO->ReadInteger(&COLUMN_INTERS.SectCenterParam.hNode);		//截面圆心的偏移基准节点
		pIO->ReadDouble(&COLUMN_INTERS.SectCenterParam.len_offset);	//截面圆心自基准节点的偏移量

		pIO->ReadPoint(COLUMN_INTERS.SectNorm.sect_norm);		//截面法向：为空时表示为柱面的垂直截面
		pIO->ReadByte(&COLUMN_INTERS.ray_vec_style);	//0.ray_vec为直接定义的射线绝对方向;>0:指定柱面起始边，并根据ray_angle确定射线方向
		pIO->ReadPoint(COLUMN_INTERS.RayVector.ray_vec);			//射线方向
		pIO->ReadDouble(&COLUMN_INTERS.ray_angle);		//ray_vec_style==1时射线方向自柱面起始边的逆时针旋转角度,°
		pIO->ReadDouble(&COLUMN_INTERS.column_r);		//柱面半径
		pIO->ReadDouble(&COLUMN_INTERS.ray_para_offset);//射线平行移动距离（垂直射线方向进行平行线移动,以右手螺旋正向侧移动为正，另一侧为负）
		pIO->ReadBooleanThin(&COLUMN_INTERS.bSpecRo);	//true:辅射半径以距柱心投影绝对距离定；false:辅射半径以柱面射线交点向外的延伸偏量确定
		pIO->ReadDouble(&COLUMN_INTERS.ray_offset_r);	//bSpecRoByOffset=true:辅射半径; bSpecRoByOffset=false:射线方向延伸偏移量
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
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
}
//快速设定由角钢楞点(含包铁楞点型式)的基准点参数,其中offsetWing=0:X肢偏移;1:Y肢偏移;其余为包铁楞点双肢法向偏移
void CLDSPoint::SetRapidAngleBer(THANDLE hJg,BYTE direction,BYTE wing_x0_y1_both2/*=0*/,double offsetDist/*=0.0*/,BYTE gStyle/*=4*/)
{
	if(wing_x0_y1_both2==0||wing_x0_y1_both2==1)
	{	//设为角钢端楞点
		datum_pos_style=1;
		des_para.RODEND.hRod=hJg;
		des_para.RODEND.direction=direction;
		des_para.RODEND.offset_wing=wing_x0_y1_both2;
		des_para.RODEND.wing_offset_style=gStyle;
		des_para.RODEND.wing_offset_dist=offsetDist;
	}
	else
	{	//设为包铁楞点
		datum_pos_style=7;
		des_para.RODEND.hRod=hJg;
		des_para.RODEND.direction=direction;
		CDesignThickPara offset(offsetDist);
		strncpy(des_para.RODEND.norm_offset,offset.key_str,50);
	}
}
void CLDSPoint::ToParaBuffer(BUFFER_IO* pIO)	//仅把参数化点位中的几何参数信息提取为POINT_PARA形式写入缓存
{
	pIO->WriteByte(datum_pos_style);
	if(datum_pos_style==0)
		pIO->WritePoint(datum_pos);
	if(datum_pos_style==1||datum_pos_style==7)		//角钢楞点定位
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
	else if(datum_pos_style==2)	//角钢节点定位
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
	else if(datum_pos_style==3||datum_pos_style==4)	//基准线空间交点
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
	else if(datum_pos_style==5)	//基准线上轴向坐标不变点
	{
		pIO->WriteInteger(des_para.DATUMLINE.hDatumLine);
		pIO->WriteByte(des_para.DATUMLINE.cMirTransType);
		pIO->WriteWord(des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2);
		pIO->WritePoint(datum_pos);
	}
	else if(datum_pos_style==6)	//基准面上指定轴向点
	{
		pIO->WriteInteger(des_para.DATUMPLANE.hDatumPlane);
		pIO->WriteByte(des_para.DATUMPLANE.cMirTransType);
		pIO->WriteWord(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2);
		pIO->WritePoint(datum_pos);
	}
	else if(datum_pos_style==8)	//与节点同位
		pIO->WriteInteger(des_para.hNode);
	else if(datum_pos_style==9)	//指定构件上的相对坐标
	{
		pIO->WriteInteger(des_para.hPart);
		pIO->WritePoint(datum_pos);
	}
	else if(datum_pos_style==10)//两节点间的比例等分点 wht 11-01-24
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
		pIO->WriteInteger(des_para.COLUMN_INTERS.hLineTube);		//>0时以该钢管定义柱面｛axis_vec,axis_pick,column_r｝
		pIO->WriteByte(des_para.COLUMN_INTERS.cPointAndVectorFlag);
		pIO->WritePoint(des_para.COLUMN_INTERS.AxisVector.axis_vec);		//柱面的轴线正方向
		pIO->WritePoint(des_para.COLUMN_INTERS.SectCenter.sect_center);		//柱面轴线与截面的交点，即圆截面（圆或椭圆）的圆心点
		pIO->WriteInteger(des_para.COLUMN_INTERS.SectCenterParam.hNode);		//截面圆心的偏移基准节点
		pIO->WriteDouble(des_para.COLUMN_INTERS.SectCenterParam.len_offset);	//截面圆心自基准节点的偏移量

		pIO->WritePoint(des_para.COLUMN_INTERS.SectNorm.sect_norm);		//截面法向：为空时表示为柱面的垂直截面
		pIO->WriteByte(des_para.COLUMN_INTERS.ray_vec_style);	//0.ray_vec为直接定义的射线绝对方向;>0:指定柱面起始边，并根据ray_angle确定射线方向
		pIO->WritePoint(des_para.COLUMN_INTERS.RayVector.ray_vec);			//射线方向
		pIO->WriteDouble(des_para.COLUMN_INTERS.ray_angle);		//ray_vec_style==1时射线方向自柱面起始边的逆时针旋转角度,°
		pIO->WriteDouble(des_para.COLUMN_INTERS.column_r);		//柱面半径
		pIO->WriteDouble(des_para.COLUMN_INTERS.ray_para_offset);//射线平行移动距离（垂直射线方向进行平行线移动,以右手螺旋正向侧移动为正，另一侧为负）
		pIO->WriteBooleanThin(des_para.COLUMN_INTERS.bSpecRo);	//true:辅射半径以距柱心投影绝对距离定；false:辅射半径以柱面射线交点向外的延伸偏量确定
		pIO->WriteDouble(des_para.COLUMN_INTERS.ray_offset_r);	//bSpecRoByOffset=true:辅射半径; bSpecRoByOffset=false:射线方向延伸偏移量
	}
}
void CLDSPoint::FromParaBuffer(BUFFER_IO* pIO)//从POINT_PARA导出缓存中读入参数化点位中的几何参数信息
{
	pIO->ReadByte(&datum_pos_style);
	if(datum_pos_style==0)
		pIO->ReadPoint(datum_pos);	//读取直接指定坐标时预拱前位置
	if(datum_pos_style==1||datum_pos_style==7)		//角钢楞点定位
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
	else if(datum_pos_style==2)	//角钢节点定位
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
	else if(datum_pos_style==3||datum_pos_style==4)	//基准线空间交点
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
	else if(datum_pos_style==5)	//基准线上轴向坐标不变点
	{
		pIO->ReadInteger(&des_para.DATUMLINE.hDatumLine);
		pIO->ReadByte(&des_para.DATUMLINE.cMirTransType);
		pIO->ReadWord(&des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2);
		pIO->ReadPoint(datum_pos);	//读取直接指定坐标时预拱前位置
	}
	else if(datum_pos_style==6)	//基准面上指定轴向点
	{
		pIO->ReadInteger(&des_para.DATUMPLANE.hDatumPlane);
		pIO->ReadByte(&des_para.DATUMPLANE.cMirTransType);
		pIO->ReadWord(&des_para.DATUMPLANE.fix_axis_style_x0_y1_z2);
		pIO->ReadPoint(datum_pos);	//读取直接指定坐标时预拱前位置
	}
	else if(datum_pos_style==8)	//与节点同位
		pIO->ReadInteger(&des_para.hNode);
	else if(datum_pos_style==9)	//指定构件上的相对坐标
	{
		pIO->ReadInteger(&des_para.hPart);
		pIO->ReadPoint(datum_pos);
	}
	else if(datum_pos_style==10)
	{	//两节点间的比例等分点 wht 11-01-24
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
		pIO->ReadInteger(&des_para.COLUMN_INTERS.hLineTube);		//>0时以该钢管定义柱面｛axis_vec,axis_pick,column_r｝
		pIO->ReadByte(&des_para.COLUMN_INTERS.cPointAndVectorFlag);
		pIO->ReadPoint(des_para.COLUMN_INTERS.AxisVector.axis_vec);		//柱面的轴线正方向
		pIO->ReadPoint(des_para.COLUMN_INTERS.SectCenter.sect_center);		//柱面轴线与截面的交点，即圆截面（圆或椭圆）的圆心点
		pIO->ReadInteger(&des_para.COLUMN_INTERS.SectCenterParam.hNode);		//截面圆心的偏移基准节点
		pIO->ReadDouble(&des_para.COLUMN_INTERS.SectCenterParam.len_offset);	//截面圆心自基准节点的偏移量

		pIO->ReadPoint(des_para.COLUMN_INTERS.SectNorm.sect_norm);		//截面法向：为空时表示为柱面的垂直截面
		pIO->ReadByte(&des_para.COLUMN_INTERS.ray_vec_style);	//0.ray_vec为直接定义的射线绝对方向;>0:指定柱面起始边，并根据ray_angle确定射线方向
		pIO->ReadPoint(des_para.COLUMN_INTERS.RayVector.ray_vec);			//射线方向
		pIO->ReadDouble(&des_para.COLUMN_INTERS.ray_angle);		//ray_vec_style==1时射线方向自柱面起始边的逆时针旋转角度,°
		pIO->ReadDouble(&des_para.COLUMN_INTERS.column_r);		//柱面半径
		pIO->ReadDouble(&des_para.COLUMN_INTERS.ray_para_offset);//射线平行移动距离（垂直射线方向进行平行线移动,以右手螺旋正向侧移动为正，另一侧为负）
		pIO->ReadBooleanThin(&des_para.COLUMN_INTERS.bSpecRo);	//true:辅射半径以距柱心投影绝对距离定；false:辅射半径以柱面射线交点向外的延伸偏量确定
		pIO->ReadDouble(&des_para.COLUMN_INTERS.ray_offset_r);	//bSpecRoByOffset=true:辅射半径; bSpecRoByOffset=false:射线方向延伸偏移量
	}
}
void CLDSPoint::CloneParaTo(CLDSPoint* pPoint)
{
	char pool[256]={0};	//sizeof(DES_PARA)<200
	CBuffer buff(pool,256);
	this->ToParaBuffer(&buff);
	buff.SeekToBegin();
	pPoint->FromParaBuffer(&buff);
	pos=pPoint->xRsltPosition;	//用户指定，基准线和基准面上轴向坐标不变点
}
//<DEVELOP_PROCESS_MARK nameId="CLDSPoint::Serialize">
//doc_tma1_lma2_tsa3_lds4_default0
void CLDSPoint::FromBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	//读取影射构件参数和构件从属句柄 wht 12-09-14
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
		bHasArcLiftBeforePos=true;	//已具备自动预拱功能
	buffer.ReadString(description);
	buffer.ReadDword(&dwPermission);
	if(CTower::IsPrevDeflateVersion(version,doc_type) ||
		datum_pos_style==0||datum_pos_style==5||datum_pos_style==6)
		buffer.ReadPoint(pos);
	if(datum_pos_style==0)
	{
		if(bHasArcLiftBeforePos)
			buffer.ReadPoint(datum_pos);	//读取直接指定坐标时预拱前位置
		else
			datum_pos=pos;
	}
	if(datum_pos_style==1||datum_pos_style==7)		//角钢楞点定位
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
	else if(datum_pos_style==2)	//角钢节点定位
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
	else if(datum_pos_style==3||datum_pos_style==4)	//基准线空间交点
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
	else if(datum_pos_style==5)	//基准线上轴向坐标不变点
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
			buffer.ReadPoint(datum_pos);	//读取直接指定坐标时预拱前位置
		else
			datum_pos=pos;
	}
	else if(datum_pos_style==6)	//基准面上指定轴向点
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
			buffer.ReadPoint(datum_pos);	//读取直接指定坐标时预拱前位置
		else
			datum_pos=pos;
	}
	else if(datum_pos_style==8)	//与节点同位
		buffer.ReadInteger(&des_para.hNode);
	else if(datum_pos_style==9)	//指定构件上的相对坐标
	{
		buffer.ReadInteger(&des_para.hPart);
		buffer.ReadPoint(datum_pos);
	}
	else if(datum_pos_style==10)
	{	//两节点间的比例等分点 wht 11-01-24
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
		buffer.ReadInteger(&des_para.COLUMN_INTERS.hLineTube);		//>0时以该钢管定义柱面｛axis_vec,axis_pick,column_r｝
		buffer.ReadByte(&des_para.COLUMN_INTERS.cPointAndVectorFlag);
		buffer.ReadPoint(des_para.COLUMN_INTERS.AxisVector.axis_vec);		//柱面的轴线正方向
		buffer.ReadPoint(des_para.COLUMN_INTERS.SectCenter.sect_center);		//柱面轴线与截面的交点，即圆截面（圆或椭圆）的圆心点
	if(  version==0||
		(doc_type==1&&version>=5000300)||	//TMA V5.0.3.0
		(doc_type==2&&version>=2000300)||	//LMA V2.0.3.0
		(doc_type==3&&version>=2010300)||	//TSA V2.1.3.0
		(doc_type==4&&version>=1030300)||	//LDS V1.3.3.0
		(doc_type==5&&version>=1030300))	//TDA V1.3.3.0
		{
			buffer.ReadInteger(&des_para.COLUMN_INTERS.SectCenterParam.hNode);		//截面圆心的偏移基准节点
			buffer.ReadDouble(&des_para.COLUMN_INTERS.SectCenterParam.len_offset);	//截面圆心自基准节点的偏移量
		}
		buffer.ReadPoint(des_para.COLUMN_INTERS.SectNorm.sect_norm);		//截面法向：为空时表示为柱面的垂直截面
		buffer.ReadByte(&des_para.COLUMN_INTERS.ray_vec_style);	//0.ray_vec为直接定义的射线绝对方向;>0:指定柱面起始边，并根据ray_angle确定射线方向
		buffer.ReadPoint(des_para.COLUMN_INTERS.RayVector.ray_vec);			//射线方向
		buffer.ReadDouble(&des_para.COLUMN_INTERS.ray_angle);		//ray_vec_style==1时射线方向自柱面起始边的逆时针旋转角度,°
		buffer.ReadDouble(&des_para.COLUMN_INTERS.column_r);		//柱面半径
		buffer.ReadDouble(&des_para.COLUMN_INTERS.ray_para_offset);//射线平行移动距离（垂直射线方向进行平行线移动,以右手螺旋正向侧移动为正，另一侧为负）
		buffer.ReadBooleanThin(&des_para.COLUMN_INTERS.bSpecRo);	//true:辅射半径以距柱心投影绝对距离定；false:辅射半径以柱面射线交点向外的延伸偏量确定
		buffer.ReadDouble(&des_para.COLUMN_INTERS.ray_offset_r);	//bSpecRoByOffset=true:辅射半径; bSpecRoByOffset=false:射线方向延伸偏移量
	}
}
void CLDSPoint::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	//保存影射构件参数和构件从属句柄 wht 12-09-14
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	if(ShadowParaAndBlockIdToBuffer(buffer,version,doc_type))
		return;
	buffer.WriteWord(iNo);
	buffer.WriteByte(datum_pos_style);
	buffer.WriteString(description);
	buffer.WriteDword(dwPermission);
	if(datum_pos_style==0||datum_pos_style==5||datum_pos_style==6)
		buffer.WritePoint(pos);	//用户指定，基准线和基准面上轴向坐标不变点
	bool bHasArcLiftBeforePos=false;
	if(	version==0||(doc_type==1&&version>=4020001)||((doc_type==2||doc_type==4||doc_type==5)&&version>=1020001))
		bHasArcLiftBeforePos=true;	//已具备自动预拱功能
	if(datum_pos_style==0&&bHasArcLiftBeforePos)
		buffer.WritePoint(datum_pos);
	if(datum_pos_style==1||datum_pos_style==7)		//角钢楞点定位
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
	else if(datum_pos_style==2)	//角钢节点定位
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
	else if(datum_pos_style==3||datum_pos_style==4)	//基准线空间交点
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
	else if(datum_pos_style==5)	//基准线上轴向坐标不变点
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
	else if(datum_pos_style==6)	//基准面上指定轴向点
	{
		buffer.WriteInteger(des_para.DATUMPLANE.hDatumPlane);
		buffer.WriteByte(des_para.DATUMPLANE.cMirTransType);
		buffer.WriteWord(des_para.DATUMPLANE.fix_axis_style_x0_y1_z2);
		if(bHasArcLiftBeforePos)
			buffer.WritePoint(datum_pos);
	}
	else if(datum_pos_style==8)	//与节点同位
		buffer.WriteInteger(des_para.hNode);
	else if(datum_pos_style==9)	//指定构件上的相对坐标
	{
		buffer.WriteInteger(des_para.hPart);
		buffer.WritePoint(datum_pos);
	}
	else if(datum_pos_style==10)//两节点间的比例等分点 wht 11-01-24
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
		buffer.WriteInteger(des_para.COLUMN_INTERS.hLineTube);		//>0时以该钢管定义柱面｛axis_vec,axis_pick,column_r｝
		buffer.WriteByte(des_para.COLUMN_INTERS.cPointAndVectorFlag);
		buffer.WritePoint(des_para.COLUMN_INTERS.AxisVector.axis_vec);		//柱面的轴线正方向
		buffer.WritePoint(des_para.COLUMN_INTERS.SectCenter.sect_center);		//柱面轴线与截面的交点，即圆截面（圆或椭圆）的圆心点
		if(  version==0||
			(doc_type==1&&version>=5000300)||	//TMA V5.0.3.0
			(doc_type==2&&version>=2000300)||	//LMA V2.0.3.0
			(doc_type==3&&version>=2010300)||	//TSA V2.1.3.0
			(doc_type==4&&version>=1030300)||	//LDS V1.3.3.0
			(doc_type==5&&version>=1030300))	//TDA V1.3.3.0
		{
			buffer.WriteInteger(des_para.COLUMN_INTERS.SectCenterParam.hNode);		//截面圆心的偏移基准节点
			buffer.WriteDouble(des_para.COLUMN_INTERS.SectCenterParam.len_offset);	//截面圆心自基准节点的偏移量
		}
		buffer.WritePoint(des_para.COLUMN_INTERS.SectNorm.sect_norm);		//截面法向：为空时表示为柱面的垂直截面
		buffer.WriteByte(des_para.COLUMN_INTERS.ray_vec_style);	//0.ray_vec为直接定义的射线绝对方向;>0:指定柱面起始边，并根据ray_angle确定射线方向
		buffer.WritePoint(des_para.COLUMN_INTERS.RayVector.ray_vec);			//射线方向
		buffer.WriteDouble(des_para.COLUMN_INTERS.ray_angle);		//ray_vec_style==1时射线方向自柱面起始边的逆时针旋转角度,°
		buffer.WriteDouble(des_para.COLUMN_INTERS.column_r);		//柱面半径
		buffer.WriteDouble(des_para.COLUMN_INTERS.ray_para_offset);//射线平行移动距离（垂直射线方向进行平行线移动,以右手螺旋正向侧移动为正，另一侧为负）
		buffer.WriteBooleanThin(des_para.COLUMN_INTERS.bSpecRo);	//true:辅射半径以距柱心投影绝对距离定；false:辅射半径以柱面射线交点向外的延伸偏量确定
		buffer.WriteDouble(des_para.COLUMN_INTERS.ray_offset_r);	//bSpecRoByOffset=true:辅射半径; bSpecRoByOffset=false:射线方向延伸偏移量
	}
}
//</DEVELOP_PROCESS_MARK>
void CLDSPoint::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="",sText2[500]="",sText3[500]="";
	fprintf(fp,"<基准点设计参数>\n");
	fprintf(fp,"<编号 iNo=\"%d\"/>\n",iNo);
	/*  0.无位置设计信息,直接指定1.角钢两端楞点为基准;2.杆件上节点为基准;
	3.角钢心线交点;4.基准线交点;5.基准线上轴向坐标不变点;
	6.基准面上轴向坐标不变点,7.沿角钢两肢法向偏移的包铁楞点,8.与节点同位,
	9.指定构件上的相对坐标,10.两节点间比例等分点(该类型仅用于填板定位,不在基准点对话框中显示)
	*/
	if (datum_pos_style==0)
		fprintf(fp,"<基点定位方式 datum_pos_style=\"0.直接指定\"/>\n");
	else if (datum_pos_style==1)
		fprintf(fp,"<基点定位方式 datum_pos_style=\"1.角钢两端楞点为基准\"/>\n");
	else if (datum_pos_style==2)
		fprintf(fp,"<基点定位方式 datum_pos_style=\"2.杆件上节点为基准\"/>\n");
	else if (datum_pos_style==3)
		fprintf(fp,"<基点定位方式 datum_pos_style=\"3.角钢心线交点\"/>\n");
	else if (datum_pos_style==4)
		fprintf(fp,"<基点定位方式 datum_pos_style=\"4.基准线交点\"/>\n");
	else if (datum_pos_style==5)
		fprintf(fp,"<基点定位方式 datum_pos_style=\"5.基准线上轴向坐标不变点\"/>\n");
	else if (datum_pos_style==6)
		fprintf(fp,"<基点定位方式 datum_pos_style=\"6.基准面上轴向坐标不变点\"/>\n");
	else if (datum_pos_style==7)
		fprintf(fp,"<基点定位方式 datum_pos_style=\"7.沿角钢两肢法向偏移的包铁楞点\"/>\n");
	else if (datum_pos_style==8)
		fprintf(fp,"<基点定位方式 datum_pos_style=\"8.与节点同位\"/>\n");
	else if (datum_pos_style==9)
		fprintf(fp,"<基点定位方式 datum_pos_style=\"9.指定构件上的相对坐标\"/>\n");
	else if (datum_pos_style==10)
		fprintf(fp,"<基点定位方式 datum_pos_style=\"10.两节点间比例等分点\"/>\n");
	fprintf(fp,"<描述 description=\"%s\"/>\n",description);
	if(datum_pos_style==0||datum_pos_style==5||datum_pos_style==6)
	{	//用户指定，基准线和基准面上轴向坐标不变点
		sprintf(sText,"%f",pos.x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",pos.y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",pos.z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<位置 pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	}
	if(datum_pos_style==0)
	{
		sprintf(sText,"%f",datum_pos.x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",datum_pos.y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",datum_pos.z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<相对坐标 datum_pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	}
	if(datum_pos_style==1||datum_pos_style==7)		//角钢楞点定位
	{
		if (des_para.RODEND.direction==0)
			fprintf(fp,"<基准角钢方向 des_para.RODEND.direction=\"0.始->终\"/>\n");
		else if (des_para.RODEND.direction==1)
			fprintf(fp,"<基准角钢方向 des_para.RODEND.direction=\"1.终->始\"/>\n");
		if (des_para.RODEND.bIncOddEffect)
			fprintf(fp,"<纵向偏移值计入正负头影响 des_para.RODEND.bIncOddEffect=\"true\"/>\n");
		else
			fprintf(fp,"<纵向偏移值计入正负头影响 des_para.RODEND.bIncOddEffect=\"false\"/>\n");
		fprintf(fp,"<角钢句柄 des_para.RODEND.hRod=\"0X%X\"/>\n",des_para.RODEND.hRod);
		if (des_para.RODEND.offset_wing==0)
			fprintf(fp,"<偏移肢 des_para.RODEND.offset_wing=\"0.X肢\"/>\n");
		else if (des_para.RODEND.offset_wing==1)
			fprintf(fp,"<偏移肢 des_para.RODEND.offset_wing=\"1.Y肢\"/>\n");
		sprintf(sText,"%f",des_para.RODEND.len_offset_dist);
		SimplifiedNumString(sText);
		fprintf(fp,"<纵向偏移值 des_para.RODEND.len_offset_dist=\"%s\"/>\n",sText);
		if (des_para.RODEND.wing_offset_style==0)	//0.g;1.g1;2.g2;3.g3;4.自定义
			fprintf(fp,"<肢向偏移类型 des_para.RODEND.wing_offset_style=\"0.g\"/>\n");
		else if (des_para.RODEND.wing_offset_style==1)
			fprintf(fp,"<肢向偏移类型 des_para.RODEND.wing_offset_style=\"1.g1\"/>\n");
		else if (des_para.RODEND.wing_offset_style==2)
			fprintf(fp,"<肢向偏移类型 des_para.RODEND.wing_offset_style=\"2.g2\"/>\n");
		else if (des_para.RODEND.wing_offset_style==3)
			fprintf(fp,"<肢向偏移类型 des_para.RODEND.wing_offset_style=\"3.g3\"/>\n");
		else if (des_para.RODEND.wing_offset_style==4)
			fprintf(fp,"<肢向偏移类型 des_para.RODEND.wing_offset_style=\"4.自定义\"/>\n");
		sprintf(sText,"%f",des_para.RODEND.wing_offset_dist);
		SimplifiedNumString(sText);
		fprintf(fp,"<肢向偏移值 des_para.RODEND.wing_offset_dist=\"%s\"/>\n",sText);
		fprintf(fp,"<法向偏移量 des_para.RODEND.norm_offset=\"%s\"/>\n",des_para.RODEND.norm_offset);
	}
	else if(datum_pos_style==2)	//角钢节点定位
	{
		if (des_para.RODEND.direction==0)
			fprintf(fp,"<基准角钢方向 des_para.LINEPARTNODE.direction=\"始->终\"/>\n");
		else if (des_para.RODEND.direction==1)
			fprintf(fp,"<基准角钢方向 des_para.LINEPARTNODE.direction=\"终->始\"/>\n");
		fprintf(fp,"<角钢句柄 des_para.LINEPARTNODE.hLinePart=\"0X%X\"/>\n",des_para.RODNODE.hRod);
		fprintf(fp,"<基准节点句柄 des_para.LINEPARTNODE.hNode=\"0X%X\"/>\n",des_para.RODNODE.hNode);
		fprintf(fp,"<bFaceOffset des_para.LINEPARTNODE.bFaceOffset=\"%d\"/>\n",des_para.RODNODE.bFaceOffset);
		if (des_para.RODNODE.offset_wing==0)
			fprintf(fp,"<偏移肢 des_para.LINEPARTNODE.offset_wing=\"0.X肢\"/>\n");
		else if (des_para.RODNODE.offset_wing==1)
			fprintf(fp,"<偏移肢 des_para.LINEPARTNODE.offset_wing=\"1.Y肢\"/>\n");
		sprintf(sText,"%f",des_para.RODNODE.len_offset_dist);
		SimplifiedNumString(sText);
		fprintf(fp,"<纵向偏移值 des_para.LINEPARTNODE.len_offset_dist=\"%s\"/>\n",sText);
		if (des_para.RODNODE.wing_offset_style==0)	//0.g;1.g1;2.g2;3.g3;4.自定义
			fprintf(fp,"<肢向偏移类型 des_para.LINEPARTNODE.wing_offset_style=\"0.g\"/>\n");
		else if (des_para.RODNODE.wing_offset_style==1)					
			fprintf(fp,"<肢向偏移类型 des_para.LINEPARTNODE.wing_offset_style=\"1.g1\"/>\n");
		else if (des_para.RODNODE.wing_offset_style==2)					
			fprintf(fp,"<肢向偏移类型 des_para.LINEPARTNODE.wing_offset_style=\"2.g2\"/>\n");
		else if (des_para.RODNODE.wing_offset_style==3)					
			fprintf(fp,"<肢向偏移类型 des_para.LINEPARTNODE.wing_offset_style=\"3.g3\"/>\n");
		else if (des_para.RODNODE.wing_offset_style==4)					
			fprintf(fp,"<肢向偏移类型 des_para.LINEPARTNODE.wing_offset_style=\"4.自定义\"/>\n");
		sprintf(sText,"%f",des_para.RODNODE.wing_offset_dist);
		SimplifiedNumString(sText);
		fprintf(fp,"<肢向偏移值 des_para.LINEPARTNODE.wing_offset_dist=\"%s\"/>\n",sText);
		fprintf(fp,"<法向偏移量 des_para.LINEPARTNODE.norm_offset=\"%s\"/>\n",des_para.RODEND.norm_offset);
		sprintf(sText,"%f",des_para.RODNODE.vPlaneNormal.x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",des_para.RODNODE.vPlaneNormal.y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",des_para.RODNODE.vPlaneNormal.z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<平推面法线 des_para.LINEPARTNODE.face_norm_offset=\"[%s,%s,%s]\"/>\n",sText,sText2,sText3);
	}
	else if(datum_pos_style==3||datum_pos_style==4)	//基准线空间交点
	{
		fprintf(fp,"<基准线1句柄 des_para.AXIS_INTERS.hDatum1=\"0X%X\"/>\n",des_para.AXIS_INTERS.hDatum1);
		fprintf(fp,"<基准线2句柄 des_para.AXIS_INTERS.hDatum2=\"0X%X\"/>\n",des_para.AXIS_INTERS.hDatum2);
		fprintf(fp,"<肢向偏移值1 des_para.AXIS_INTERS.wing_offset_dist1=\"%d\"/>\n",des_para.AXIS_INTERS.wing_offset_dist1);
		fprintf(fp,"<肢向偏移值2 des_para.AXIS_INTERS.wing_offset_dist2=\"%d\"/>\n",des_para.AXIS_INTERS.wing_offset_dist2);
		if (des_para.AXIS_INTERS.wing_offset_style1==0)	//0.g;1.g1;2.g2;3.g3;4.自定义
			fprintf(fp,"<肢向偏移类型1 des_para.AXIS_INTERS.wing_offset_style1=\"0.g\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style1==1)					 
			fprintf(fp,"<肢向偏移类型1 des_para.AXIS_INTERS.wing_offset_style1=\"1.g1\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style1==2)					 
			fprintf(fp,"<肢向偏移类型1 des_para.AXIS_INTERS.wing_offset_style1=\"2.g2\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style1==3)					 
			fprintf(fp,"<肢向偏移类型1 des_para.AXIS_INTERS.wing_offset_style1=\"3.g3\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style1==4)					 
			fprintf(fp,"<肢向偏移类型1 des_para.AXIS_INTERS.wing_offset_style1=\"4.自定义\"/>\n");
		if (des_para.AXIS_INTERS.wing_offset_style2==0)	//0.g;1.g1;2.g2;3.g3;4.自定义
			fprintf(fp,"<肢向偏移类型1 des_para.AXIS_INTERS.wing_offset_style2=\"0.g\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style2==1)					 
			fprintf(fp,"<肢向偏移类型1 des_para.AXIS_INTERS.wing_offset_style2=\"1.g1\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style2==2)					 
			fprintf(fp,"<肢向偏移类型1 des_para.AXIS_INTERS.wing_offset_style2=\"2.g2\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style2==3)					 
			fprintf(fp,"<肢向偏移类型1 des_para.AXIS_INTERS.wing_offset_style2=\"3.g3\"/>\n");
		else if (des_para.AXIS_INTERS.wing_offset_style2==4)					 
			fprintf(fp,"<肢向偏移类型1 des_para.AXIS_INTERS.wing_offset_style2=\"4.自定义\"/>\n");
		sprintf(sText,"%f",des_para.AXIS_INTERS.near_vector[0]);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",des_para.AXIS_INTERS.near_vector[1]);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",des_para.AXIS_INTERS.near_vector[2]);
		SimplifiedNumString(sText3);
		fprintf(fp,"<近似法线 des_para.AXIS_INTERS.near_vector=\"[%s,%s,%s]\"/>\n",sText,sText2,sText3);
		fprintf(fp,"<法向偏移量 des_para.AXIS_INTERS.norm_offset=\"%s\"/>\n",des_para.AXIS_INTERS.norm_offset);
	}
	else if(datum_pos_style==5)	//基准线上轴向坐标不变点
	{
		fprintf(fp,"<基准线句柄 des_para.DATUMLINE.hDatumLine=\"0X%X\"/>\n",des_para.DATUMLINE.hDatumLine);
		if (des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			fprintf(fp,"<指定轴向 des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=\"0.X轴\"/>\n");
		else if (des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)
			fprintf(fp,"<指定轴向 des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=\"1.Y轴\"/>\n");
		else if (des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==2)
			fprintf(fp,"<指定轴向 des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=\"2.Z轴\"/>\n");
		sprintf(sText,"%f",datum_pos.x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",datum_pos.y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",datum_pos.z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<相对坐标 datum_pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	}
	else if(datum_pos_style==6)	//基准面上指定轴向点
	{
		fprintf(fp,"<基准面句柄 des_para.DATUMPLANE.hDatumPlane=\"0X%X\"/>\n",des_para.DATUMPLANE.hDatumPlane);
		fprintf(fp,"<对称变换类型 des_para.DATUMPLANE.cMirTransType=\"%c\"/>\n",des_para.DATUMPLANE.cMirTransType);
		if (des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==0)
			fprintf(fp,"<指定轴向 des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=\"0.X轴\"/>\n");
		else if (des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==1)				   
			fprintf(fp,"<指定轴向 des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=\"1.Y轴\"/>\n");
		else if (des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2==2)				   
			fprintf(fp,"<指定轴向 des_para.DATUMLINE.fix_axis_val_style_x0_y1_z2=\"2.Z轴\"/>\n");
		sprintf(sText,"%f",datum_pos.x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",datum_pos.y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",datum_pos.z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<相对坐标 datum_pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	}
	else if(datum_pos_style==8)	//与节点同位
		fprintf(fp,"<基准节点句柄 des_para.hNode=\"0X%X\"/>\n",des_para.hNode);
	else if(datum_pos_style==9)	//指定构件上的相对坐标
	{
		fprintf(fp,"<基准构件句柄 des_para.hPart=\"0X%X\"/>\n",des_para.hPart);
		sprintf(sText,"%f",datum_pos.x);
		SimplifiedNumString(sText);
		sprintf(sText2,"%f",datum_pos.y);
		SimplifiedNumString(sText2);
		sprintf(sText3,"%f",datum_pos.z);
		SimplifiedNumString(sText3);
		fprintf(fp,"<相对坐标 datum_pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	}
	else if(datum_pos_style==10)//两节点间的比例等分点 wht 11-01-24
	{
		fprintf(fp,"<基准杆件句柄 des_para.SCALE_NODE.hLinePart=\"0X%X\"/>\n",des_para.SCALE_NODE.hLinePart);
		fprintf(fp,"<始端节点句柄 des_para.SCALE_NODE.hStartNode=\"0X%X\"/>\n",des_para.SCALE_NODE.hStartNode);
		fprintf(fp,"<终端节点句柄 des_para.SCALE_NODE.hEndNode=\"0X%X\"/>\n",des_para.SCALE_NODE.hEndNode);
		sprintf(sText,"%f",des_para.SCALE_NODE.start_offset_dist);
		SimplifiedNumString(sText);
		fprintf(fp,"<始端偏移量 des_para.SCALE_NODE.start_offset_dist=\"%s\"/>\n",sText);
		sprintf(sText,"%f",des_para.SCALE_NODE.end_offset_dist);
		SimplifiedNumString(sText);
		fprintf(fp,"<终端偏移量 des_para.SCALE_NODE.end_offset_dist=\"%s\"/>\n",sText);
		sprintf(sText,"%f",des_para.SCALE_NODE.offset_scale);
		SimplifiedNumString(sText);
		fprintf(fp,"<偏移比例 des_para.SCALE_NODE.offset_scale=\"%s\"/>\n",sText);
		sprintf(sText,"%f",des_para.SCALE_NODE.offset_dist);
		SimplifiedNumString(sText);
		fprintf(fp,"<在等分基础上的偏移值 des_para.SCALE_NODE.offset_dist=\"%s\"/>\n",sText);
	}
	fprintf(fp,"</基准点设计参数>\n");
}
void CLDSPoint::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//已调用过)
		return;
	if(handle>0x20)	//只添加全局基准点
		_console->AddToObjSetBuf(this);
	CLDSPart *pPart;
	CLDSNode *pNode;
	bAddRelaObj=bAddRelaObj&&bAddSubLevelRelaObj;
	if(datum_pos_style==1||datum_pos_style==7)
	{	//角钢楞点定位
		pPart=m_pModel->FromPartHandle(des_para.RODEND.hRod);
		if(pPart!=NULL)
			pPart->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==2)
	{	//杆件节点定位
		pPart=m_pModel->FromPartHandle(des_para.RODNODE.hRod);
		if(pPart!=NULL)
			pPart->AddToConsoleObjBuf(bAddRelaObj);
		pNode=m_pModel->FromNodeHandle(des_para.RODNODE.hNode);
		if(pNode!=NULL)
			pNode->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==3)
	{	//心线交点
		pPart=m_pModel->FromPartHandle(des_para.AXIS_INTERS.hDatum1);
		if(pPart!=NULL)
			pPart->AddToConsoleObjBuf(bAddRelaObj);
		pPart=m_pModel->FromPartHandle(des_para.AXIS_INTERS.hDatum2);
		if(pPart!=NULL)
			pPart->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==4)
	{	//基准线交点
		CLDSLine *pLine=m_pModel->FromLineHandle(des_para.AXIS_INTERS.hDatum1);
		if(pLine!=NULL)
			pLine->AddToConsoleObjBuf(bAddRelaObj);
		pLine=m_pModel->FromLineHandle(des_para.AXIS_INTERS.hDatum2);
		if(pLine!=NULL)
			pLine->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==5)
	{	//基准线上轴向坐标不变点
		CLDSLine *pLine=m_pModel->FromLineHandle(des_para.DATUMLINE.hDatumLine);
		if(pLine!=NULL)
			pLine->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==6)
	{	//基准面上轴向坐标不变点
		CLDSPlane *pPlane=m_pModel->FromPlaneHandle(des_para.DATUMPLANE.hDatumPlane);
		if(pPlane!=NULL)
			pPlane->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==8)
	{	//与节点同位
		pNode=m_pModel->FromNodeHandle(des_para.hNode);
		if(pNode!=NULL)
			pNode->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==9)
	{	//指定构件上的相对坐标
		pPart=m_pModel->FromPartHandle(des_para.hPart);
		if(pPart!=NULL)
			pPart->AddToConsoleObjBuf(bAddRelaObj);
	}
	else if(datum_pos_style==10)
	{	//两节点间比例等分点
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
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
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
	//始端
	if(hPointArr[0].bSimpleNodePoint)
	{
		tempPointArr[0].datum_pos_style=8;
		tempPointArr[0].des_para.hNode=hPointArr[0].handle;
		pStartPoint=&tempPointArr[0];
	}
	else if(hPointArr[0].bRodTipStart)
	{
		tempPointArr[0].datum_pos_style=1;	//杆件端点定位
		tempPointArr[0].des_para.RODEND.hRod=hPointArr[0].handle;
		tempPointArr[0].des_para.RODEND.wing_offset_style=4;
		tempPointArr[0].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[0].des_para.RODEND.len_offset_dist=0;
		tempPointArr[0].des_para.RODEND.direction=0;
		pStartPoint=&tempPointArr[0];
	}
	else if(hPointArr[0].bRodTipEnd)
	{
		tempPointArr[0].datum_pos_style=1;	//杆件端点定位
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
	//终端
	CLDSPoint *pEndPoint=NULL;
	if(hPointArr[1].bSimpleNodePoint)
	{
		tempPointArr[1].datum_pos_style=8;
		tempPointArr[1].des_para.hNode=hPointArr[1].handle;
		pEndPoint=&tempPointArr[1];
	}
	else if(hPointArr[1].bRodTipStart)
	{
		tempPointArr[1].datum_pos_style=1;	//杆件端点定位
		tempPointArr[1].des_para.RODEND.hRod=hPointArr[1].handle;
		tempPointArr[1].des_para.RODEND.wing_offset_style=4;
		tempPointArr[1].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[1].des_para.RODEND.len_offset_dist=0;
		tempPointArr[1].des_para.RODEND.direction=0;
		pEndPoint=&tempPointArr[1];
	}
	else if(hPointArr[1].bRodTipEnd)
	{
		tempPointArr[1].datum_pos_style=1;	//杆件端点定位
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
	//读取影射构件参数和构件从属句柄 wht 12-09-14
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
	//保存影射构件参数和构件从属句柄 wht 12-09-14
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
	fprintf(fp,"<线 handle=\"0X%X\">\n",handle);
	fprintf(fp,"<编号 iNo=\"%d\"/>\n",iNo);
	fprintf(fp,"<权限字 dwPermission=\"%d\"/>\n",dwPermission);
	fprintf(fp,"<描述 description=\"%s\"/>\n",description);
	fprintf(fp,"<点数组 hPointArr=\"[0X%X,0X%X]\"/>\n",hPointArr[0],hPointArr[1]);
	sprintf(sText,"%f",start_pos.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",start_pos.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",start_pos.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<起点位置 start_pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",end_pos.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",end_pos.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",end_pos.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<终点位置 end_pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	fprintf(fp,"</线>\n");
}
void CLDSLine::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//已调用过)
		return;
	if(handle>0x20)
		_console->AddToObjSetBuf(this);
}
CSupplementPart::CSupplementPart()
{
	//strcpy(m_sClassName,"CSupplementPart");
	m_nClassTypeId=CLS_SUPPLEMENTPART;
	cMaterial = 'S';		//材质:'S'-Q235;'H'-Q345
	strcpy(sPartNo,"");			//构件编号
	strcpy(sNotes,"");			//备注
	length= 0;			//长度
	piece_weight = 0;	//单件重
	nQuantity = 1;		//单基数量
	part_type=0;		//构件类型
	bLegPart=FALSE;		//腿部构件
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
	m_uStatMatCoef=1;
}
void CSupplementPart::FromBuffer(CBuffer &buffer,long version,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	CfgwordFromBuffer(cfgword,buffer,version,doc_type);
	BelongModel()->EvolveCfgword(cfgword,version,doc_type);
	buffer.ReadByte(&_material);		//材质:'S'-Q235;'H'-Q345
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.ReadByte(&_cQualityLevel);
	buffer.ReadString(sPartNo);			//构件编号
	buffer.ReadString((char*)guige);			//规格
	buffer.ReadString(sNotes,51);			//备注
	buffer.ReadDouble(&length);			//长度
	buffer.ReadDouble(&piece_weight);	//单件重
	buffer.ReadInteger(&nQuantity);		//单基数量
	buffer.ReadInteger(&_iSeg.iSeg);		//分段号
	buffer.ReadInteger(&part_type);		//构件类型
	buffer.ReadInteger(&bLegPart);		//腿部构件
}
void CSupplementPart::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	CfgwordToBuffer(cfgword,buffer,version,doc_type);
	buffer.WriteByte(cMaterial);		//材质:'S'-Q235;'H'-Q345
	if ((version == 0 && doc_type != 3) ||
		(doc_type == 1 && version >= 5020100) ||	//TMA V5.2.1.0
		(doc_type == 2 && version >= 2010100) ||	//LMA V2.1.1.0
		(doc_type == 4 && version >= 1030900) ||	//LDS V1.3.9.0
		(doc_type == 5 && version >= 1030900))		//TDA V1.3.9.0
		buffer.WriteByte(m_cQualityLevel);
	buffer.WriteString(sPartNo);		//构件编号
	buffer.WriteString((char*)guige);			//规格
	buffer.WriteString(sNotes);			//备注
	buffer.WriteDouble(length);			//长度
	buffer.WriteDouble(piece_weight);	//单件重
	buffer.WriteInteger(nQuantity);		//单基数量
	buffer.WriteInteger(iSeg);		//分段号
	buffer.WriteInteger(part_type);		//构件类型
	buffer.WriteInteger(bLegPart);		//腿部构件
}

void CSupplementPart::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="";
	fprintf(fp,"<附加构件材料 handle=\"0X%X\">\n",handle);
	fprintf(fp,"<配材号 cfgword.word=\"%d,%d,%d,%d,%d\"/>\n",cfgword.flag.word[0],cfgword.flag.word[1],cfgword.flag.word[2],cfgword.flag.word[3],cfgword.flag.word[4]);
	fprintf(fp,"<材质 cMaterial=\"%c\"/>\n",cMaterial);
	fprintf(fp,"<构件编号 sPartNo=\"%s\"/>\n",(char*)sPartNo);
	fprintf(fp,"<规格 guige=\"%s\"/>\n",(char*)guige);
	fprintf(fp,"<备注 sNotes=\"%s\"/>\n",sNotes);
	sprintf(sText,"%f",length);
	SimplifiedNumString(sText);
	fprintf(fp,"<长度 length=\"%s\"/>\n",sText);
	sprintf(sText,"%f",piece_weight);
	SimplifiedNumString(sText);
	fprintf(fp,"<单件重 piece_weight=\"%s\"/>\n",sText);
	fprintf(fp,"<单基数量 nQuantity=\"%d\"/>\n",nQuantity);
	fprintf(fp,"<分段号 iSeg=\"%d\"/>\n",iSeg);
	if (part_type==0)
		fprintf(fp,"<构件类型 part_type=\"0.角钢\"/>\n");
	else if (part_type==1)				  
		fprintf(fp,"<构件类型 part_type=\"1.钢板\"/>\n");
	else if (part_type==2)
		fprintf(fp,"<构件类型 part_type=\"2.其它\"/>\n");
	if (bLegPart)
		fprintf(fp,"<腿部构件 bLegPart=\"true\"/>\n");
	else
		fprintf(fp,"<腿部构件 bLegPart=\"false\"/>\n");
	fprintf(fp,"</附加构件材料>\n");
}

CLDSPlane::CLDSPlane()
{
	m_nClassTypeId=CLS_GEPLANE;
	//strcpy(m_sClassName,"CLDSPlane");
	strcpy(description,"");
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
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
		return false;	//Z向最多允许0.01mm误差，这个共面误差在工程上精度足够
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
		tempPointArr[0].datum_pos_style=1;	//杆件端点定位
		tempPointArr[0].des_para.RODEND.hRod=hPointArr[0].handle;
		tempPointArr[0].des_para.RODEND.wing_offset_style=4;
		tempPointArr[0].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[0].des_para.RODEND.len_offset_dist=0;
		tempPointArr[0].des_para.RODEND.direction=0;
		
		pFirPoint=&tempPointArr[0];
	}
	else if(hPointArr[0].bRodTipEnd)
	{
		tempPointArr[0].datum_pos_style=1;	//杆件端点定位
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
		tempPointArr[1].datum_pos_style=1;	//杆件端点定位
		tempPointArr[1].des_para.RODEND.hRod=hPointArr[1].handle;
		tempPointArr[1].des_para.RODEND.wing_offset_style=4;
		tempPointArr[1].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[1].des_para.RODEND.len_offset_dist=0;
		tempPointArr[1].des_para.RODEND.direction=0;
		pSecPoint=&tempPointArr[1];
	}
	else if(hPointArr[1].bRodTipEnd)
	{
		tempPointArr[1].datum_pos_style=1;	//杆件端点定位
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
		tempPointArr[2].datum_pos_style=1;	//杆件端点定位
		tempPointArr[2].des_para.RODEND.hRod=hPointArr[2].handle;
		tempPointArr[2].des_para.RODEND.wing_offset_style=4;
		tempPointArr[2].des_para.RODEND.wing_offset_dist=0;
		tempPointArr[2].des_para.RODEND.len_offset_dist=0;
		tempPointArr[2].des_para.RODEND.direction=0;
		pThirPoint=&tempPointArr[2];
	}
	else if(hPointArr[2].bRodTipEnd)
	{
		tempPointArr[2].datum_pos_style=1;	//杆件端点定位
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
			logerr.Log("Datum plane 0X%X's parametric point 0X%X  has been deleted,the plane update failed！",handle,hPointArr[0]);
#else 
			logerr.Log("基准面0X%X内的0X%X参数化点已被删除，平面更新失败！",handle,hPointArr[0].handle);
#endif
		if(pSecPoint)
			pSecPoint->UpdatePos(pTower,pLift,bReverseLift);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Datum plane 0X%X's parametric point 0X%X  has been deleted,the plane update failed！",handle,hPointArr[1]);
#else 
			logerr.Log("基准面0X%X内的0X%X参数化点已被删除，平面更新失败！",handle,hPointArr[1].handle);
#endif
		if(pThirPoint)
			pThirPoint->UpdatePos(pTower,pLift,bReverseLift);
		else
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Datum plane 0X%X's parametric point 0X%X  has been deleted,the plane update failed！",handle,hPointArr[2]);
#else 
			logerr.Log("基准面0X%X内的0X%X参数化点已被删除，平面更新失败！",handle,hPointArr[2].handle);
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
				logerr.Log("The three parametric point of datum plane 0X%X are collinear,the plane update failed！",handle);
#else 
				logerr.Log("基准面0X%X内的三个参数化点共线，平面更新失败！",handle);
#endif
				return FALSE;	//三基准点共线
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
		return FALSE;	//两方向平行，不合法平面
	else*/
		return TRUE;	//两方向不平行，合法平面

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
	//读取影射构件参数和构件从属句柄 wht 12-09-14
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
	//保存影射构件参数和构件从属句柄 wht 12-09-14
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
	fprintf(fp,"<面 handle=\"0X%X\">\n",handle);
	fprintf(fp,"<编号 iNo=\"%d\"/>\n",iNo);
	fprintf(fp,"<权限字 dwPermission=\"%d\"/>\n",dwPermission);
	fprintf(fp,"<描述 description=\"%s\"/>\n",description);
	sprintf(sText,"%f",base_pos.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",base_pos.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",base_pos.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<基点 base_pos=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",work_norm.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",work_norm.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",work_norm.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<工作法线 work_norm=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	fprintf(fp,"<点数组 hPointArr=\"[0X%X,0X%X,0X%X]\"/>\n",hPointArr[0],hPointArr[1],hPointArr[2]);
	fprintf(fp,"</面>\n");	
}
void CLDSPlane::AddToConsoleObjBuf(bool bAddRelaObj/*=false*/,bool bAddSubLevelRelaObj/*=true*/)
{
	if(_console==NULL||m_pModel==NULL||!Lock())	//已调用过)
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
	//基点位置设计数据
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
	fprintf(fp,"<螺栓位置设计参数>\n");
	datumPoint.ToXmlFile(fp,schema);
	if (cLocationStyle)	//true基点坐标为基准构件的相对坐标,false基点坐标为绝对坐标
		fprintf(fp,"<基点坐标为基准构件的相对坐标 bDatumPointInUcs=\"true\"/>\n");
	else
		fprintf(fp,"<基点坐标为基准构件的相对坐标 bDatumPointInUcs=\"false\"/>\n");
	fprintf(fp,"<定位基准构件句柄 hPart=\"OX%X\"/>\n",hPart);
	sprintf(sText,"%f",len_offset_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<纵向偏移值 len_offset_dist=\"%s\"/>\n",sText);
	sprintf(sText,"%f",wing_offset_dist);
	SimplifiedNumString(sText);
	fprintf(fp,"<肢向偏移值 wing_offset_dist=\"%s\"/>\n",sText);
	if (direction==0)
		fprintf(fp,"<基准角钢方向 direction=\"0.始->终\"/>\n");
	else if (direction==1)					  
		fprintf(fp,"<基准角钢方向 direction=\"1.终->始\"/>\n");
	if (offset_wing==0)
		fprintf(fp,"<偏移肢 offset_wing=\"0.X肢\"/>\n");
	else if (offset_wing==1)			  
		fprintf(fp,"<偏移肢 offset_wing=\"1.Y肢\"/>\n");
	fprintf(fp,"<法向偏移量字符串 norm_offset.key_str=\"%s\"/>\n",(char*)norm_offset.key_str);
	sprintf(sText,"%f",offset_angle);
	SimplifiedNumString(sText);
	fprintf(fp,"<螺栓基于基准钢管的偏移角度 offset_angle=\"%s\"/>\n",sText);
	sprintf(sText,"%f",R);
	SimplifiedNumString(sText);
	fprintf(fp,"<螺栓所在圆的半径 R=\"%s\"/>\n",sText);
	fprintf(fp,"</螺栓位置设计参数>\n");
}

CDesignProfileVertex::CDesignProfileVertex()
{
	hJg=0;					//角钢句柄
	hNode=0;				//基准节点句柄
	g=ber_space=edge_space=0;//心距;心线向楞线一侧的偏移量;心线向肢边一侧的偏移量
	direction=0;			//0.基准角钢方向:始->终;1.终->始
	offset_wing=0;			//0.X肢;1.Y肢
	len_offset_dist=0;		//纵向偏移值
	face_i=1;				//1表示基准面,2表示第一火曲面,3表示第二火曲面
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
	if(offset_wing==0)	//X肢
	{
		edge_pos=base_pos+len_vec*len_offset_dist+pCommJg->GetWingVecX()*(g+edge_space);
		ber_pos=base_pos+len_vec*len_offset_dist+pCommJg->GetWingVecX()*(g-ber_space);
	}
	else				//Y肢
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
	dwPermission=0x80000000;		//PERM_TOWER_ADMIN铁塔管理员身份;
	crMaterial=GetColor(CLS_PLATE,-1,'S');
}
int CLDSSphere::GetSpecification(char *spec,BOOL bIncMat/*=TRUE*/)
{
	if(spec!=NULL)
	{
		sprintf(spec,"○%sX%s",(char*)CXhChar16(D),(char*)CXhChar16(thick));
		return strlen(spec);
	}
	return 0;
}
CLDSPart *CLDSSphere::GetMirPart(MIRMSG mirmsg,const double *pPickPos/*=NULL*/,BOOL bCheckProfile/*=TRUE*/)
{
	for(RELATIVE_OBJECT *pItem=relativeObjs.GetFirst();pItem;pItem=relativeObjs.GetNext())
	{	//先在关联构件列表中查找
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
	BOOL pushed=m_pModel->PushPartStack(); //遍历构件列表前应先压栈 wht 12-03-05
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
	COLORREF plank_color=GetColor(CLS_PLATE,-1,cMaterial);	//默认板颜色
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
	pBody->vertex.append(vertice);	//顶极点
	for(i=uQuadSlices-1;i>-uQuadSlices;i--)	//纬度
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
			pBody->vertex.append(vertice);	//顶极点
		}
	}
	vertice=ucs.origin-ucs.axis_z*radius;
	pBody->vertex.append(vertice);	//底极点
	int num=pBody->vertex.GetNodeNum();
	delete[]cosa_arr;
	delete[]sina_arr;
	//初始化链表
	CCreateFace createFace;
	createFace.InitVertexList(pBody);

	f3dPolyFace *pFace=NULL;
	for(i=1-uQuadSlices;i<=uQuadSlices;i++)
	{
		int base_index=(i+uQuadSlices-1)*uSlices+1;
		if(i==1-uQuadSlices)	//顶极带
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
		{	//底极带
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
//属性管理
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
			strcpy(sText,"默认");
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
		if(layer(0)=='L')	//接腿构件
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
			strcpy(sText,"是");
		else
			strcpy(sText,"否");
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
			sprintf(sText,"是");
		else
			sprintf(sText,"否");
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
		/*CString prompt="没找到名为\"";
		prompt+=propStr;
		prompt+="\"的属性ID！";
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
			return 0;	//不隐藏
		else if(dwStatus==STATUS_COLLAPSE)
			return 1;	//隐藏
	}
	return -1;	//未找到保存的状态
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
	//保存影射构件参数和构件从属句柄 wht 12-09-14
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
	fprintf(fp,"<球 handle=\"0X%X\">\n",handle);
	fprintf(fp,"<权限字 dwPermission=\"%d\"/>\n",dwPermission);
	fprintf(fp,"<配材号 cfgword.word=\"%d,%d,%d,%d,%d\"/>\n",cfgword.flag.word[0],cfgword.flag.word[1],cfgword.flag.word[2],cfgword.flag.word[3],cfgword.flag.word[4]);
	fprintf(fp,"<构件编号 sPartNo=\"%s\"/>\n",(char*)sPartNo);
	fprintf(fp,"<统材系数 m_uStatMatCoef=\"%d\"/>\n",m_uStatMatCoef);
	fprintf(fp,"<焊接父构件 m_hPartWeldParent=\"0X%X\"/>\n",m_hPartWeldParent);
	fprintf(fp,"<图层 layer=\"%s\"/>\n",layer());
	fprintf(fp,"<分段号 iSeg=\"%d\"/>\n",iSeg);
	fprintf(fp,"<材质 cMaterial=\"%c\"/>\n",cMaterial);
	if (m_bDisplayHole)
		fprintf(fp,"<显示孔及切割面 m_bDisplayHole=\"true\"/>\n");
	else
		fprintf(fp,"<显示孔及切割面 m_bDisplayHole=\"false\"/>\n");
	sprintf(sText,"%f",D);
	SimplifiedNumString(sText);
	fprintf(fp,"<球外直径 D=\"%s\"/>\n",sText);
	sprintf(sText,"%f",d);
	SimplifiedNumString(sText);
	fprintf(fp,"<球内直径 d=\"%s\"/>\n",sText);
	fprintf(fp,"<基准节点句柄 hDatumNode=\"0X%X\"/>\n",hDatumNode);
	sprintf(sText,"%f",ucs.axis_x.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.axis_x.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.axis_x.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<X轴 ucs.axis_x=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",ucs.axis_y.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.axis_y.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.axis_y.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<Y轴 ucs.axis_y=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	sprintf(sText,"%f",ucs.axis_z.x);
	SimplifiedNumString(sText);
	sprintf(sText2,"%f",ucs.axis_z.y);
	SimplifiedNumString(sText2);
	sprintf(sText3,"%f",ucs.axis_z.z);
	SimplifiedNumString(sText3);
	fprintf(fp,"<Z轴 ucs.axis_z=\"(%s,%s,%s)\"/>\n",sText,sText2,sText3);
	fprintf(fp,"<备注 sNotes=\"%s\"/>\n",sNotes);
	fprintf(fp,"</球>\n");
}
void CLDSSphere::FromBuffer(CBuffer &buffer,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer,version,doc_type))
		return;
	DWORD dw;
	bool bPrevDeflateVersion=false;	//文件瘦身之前的版本
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
		buffer.ReadInteger(&dw);//显示螺栓孔及切割面
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
			sprintf(error_info,"{%s}内径不同",(char*)sPartNo);
#endif
		return false;
	}
	else if(D-pMirSphere->D!=0)
	{
		if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(error_info,"{%s}Outer radius is different",(char*)sPartNo);
#else 
			sprintf(error_info,"{%s}外径不同",(char*)sPartNo);
#endif
		return false;
	}
	else if(cMaterial!=pMirSphere->cMaterial)
	{
		if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
			sprintf(error_info,"{%s}Material is different",(char*)sPartNo);
#else 
			sprintf(error_info,"{%s}材质不同",(char*)sPartNo);
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
	bool bPrevDeflateVersion=false;	//文件瘦身之前的版本
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
	fprintf(fp,"<螺栓引用>\n");
	if(pLs)
		fprintf(fp,"<句柄 handle=\"0X%X\"/>\n",pLs->handle);
	fprintf(fp,"<所在的射线索引号 dwRayNo=\"%d\"/>\n",dwRayNo);
	if (start1_end2_mid0==0)
		fprintf(fp,"<螺栓位置 start1_end2_mid0=\"0.角钢中间或其他杆件上\"/>\n");
	else if (start1_end2_mid0==1)
		fprintf(fp,"<螺栓位置 start1_end2_mid0=\"1.角钢始端\"/>\n");
	else if (start1_end2_mid0==2)
		fprintf(fp,"<螺栓位置 start1_end2_mid0=\"2.角钢终端\"/>\n");
	fprintf(fp,"<腰圆孔腰长 waistLen=\"%d\"/>\n",waistLen);
	fprintf(fp,"</螺栓引用>\n");
}
//焊接线
CLDSWeldLine::CLDSWeldLine()
{
	weld_type = 0;	//丁字焊
	weld_wide=weld_high = 12;
	thick=10;
}
double CLDSWeldLine::Volume()
{
	double len=weldline.Length();
	if(weld_type==0)		//丁字焊
		return len*weld_wide*weld_high;
	else if(weld_type==1)	//搭焊
		return 0.5*len*weld_wide*weld_high;
	else					//对焊
		return 2*len*weld_wide*weld_high;
}
// 方钢
CLDSCube::CLDSCube(void)
{
	b = 40;
	cMaterial = 'S';
}

// 圆钢
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
		throw "The part type of projective mother part is wrong！";
#else 
		throw "影射构件母本构件类型错误！";
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
	{	//始端节点
		return Start();
	}
	else if(pNode==pEnd)
	{	//终端节点
		return End();
	}
	else
	{	
		f3dPoint perp;
		SnapPerp(&perp,Start(),End(),pNode->Position(true));
		return perp;
		//中间节点中除交叉点，其余情况节点父角钢均应为当前角钢
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
	//影射角钢
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
		if( fabs(vStartOffset.x)+fabs(vStartOffset.y)+fabs(vStartOffset.z)<tolerance)	//误差小于0.1mm时不触发螺栓自动正位设计
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
	{	//不知道为什么隐去组合角钢中重载的引入螺栓的函数，隐去后此处会出现死循环
		//如果往组合角钢上引入螺栓时认为是将螺栓分别引入到每个可以引入螺栓的子角钢上  wht 09-12-25
		int i=0;
		CLsRef *pLsRef=NULL;
		CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)this;
		for(i=0;i<4;i++)
		{
			CLDSLineAngle *pSonAngle=NULL;
			if(pGroupAngle->son_jg_h[i]>0x20)
				pSonAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
			if(pSonAngle==NULL||pSonAngle->m_bVirtualPart)
				continue;	//不存在子角钢或子角钢为虚拟构件 wht 11-08-05
			//螺栓在子角钢上可以引入
			if(pSonAngle->IsLsInWing(ls_ref.GetLsPtr()))
				pLsRef=pSonAngle->AppendStartLsRef(ls_ref,bNearLsCheck);
		}
		return pLsRef;
		//return ((CLDSGroupLineAngle*)this)->AppendStartLsRef(ls_ref,bNearLsCheck);
	}
	else
	{
		if(bNearLsCheck)	//检查附近螺栓
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
	{	//不知道为什么隐去组合角钢中重载的引入螺栓的函数，隐去后此处会出现死循环
		//如果往组合角钢上引入螺栓时认为是将螺栓分别引入到每个可以引入螺栓的子角钢上  wht 09-12-25
		int i=0;
		CLsRef *pLsRef=NULL;
		CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)this;
		for(i=0;i<4;i++)
		{
			CLDSLineAngle *pSonAngle=NULL;
			if(pGroupAngle->son_jg_h[i]>0x20)
				pSonAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
			if(pSonAngle==NULL||pSonAngle->m_bVirtualPart)
				continue;	//不存在子角钢或子角钢为虚拟构件 wht 11-08-05
			//螺栓在子角钢上可以引入
			if(pSonAngle->IsLsInWing(ls_ref.GetLsPtr()))
				pLsRef=pSonAngle->AppendMidLsRef(ls_ref,bNearLsCheck);
		}
		return pLsRef;
		//return ((CLDSGroupLineAngle*)this)->AppendMidLsRef(ls_ref,bNearLsCheck);
	}
	else
	{
		if(bNearLsCheck)	//检查附近螺栓
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
	{	//不知道为什么隐去组合角钢中重载的引入螺栓的函数，隐去后此处会出现死循环
		//如果往组合角钢上引入螺栓时认为是将螺栓分别引入到每个可以引入螺栓的子角钢上  wht 09-12-25
		int i=0;
		CLsRef *pLsRef=NULL;
		CLDSGroupLineAngle *pGroupAngle=(CLDSGroupLineAngle*)this;
		for(i=0;i<4;i++)
		{
			CLDSLineAngle *pSonAngle=NULL;
			if(pGroupAngle->son_jg_h[i]>0x20)
				pSonAngle=(CLDSLineAngle*)BelongModel()->FromPartHandle(pGroupAngle->son_jg_h[i],CLS_LINEANGLE);
			if(pSonAngle==NULL||pSonAngle->m_bVirtualPart)
				continue;	//不存在子角钢或子角钢为虚拟构件 wht 11-08-05
			//螺栓在子角钢上可以引入
			if(pSonAngle->IsLsInWing(ls_ref.GetLsPtr()))
				pLsRef=pSonAngle->AppendEndLsRef(ls_ref,bNearLsCheck);
		}
		return pLsRef;
		//return ((CLDSGroupLineAngle*)this)->AppendEndLsRef(ls_ref,bNearLsCheck);
	}
	else
	{
		if(bNearLsCheck)	//检查附近螺栓
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
			if(start1_end2_mid0==1&&lengthpos<=length_scope)	//始端螺栓
				hits++;
			else if(start1_end2_mid0==2&&lengthpos>=length-length_scope)	//始端螺栓
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
	{	//根据碰撞计算正负头
		CLDSPart *pPartArr[2]={NULL};
		pPartArr[0]=BelongModel()->FromPartHandle(desStartOdd.m_hRefPart1);
		pPartArr[1]=BelongModel()->FromPartHandle(desStartOdd.m_hRefPart2);
		if(pPartArr[0]||pPartArr[1])
			start_oddment=liInitOddment;
		else
			return;
		if(pPartArr[0]&&pPartArr[1]&&(pPartArr[0]->handle==pPartArr[1]->handle))
			pPartArr[1]=NULL;	//避免重复计算
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
	{	//根据碰撞计算正负头
		CLDSPart *pPartArr[2]={NULL};
		pPartArr[0]=BelongModel()->FromPartHandle(desEndOdd.m_hRefPart1);
		pPartArr[1]=BelongModel()->FromPartHandle(desEndOdd.m_hRefPart2);
		if(pPartArr[0]||pPartArr[1])
			end_oddment=liInitOddment;
		else
			return;
		if(pPartArr[0]&&pPartArr[1]&&(pPartArr[0]->handle==pPartArr[1]->handle))
			pPartArr[1]=NULL;	//避免重复计算
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
			if(pMirObj->mirInfo.axis_flag&0x10)	//镜像
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
		if(pMirObj->mirInfo.axis_flag&0x10)	//镜像
			buffer.WritePoint(pMirObj->mirInfo.mir_norm);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of relative object is wrong!");
#else
		CLDSObject::Log2File()->Log("关联构件记录数出现错误!");
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
	{	//拉线类型
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
	buffer.WriteDword(m_uStatMatNo);			//统材组号
	buffer.WriteDouble(m_fWeightRaiseCoef);	// 重力放大系数，考虑联板、螺栓等对重力的放大作用
	if(	version==0|| (doc_type==4&&version>=1030800))	//LDS V1.3.8.0
		buffer.WriteByte(m_cbRodFlag);
	buffer.WriteDouble(connectStart.maxTension);
	buffer.WriteDouble(connectStart.maxCompression);
	buffer.WriteDouble(connectStart.maxMoment);
	buffer.WriteDouble(connectEnd.maxTension);
	buffer.WriteDouble(connectEnd.maxCompression);
	buffer.WriteDouble(connectEnd.maxMoment);
	// --------- 3. 其它数据: 螺栓信息和杆件类型 -----------
	buffer.WriteDouble(hole_out);
	buffer.WriteInteger(m_bAutoCalHoleOutCoef);
	buffer.WriteInteger(connectStart.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Write(connectStart.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Write(&connectStart.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.WriteInteger(connectEnd.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Write(connectEnd.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Write(&connectEnd.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
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
		CLDSObject::Log2File()->Log("关联构件记录数出现错误!");
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
	{	//拉线类型
		buffer.ReadString(size.wireType);
		buffer.ReadString(size.spec);
		//buffer.ReadDouble(&m_fPreTension);
	}
	buffer.ReadDouble(&size.wide);
	buffer.ReadDouble(&size.thick);
	buffer.ReadByte(&_material);
	buffer.ReadDword(&_uStatMatNo);			//统材组号
	// --------- 3. 其它数据: 螺栓信息和杆件类型 -----------
	buffer.ReadInteger(&connectStart.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Read(connectStart.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Read(&connectStart.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.ReadInteger(&connectEnd.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Read(connectEnd.grade,4);   		//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Read(&connectEnd.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	//法兰信息
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
	{	//拉线类型
		buffer.WriteString(GetWireType());
		buffer.WriteString(GetSpec());
		//buffer.WriteDouble(m_fPreTension);
	}
	buffer.WriteDouble(size_wide);
	buffer.WriteDouble(size_thick);
	buffer.WriteByte(cMaterial);
	buffer.WriteDword(m_uStatMatNo);			//统材组号
	// --------- 3. 其它数据: 螺栓信息和杆件类型 -----------
	buffer.WriteInteger(connectStart.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Write(connectStart.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Write(&connectStart.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.WriteInteger(connectEnd.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Write(connectEnd.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Write(&connectEnd.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
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
		CLDSObject::Log2File()->Log("关联构件记录数出现错误!");
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
			if(toupper(layer(0))!='L'&&toupper(layer(0))!='X')	//不是腿部斜材
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
			CalLenCoef.iTypeNo=5;	//将指定计算长度统一归为平行轴最小轴同时指定一种类型
	}
	buffer.ReadInteger(&CalLenCoef.iSelRFlag);
	buffer.ReadInteger(&size.idClassType);	
	buffer.ReadByte(&size.cSubClassType);
	if(size.idClassType==CLS_CABLE)
	{	//拉线类型
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
	buffer.ReadDword(&_uStatMatNo);			//统材组号
	buffer.ReadDouble(&m_fWeightRaiseCoef);	// 重力放大系数，考虑联板、螺栓等对重力的放大作用
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
	// --------- 3. 其它数据: 螺栓信息和杆件类型 -----------
	buffer.ReadDouble(&hole_out);
	buffer.ReadInteger(&m_bAutoCalHoleOutCoef);
	buffer.ReadInteger(&connectStart.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Read(connectStart.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Read(&connectStart.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.ReadInteger(&connectEnd.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Read(connectEnd.grade,4);   		//(非水平杆件的上端或水平杆件的)起始端螺栓信息
	buffer.Read(&connectEnd.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
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
	//保存影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdToBuffer(buffer))
	{
		IndependentShadowPropToBuffer(buffer);
		return;
	}
	//权限
	buffer.WriteDword(dwPermission);
	buffer.WriteDword(dwStartPermission);
	buffer.WriteDword(dwEndPermission);
	//基本信息
	if(dwPermission&dwUpdateWord)
	{
		buffer.Write(cfgword.flag.word,24);
		buffer.Write(layer_tag,4);
		buffer.WriteWord(m_iElemType);
		buffer.WriteInteger(m_bNeedSelMat);
		buffer.WriteInteger(m_bNeedSelSize);
		buffer.WriteInteger(sel_mat_method); //选材方式
		
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
		{	//拉线类型
			buffer.WriteString(GetWireType());
			buffer.WriteString(GetSpec());
			buffer.WriteDouble(m_fPreTension);
		}
		buffer.WriteDouble(size_wide);
		buffer.WriteDouble(size_thick);
		buffer.WriteByte(cMaterial);
		buffer.WriteByte(m_cQualityLevel);
		buffer.WriteDword(m_uStatMatNo);		//统材组号
		buffer.WriteDouble(m_fWeightRaiseCoef);	// 重力放大系数，考虑联板、螺栓等对重力的放大作用
		buffer.WriteByte(m_cbRodFlag);
		//螺栓信息
		buffer.WriteDouble(hole_out);
		buffer.WriteInteger(m_bAutoCalHoleOutCoef);
		//杆件类型
		if(size_idClassType==0)
		{
			buffer.WriteDouble(userdef.E);
			buffer.WriteDouble(userdef.area);
			buffer.WriteDouble(userdef.ft);
			buffer.WriteDouble(userdef.fc);
			buffer.WriteDouble(userdef.unit_weight);
			buffer.WriteDouble(userdef.pre_stress);
		}
		//关联信息
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
			CLDSObject::Log2File()->Log("关联构件记录数出现错误!");
#endif
	}
	//始端信息
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
		//螺栓信息
		buffer.WriteInteger(connectStart.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Write(connectStart.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Write(&connectStart.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.WriteInteger(connectStart.m_iConnectType);
		buffer.WriteString(connectStart.flNo);
	}
	//终端信息
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
		//螺栓信息
		buffer.WriteInteger(connectEnd.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Write(connectEnd.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Write(&connectEnd.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.WriteInteger(connectEnd.m_iConnectType);
		buffer.WriteString(connectEnd.flNo);	
	}
#endif
}

void CLDSLinePart::FromPipeBuffer(CBuffer &buffer,DWORD dwUpdateWord)
{
#ifdef __PART_DESIGN_INC_//defined(__TSA_)||defined(__TSA_FILE_)||defined(__LDS_)||defined(__LDS_FILE_)
	//读取影射构件参数和构件从属句柄 wht 12-09-14
	if(ShadowParaAndBlockIdFromBuffer(buffer))
	{
		IndependentShadowPropFromBuffer(buffer);
		return;
	}
	long h=0;
	//权限
	buffer.ReadDword(&dwPermission);	
	buffer.ReadDword(&dwStartPermission);
	buffer.ReadDword(&dwEndPermission);
	//基本信息
	if(dwPermission&dwUpdateWord)
	{
		buffer.Read(cfgword.flag.word,24);
		buffer.Read(layer_tag,4);
		layer_tag[3]='\0';
		buffer.ReadWord(&m_iElemType);
		buffer.ReadInteger(&m_bNeedSelMat);
		buffer.ReadInteger(&m_bNeedSelSize);
		buffer.ReadInteger(&h);					//塔材选材方式
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
		{	//拉线类型
			buffer.ReadString(size.wireType);
			buffer.ReadString(size.spec);
			buffer.ReadDouble(&m_fPreTension);
		}
		buffer.ReadDouble(&size.wide);
		buffer.ReadDouble(&size.thick);
		buffer.ReadByte(&_material);
		buffer.ReadByte(&_cQualityLevel);
		buffer.ReadDword(&_uStatMatNo);		//统材组号
		buffer.ReadDouble(&m_fWeightRaiseCoef);	// 重力放大系数，考虑联板、螺栓等对重力的放大作用
		buffer.ReadByte(&m_cbRodFlag);
		//螺栓信息
		buffer.ReadDouble(&hole_out);
		buffer.ReadInteger(&m_bAutoCalHoleOutCoef);
		//杆件类型
		if(size.idClassType==0)
		{
			buffer.ReadDouble(&userdef.E);
			buffer.ReadDouble(&userdef.area);
			buffer.ReadDouble(&userdef.ft);
			buffer.ReadDouble(&userdef.fc);
			buffer.ReadDouble(&userdef.unit_weight);
			buffer.ReadDouble(&userdef.pre_stress);
		}
		//关联信息
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
	//始端信息
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
		//螺栓信息
		buffer.ReadInteger(&connectStart.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Read(connectStart.grade,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Read(&connectStart.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		int iConnType=0;
		buffer.ReadInteger(&iConnType);
		connectStart.m_iConnectType=iConnType;
		buffer.ReadString(connectStart.flNo);
	}
	//终端信息
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
		//螺栓信息
		buffer.ReadInteger(&connectEnd.d);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Read(connectEnd.grade,4);   		//(非水平杆件的上端或水平杆件的)起始端螺栓信息
		buffer.Read(&connectEnd.uiLegacyN,4);   	//(非水平杆件的上端或水平杆件的)起始端螺栓信息
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
//返回当前材料的弹性模量值(MPa)
double CLDSLinePart::GetElasticityModulus(double para/*=1.0*/)
{
	if(library)
		return library->GetElasticityModulus(cMaterial);
	/*else if(guige.pole_type==TRUSS_POLE_USERDEF)
		return userdef.E;*/
	else
		return 2.06e5;
}
//返回杆件指定（角钢时para=0平行轴或para=1最小轴）惯性矩(mm4)
double CLDSLinePart::GetWorkI(int para/*=0*/)
{
	if (para != 0 && para != 1)
	{	//根据杆件输入的最小轴平行轴数据自动判断
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
//返回杆件指定（角钢时para=0平行轴或para=1最小轴）回转半径(mm4)
double CLDSLinePart::GetWorkR(int para/*=0*/)
{
	if(library)
		return library->GetWorkR(size_wide,size_thick,0,para,size_idClassType,size_cSubClassType);
	else
		return 0;
}
//返回杆件指定（角钢时para=0平行轴或para=1最小轴）回转半径(mm3)
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
#if defined(__TSA_)||defined(__LDS_)	//因用到了GetSysPath函数，不适宜于在MapLma.arx中调用
//线缆专用查询函数
//线缆截面积,mm2
double CLDSLinePart::GetWireArea()
{
	if(library)
		return library->GetWireArea(GetWireType(),GetSpec());
	else
		return 0;
}
//单位长重量,kg/km
double CLDSLinePart::GetWireUnitWeight()
{
	if(library)
		return library->GetWireUnitWeightPerKM(GetWireType(),GetSpec());
	else
		return 0;
}
//额定抗拉力,N
double CLDSLinePart::GetWireForcePermit()
{
	if(library)
		return library->GetWireForcePermit(GetWireType(),GetSpec());
	else
		return 0;
}
//弹性系数(模量),MPa
double CLDSLinePart::GetWireElasticCoef()
{
	if(library)
		return library->GetWireElasticCoef(GetWireType(),GetSpec());
	else
		return 0;
}
//温度线膨胀系数,
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
	{	//调换小号至始端
		endPos=pStart->Position(false);
		startPos=pEnd->Position(false);
		quad_start=pEnd->point_i;
		quad_end=pStart->point_i;
	}
	for(pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		if(pMirObj->mirInfo.axis_flag==0x01)	//X轴对称
		{
			if(fabs(startPos.y)<EPS)	//X轴线上对称节点是自己
				quad_mir_start=quad_start;
			else
				quad_mir_start=MirPointI(quad_start,pMirObj->mirInfo);
			if(fabs(endPos.y)<EPS)
				quad_mir_end=quad_end;
			else
				quad_mir_end=MirPointI(quad_end,pMirObj->mirInfo);
			if(quad_mir_start<0||quad_mir_end<0)
				continue;	//错误数据
			if(quad_mir_start>quad_mir_end)
				swap_pointI(quad_mir_start,quad_mir_end);
			if(quad_start>quad_mir_start||(quad_start==quad_mir_start&&quad_end>quad_mir_end))
				return FALSE;	//有更小点号的关联构件
		}
		else if(pMirObj->mirInfo.axis_flag==0x02)	//Y轴对称
		{
			if(fabs(startPos.x)<EPS)	//X轴线上对称节点是自己
				quad_mir_start=quad_start;
			else
				quad_mir_start=MirPointI(quad_start,pMirObj->mirInfo);
			if(fabs(endPos.x)<EPS)
				quad_mir_end=quad_end;
			else
				quad_mir_end=MirPointI(quad_end,pMirObj->mirInfo);
			if(quad_mir_start<0||quad_mir_end<0)
				continue;	//错误数据
			if(quad_mir_start>quad_mir_end)
				swap_pointI(quad_mir_start,quad_mir_end);
			if(quad_start>quad_mir_start||(quad_start==quad_mir_start&&quad_end>quad_mir_end))
				return FALSE;	//有更小点号的关联构件
		}
		else if(pMirObj->mirInfo.axis_flag==0x04)	//Z轴对称
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
				continue;	//错误数据
			if(quad_mir_start>quad_mir_end)
				swap_pointI(quad_mir_start,quad_mir_end);
			if(quad_start>quad_mir_start||(quad_start==quad_mir_start&&quad_end>quad_mir_end))
				return FALSE;	//有更小点号的关联构件
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
	{	//调换小号至始端
		endPos=pStart->Position(false);
		startPos=pEnd->Position(false);
		quad_end_min=quad_end=pStart->point_i;
		quad_start_min=quad_start=pEnd->point_i;
	}

	for(pMirObj=relativeObjs.GetFirst();pMirObj;pMirObj=relativeObjs.GetNext())
	{
		if(pMirObj->mirInfo.axis_flag&0x08)	//旋转对称跳过
			continue;
		if(pMirObj->mirInfo.axis_flag==0x01)	//X轴对称
		{
			if(fabs(startPos.y)<EPS)	//X轴线上对称节点是自己
				quad_mir_start=quad_start;
			else
				quad_mir_start=MirPointI(quad_start,pMirObj->mirInfo);
			if(fabs(endPos.y)<EPS)
				quad_mir_end=quad_end;
			else
				quad_mir_end=MirPointI(quad_end,pMirObj->mirInfo);
			if(quad_mir_start<0||quad_mir_end<0)
				continue;	//错误数据
			if(quad_mir_start>quad_mir_end)
				swap_pointI(quad_mir_start,quad_mir_end);
			if(quad_start_min>quad_mir_start||(quad_start_min==quad_mir_start&&quad_end_min>quad_mir_end))
			{
				quad_start_min=quad_mir_start;
				quad_end_min=quad_mir_end;
				relaFatherObj = *pMirObj;	//有更小点号的关联构件
			}
		}
		else if(pMirObj->mirInfo.axis_flag==0x02)	//Y轴对称
		{
			if(fabs(startPos.x)<EPS)	//X轴线上对称节点是自己
				quad_mir_start=quad_start;
			else
				quad_mir_start=MirPointI(quad_start,pMirObj->mirInfo);
			if(fabs(endPos.x)<EPS)
				quad_mir_end=quad_end;
			else
				quad_mir_end=MirPointI(quad_end,pMirObj->mirInfo);
			if(quad_mir_start<0||quad_mir_end<0)
				continue;	//错误数据
			if(quad_mir_start>quad_mir_end)
				swap_pointI(quad_mir_start,quad_mir_end);
			if(quad_start_min>quad_mir_start||(quad_start_min==quad_mir_start&&quad_end_min>quad_mir_end))
			{
				quad_start_min=quad_mir_start;
				quad_end_min=quad_mir_end;
				relaFatherObj = *pMirObj;	//有更小点号的关联构件
			}
		}
		else if(pMirObj->mirInfo.axis_flag==0x04)	//Z轴对称
		{
			quad_mir_start=MirPointI(quad_start,pMirObj->mirInfo);
			quad_mir_end=MirPointI(quad_end,pMirObj->mirInfo);
			if(quad_mir_start<0||quad_mir_end<0)
				continue;	//错误数据
			if(quad_mir_start>quad_mir_end)
				swap_pointI(quad_mir_start,quad_mir_end);
			if(quad_start_min>quad_mir_start||(quad_start_min==quad_mir_start&&quad_end_min>quad_mir_end))
			{
				quad_start_min=quad_mir_start;
				quad_end_min=quad_mir_end;
				relaFatherObj = *pMirObj;	//有更小点号的关联构件
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
BOOL CLDSLinePart::IsAuxPole()	//是否是辅助材
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
BOOL CLDSLinePart::IsPostRod()	//是否是辅助材
{
	if(toupper(layer_tag[1])=='Z')
		return TRUE;
	else
		return FALSE;
}
BOOL CLDSLinePart::IsPrimaryRod(bool bOnlyBevelRod/*=false*/)	//是否为柱普通受力杆(即斜材横材等)
{
	if(toupper(layer_tag[1])=='X')
		return TRUE;
	else if(bOnlyBevelRod||IsAuxPole())
		return FALSE;	//仅考虑斜材时，横材不视为主要受力材(Primary)杆件
	else	//横材等受力材
		return TRUE;
}

UCS_STRU CLDSLinePart::GetUnifiedACS()
{	//计算构件装配坐标系时考虑杆件正头、钢板法向偏移量 wjh 16-1-03
	UCS_STRU acs=BuildUCS();
	acs.origin-=acs.axis_z*startOdd();
	return acs;
}
//单位为mm2
double CLDSLinePart::GetArea()
{
#if defined(__TSA_)||defined(__LDS_)	//因用到了GetSysPath函数，不适宜于在MapLma.arx中调用
	if(size_idClassType==CLS_CABLE)
		return GetWireArea();
	else 
#endif
	if(library)
		return library->GetPartArea(size_wide,size_thick,size_height,size_idClassType,size_cSubClassType);
	else
		return 0.0;
}
double CLDSLinePart::GetWeight(BOOL bSolidLength/*=TRUE*/)	//单位为kg
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
		return "Φ";
	else if(size_idClassType==CLS_LINEFLAT)
		return "━";
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
}	//单位为mm
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
//单位为mm
double CLDSLinePart::GetHeight()
{
	CLDSPart *pMotherPart=MotherPart(false);
	if(pMotherPart!=this)
		return pMotherPart->GetHeight();
	return size_height;
}	//单位为mm
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
    SetLayer(pPart->layer());						// 本角钢所在图层.
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
	{	//始端
		if(pStart&&pEnd)
			return pStart->Position(false);
		else
			return Start();
	}
	else //if(start0_end1==1)
	{	//终端
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
		{	//交叉节点
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
		{	//当前射线杆件上的其他节点
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
	//处理CLDSPoint中相关的替换，应考虑系统全局的及个别构件局部的参数化点，建议增加CLDSPoint::ReplaceRodEndNode函数　wjh-2018.1.25
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
		{	//更新始末端节点对称关联信息时，必须优先以relativeObjs中存在的对称信息为主，否则会因同时具备X/Y轴对称与Z轴对称发生混淆 wjh-2016.7.6
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
	if(pArcLine->WorkNorm()==pArcLine->ColumnNorm())	//圆弧
	{
		double sina = sin(slice_angle);	//扇片角正弦
		double cosa = cos(slice_angle);	//扇片角余弦
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
	else	//椭圆弧
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
	double sina = sin(slice_angle);	//扇片角正弦
	double cosa = cos(slice_angle);	//扇片角余弦
	vertex_arr[0].Set(radius);
	for(int i=1;i<slices;i++)
	{
		vertex_arr[i].x=vertex_arr[i-1].x*cosa-vertex_arr[i-1].y*sina;
		vertex_arr[i].y=vertex_arr[i-1].y*cosa+vertex_arr[i-1].x*sina;
		vertex_arr[i].z=0;
	}
}

//itemType--辅助参数 -1表示不启用该参数 cMaterial--材质
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
//得到构件工艺信息中性文件buffer
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
	//写入版本号，从版本号1000003开始应与CProccessPart::ToPPIbuffer一致
	BUFFER_VERSION v(1000006);	//版本号最大值为:16777215(0x00FFFFFF)
	if(m_nClassTypeId!=CLS_PLATE)
		v.version=1000005;	//1000006主要是增加普通钢板的卷边偏偏移信息
	v.bytes[3]=0xFF;			//前三个字节用来表示版本号，第四个字节用来判断是否为版本号
	buffer.WriteInteger(v.version);		//版本号
	v.bytes[3]=0x0;	
	BUFFER_VERSION serial;
	if(m_pModel&&m_pModel->IsTowerModel())
		serial.major=((CTower*)m_pModel)->m_wiVerSerial;
	buffer.WriteInteger(serial.version);		//流水号
	//1.写入构件基本信息
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
	buffer.WriteByte(cMaterial);			//写入材质简化字符
	buffer.WriteInteger(iSeg);				//写入归属分段号
	buffer.WriteFloat((float)GetWidth());	//写入宽度
	buffer.WriteFloat((float)GetThick());	//写入厚度
	buffer.WriteFloat((float)GetHeight());	//写入高度
	buffer.WriteWord((WORD)ftoi(GetLength()));	//写入长度(mm)
	buffer.WriteFloat((float)GetWeight());	//写入单重(kg)
	if(v.version>=1000005)
	{
		WORD wAttachBuff=2;
		//if(cls_id==CLS_PLATE||cls_id==CLS_PARAMPLATE)
		//	wAttachBuff=3;
		buffer.WriteWord(wAttachBuff);
		//1.写入构件附加信息
		buffer.WriteInteger(1);	//缓存区类型标识, 1:基础附加缓存区
		DWORD stackPos=buffer.CursorPosition;
		buffer.WriteInteger(0);	//预留基础附加缓存区长度占位字节
		
		//构件工艺信息的版本流水号>=V1000005
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
		//2.写入工艺信息
		buffer.WriteInteger(2);	//缓存区类型标识, :工艺附加缓存区
		CBuffer processBuf;
		WriteProcessBuffer(processBuf,true,v.version);
		buffer.WriteInteger(processBuf.Length);
		buffer.Write(processBuf.GetBufferPtr(),processBuf.GetLength());

		//if(cls_id==CLS_PLATE||cls_id==CLS_PARAMPLATE)
		//{	//3.钢板切割点信息
		//	buffer.WriteInteger(3);	//缓存区类型标识, 3:钢板切割点信息附加缓存区
		//	stackPos=buffer.CursorPosition;
		//	buffer.WriteInteger(0);	//预留基础附加缓存区长度占位字节
		//	((CProcessPlate*)this)->CutPointToBuffer(buffer,v.version);
		//	currPos=buffer.CursorPosition;
		//	buffer.SeekPosition(stackPos);
		//	buffer.WriteInteger(currPos-stackPos-4);
		//	buffer.SeekPosition(currPos);
		//}
	}
	else
	{
		//2.写入构件附加信息
		buffer.WriteString(GetPartNo());
		CXhChar50 spec;
		GetSpecification(spec,TRUE);
		buffer.WriteString(spec);
		buffer.WriteString(sNotes);
		//3.写入构件工艺信息
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

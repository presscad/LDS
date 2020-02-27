//<LOCALE_TRANSLATE BY hxr /> 2015-04-20
// LsList.cpp: implementation of the CLsList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdio.h"
#include "Tower.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#ifdef __COMMON_PART_INC_

//相求ucs与vice_ucs两个相对坐标系对于螺栓来说相匹配,也就是说两个螺栓链表
//内的螺栓在各自的相对坐标系下的相对坐标位置相同或相近
//error_info输出比较结果字符串,pH输出位置不同的螺栓句柄 wht 11-01-15
BOOL IsLsRefListEqual(CLsRefList *pLsRefList,const UCS_STRU &ucs,
				   CLsRefList *pViceLsRefList, const UCS_STRU &vice_ucs,
				   double epsilon,char *error_info,long *pH)	
{
	CLsRef *pLsRef,*pViceLsRef;
	CLDSBolt *pLs,*pViceLs;
	f3dPoint base;
	if(pLsRefList==NULL&&pViceLsRefList==NULL)	//同时都为空
		return TRUE;
	else if(pLsRefList!=NULL&&pViceLsRefList!=NULL)	//都不为空
	{
		if(pLsRefList->GetNodeNum()!=pViceLsRefList->GetNodeNum())
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"Bolt's quantity is different");
#else
				strcpy(error_info,"螺栓数量不同");
#endif
			return FALSE;		//两螺栓数不同
		}
	}
	else	//一个为空，而另一个不为空
	{
		if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(error_info,"Bolt's quantity is different");
#else
			strcpy(error_info,"螺栓数量不同");
#endif
		return FALSE;
	}
	CHashList<long> hashMatchHoles;	//避免当前多孔映射到目标角钢同一孔情况（一定是出问题了） wjh-2019.9.18
	for (pLsRef=pLsRefList->GetFirst();pLsRef!=NULL;pLsRef=pLsRefList->GetNext())
	{
		pLs=pLsRef->GetLsPtr();
		base = pLs->ucs.origin;
		//coord_trans(base,ucs,FALSE);	//将螺栓位置转到相对坐标系下
		GEPOINT origin(ucs.origin.x,ucs.origin.y,ucs.origin.z);
		GEPOINT axis_x(ucs.axis_x.x,ucs.axis_x.y,ucs.axis_x.z);
		GEPOINT axis_y(ucs.axis_y.x,ucs.axis_y.y,ucs.axis_y.z);
		if (fabs(dot_prod(pLs->get_norm(),ucs.axis_x))>EPS_COS2)
		{		//螺栓工作法线为相对坐标系的X轴
			Int3dlf(base,base,pLs->get_norm(),origin,axis_x);
			coord_trans(base,ucs,FALSE);	//将螺栓位置转到相对坐标系下
			pViceLsRef = pViceLsRefList->FindLsByUcsPos(
				base,vice_ucs,0,epsilon);
		}
		else if (fabs(dot_prod(pLs->get_norm(),ucs.axis_y))>EPS_COS2)
		{		//螺栓工作法线与相对坐标系的Y轴靠近
			Int3dlf(base,base,pLs->get_norm(),origin,axis_y);
			coord_trans(base,ucs,FALSE);	//将螺栓位置转到相对坐标系下
			pViceLsRef = pViceLsRefList->FindLsByUcsPos(
				base,vice_ucs,1,epsilon);
		}
		else		//螺栓工作法线与相对坐标系的Z轴靠近
		{
			coord_trans(base,ucs,FALSE);	//将螺栓位置转到相对坐标系下
			pViceLsRef = pViceLsRefList->FindLsByUcsPos(
				base,vice_ucs,2,epsilon);
		}
		if(pViceLsRef==NULL)	//不找到相同位置的螺栓
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"Don't find the bolt matching with 0X%X",pLs->handle);
#else
				sprintf(error_info,"未找到与0X%X匹配的螺栓",pLs->handle);
#endif
			if(pH)	//螺栓句柄
				*pH=pLs->handle;
			return FALSE;
		}
		pViceLs = pViceLsRef->GetLsPtr();
		long *phExistMatchBolt=hashMatchHoles.GetValue(pViceLs->handle);
		if (phExistMatchBolt!=NULL)
		{
			if(error_info)
	#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"Bolt's position is different");
	#else
				_snprintf(error_info,200,"螺栓孔0x%X同时与目标螺栓孔0x%X和0x%X相映射",pViceLs->handle,*phExistMatchBolt,pLs->handle);
	#endif
			return FALSE;
		}
		hashMatchHoles.SetValue(pViceLs->handle,pLs->handle);
		//只比较螺栓的直径不比较螺栓有效长及级别
		if(pLs->get_d()!=pViceLs->get_d())//规格不同
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"The diameter of  bolt 0X%X is different with 0X%X",pLs->handle,pViceLs->handle);
#else
				sprintf(error_info,"螺栓0X%X与0X%X直径不同",pLs->handle,pViceLs->handle);
#endif
			if(pH)	//螺栓句柄
				*pH=pLs->handle;
			return FALSE;
		}
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CLsRefList::FromBuffer(CBuffer &buffer,IModel *pModel,long version/*=NULL*/,long doc_type/*=0*/)
{
	if(doc_type==0)
		doc_type=PRODUCT_TYPE;
	long i,n;
	buffer.ReadInteger(&n);
	Empty();
	for(i=0;i<n;i++)
	{
		CLsRef lsref;
		lsref.FromBuffer(buffer,pModel,version,doc_type);
		if(lsref.GetLsPtr()!=NULL)
			append(lsref);
	}
}
void CLsRefList::ToBuffer(CBuffer &buffer,long version/*=0*/,long doc_type/*=0*/)
{
	BUFFERPOP stack(&buffer,GetNodeNum());
	buffer.WriteInteger(GetNodeNum());
	for(CLsRef *pLsRef=GetFirst();pLsRef;pLsRef=GetNext())
	{
		pLsRef->ToBuffer(buffer);
		stack.Increment();
	}
	if(!stack.VerifyAndRestore())
#ifdef AFX_TARG_ENU_ENGLISH
		CLDSObject::Log2File()->Log("The number record of bolt reference is wrong!");
#else
		CLDSObject::Log2File()->Log("螺栓引用记录数出现错误!");
#endif
}
void CLsRefList::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="";
	fprintf(fp,"<螺栓引用列表>\n");
	fprintf(fp,"<螺栓引用数 LsRefNum=\"%d\"/>\n",GetNodeNum());
	for(CLsRef *pLsRef=GetFirst();pLsRef;pLsRef=GetNext())
		pLsRef->ToXmlFile(fp,schema);
	fprintf(fp,"</螺栓引用列表>\n");

}

//在链表末尾添加节点  --00.05.25
/*CLsRef* CLsRefList::append()//在节点链表的末尾添加节点
{
	if(m_pModified)
		*m_pModified=TRUE;
	return ATOM_LIST<CLsRef>::append();
}*/
//在链表末尾添加节点  --00.05.25
CLsRef* CLsRefList::append(CLsRef ls_ref)//在节点链表的末尾添加节点
{
	if(m_pModified)
		*m_pModified=TRUE;
	if(ls_ref.GetLsPtr())
		return SetValue(ls_ref.GetLsPtr()->handle,ls_ref);
	else
		return NULL;
	//return ATOM_LIST<CLsRef>::append(ls_ref);
}

BOOL CLsRefList::DeleteNode(THANDLE handle)
{
	return CHashList<CLsRef>::DeleteNode(handle);
	/*
	BOOL bPush=push_stack();
	bool hasDeleted=false;
	for(CLsRef *pLsRef=GetFirst();pLsRef;pLsRef=GetNext())
	{
		if(pLsRef->GetLsPtr()&&(*pLsRef)->handle==handle)
		{
			if(m_pModified)
				*m_pModified=TRUE;
			hasDeleted=true;
			DeleteCursor();
		}
		else if(pLsRef->GetLsPtr()==NULL)
		{
			hasDeleted=true;
			DeleteCursor();
		}
	}
	//由只删除一个相符螺栓引用，变为删除全部符合的螺栓引用是为了避免重复螺栓引用所对应螺栓删除后的死机 wjh-2015.1.15
	if(hasDeleted)
		Clean();
	if(bPush)
		pop_stack();
	return FALSE;
	*/
}
//base点表示螺栓的基点位置(相对坐标系下),ucs表示一个螺栓的所在的相对坐标系,
//此坐标系必须有一轴线与螺栓的工作法线相靠近,iNormX0Y1Z2就表示
//与螺栓工作法线相靠近的相对坐标系的坐标轴
CLsRef* CLsRefList::FindLsByUcsPos(f3dPoint base,UCS_STRU ucs,int iNormX0Y1Z2,double epsilon)
{
	f3dPoint pt,work_norm;
	CLsRef *pLsRef;
	//CAtomListStack<CLsRef> listStack(this);
	BOOL pushed=CHashList<CLsRef>::push_stack();
	for(pLsRef=GetFirst();pLsRef!=NULL;pLsRef=GetNext())
	{
		pt = (*pLsRef)->ucs.origin;
		work_norm = (*pLsRef)->get_norm();
		if(iNormX0Y1Z2==0)	//X轴为法线
		{
			Int3dlf(pt,pt,work_norm,ucs.origin,ucs.axis_x);
			coord_trans(pt,ucs,FALSE);
			vector_trans(work_norm,ucs,FALSE);
			if(fabs(work_norm.x)<EPS_COS2)	//判断螺栓工作法线方向
				continue;
			if(fabs(pt.y -base.y)>epsilon)		//判断螺栓位置是否相符
				continue;
			else if(fabs(pt.z -base.z)>epsilon)
				continue;
		}
		else if(iNormX0Y1Z2==1)//Y轴为法线
		{
			Int3dlf(pt,pt,work_norm,ucs.origin,ucs.axis_y);
			coord_trans(pt,ucs,FALSE);
			vector_trans(work_norm,ucs,FALSE);
			if(fabs(work_norm.y)<EPS_COS2)	//判断螺栓工作法线方向
				continue;
			if(fabs(pt.x -base.x)>epsilon)		//判断螺栓位置是否相符
				continue;
			else if(fabs(pt.z -base.z)>epsilon)
				continue;
		}
		else	//Z轴为法线
		{
			coord_trans(pt,ucs,FALSE);
			vector_trans(work_norm,ucs,FALSE);
			if(fabs(work_norm.z)<EPS_COS2)	//判断螺栓工作法线方向
				continue;
			if(fabs(pt.y -base.y)>epsilon)		//判断螺栓位置是否相符
				continue;
			else if(fabs(pt.x -base.x)>epsilon)
				continue;
		}
		break;//螺栓方位相同
	}
	if(pushed)
		CHashList<CLsRef>::pop_stack(pushed);
	return pLsRef;//没找到
}

CLsRef* CLsRefList::FindLsByPos(f3dPoint base,f3dPoint work_norm,double epsilon)
{
	CLsRef *pLsRef;
	normalize(work_norm);
	UCS_STRU ucs;
	ucs.origin = base;
	ucs.axis_z=work_norm;
	ucs.axis_y=inters_vec(ucs.axis_z);
	ucs.axis_x=ucs.axis_y^ucs.axis_z;
	ucs.axis_y=ucs.axis_z^ucs.axis_x;
	normalize(ucs.axis_x);
	normalize(ucs.axis_y);
	for(pLsRef=GetFirst();pLsRef!=NULL;pLsRef=GetNext())
	{
		f3dPoint ref_org_ls_pos=(*pLsRef)->ucs.origin;
		coord_trans(ref_org_ls_pos,ucs,FALSE);	//向法线平面投影
		ref_org_ls_pos.z=0;
		double dd = ref_org_ls_pos.mod();
		if(dd>epsilon)		//螺栓位置不符
			continue;
		else if(fabs(dot_prod(work_norm,(*pLsRef)->get_norm()))<EPS_COS2)
			continue;//螺栓工作法线方向不符
		else			//找到相匹配的螺栓
			return pLsRef;
	}
	return NULL;
}
int CLsRefList::GetLsCount(short d)
{
	int nHits=0;
	if(d==0)
		return GetNodeNum();
	for(CLsRef *pLsRef=GetFirst();pLsRef;pLsRef=GetNext())
		if((*pLsRef)->get_d()==d&&(*pLsRef)->hole_d_increment<=2.0)
			nHits++;
	return nHits;
}

int CLsRefList::GetLocalLsCount(short start1_end2_mid0/*=0*/) //得到局部螺栓数 始端/终端/中间
{
	int nHits=0;
	if(start1_end2_mid0>2||start1_end2_mid0<0)
		return 0;
	for(CLsRef *pLsRef=GetFirst();pLsRef;pLsRef=GetNext())
		if(pLsRef->start1_end2_mid0==start1_end2_mid0)
			nHits++;
	return nHits;
}

CLsRef* CLsRefList::FromHandle(THANDLE handle)
{
	CLsRef *pLsRef;
	BOOL bPush=push_stack();	//压栈不一定会成功
	for(pLsRef=GetFirst();pLsRef!=NULL;pLsRef=GetNext())
	{
		if((*pLsRef)->handle==handle)
		{
			pop_stack();
			return pLsRef;
		}
	}
	if(bPush)
		pop_stack();
	return NULL;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLDSPart::HOLE::HOLE(CLsRef* pLsRef/*=NULL*/)
{
	if(pLsRef)
	{
		pBolt=pLsRef->GetLsPtr();
		wBoltD=pBolt->d;
		fHoleMargin=(float)pBolt->hole_d_increment;
		wWaistLen=pLsRef->waistLen;
		cFlag=pLsRef->cFlag;
		ciResidePlane=0;
	}
	else
	{
		pBolt=NULL;
		wBoltD=20;
		fHoleMargin=1.5;
		wWaistLen=cFlag=ciResidePlane=0;
	}
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
struct LS_HOLE{
	double hole_d;
	short n;
};
int CLsRefList::GetLsString(char *ls_str)
{
	ATOM_LIST<LS_HOLE>hole_list;
	for(CLsRef *pLsRef=GetFirst();pLsRef;pLsRef=GetNext())
	{
		LS_HOLE *pHole;
		for(pHole=hole_list.GetFirst();pHole;pHole=hole_list.GetNext())
		{
			if(pHole->hole_d<((*pLsRef)->get_d()+(*pLsRef)->hole_d_increment))
				continue;
			else if(pHole->hole_d==((*pLsRef)->get_d()+(*pLsRef)->hole_d_increment))
			{
				pHole->n++;
				break;
			}
			else //if(pHole->hole_d>((*pLsRef)->get_d()+(*pLsRef)->hole_d_increment))
			{
				pHole=hole_list.insert(*pHole);
				pHole->hole_d=(*pLsRef)->get_d()+(*pLsRef)->hole_d_increment;
				pHole->n=1;
				break;
			}
		}
		if(pHole==NULL)
		{
			pHole=hole_list.append();
			pHole->hole_d=(*pLsRef)->get_d()+(*pLsRef)->hole_d_increment;
			pHole->n=1;
		}
	}
	char str[100]="",ss[500]="";
	for(LS_HOLE *pHole=hole_list.GetFirst();pHole;pHole=hole_list.GetNext())
	{
		if(pHole->hole_d-(int)pHole->hole_d>0)
			sprintf(str,"%.1fX%d",pHole->hole_d,pHole->n);
		else
			sprintf(str,"%.0fX%d",pHole->hole_d,pHole->n);
		if(strlen(ss)>0)
		{
			strcat(ss," ");
			strcat(ss,str);
		}
		else
			strcpy(ss,str);
	}
	if(ls_str)
		strcpy(ls_str,ss);
	return strlen(ss);
}
int CLsRefList::GetBoltRecordArr(BOLT_RECORD *recArr/*=NULL*/)
{
	ATOM_LIST<BOLT_RECORD> boltRecList;
	for(CLsRef *pLsRef=GetFirst();pLsRef;pLsRef=GetNext())
	{
		BOLT_RECORD *pBoltRec=NULL;
		for(pBoltRec=boltRecList.GetFirst();pBoltRec;pBoltRec=boltRecList.GetNext())
		{
			if(pBoltRec->d==(*pLsRef)->get_d()&&pBoltRec->L==(*pLsRef)->get_L()&&
				pBoltRec->m_hFamily==(*pLsRef)->m_hFamily)
			{
				pBoltRec->num++;
				break;
			}
		}
		if(pBoltRec==NULL)
		{
			pBoltRec=boltRecList.append();
			strcpy(pBoltRec->grade,(*pLsRef)->Grade());
			pBoltRec->m_hFamily=(*pLsRef)->m_hFamily;
			pBoltRec->d=(*pLsRef)->get_d();
			pBoltRec->L=(*pLsRef)->get_L();
			pBoltRec->m_bAntiTheft=(*pLsRef)->m_dwFlag.IsHasFlag(CLDSBolt::ANTI_THEFT);
			//pBoltRec->m_bTwoCapLs=(*pLsRef)->m_dwFlag.IsHasFlag(CLDSBolt::TWO_CAP);
			//pBoltRec->m_bFootNail=(*pLsRef)->m_dwFlag.IsHasFlag(CLDSBolt::FOOT_NAIL);
			//pBoltRec->m_bAntiLoose=(*pLsRef)->m_nDisplayWasher;
			pBoltRec->num=1;
		}
	}
	if(recArr)
	{
		int i=0;
		for(BOLT_RECORD *pBoltRec=boltRecList.GetFirst();pBoltRec;pBoltRec=boltRecList.GetNext())
		{
			recArr[i]=*pBoltRec;
			i++;
		}
	}
	return boltRecList.GetNodeNum();
}

struct LSREF_RELAINFO
{
	CLsRef *pLsRef;
	GEPOINT localCoord;	//当前构件局部坐标系下的螺栓孔坐标
	struct RELA{
		long hPart;
		DWORD dwRayNo;
		GEPOINT rayVec;
	}relaArr[5];
	LSREF_RELAINFO(){memset(this,0,sizeof(LSREF_RELAINFO));}
};
struct RAY_LINE{
	long hRelaPart;	//关联构件，键值1
	long iRelaPartRayNo;//相对于关联构件的射线号，键值2
	long iRayNo;	//最终相对于当前构件的射线号
	bool initRayVecBy2Bolt;
	GEPOINT coord;		//射线的极点, 怱略Z分量
	GEPOINT rayVecStd;	//单位长度的射线方向, 怱略Z分量
	RAY_LINE(){iRayNo=0;initRayVecBy2Bolt=false;}
	bool IsOnRay(GEPOINT pos,double tolerance=0.5)
	{
		if(rayVecStd.IsZero())
			return false;
		double criterion=DistOf2dPtRay(coord.x,coord.y,rayVecStd.x,rayVecStd.y,pos.x,pos.y);
		return fabs(criterion)<tolerance;
	}
};
void LoadDefaultRayLineInfo(RAY_LINE* pRay,ULONG hRelaPart,ULONG iRelaPartRayNo)
{
	pRay->hRelaPart=hRelaPart;
	pRay->iRelaPartRayNo=iRelaPartRayNo;
}
void CLsRefList::ResetBoltRefRayNo(CLDSPart *pPart)
{
#ifdef __DRAWING_CONTEXT_
	CLsRef *pLsRef=NULL;
	if(pPart->IsAngle())	//角钢件只考虑当前角钢件
	{
		for(pLsRef=GetFirst();pLsRef;pLsRef=GetNext())
		{
			for(int i=0;i<5;i++)
			{
				if(i>0&&pLsRef->GetLsPtr()->relaRayArr[i-1].hRelaPart==0)
					break;	//己无剩余有效关联构件
				if(i<4&&pPart->handle==pLsRef->GetLsPtr()->relaRayArr[i].hRelaPart)
				{
					pLsRef->dwRayNo=pLsRef->GetLsPtr()->relaRayArr[i].dwRayNo;
					break;
				}
				else if(i==4&&pPart->handle==pLsRef->GetLsPtr()->des_base_pos.hPart)
				{
					pLsRef->dwRayNo=pLsRef->GetLsPtr()->dwRayNo;
					break;
				}
			}
		}
		return;
	}
	ATOM_LIST<LSREF_RELAINFO> boltList;
	for(pLsRef=GetFirst();pLsRef;pLsRef=GetNext())
	{
		f3dPoint ls_pos=pLsRef->GetLsPtr()->ucs.origin;
		int iResidePlane=0;
		if(pPart->GetClassTypeId()==CLS_PLATE)
		{
			iResidePlane=((CLDSPlate*)pPart)->GetNormFaceI((*pLsRef)->get_norm())+1;
			((CLDSPlate*)pPart)->SpreadLsPos(pLsRef,ls_pos);
		}
		else
		{
			Int3dlf(ls_pos,ls_pos,pLsRef->GetLsPtr()->ucs.axis_z,pPart->ucs.origin,pPart->ucs.axis_z);
			coord_trans(ls_pos,pPart->ucs,FALSE);
		}
		LSREF_RELAINFO *pInfo=boltList.append();
		pInfo->pLsRef=pLsRef;
		pInfo->localCoord.Set(ls_pos.x,ls_pos.y,0);
		pInfo->relaArr[0].hPart=pLsRef->GetLsPtr()->des_base_pos.hPart;
		pInfo->relaArr[0].dwRayNo=pLsRef->GetLsPtr()->dwRayNo;
		pInfo->relaArr[0].rayVec=pLsRef->GetLsPtr()->baseRayVec;
		if(iResidePlane>0)
			pInfo->relaArr[0].rayVec=((CLDSPlate*)pPart)->SpreadToACS(pInfo->relaArr[0].rayVec,iResidePlane,CLDSPlate::IS_VECTOR|CLDSPlate::FROM_MCS);
		else
			vector_trans(pInfo->relaArr[0].rayVec,pPart->ucs,FALSE);
		for(int i=0;i<4;i++)
		{
			if(pLsRef->GetLsPtr()->relaRayArr[i].hRelaPart==0)
				break;	//己无剩余有效关联构件
			pInfo->relaArr[i+1].hPart=pLsRef->GetLsPtr()->relaRayArr[i].hRelaPart;
			pInfo->relaArr[i+1].dwRayNo=pLsRef->GetLsPtr()->relaRayArr[i].dwRayNo;
			pInfo->relaArr[i+1].rayVec=pLsRef->GetLsPtr()->relaRayArr[i].rayVec;
			if(iResidePlane>0)
				pInfo->relaArr[i+1].rayVec=((CLDSPlate*)pPart)->SpreadToACS(pInfo->relaArr[i+1].rayVec,iResidePlane,CLDSPlate::IS_VECTOR|CLDSPlate::FROM_MCS);
			else
				vector_trans(pInfo->relaArr[i+1].rayVec,pPart->ucs,FALSE);
		}
	}
	RAY_LINE *pRayLine;
	CMapList<RAY_LINE>hashRays;
	hashRays.LoadDefaultObjectInfo=LoadDefaultRayLineInfo;
	for(LSREF_RELAINFO *pLsRefRela=boltList.GetFirst();pLsRefRela;pLsRefRela=boltList.GetNext())
	{
		for(int i=0;i<5&&pLsRefRela->relaArr[i].hPart>0;i++)
		{
			for(int j=1;j<=32;j++)
			{
				if((pLsRefRela->relaArr[i].dwRayNo&GetSingleWord(j))==0)
					continue;
				pRayLine=hashRays.Add(pLsRefRela->relaArr[i].hPart,j);
				if(pRayLine->iRayNo==0)
				{	//该射线的第一颗螺栓
					pRayLine->iRayNo=hashRays.GetNodeNum();
					pRayLine->coord=pLsRefRela->localCoord;
					pRayLine->rayVecStd=pLsRefRela->relaArr[i].rayVec;
				}
				else if(pRayLine->rayVecStd.IsZero()||!pRayLine->initRayVecBy2Bolt)
				{	//该射线的第二颗螺栓
					pRayLine->initRayVecBy2Bolt=true;	//当满足多个螺栓时，优先根据螺栓计算射线方向
					f3dPoint vec=f3dPoint(pLsRefRela->localCoord)-f3dPoint(pRayLine->coord);
					normalize(vec);
					pRayLine->rayVecStd=vec;
				}
			}
		}
	}
	//合并重合的螺栓射线
	for(pRayLine=hashRays.GetFirst();pRayLine;pRayLine=hashRays.GetNext())
	{
		BOOL pushed=hashRays.push_stack();
		for(RAY_LINE *pAfterRay=hashRays.GetNext();pAfterRay;pAfterRay=hashRays.GetNext())
		{
			if(pRayLine->iRayNo==pAfterRay->iRayNo)
				continue;	//已合并过
			//0.9999998=cos(0.0002),即1000mm长的射线距离上不能偏离心线0.2mm wjh-2012.9.14
			if(pRayLine->IsOnRay(pAfterRay->coord)&&fabs(pRayLine->rayVecStd*pAfterRay->rayVecStd)>0.9999998)
				pAfterRay->iRayNo=pRayLine->iRayNo;
		}
		hashRays.pop_stack(pushed);
	}
	//向螺栓引用赋值射线号标识字
	BOOL bError=FALSE;
	for(LSREF_RELAINFO *pLsRefRela=boltList.GetFirst();pLsRefRela;pLsRefRela=boltList.GetNext())
	{
		pLsRefRela->pLsRef->dwRayNo=0;
		//赋值因构件关联而产生的射线号
		for(int i=0;i<5&&pLsRefRela->relaArr[i].hPart>0;i++)
		{
			for(int j=1;j<=32;j++)
			{
				if((pLsRefRela->relaArr[i].dwRayNo&GetSingleWord(j))==0)
					continue;
				pRayLine=hashRays.GetValue(pLsRefRela->relaArr[i].hPart,j);
				pLsRefRela->pLsRef->dwRayNo|=GetSingleWord(pRayLine->iRayNo);
				if(pRayLine->iRayNo<32)
					pLsRefRela->pLsRef->rayVecArr[pRayLine->iRayNo-1]=pRayLine->rayVecStd;
				else if(!bError)
					bError=TRUE;
			}
		}
		//赋值因位置产生的射线号（如射线角钢螺栓与基准材心线交点螺栓共射线）
		/*通过是否共线初始化pLsRefRela->pLsRef->dwRayNo，会出现若干无意义的螺栓射线，
			并导致后期通过连接杆件更新螺栓射线信息失败   wxc-2017.8.22
		*/
		//for(pRayLine=hashRays.GetFirst();pRayLine;pRayLine=hashRays.GetNext())
		//{
		//	if(pLsRefRela->pLsRef->dwRayNo&GetSingleWord(pRayLine->iRayNo))
		//		continue;	//已在此射线上
		//	if(pRayLine->IsOnRay(pLsRefRela->localCoord))
		//	{
		//		pLsRefRela->pLsRef->dwRayNo|=GetSingleWord(pRayLine->iRayNo);
		//		if(pRayLine->iRayNo<32)
		//			pLsRefRela->pLsRef->rayVecArr[pRayLine->iRayNo-1]=pRayLine->rayVecStd;
		//		else if(!bError)
		//			bError=TRUE;
		//	}
		//}
	}
	if(bError)
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("0x%X bolt ray number over 32,please check!",pPart->handle);
#else
		logerr.Log("0x%X螺栓射线号超过32,请检查!",pPart->handle);
#endif
#endif
}
bool CLsRefList::MergeBolts(CLDSBolt *pFromBolt,CLDSBolt *pDestBolt)	//将pFromBolt合并到pDestBolt
{
	//预扫描是否已有pDestBolt存在
	bool aliveDestBolt=false;
	bool aliveFromBolt=false;
	for(CLsRef *pLsRef=GetFirst();pLsRef;pLsRef=GetNext())
	{
		if(pLsRef->GetLsPtr()==pDestBolt)
		{
			aliveDestBolt=true;
			break;
		}
		if(pLsRef->GetLsPtr()==pFromBolt)
		{
			aliveFromBolt=true;
			if(aliveDestBolt)
			{	//已存在目标螺栓,可将源螺栓删除
				DeleteCursor();
				return true;
			}
		}
	}
	if(aliveFromBolt)
	{
		for(CLsRef *pLsRef=GetFirst();pLsRef;pLsRef=GetNext())
		{
			if(pLsRef->GetLsPtr()==pFromBolt)
			{
				if(aliveDestBolt)
				{	//已存在目标螺栓,可将源螺栓删除
					DeleteCursor();
					return true;
				}
				else
				{	//不存在目标螺栓,可用目标螺栓替换源螺栓
					*pLsRef=pDestBolt;
					return true;
				}
			}
		}
	}
	return false;
}
#endif
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

//����ucs��vice_ucs�����������ϵ������˨��˵��ƥ��,Ҳ����˵������˨����
//�ڵ���˨�ڸ��Ե��������ϵ�µ��������λ����ͬ�����
//error_info����ȽϽ���ַ���,pH���λ�ò�ͬ����˨��� wht 11-01-15
BOOL IsLsRefListEqual(CLsRefList *pLsRefList,const UCS_STRU &ucs,
				   CLsRefList *pViceLsRefList, const UCS_STRU &vice_ucs,
				   double epsilon,char *error_info,long *pH)	
{
	CLsRef *pLsRef,*pViceLsRef;
	CLDSBolt *pLs,*pViceLs;
	f3dPoint base;
	if(pLsRefList==NULL&&pViceLsRefList==NULL)	//ͬʱ��Ϊ��
		return TRUE;
	else if(pLsRefList!=NULL&&pViceLsRefList!=NULL)	//����Ϊ��
	{
		if(pLsRefList->GetNodeNum()!=pViceLsRefList->GetNodeNum())
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(error_info,"Bolt's quantity is different");
#else
				strcpy(error_info,"��˨������ͬ");
#endif
			return FALSE;		//����˨����ͬ
		}
	}
	else	//һ��Ϊ�գ�����һ����Ϊ��
	{
		if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
			strcpy(error_info,"Bolt's quantity is different");
#else
			strcpy(error_info,"��˨������ͬ");
#endif
		return FALSE;
	}
	CHashList<long> hashMatchHoles;	//���⵱ǰ���ӳ�䵽Ŀ��Ǹ�ͬһ�������һ���ǳ������ˣ� wjh-2019.9.18
	for (pLsRef=pLsRefList->GetFirst();pLsRef!=NULL;pLsRef=pLsRefList->GetNext())
	{
		pLs=pLsRef->GetLsPtr();
		base = pLs->ucs.origin;
		//coord_trans(base,ucs,FALSE);	//����˨λ��ת���������ϵ��
		GEPOINT origin(ucs.origin.x,ucs.origin.y,ucs.origin.z);
		GEPOINT axis_x(ucs.axis_x.x,ucs.axis_x.y,ucs.axis_x.z);
		GEPOINT axis_y(ucs.axis_y.x,ucs.axis_y.y,ucs.axis_y.z);
		if (fabs(dot_prod(pLs->get_norm(),ucs.axis_x))>EPS_COS2)
		{		//��˨��������Ϊ�������ϵ��X��
			Int3dlf(base,base,pLs->get_norm(),origin,axis_x);
			coord_trans(base,ucs,FALSE);	//����˨λ��ת���������ϵ��
			pViceLsRef = pViceLsRefList->FindLsByUcsPos(
				base,vice_ucs,0,epsilon);
		}
		else if (fabs(dot_prod(pLs->get_norm(),ucs.axis_y))>EPS_COS2)
		{		//��˨�����������������ϵ��Y�῿��
			Int3dlf(base,base,pLs->get_norm(),origin,axis_y);
			coord_trans(base,ucs,FALSE);	//����˨λ��ת���������ϵ��
			pViceLsRef = pViceLsRefList->FindLsByUcsPos(
				base,vice_ucs,1,epsilon);
		}
		else		//��˨�����������������ϵ��Z�῿��
		{
			coord_trans(base,ucs,FALSE);	//����˨λ��ת���������ϵ��
			pViceLsRef = pViceLsRefList->FindLsByUcsPos(
				base,vice_ucs,2,epsilon);
		}
		if(pViceLsRef==NULL)	//���ҵ���ͬλ�õ���˨
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"Don't find the bolt matching with 0X%X",pLs->handle);
#else
				sprintf(error_info,"δ�ҵ���0X%Xƥ�����˨",pLs->handle);
#endif
			if(pH)	//��˨���
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
				_snprintf(error_info,200,"��˨��0x%Xͬʱ��Ŀ����˨��0x%X��0x%X��ӳ��",pViceLs->handle,*phExistMatchBolt,pLs->handle);
	#endif
			return FALSE;
		}
		hashMatchHoles.SetValue(pViceLs->handle,pLs->handle);
		//ֻ�Ƚ���˨��ֱ�����Ƚ���˨��Ч��������
		if(pLs->get_d()!=pViceLs->get_d())//���ͬ
		{
			if(error_info)
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(error_info,"The diameter of  bolt 0X%X is different with 0X%X",pLs->handle,pViceLs->handle);
#else
				sprintf(error_info,"��˨0X%X��0X%Xֱ����ͬ",pLs->handle,pViceLs->handle);
#endif
			if(pH)	//��˨���
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
		CLDSObject::Log2File()->Log("��˨���ü�¼�����ִ���!");
#endif
}
void CLsRefList::ToXmlFile(FILE* fp,DWORD schema)
{
	if (this==NULL)
		return;
	char sText[500]="";
	fprintf(fp,"<��˨�����б�>\n");
	fprintf(fp,"<��˨������ LsRefNum=\"%d\"/>\n",GetNodeNum());
	for(CLsRef *pLsRef=GetFirst();pLsRef;pLsRef=GetNext())
		pLsRef->ToXmlFile(fp,schema);
	fprintf(fp,"</��˨�����б�>\n");

}

//������ĩβ��ӽڵ�  --00.05.25
/*CLsRef* CLsRefList::append()//�ڽڵ������ĩβ��ӽڵ�
{
	if(m_pModified)
		*m_pModified=TRUE;
	return ATOM_LIST<CLsRef>::append();
}*/
//������ĩβ��ӽڵ�  --00.05.25
CLsRef* CLsRefList::append(CLsRef ls_ref)//�ڽڵ������ĩβ��ӽڵ�
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
	//��ֻɾ��һ�������˨���ã���Ϊɾ��ȫ�����ϵ���˨������Ϊ�˱����ظ���˨��������Ӧ��˨ɾ��������� wjh-2015.1.15
	if(hasDeleted)
		Clean();
	if(bPush)
		pop_stack();
	return FALSE;
	*/
}
//base���ʾ��˨�Ļ���λ��(�������ϵ��),ucs��ʾһ����˨�����ڵ��������ϵ,
//������ϵ������һ��������˨�Ĺ��������࿿��,iNormX0Y1Z2�ͱ�ʾ
//����˨���������࿿�����������ϵ��������
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
		if(iNormX0Y1Z2==0)	//X��Ϊ����
		{
			Int3dlf(pt,pt,work_norm,ucs.origin,ucs.axis_x);
			coord_trans(pt,ucs,FALSE);
			vector_trans(work_norm,ucs,FALSE);
			if(fabs(work_norm.x)<EPS_COS2)	//�ж���˨�������߷���
				continue;
			if(fabs(pt.y -base.y)>epsilon)		//�ж���˨λ���Ƿ����
				continue;
			else if(fabs(pt.z -base.z)>epsilon)
				continue;
		}
		else if(iNormX0Y1Z2==1)//Y��Ϊ����
		{
			Int3dlf(pt,pt,work_norm,ucs.origin,ucs.axis_y);
			coord_trans(pt,ucs,FALSE);
			vector_trans(work_norm,ucs,FALSE);
			if(fabs(work_norm.y)<EPS_COS2)	//�ж���˨�������߷���
				continue;
			if(fabs(pt.x -base.x)>epsilon)		//�ж���˨λ���Ƿ����
				continue;
			else if(fabs(pt.z -base.z)>epsilon)
				continue;
		}
		else	//Z��Ϊ����
		{
			coord_trans(pt,ucs,FALSE);
			vector_trans(work_norm,ucs,FALSE);
			if(fabs(work_norm.z)<EPS_COS2)	//�ж���˨�������߷���
				continue;
			if(fabs(pt.y -base.y)>epsilon)		//�ж���˨λ���Ƿ����
				continue;
			else if(fabs(pt.x -base.x)>epsilon)
				continue;
		}
		break;//��˨��λ��ͬ
	}
	if(pushed)
		CHashList<CLsRef>::pop_stack(pushed);
	return pLsRef;//û�ҵ�
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
		coord_trans(ref_org_ls_pos,ucs,FALSE);	//����ƽ��ͶӰ
		ref_org_ls_pos.z=0;
		double dd = ref_org_ls_pos.mod();
		if(dd>epsilon)		//��˨λ�ò���
			continue;
		else if(fabs(dot_prod(work_norm,(*pLsRef)->get_norm()))<EPS_COS2)
			continue;//��˨�������߷��򲻷�
		else			//�ҵ���ƥ�����˨
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

int CLsRefList::GetLocalLsCount(short start1_end2_mid0/*=0*/) //�õ��ֲ���˨�� ʼ��/�ն�/�м�
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
	BOOL bPush=push_stack();	//ѹջ��һ����ɹ�
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
	GEPOINT localCoord;	//��ǰ�����ֲ�����ϵ�µ���˨������
	struct RELA{
		long hPart;
		DWORD dwRayNo;
		GEPOINT rayVec;
	}relaArr[5];
	LSREF_RELAINFO(){memset(this,0,sizeof(LSREF_RELAINFO));}
};
struct RAY_LINE{
	long hRelaPart;	//������������ֵ1
	long iRelaPartRayNo;//����ڹ������������ߺţ���ֵ2
	long iRayNo;	//��������ڵ�ǰ���������ߺ�
	bool initRayVecBy2Bolt;
	GEPOINT coord;		//���ߵļ���, �S��Z����
	GEPOINT rayVecStd;	//��λ���ȵ����߷���, �S��Z����
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
	if(pPart->IsAngle())	//�Ǹּ�ֻ���ǵ�ǰ�Ǹּ�
	{
		for(pLsRef=GetFirst();pLsRef;pLsRef=GetNext())
		{
			for(int i=0;i<5;i++)
			{
				if(i>0&&pLsRef->GetLsPtr()->relaRayArr[i-1].hRelaPart==0)
					break;	//����ʣ����Ч��������
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
				break;	//����ʣ����Ч��������
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
				{	//�����ߵĵ�һ����˨
					pRayLine->iRayNo=hashRays.GetNodeNum();
					pRayLine->coord=pLsRefRela->localCoord;
					pRayLine->rayVecStd=pLsRefRela->relaArr[i].rayVec;
				}
				else if(pRayLine->rayVecStd.IsZero()||!pRayLine->initRayVecBy2Bolt)
				{	//�����ߵĵڶ�����˨
					pRayLine->initRayVecBy2Bolt=true;	//����������˨ʱ�����ȸ�����˨�������߷���
					f3dPoint vec=f3dPoint(pLsRefRela->localCoord)-f3dPoint(pRayLine->coord);
					normalize(vec);
					pRayLine->rayVecStd=vec;
				}
			}
		}
	}
	//�ϲ��غϵ���˨����
	for(pRayLine=hashRays.GetFirst();pRayLine;pRayLine=hashRays.GetNext())
	{
		BOOL pushed=hashRays.push_stack();
		for(RAY_LINE *pAfterRay=hashRays.GetNext();pAfterRay;pAfterRay=hashRays.GetNext())
		{
			if(pRayLine->iRayNo==pAfterRay->iRayNo)
				continue;	//�Ѻϲ���
			//0.9999998=cos(0.0002),��1000mm�������߾����ϲ���ƫ������0.2mm wjh-2012.9.14
			if(pRayLine->IsOnRay(pAfterRay->coord)&&fabs(pRayLine->rayVecStd*pAfterRay->rayVecStd)>0.9999998)
				pAfterRay->iRayNo=pRayLine->iRayNo;
		}
		hashRays.pop_stack(pushed);
	}
	//����˨���ø�ֵ���ߺű�ʶ��
	BOOL bError=FALSE;
	for(LSREF_RELAINFO *pLsRefRela=boltList.GetFirst();pLsRefRela;pLsRefRela=boltList.GetNext())
	{
		pLsRefRela->pLsRef->dwRayNo=0;
		//��ֵ�򹹼����������������ߺ�
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
		//��ֵ��λ�ò��������ߺţ������߽Ǹ���˨���׼�����߽�����˨�����ߣ�
		/*ͨ���Ƿ��߳�ʼ��pLsRefRela->pLsRef->dwRayNo��������������������˨���ߣ�
			�����º���ͨ�����Ӹ˼�������˨������Ϣʧ��   wxc-2017.8.22
		*/
		//for(pRayLine=hashRays.GetFirst();pRayLine;pRayLine=hashRays.GetNext())
		//{
		//	if(pLsRefRela->pLsRef->dwRayNo&GetSingleWord(pRayLine->iRayNo))
		//		continue;	//���ڴ�������
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
		logerr.Log("0x%X��˨���ߺų���32,����!",pPart->handle);
#endif
#endif
}
bool CLsRefList::MergeBolts(CLDSBolt *pFromBolt,CLDSBolt *pDestBolt)	//��pFromBolt�ϲ���pDestBolt
{
	//Ԥɨ���Ƿ�����pDestBolt����
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
			{	//�Ѵ���Ŀ����˨,�ɽ�Դ��˨ɾ��
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
				{	//�Ѵ���Ŀ����˨,�ɽ�Դ��˨ɾ��
					DeleteCursor();
					return true;
				}
				else
				{	//������Ŀ����˨,����Ŀ����˨�滻Դ��˨
					*pLsRef=pDestBolt;
					return true;
				}
			}
		}
	}
	return false;
}
#endif
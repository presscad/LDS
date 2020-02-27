//<LOCALE_TRANSLATE/>
// SegMatSum.cpp: implementation of the CMaterialSummator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SegMatSum.h"
#include "..\Query.h"
#include "GlobalFunc.h"
#include "ComparePartNoString.h"
#include "XhCharString.h"
#include "LibraryQuery.h"
#include "material.h"
#include "SysPara.h"
#include ".\LdsDrwLocale.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef __TSA_FILE_
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//下面的函数来自SegMatTable.cpp,用于计算连接板的包容矩形
extern CTower Ta;
extern SCOPE_STRU GetRectFromPoly(ATOM_LIST<PROFILE_VERTEX> &vertex_list);

int CSegMatPolePlankRecord::GetPartN()		//获得构件数
{
	return N;
}
int CSegMatPolePlankRecord::SetPartN(int n)
{
	if(n<=0)
		n=0;
	N=n;
	return N;
}
double CSegMatPolePlankRecord::GetLength()
{
	return length;
}
//mm为单位
double CSegMatPolePlankRecord::SetLength(double L)
{
	if(L<=0)		//设置长度太短,长度定为零
		L = 0;
	length = L;
	sprintf(sLength,"%.f",L);	//保存字符形式的长度
	return length;
}
double CSegMatPolePlankRecord::AddPart(int n)
{
	N+=n;
	sum_weight += n*unit_weight;
	return sum_weight;
}
double CSegMatPolePlankRecord::SetUnitWeight(double unit_wei)
{
	if(unit_wei<0)
		return unit_weight;
	else
	{
		unit_weight = unit_wei;
		sum_weight  = unit_weight*N;
		return unit_weight;
	}
}
double CSegMatPolePlankRecord::GetUnitWeight()
{
	return unit_weight;
}
double CSegMatPolePlankRecord::SetSumWeitht(double sum_wei)
{
	if(sum_wei<0)
		return sum_weight;
	else
	{
		sum_weight=sum_wei;
		return sum_weight;
	}
}
double CSegMatPolePlankRecord::GetSumWeight()
{
	return sum_weight;
}
char* CSegMatPolePlankRecord::GetPartNo(int nMatCharPosType /* =0 */)
{
	if(nMatCharPosType==1&&cMaterial!=CSteelMatLibrary::Q235BriefMark())
		sprintf(sPartNoIncMat,"%c%s",cMaterial,sPartNo);
	else if(nMatCharPosType==2&&cMaterial!=CSteelMatLibrary::Q235BriefMark())
		sprintf(sPartNoIncMat,"%s%c",sPartNo,cMaterial);
	else 
		strcpy(sPartNoIncMat,sPartNo);
	return sPartNoIncMat;
}
//------螺栓脚钉垫圈记录--------
int CSegMatLsRecord::SetPartN(int n)
{
	if(n<=0)
		N=0;
	else
		N=n;
	return N;
}
int CSegMatLsRecord::GetPartN()		//获得构件数
{
	return N;
}
void CSegMatLsRecord::SetGuiGe(char *sGuiGe)
{
	CLDSBolt ls;
	strcpy(guige,sGuiGe);
	restore_Ls_guige(guige,ls);
	int str_len=strlen(sGuiGe);
	if(sGuiGe[str_len-1]>='A')
		hFamily=sGuiGe[str_len-1]-'A';
	unit_weight = globalLibrary.GetLsWeight(ls.get_d(),(long)ls.get_L(),hFamily);
	sum_weight  = unit_weight*N;
}
void CSegMatLsRecord::GetGrade(char *sGrade)
{
	char temstr[50];
	strcpy(temstr,guige);
	char* splitter=strchr(temstr,'M');
	if(splitter==NULL||splitter-temstr==0)	//无螺栓级别信息
		strcpy(sGrade,"");
	else
	{
		*splitter='\0';
		strncpy(sGrade,temstr,5);
	}
}
void CSegMatLsRecord::GetGuiGe(char *sGuiGe,bool bIncGrade/*=true*/)
{
	strcpy(sGuiGe,guige);
	size_t str_len=strlen(sGuiGe);
	if(sGuiGe[str_len-1]>='A')
	{
		sGuiGe[str_len-1]='\0';	//去掉双帽螺栓中双帽的特殊标记
		str_len--;
	}
	if(!bIncGrade)	//剔除掉规格中的螺栓级别
	{
		char* splitter=strchr(sGuiGe,'M');
		size_t pos=splitter-sGuiGe;
		if(splitter!=NULL&&pos>0)
		{
			memmove(sGuiGe,splitter,strlen(splitter));
			sGuiGe[str_len-pos]='\0';
		}
	}
}
double CSegMatLsRecord::AddPart(BITFLAG flagBolt,int n)
{
	N+=n;
	if(flagBolt.IsHasFlag(CLDSBolt::TWO_CAP))		//双帽螺栓个数
		nTwoCap+=n;
	if(flagBolt.IsHasFlag(CLDSBolt::ANTI_LOOSE))	//带防松垫圈螺栓个数
		nSpringPad+=n;
	if(flagBolt.IsHasFlag(CLDSBolt::ANTI_THEFT))	//防盗螺栓个数
		nBurglarproof+=n;
	CLDSBolt ls;
	restore_Ls_guige(guige,ls);
	long d=ls.get_d();		//螺栓直径
	long L=(long)ls.get_L();//螺栓有效长度
	/*if(bSpringPad&&!bTwoCap)//双帽螺栓与含一垫圈单帽普通相排斥 wjh-2011.10.9
		sum_weight+=FindLsWei(d,L,1)*n;
	else if(unit_weight>0)
		sum_weight+=unit_weight*n;
	else 
	{
		if(bTwoCap)
			unit_weight=FindLsWei(d,L,2);
		else
			unit_weight=FindLsWei(d,L,0);*/
		sum_weight+=unit_weight*n;
	//}
	return sum_weight;
}
double CSegMatLsRecord::SetSumWeight(double Sum_wei)
{
	if(Sum_wei<=0)
		return sum_weight;
	else
	{
		sum_weight=Sum_wei;
		return sum_weight;
	}
}
double CSegMatLsRecord::GetSumWeight()
{
	return sum_weight;
}
//--------垫圈记录类型定义-----------
//thick表示垫圈的厚度, d表示垫圈所附属螺栓的直径(规格)
void CSegMatPadRecord::SetGuiGe(int thick,int d)
{
	sprintf(guige,"M%dX%d",d,thick);
	unit_weight = FindPadWei(thick,d);
	sum_weight = unit_weight*N;
}
void CSegMatPadRecord::SetGuiGe(char *sGuiGe)
{
	CLDSBolt ls;
	int L,d;
	restore_Ls_guige(sGuiGe,ls);
	L = (int)ls.get_L();
	d = ls.get_d();
	SetGuiGe(L,d);
}
void CSegMatPadRecord::SetPartN(int n)
{
	if(n<=0)
		N=0;
	else
		N=n;
}
void CSegMatPadRecord::SetSumWeight(double sum_wei)
{
	if(sum_wei<=0)
		sum_weight=0;
	else
		sum_weight=sum_wei;
}
double CSegMatPadRecord::AddPart(int n)
{
	N+=n;
	sum_weight += unit_weight*n;
	return sum_weight;
}

//--------分段图材料统计类定义-----------
bool CSegMaterialSummator::EXPORT_BY_ENGLISH=false;
CSegMaterialSummator::CSegMaterialSummator()
{
	m_iSeg = m_nPolePlank = 0;
	m_pPolePlankRecArr = NULL;
	m_pLsRecArr = m_pNailRecArr = NULL;
	m_pPadRecArr = NULL;
	//m_pTwoCapLsRecArr = NULL;
}
CSegMaterialSummator::~CSegMaterialSummator()
{
	if(m_pPolePlankRecArr!=NULL)
		delete []m_pPolePlankRecArr;
	if(m_pLsRecArr!=NULL)
		delete []m_pLsRecArr;
	if(m_pNailRecArr!=NULL)	//螺栓脚钉垫圈记录
		delete []m_pNailRecArr;
	if(m_pPadRecArr!=NULL)	//螺栓脚钉垫圈记录
		delete []m_pPadRecArr;
	//if(m_pTwoCapLsRecArr)	//双帽螺栓
	//	delete []m_pTwoCapLsRecArr; 
}

BOOL CSegMaterialSummator::CreateRecordSet(int nJgPlank,int nLs,int nNail,int nPad)//,int nTwoCapLs)
{
	int i;
	char sGuiGe[50];
	if(nJgPlank>0)
	{
		m_pPolePlankRecArr = new CSegMatPolePlankRecord[nJgPlank];
		for(i=0;i<nJgPlank;i++)
		{
			sprintf(m_pPolePlankRecArr[i].guige,"%s",sPolePlankGuiGe[i]);
			m_pPolePlankRecArr[i].SetPartNo(sPolePlankNo[i].GetBuffer());
			m_pPolePlankRecArr[i].cMaterial=cPolePlankMat[i];
		}
	}
	if(nLs>0)
	{
		m_pLsRecArr	= new CSegMatLsRecord[nLs];
		for(i=0;i<nLs;i++)
		{
			sprintf(sGuiGe,"%s",sLsGuiGe[i]);
			m_pLsRecArr[i].SetGuiGe(sGuiGe);
			int str_len=strlen(sGuiGe);
			if(m_pLsRecArr[i].hFamily==2)
#ifdef AFX_TARG_ENU_ENGLISH
				strcpy(m_pLsRecArr[i].sNotes,"double nut");
			else if(m_pLsRecArr[i].hFamily==4)
				strcpy(m_pLsRecArr[i].sNotes,"double nut and double washer");
#else
				strcpy(m_pLsRecArr[i].sNotes,"双帽");
			else if(m_pLsRecArr[i].hFamily==4)
				strcpy(m_pLsRecArr[i].sNotes,"双帽双垫");
#endif
		}
	}
	if(nNail>0)
	{
		m_pNailRecArr = new CSegMatLsRecord[nNail];
		for(i=0;i<nNail;i++)
		{
			sprintf(sGuiGe,"%s",sNailGuiGe[i]);
			m_pNailRecArr[i].SetGuiGe(sGuiGe);
		}
	}
	if(nPad>0)
	{
		m_pPadRecArr = new CSegMatPadRecord[nPad];
		for(i=0;i<nPad;i++)
		{
			sprintf(sGuiGe,"%s",sPadGuiGe[i]);
			m_pPadRecArr[i].SetGuiGe(sGuiGe);
		}
	}
	return TRUE;
}
void CSegMaterialSummator::AnalyzeSuppPart()	//分析附加构件
{
	for(CSupplementPart *pSuppPart=Ta.SuppPart.GetFirst();pSuppPart;pSuppPart=Ta.SuppPart.GetNext())
	{
		if(pSuppPart->iSeg.iSeg!=m_iSeg.iSeg)
			continue;
		CString sPureGuiGe=pSuppPart->guige;
		char sMaterial[10];
		if(QuerySteelMatMark(pSuppPart->cMaterial,sMaterial))
			sPureGuiGe.Insert(0,sMaterial);
		if(FindPolePlankNoIndex(pSuppPart->GetPartNo())==-1)	//还没有这种规格
		{
			sPolePlankNo.Add(pSuppPart->GetPartNo());
			sPolePlankGuiGe.Add(sPureGuiGe);
			cPolePlankMat.Add(pSuppPart->cMaterial);
		}
	}
}

//分析构件
void CSegMaterialSummator::AnalyzeParts(IModel *pModel,BOOL bIgnoreSegI/*=FALSE*/)
{
	CLDSPart* pPart;
	CString sPureGuiGe;
	CXhChar200 spec;
	for(pPart=pModel->EnumPartFirst();pPart;pPart=pModel->EnumPartNext())
	{
		if(pPart->m_bVirtualPart)	//隐含角钢不统计
			continue;
		else if(!bIgnoreSegI&&pPart->GetClassTypeId()==CLS_BOLT&&!((CLDSBolt*)pPart)->IsStatSeg(m_iSeg))
			continue;
		else if(!bIgnoreSegI&&pPart->GetClassTypeId()!=CLS_BOLT&&pPart->iSeg.iSeg!=m_iSeg.iSeg)//不是当前段构件
			continue;
		else if(pPart->IsLinePart()&&(((CLDSLinePart*)pPart)->bHuoQuStart||((CLDSLinePart*)pPart)->bHuoQuEnd))
			continue;	//火曲子构件
		if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			continue;
		pPart->GetSpecification(spec);
		if(pPart->GetClassTypeId()!=CLS_BOLT)
		{
			if(strlen(pPart->GetPartNo())<=0)
				continue;	//不统计编号为空的钢板或者杆件
			if(FindPolePlankNoIndex(pPart->GetPartNo())==-1)	//还没有这种规格
			{
				sPolePlankNo.Add(pPart->GetPartNo());
				if(pPart->GetClassTypeId()==CLS_PLATE||pPart->GetClassTypeId()==CLS_PARAMPLATE)
				{
					char wide_str[50]="";
					sprintf(wide_str,"X%.0f",pPart->GetWidth());
					spec.Append(wide_str);
				}
				sPolePlankGuiGe.Add(spec);
				cPolePlankMat.Add(pPart->cMaterial);
			}
		}
		else
		{
			CLDSBolt *pBolt=(CLDSBolt*)pPart;
			if(pBolt->IsFootNail())	//此螺栓是登脚钉
			{
				spec.Append('A'+(BYTE)pBolt->m_hFamily);
				if(FindNailGuiGeIndex(spec)==-1)
					sNailGuiGe.Add(spec);
			}
			else	//螺栓
			{
				spec.Append('A'+(BYTE)pBolt->m_hFamily);
				if(FindLsGuiGeIndex(spec)==-1)
					sLsGuiGe.Add(spec);
				if(pBolt->DianQuan.N>0)
				{
					spec.Printf("M%dX%d",pBolt->get_d(),pBolt->DianQuan.thick);
					if(FindPadGuiGeIndex(spec)==-1)
						sPadGuiGe.Add(spec);
				}
			}
		}
	}
	POLYPARTLIST polylist;
	if(pModel->IsTowerModel())
		((CTower*)pModel)->RetrievePolyPartList(polylist);
	CLDSPolyPart *pPolyPart;
	for(pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
	{
		if(!bIgnoreSegI&&pPolyPart->iSeg.iSeg!=m_iSeg.iSeg)	//不是当前段角钢
			continue;
		pPolyPart->GetSpecification(spec);
		if(FindPolePlankNoIndex(pPolyPart->GetPartNo())==-1)	//还没有这种规格
		{
			sPolePlankNo.Add(pPolyPart->GetPartNo());
			sPolePlankGuiGe.Add(spec);
			cPolePlankMat.Add(pPolyPart->cMaterial);
		}
	}
}

//分析部件引用
void CSegMaterialSummator::AnalyzeBlockRefs()
{
	for(CBlockReference *pBlockRef=Ta.BlockRef.GetFirst();pBlockRef;pBlockRef=Ta.BlockRef.GetNext())
	{
		if(pBlockRef->iSeg.iSeg!=m_iSeg.iSeg)
			continue;
		if(pBlockRef->IsExploded())
			continue;
		CBlockModel *pBlock=Ta.Block.FromHandle(pBlockRef->m_hBlock);
		if(pBlock==NULL||pBlock->IsEmbeded())
			continue;
		AnalyzeParts(pBlock,TRUE);
	}
}
static void SortBoltSize(CStringArray &str_arr)
{
	int i,j;
	int nCount = str_arr.GetSize();
	for(i=0;i<nCount;i++)
	{
		BOOL bSorted = TRUE;
		for(j=0;j<nCount-i;j++)
		{
			if(CompareBoltSizeString(str_arr[j].GetBuffer(),str_arr[(j+1)%nCount].GetBuffer())>0)
			{
				//值交换
				CString temStr=str_arr[j];
				str_arr[j]=str_arr[(j+1)%nCount];
				str_arr[(j+1)%nCount]=temStr;
				bSorted=FALSE;
			}
		}
		if(bSorted)	//已有序
			break;
	}
}
static void SortGuiGeOrPartNo(CStringArray &str_arr,int *order,BOOL bPartNo)
{
	int i,j;
	int nCount = str_arr.GetSize();
	if(order)
	{
		for(i=0;i<nCount;i++)
			order[i] = i;
	}
	for(i=0;i<nCount;i++)
	{
		BOOL bSorted = TRUE;
		for(j=0;j<nCount-i;j++)
		{
			int nRetCode=0;
			if(bPartNo)
				nRetCode=ComparePartNoString(str_arr[j].GetBuffer(),str_arr[(j+1)%nCount].GetBuffer());
			else 
				nRetCode=CompareMultiSectionString(str_arr[j].GetBuffer(),str_arr[(j+1)%nCount].GetBuffer());
			if(nRetCode>0)
			{
				//值交换
				CString temStr=str_arr[j];
				str_arr[j]=str_arr[(j+1)%nCount];
				str_arr[(j+1)%nCount]=temStr;
				//序号交换
				if(order)
				{
					int temOrder = order[j];
					order[j] = order[(j+1)%nCount];
					order[(j+1)%nCount] = temOrder;
				}
				bSorted=FALSE;
			}
		}
		if(bSorted)	//已有序
			break;
	}
}

void CSegMaterialSummator::RunAnalyze()		//进行材料统计前的预分析
{
	AnalyzeParts(&Ta);
	AnalyzeSuppPart();		//分析附加构件
	AnalyzeBlockRefs();
	/*SortGuiGe(sLsGuiGe,TRUE);
	SortGuiGe(sNailGuiGe,TRUE);
	SortGuiGe(sPadGuiGe,TRUE);
	int *order = new int[sPolePlankNo.GetSize()];
	CStringArray guige_order;
	guige_order.SetSize(sPolePlankNo.GetSize());
	SortGuiGe(sPolePlankNo,FALSE,order);
	int i;
	for(i=0;i<sPolePlankNo.GetSize();i++)
		guige_order[i] = sPolePlankGuiGe[i];
	for(i=0;i<sPolePlankNo.GetSize();i++)
		sPolePlankGuiGe[i] = guige_order[order[i]];
	delete []order;*/
	
	SortBoltSize(sLsGuiGe);
	//SortBoltSize(sTwoCapLsGuiGe,NULL);
	SortBoltSize(sNailGuiGe);
	SortGuiGeOrPartNo(sPadGuiGe,NULL,FALSE);
	int *order = new int[sPolePlankNo.GetSize()];
	CStringArray guige_order;
	guige_order.SetSize(sPolePlankNo.GetSize());
	SortGuiGeOrPartNo(sPolePlankNo,order,TRUE);
	int i;
	for(i=0;i<sPolePlankNo.GetSize();i++)
		guige_order[i] = sPolePlankGuiGe[i];
	for(i=0;i<sPolePlankNo.GetSize();i++)
		sPolePlankGuiGe[i] = guige_order[order[i]];
	delete []order;

	m_nPolePlank = sPolePlankNo.GetSize();	//角钢钢板记录
	//螺栓脚钉垫圈
	int nLs,nNail,nPad;//,nTwoCapLs;
	nLs = sLsGuiGe.GetSize();
	nNail = sNailGuiGe.GetSize();
	nPad = sPadGuiGe.GetSize();
	//nTwoCapLs = sTwoCapLsGuiGe.GetSize();
	CreateRecordSet(m_nPolePlank,nLs,nNail,nPad);//,nTwoCapLs);
}
void CSegMaterialSummator::SummarizeBolt(IModel *pModel,BOOL bIgnoreSegI/*=FALSE*/,CBlockModel *pEmbededBlock/*=NULL*/)
{
	int i;
	CLDSBolt *pBolt;
	CXhChar200 spec;
	int hits=0;
	for(pBolt=(CLDSBolt*)pModel->EnumPartFirst(CLS_BOLT);pBolt!=NULL;pBolt=(CLDSBolt*)pModel->EnumPartNext(CLS_BOLT))
	{
		if(pEmbededBlock&&pEmbededBlock->handle!=pBolt->m_hBlock&&!pBolt->IsBlockRefPart())
			continue;	//过滤出属于指定部件的构件
		if(pBolt->m_bVirtualPart)	//非统计螺栓
			continue;
		else if(!bIgnoreSegI&&!pBolt->IsStatSeg(m_iSeg))	//不是当前段角钢
			continue;
		hits++;
		pBolt->GetSpecification(spec);
		//sGuiGe.Format("%sM%dX%.0f",pBolt->grade,pBolt->get_d(),pBolt->get_L());
		if(pBolt->IsFootNail())	//此螺栓是登脚钉
		{
			spec.Append('A'+(BYTE)pBolt->m_hFamily);
			i=FindNailGuiGeIndex(spec);
			if(i>=0)
				m_pNailRecArr[i].AddPart(pBolt->m_uStatMatCoef);
			else
			{
				char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"the bolt data of %s size is wrong",spec);
#else
				sprintf(sError,"%s规格的螺栓数据出错",spec);
#endif
				AfxMessageBox(sError);
			}
		}
		else	//螺栓
		{
			spec.Append('A'+(BYTE)pBolt->m_hFamily);
			i=FindLsGuiGeIndex(spec);
			if(i>=0)
			{
				m_pLsRecArr[i].AddPart(pBolt->m_dwFlag,pBolt->m_uStatMatCoef);
				if(pBolt->DianQuan.N>0)
				{
					spec.Printf("M%dX%d",pBolt->get_d(),pBolt->DianQuan.thick);
					i=FindPadGuiGeIndex(spec);
					m_pPadRecArr[i].AddPart(pBolt->DianQuan.N);
				}
			}
			else
			{
				char sError[200]="";
#ifdef AFX_TARG_ENU_ENGLISH
				sprintf(sError,"the bolt data of %s size is wrong",spec);
#else
				sprintf(sError,"%s规格的螺栓数据出错",spec);
#endif
				AfxMessageBox(sError);
			}
		}
	}
	//填充螺栓备注信息 wht 11-01-10
	for(i=0;i<sLsGuiGe.GetSize();i++)
	{
		CXhChar50 sTemp;
		/* 双帽螺栓单算规格，故不在备注中添加统计项
		if(m_pLsRecArr[i].GetTwoCapLsCount()>0)
		{
			sTemp.Printf("双帽(%d)",m_pLsRecArr[i].GetTwoCapLsCount());
			strcat(m_pLsRecArr[i].sNotes,sTemp);
		}*/
		if(m_pLsRecArr[i].GetBurglarproofLsCount()>0)
		{
			if(EXPORT_BY_ENGLISH)
				sTemp.Printf("Anti-theft(%d)",m_pLsRecArr[i].GetBurglarproofLsCount());
			else
				sTemp.Printf("防盗(%d)",m_pLsRecArr[i].GetBurglarproofLsCount());
			strcat(m_pLsRecArr[i].sNotes,sTemp);
		}
		if(m_pLsRecArr[i].GetSpringPadLsCount()>0)
		{
			if(EXPORT_BY_ENGLISH)
				sTemp.Printf("Anti-Loose washer(%d)",m_pLsRecArr[i].GetSpringPadLsCount());
			else
				sTemp.Printf("防松垫圈(%d)",m_pLsRecArr[i].GetSpringPadLsCount());
			strcat(m_pLsRecArr[i].sNotes,sTemp);
		}
	}
}

void CSegMaterialSummator::SummarizeSuppPart()		//统计附加构件
{
	int index;
	CString sPureGuiGe;//材质,纯规格(不带有材质信息)
	for(CSupplementPart *pSuppPart=Ta.SuppPart.GetFirst();pSuppPart;pSuppPart=Ta.SuppPart.GetNext())
	{
		if(pSuppPart->iSeg.iSeg!=m_iSeg.iSeg)
			continue;
		index = FindPolePlankNoIndex(pSuppPart->GetPartNo());
		if(index==-1)
			continue;	//index==-1时如果执行以下代码会导致内存越界 wht 10-12-14
		if(m_pPolePlankRecArr[index].GetPartN()<=0)	//此记录的第一根构件
		{
			double unit_wei = TruncateDouble(pSuppPart->piece_weight,2);
			m_pPolePlankRecArr[index].SetUnitWeight(unit_wei);
			m_pPolePlankRecArr[index].SetLength(pSuppPart->length);
			strcpy(m_pPolePlankRecArr[index].sNotes,pSuppPart->sNotes);
		}
		for(UINT ii=0;ii<pSuppPart->m_uStatMatCoef;ii++)
		{
			for(int i=0;i<pSuppPart->nQuantity;i++)
				m_pPolePlankRecArr[index].AddPart();
		}
	}
}
//统计构件
void CSegMaterialSummator::SummarizeParts(IModel *pModel,BOOL bIgnoreSegI/*=FALSE*/,CBlockModel *pEmbededBlock/*=NULL*/)	
{
	CSuperSmartPtr<CLDSPart>pPart;
	CHashList<long> attachPlateHash;
	for(pPart=pModel->EnumPartFirst();pPart.IsHasPtr();pPart=pModel->EnumPartNext())
	{
		if(pEmbededBlock&&pEmbededBlock->handle!=pPart->m_hBlock&&!pPart->IsBlockRefPart())
			continue;	//过滤出属于指定部件的构件
		if(pPart->GetClassTypeId()!=CLS_PARAMPLATE)
			continue;
		CLDSParamPlate *pPlate=pPart.ParamPlatePointer();
		if(!pPlate->IsFL()&&pPlate->m_iParamType!=TYPE_XEND)
			continue;
		if(sys.stru_map.bMergeAttachPlatesToBasePlateInBOM)
		{	//十字插板或法兰附加构件列表，构件明细中不需要显示附加构件，仅在主构件重量中考虑附加构件重量可 wht 12-03-22
			for(ATTACHPART_ITEM *pItem=pPlate->GetFirstAttachPart();pItem;pItem=pPlate->GetNextAttachPart())
			{
				if(!attachPlateHash.GetValue(pItem->h))
					attachPlateHash.SetValue(pItem->h,pItem->h);
			}
		}
	}
	for(pPart=pModel->EnumPartFirst();pPart.IsHasPtr();pPart=pModel->EnumPartNext())
	{
		if(pEmbededBlock&&pEmbededBlock->handle!=pPart->m_hBlock&&!pPart->IsBlockRefPart())
			continue;	//过滤出属于指定部件的构件
		if(pPart->m_bVirtualPart)	//隐含角钢不统计
			continue;
		else if(!bIgnoreSegI&&pPart->iSeg.iSeg!=m_iSeg.iSeg)		//不是当前段角钢
			continue;
		if(pPart->IsLinePart()&&(pPart.LinePartPointer()->bHuoQuStart||pPart.LinePartPointer()->bHuoQuEnd))	//不是普通直角钢不统计
			continue;
		if(pPart->GetClassTypeId()==CLS_GROUPLINEANGLE)
			continue;	//材料汇总时不统计组合角钢(单线)重量 wht 11-03-11
		if(strlen(pPart->GetPartNo())<=0)
			continue;	//构件号为空
		if(sys.stru_map.bMergeAttachPlatesToBasePlateInBOM&&attachPlateHash.GetValue(pPart->handle))
			continue;	//构件明细中不需要显示附加构件
		int index = FindPolePlankNoIndex(pPart->GetPartNo());
		if(index==-1)
			continue;	//未找到合法索引，一般是由于构件号为空导致 wht 10-06-11
		if(pPart->GetClassTypeId()==CLS_PARAMPLATE)
		{
			if(pPart.ParamPlatePointer()->m_bStdPart)
				m_pPolePlankRecArr[index].bStdPart=TRUE;	//当前构件为标准件 wht 12-03-22
			if(pPart.ParamPlatePointer()->IsFL())
				m_pPolePlankRecArr[index].bStdPart=FALSE;	//法兰不按标准件进行处理 wxc-16.12.20
		}
		if(m_pPolePlankRecArr[index].GetPartN()<=0)	//此记录的第一根构件
		{
			double unit_wei = TruncateDouble(pPart->GetWeight(),2);
			if(pPart->GetClassTypeId()==CLS_PARAMPLATE&&
				(pPart.ParamPlatePointer()->m_iParamType==TYPE_FL||
				pPart.ParamPlatePointer()->m_iParamType==TYPE_FLR||
				pPart.ParamPlatePointer()->m_iParamType==TYPE_FLG||
				pPart.ParamPlatePointer()->m_iParamType==TYPE_XEND))
			{	//设置单位重量时考虑法兰、十字插板附加板的重量 wht 12-03-22
				double weight=0;
				if(pPart.ParamPlatePointer()->m_iParamType==TYPE_FLR||
					pPart.ParamPlatePointer()->m_iParamType==TYPE_FLG)
				{
					CLDSStdPartModel stdPartModel;
					if(pPart.ParamPlatePointer()->GetStdPartModel(&stdPartModel))
						unit_wei=stdPartModel.param.fl_param2.SumWeight();
				}
				else 
				{
					for(ATTACHPART_ITEM *pItem=pPart.ParamPlatePointer()->GetFirstAttachPart();pItem;
						pItem=pPart.ParamPlatePointer()->GetNextAttachPart())
					{
						CLDSPart *pAttachPart=pModel->FromPartHandle(pItem->h);
						if(pAttachPart)
							weight+=pAttachPart->GetWeight();
					}
					unit_wei += weight;
				}
			}
			unit_wei=TruncateDouble(unit_wei,2);
			m_pPolePlankRecArr[index].SetUnitWeight(unit_wei);
			m_pPolePlankRecArr[index].SetLength(pPart->GetLength());
			//完善构件备注信息，包括钢管角钢是否带脚钉，钢板是否为螺栓垫板 wht 11-01-07
			if(pPart->GetClassTypeId()==CLS_LINETUBE)
			{
				if(pPart.LineTubePointer()->GetFootNailCount()>0)
				if(EXPORT_BY_ENGLISH)
					strcpy(m_pPolePlankRecArr[index].sNotes,"With Step Bolts");
				else
					strcpy(m_pPolePlankRecArr[index].sNotes,"带脚钉");
			}
			else if(pPart->GetClassTypeId()==CLS_LINEANGLE
				&&pPart.LineAnglePointer()->pStart&&pPart.LineAnglePointer()->pEnd)
			{	//GetProcessStr()已处理了带脚钉的备注情况
				pPart.LineAnglePointer()->GetProcessStr(m_pPolePlankRecArr[index].sNotes,99,EXPORT_BY_ENGLISH);
			}
			else if(pPart->GetClassTypeId()==CLS_PLATE)
			{	//钢板备注信息
				double roll_height=pPart.PlatePointer()->GetRollEdgeHeight();
				if(EXPORT_BY_ENGLISH)
				{
					if(pPart.PlatePointer()->face_N>1)
						strcpy(m_pPolePlankRecArr[index].sNotes,"Bending");
				}
				else
				{
					if(pPart.PlatePointer()->face_N>1)
						strcpy(m_pPolePlankRecArr[index].sNotes,"火曲");
				}
				if(roll_height>0)
				{	//标注卷边高度 wht 11-04-28
					CXhChar50 sRoll;
					if(EXPORT_BY_ENGLISH)
					{
						if(strlen(m_pPolePlankRecArr[index].sNotes)>0)
							sRoll.Printf(" Roll edge %.fmm",roll_height);
						else 
							sRoll.Printf("Roll edge %.fmm",roll_height);
					}
					else
					{
						if(strlen(m_pPolePlankRecArr[index].sNotes)>0)
							sRoll.Printf(" 卷边%.fmm",roll_height);
						else 
							sRoll.Printf("卷边%.fmm",roll_height);
					}
					strcat(m_pPolePlankRecArr[index].sNotes,sRoll);
				}
				else if(pPart.PlatePointer()->jdb_style==4)	//螺栓垫板
				{
					if(EXPORT_BY_ENGLISH)
					{
						if(strlen(m_pPolePlankRecArr[index].sNotes)>0)
							strcat(m_pPolePlankRecArr[index].sNotes," Gasket");
						else 
							strcat(m_pPolePlankRecArr[index].sNotes,"Gasket");
					}
					else
					{
						if(strlen(m_pPolePlankRecArr[index].sNotes)>0)
							strcat(m_pPolePlankRecArr[index].sNotes," 垫板");
						else 
							strcat(m_pPolePlankRecArr[index].sNotes,"垫板");
					}
				}
				else if(pPart.PlatePointer()->m_bWeldPart||pPart.PlatePointer()->m_hPartWeldParent>0)
				{
					if(EXPORT_BY_ENGLISH)
					{
						if(strlen(m_pPolePlankRecArr[index].sNotes)>0)
							strcat(m_pPolePlankRecArr[index].sNotes," Welding");
						else 
							strcat(m_pPolePlankRecArr[index].sNotes,"Welding");
					}
					else
					{
						if(strlen(m_pPolePlankRecArr[index].sNotes)>0)
							strcat(m_pPolePlankRecArr[index].sNotes," 焊接");
						else 
							strcat(m_pPolePlankRecArr[index].sNotes,"焊接");
					}
				}
			}
			else if(pPart->GetClassTypeId()==CLS_PARAMPLATE && pPart.pParamPlate->IsFL())
			{	//法兰
				double D=0,B=0;
				double& R=D;
				double& W=B;
				if(pPart.ParamPlatePointer()->m_iParamType==TYPE_FL||pPart.ParamPlatePointer()->m_iParamType==TYPE_FLG||
					pPart.ParamPlatePointer()->m_iParamType==TYPE_FLR)
				{
					pPart.ParamPlatePointer()->GetDesignItemValue('W',&D);	//法半外径
					pPart.ParamPlatePointer()->GetDesignItemValue('R',&B);	//法兰内圆半径
				}
				else if(pPart.ParamPlatePointer()->m_iParamType==TYPE_FLD||pPart.ParamPlatePointer()->m_iParamType==TYPE_FLP)
				{
					pPart.ParamPlatePointer()->GetDesignItemValue('D',&D);	//法兰外径
					pPart.ParamPlatePointer()->GetDesignItemValue('B',&B);	//法兰内径
				}
				CXhChar200 spec;
				if(gxDrwLocalizer.ciCustomerSerial!=3)
				{	//设置法兰长度
					/*char len[50];
					sprintf(m_pPolePlankRecArr[index].sLength,"%%%%C%f",R*2);
					SimplifiedNumString(m_pPolePlankRecArr[index].sLength);
					sprintf(len,"~%%%%C%f",W);
					SimplifiedNumString(len);
					strcat(m_pPolePlankRecArr[index].sLength,len);*/
					sprintf(m_pPolePlankRecArr[index].sLength,"%%%%C%g/%g",W,R*2);
					//strcpy(m_pPolePlankRecArr[index].sNotes,"理论实际重量");
					pPart->GetSpecification(m_pPolePlankRecArr[index].guige);
				}
				else	//青岛武晓
				{
					pPart->GetSpecification(spec);
					sprintf(m_pPolePlankRecArr[index].guige,"%sX%.f",(char*)spec,D);
				}
			}
		}
		for(UINT ii=0;ii<pPart->m_uStatMatCoef;ii++)
			m_pPolePlankRecArr[index].AddPart();
	}

	POLYPARTLIST polylist;
	if(pModel->IsTowerModel())
		((CTower*)pModel)->RetrievePolyPartList(polylist);
	CLDSPolyPart *pPolyPart=NULL;
	for(pPolyPart=polylist.GetFirst();pPolyPart;pPolyPart=polylist.GetNext())
	{
		if(pEmbededBlock&&pEmbededBlock->handle!=pPart->m_hBlock&&!pPart->IsBlockRefPart())
			continue;	//过滤出属于指定部件的构件
		if(!bIgnoreSegI&&pPolyPart->iSeg.iSeg!=m_iSeg.iSeg)		//不是当前段角钢
			continue;
		//非当前段构件时索引可能为-1
		int index = FindPolePlankNoIndex(pPolyPart->GetPartNo());
		if(index==-1)	
			continue;	//index==-1时如果执行以下代码会导致内存越界 wht 10-12-14
		if(m_pPolePlankRecArr[index].GetPartN()<=0)
		{
			double unit_wei = TruncateDouble(pPolyPart->GetWeight(),2);
			m_pPolePlankRecArr[index].SetUnitWeight(unit_wei);
			m_pPolePlankRecArr[index].SetLength(pPolyPart->GetLength());
			if(EXPORT_BY_ENGLISH)
				strcpy(m_pPolePlankRecArr[index].sNotes,"bending");
			else
				strcpy(m_pPolePlankRecArr[index].sNotes,"火曲");
		}
		for(UINT ii=0;ii<pPolyPart->m_uStatMatCoef;ii++)
			m_pPolePlankRecArr[index].AddPart();
	}
}

//统计部件引用
void CSegMaterialSummator::SummarizeBlockRefs()
{
	for(CBlockReference *pBlockRef=Ta.BlockRef.GetFirst();pBlockRef;pBlockRef=Ta.BlockRef.GetNext())
	{
		if(pBlockRef->iSeg.iSeg!=m_iSeg.iSeg)
			continue;
		if(pBlockRef->IsExploded())
			continue;
		CBlockModel *pBlock=Ta.Block.FromHandle(pBlockRef->m_hBlock);
		if(pBlock==NULL)
			continue;
		if(pBlock->IsEmbeded())
		{	//嵌入式部件
			SummarizeParts(&Ta,TRUE,pBlock);
			SummarizeBolt(&Ta,TRUE,pBlock);
		}
		else 
		{	//导入式部件
			SummarizeParts(pBlock,TRUE);
			SummarizeBolt(pBlock,TRUE);
		}
	}
}

BOOL CSegMaterialSummator::RunSummarize(SEGI iSeg)	//进行材料统计
{
	if(iSeg.iSeg<1)
	{
#ifdef AFX_TARG_ENU_ENGLISH
		AfxMessageBox("Material's segment number can't be zero in segment map,please select a right segment number");
#else
		AfxMessageBox("分段图中材料分段号不能为零,请选择一个合适的分段号");
#endif
		return FALSE;
	}
	try{
		m_iSeg = iSeg;
		RunAnalyze();		//进行规格分析和分类
		SummarizeParts(&Ta);
		SummarizeSuppPart();//统计附加构件
		SummarizeBolt(&Ta);
		SummarizeBlockRefs();
	}
	catch(char* sError)
	{
		AfxMessageBox(sError);
		return FALSE;
	}
	return TRUE;
}

void CSegMaterialSummator::RunSummarizeFromBOM(CTowerBOM &bom,SEGI iSeg, bool bStatPart, bool bStatBolt)	//进行材料统计
{
	CManuPart *pPart;
	CStatBolt* pBolt;
	ATOM_LIST<CManuPart> manuSegPartList;
	ATOM_LIST<CStatBolt> statSegBoltList;
	//根据manuPartSegArr填充sPolePlankNo、sPolePlankGuiGe,统计构件数nPartCount,螺栓数nLsCount,脚钉数nNailCount,垫圈//数nWasherCount
	int nLsCount=0,nNailCount=0,nWasherCount=0;
	m_nPolePlank=0;
	if(bStatPart)
	{
		for(pPart=bom.Parts.GetFirst();pPart;pPart=bom.Parts.GetNext())
		{
			if(iSeg.iSeg!=pPart->nSeg.iSeg)
				continue;
			manuSegPartList.append(*pPart);
			if(FindPolePlankNoIndex(pPart->sPartNo)==-1)	//还没有这种规格
			{
				sPolePlankNo.Add(pPart->sPartNo);
				sPolePlankGuiGe.Add(pPart->sSpec);
				cPolePlankMat.Add(pPart->cMaterial);
				m_nPolePlank++;
			}
		}
	}
	if(bStatBolt)
	{
		//根据statSegBoltArr填充sLsGuiGe,sNailGuiGe,sPadGuiGe
		for(pBolt=bom.Bolts.GetFirst();pBolt;pBolt=bom.Bolts.GetNext())
		{
			if(iSeg.iSeg!=pBolt->nSeg.iSeg)
				continue;
			statSegBoltList.append(*pBolt);
			if(pBolt->type==0)//螺栓
			{
				if(FindLsGuiGeIndex(pBolt->sSpec)==-1)//还没有这种规格
				{
					sLsGuiGe.Add(pBolt->sSpec);
					nLsCount++;
				}
			}
			else if(pBolt->type==1)//脚钉
			{
				if(FindLsGuiGeIndex(pBolt->sSpec)==-1)
				{
					sNailGuiGe.Add(pBolt->sSpec);	
					nNailCount++;
				}
			}
			else if(pBolt->type==2)//垫圈
			{
				if(FindLsGuiGeIndex(pBolt->sSpec)==-1)
				{
					sPadGuiGe.Add(pBolt->sSpec);	
					nWasherCount++;
				}
			}
		}
	}
	CreateRecordSet(m_nPolePlank,nLsCount,nNailCount,nWasherCount);
	if(bStatPart&&m_nPolePlank>0)
	{
		int nPart=0;
		// 根据manuSegPartArr更新m_pPolePlankRecArr[iRow]按照原顺序填充当前段构件信息
		for(CManuPart* pManuPart=manuSegPartList.GetFirst();pManuPart;pManuPart=manuSegPartList.GetNext(),nPart++)
		{
			m_pPolePlankRecArr[nPart].SetPartNo(pManuPart->sPartNo);
			strcpy(m_pPolePlankRecArr[nPart].guige,pManuPart->sSpec);
			m_pPolePlankRecArr[nPart].cMaterial = pManuPart->cMaterial;
			m_pPolePlankRecArr[nPart].SetLength(pManuPart->length);
			m_pPolePlankRecArr[nPart].SetPartN(pManuPart->nPartNum);
			m_pPolePlankRecArr[nPart].SetUnitWeight(pManuPart->singWeight);
			m_pPolePlankRecArr[nPart].SetSumWeitht(pManuPart->sumWeight);
			strcpy(m_pPolePlankRecArr[nPart].sNotes,pManuPart->sNote);
		}
	}
	if(bStatBolt&&(nLsCount>0||nNailCount>0||nWasherCount>0))
	{
		int nLs=0,nNail=0,nPat=0;
		// 根据statSegBoltArr更新m_pLsRecArr、m_pNailRecArr、m_pPadRecArr按照原顺序填充当前段螺栓脚钉垫圈信息。
		for(pBolt=statSegBoltList.GetFirst();pBolt;pBolt=statSegBoltList.GetNext())
		{
			if(pBolt->type==0)//螺栓
			{
				m_pLsRecArr[nLs].SetGuiGe(pBolt->sSpec);
				m_pLsRecArr[nLs].SetPartN(pBolt->nCount);
				m_pLsRecArr[nLs].SetSumWeight(pBolt->weight);
#ifdef AFX_TARG_ENU_ENGLISH
				if(strcmp(pBolt->sNote,"Bolt")!=0)
#else
				if(strcmp(pBolt->sNote,"螺栓")!=0)
#endif
					strcpy(m_pLsRecArr[nLs].sNotes,pBolt->sNote);
				nLs++;
			}
			else if(pBolt->type==1)//脚钉
			{
				m_pNailRecArr[nNail].SetGuiGe(pBolt->sSpec);
				m_pNailRecArr[nNail].SetPartN(pBolt->nCount);
				m_pNailRecArr[nNail].SetSumWeight(pBolt->weight);
				if((EXPORT_BY_ENGLISH&&strcmp(pBolt->sNote,"Step Bolt")!=0) ||
				   (!EXPORT_BY_ENGLISH&&strcmp(pBolt->sNote,"脚钉")!=0))
					strcpy(m_pNailRecArr[nNail].sNotes,pBolt->sNote);
				nNail++;
			}
			else if(pBolt->type==2)//垫圈
			{
				m_pPadRecArr[nPat].SetGuiGe(pBolt->sSpec);
				m_pPadRecArr[nPat].SetPartN(pBolt->nCount);
				m_pPadRecArr[nPat].SetSumWeight(pBolt->weight);
				if((EXPORT_BY_ENGLISH&&strcmp(pBolt->sNote,"Washer")!=0) ||
				   (!EXPORT_BY_ENGLISH&&strcmp(pBolt->sNote,"垫圈")!=0))
					strcpy(m_pPadRecArr[nPat].sNotes,pBolt->sNote);
				nPat++;
			}
		}
	}
}
int CSegMaterialSummator::GetJgPlankRecCount()
{
	return m_nPolePlank;
}
CSegMatPolePlankRecord* CSegMaterialSummator::GetJgPlankRecAt(int iRow)
{
	if(iRow<0||iRow>=m_nPolePlank)
		return NULL;
	return &m_pPolePlankRecArr[iRow];
}

int CSegMaterialSummator::GetLsRecCount()	//获得螺栓记录数
{
	return sLsGuiGe.GetSize();
}
int CSegMaterialSummator::GetNailRecCount()	//获得脚钉记录数
{
	return sNailGuiGe.GetSize();
}
/*int CSegMaterialSummator::GetTwoCapLsRecCount()	//获得双帽螺栓记录数
{
	return sTwoCapLsGuiGe.GetSize();
}*/
int CSegMaterialSummator::GetPadRecCount()	//获得垫圈记录数
{
	return sPadGuiGe.GetSize();
}
CSegMatLsRecord* CSegMaterialSummator::GetLsRecAt(int iRow)
{
	if(iRow<0||iRow>=sLsGuiGe.GetSize())
		return NULL;
	return &m_pLsRecArr[iRow];
}
CSegMatLsRecord* CSegMaterialSummator::GetNailRecAt(int iRow)
{
	if(iRow<0||iRow>=sNailGuiGe.GetSize())
		return NULL;
	return &m_pNailRecArr[iRow];
}

/*CSegMatLsRecord* CSegMaterialSummator::GetTwoCapLsRecAt(int iRow)
{
	if(iRow<0||iRow>=sTwoCapLsGuiGe.GetSize())
		return NULL;
	return &m_pTwoCapLsRecArr[iRow];
}*/

CSegMatPadRecord* CSegMaterialSummator::GetPadRecAt(int iRow)
{
	if(iRow<0||iRow>=sPadGuiGe.GetSize())
		return NULL;
	return &m_pPadRecArr[iRow];
}
int CSegMaterialSummator::GetCurSegI()
{
	return m_iSeg;
}

int CSegMaterialSummator::FindPolePlankNoIndex(const char *sPartNo)
{
	int i;
	for(i=0;i<sPolePlankNo.GetSize();i++)
	{
		if(sPolePlankNo[i].CompareNoCase(sPartNo)==0)
			return i;
	}
	return -1;
}
int CSegMaterialSummator::FindLsGuiGeIndex(char *spec)
{
	int i;
	for(i=0;i<sLsGuiGe.GetSize();i++)
	{
		if(sLsGuiGe[i].CompareNoCase(spec)==0)
			return i;
	}
	return -1;
}
int CSegMaterialSummator::FindNailGuiGeIndex(char *spec)
{
	int i;
	for(i=0;i<sNailGuiGe.GetSize();i++)
	{
		if(sNailGuiGe[i].CompareNoCase(spec)==0)
			return i;
	}
	return -1;
}
int CSegMaterialSummator::FindPadGuiGeIndex(char *spec)
{
	int i;
	for(i=0;i<sPadGuiGe.GetSize();i++)
	{
		if(sPadGuiGe[i].CompareNoCase(spec)==0)
			return i;
	}
	return -1;
}
/*int CSegMaterialSummator::FindTwoCapLsGuiGeIndex(char *spec)
{
	int i;
	for(i=0;i<sTwoCapLsGuiGe.GetSize();i++)
	{
		if(sTwoCapLsGuiGe[i].CompareNoCase(spec)==0)
			return i;
	}
	return -1;
}*/
#endif
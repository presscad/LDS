// FLSizeOptimizor.cpp: implementation of the CFLSizeOptimizor class.
//<LOCALE_TRANSLATE BY wbt />
////////////////////////////////////////////////////////////////////// 
#include "stdafx.h"
#include "lds.h"
#include "FLSizeOptimizor.h"
#include "env_def.h"
#include "SortFunc.h"
#include "ArrayList.h"
#include "GlobalFunc.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef __PART_DESIGN_INC_//defined(__LDS_)||defined(__LDS_FILE_)||defined(__TSA_)||defined(__TSA_FILE_)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL CJointCouple::IsRelativeMirWith(CJointCouple *pCouple)
{
	if(pNode->GetRelativeObjInfo(pCouple->pNode->handle,false)==NULL)
		return FALSE;
	else if(pRod->GetRelativeObjInfo(pCouple->pRod->handle,false)==NULL&&
		pRod->GetRelativeObjInfo(pCouple->pLinkRod->handle,false)==NULL)
		return FALSE;
	else if(pLinkRod->GetRelativeObjInfo(pCouple->pLinkRod->handle,false)==NULL&&
		pLinkRod->GetRelativeObjInfo(pCouple->pRod->handle,false)==NULL)
		return FALSE;
	else
		return TRUE;
}

//////////////////////////////////////////////////////////////////////
// class CFLSizeOptimizor Construction/Destruction
//////////////////////////////////////////////////////////////////////
long CFLSizeOptimizor::BOLT_TYPE_N=3;
BOLT_SIZE_INFO CFLSizeOptimizor::BOLTSIZE_TBL[24]={
	{4.8,16, 40212,30,30},
	{6.8,20, 94247,40,40},
	{8.8,24,180955,50,50},
};
DWORD CFLSizeOptimizor::eSpecBoltNum		=0x00000100;	//0x00 0001 00:00000000 0000000000000001 00000000
DWORD CFLSizeOptimizor::eSpecFlDiameter		=0x00000200;	//0x00 0002 00:00000000 0000000000000010 00000000
DWORD CFLSizeOptimizor::eSpecBoltDiameter	=0x00000400;	//0x00 0004 00:00000000 0000000000000100 00000000
DWORD CFLSizeOptimizor::eSpecRibHeight		=0x00000800;	//0x00 0008 00:00000000 0000000000001000 00000000
DWORD CFLSizeOptimizor::eSpecRibThick		=0x80001000;	//0x00 0010 00:00000000 0000000000010000 00000000
CFLSizeOptimizor::CFLSizeOptimizor()
{
	flNoGroup.InitMaxNo(1000);
}

CFLSizeOptimizor::~CFLSizeOptimizor()
{

}
void CFLSizeOptimizor::InitBoltSizeTbl(BOLTSIZE_ITEM *boltSizeArr,int size_n/*=0*/)
{
	if(size_n==0)
	{
		BOLT_TYPE_N=3;
		BOLTSIZE_TBL[0].gradeF=4.8;
		BOLTSIZE_TBL[0].d=16;
		BOLTSIZE_TBL[0].Nt=40212;
		BOLTSIZE_TBL[0].inner_space=30;
		BOLTSIZE_TBL[0].outer_space=30;
		BOLTSIZE_TBL[1].gradeF=6.8;
		BOLTSIZE_TBL[1].d=20;
		BOLTSIZE_TBL[1].Nt=94247;
		BOLTSIZE_TBL[1].inner_space=40;
		BOLTSIZE_TBL[1].outer_space=40;
		BOLTSIZE_TBL[2].gradeF=8.8;
		BOLTSIZE_TBL[2].d=24;
		BOLTSIZE_TBL[2].Nt=180955;
		BOLTSIZE_TBL[2].inner_space=50;
		BOLTSIZE_TBL[2].outer_space=50;
	}
	else
	{
		BOLT_TYPE_N=min(size_n,24);
		for(int i=0;i<BOLT_TYPE_N;i++)
		{
			BOLTSIZE_TBL[i].gradeF=boltSizeArr[i].gradeF;
			BOLTSIZE_TBL[i].d=boltSizeArr[i].d;
			for(int j=0;j<LsTypeCount;j++)
			{
				double gradeF=atof(ls_table[j].grade);
				if(boltSizeArr[i].gradeF==gradeF)
					break;
			}
			if(j<LsTypeCount)
				BOLTSIZE_TBL[i].Nt=Pi*boltSizeArr[i].d*boltSizeArr[i].d*0.25*ls_table[j].f;
			if(boltSizeArr[i].d==16)
				BOLTSIZE_TBL[i].inner_space=BOLTSIZE_TBL[i].outer_space=30;
			else if(boltSizeArr[i].d==20)
				BOLTSIZE_TBL[i].inner_space=BOLTSIZE_TBL[i].outer_space=40;
			else //if(boltSizeArr[i].d==24)
				BOLTSIZE_TBL[i].inner_space=BOLTSIZE_TBL[i].outer_space=50;
		}
	}
}
void CFLSizeOptimizor::Clear()	//清空以前法兰优选缓存数据
{
	flNoGroup.EmptyNo();
	listCouple.Empty();
	hashConnectFL.Empty();
}
void CJointCouple::StatJointLinkPart(ATOM_LIST<CJointCouple>* pListCouple, int idClassType)
{
	CJointCouple *pCouple;
	CSuperSmartPtr<CLDSLinePart>pLinePart,pCoupleRod;
	pListCouple->Empty();
	//清空关联杆件
	for(pLinePart=Ta.Parts.GetFirstLinePart();pLinePart.IsHasPtr();pLinePart=Ta.Parts.GetNextLinePart())
		pLinePart->connectStart.m_hCoupleRod=pLinePart->connectEnd.m_hCoupleRod=0;
	for(pLinePart=Ta.Parts.GetFirstLinePart();pLinePart.IsHasPtr();pLinePart=Ta.Parts.GetNextLinePart())
	{
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;	//非完整力学分析杆件
		if(pLinePart->size_idClassType!=idClassType&&idClassType!=CLS_LINEPART&&idClassType!=0)
			continue;	//非当前匹配范围杆件

		f3dPoint line_vec=pLinePart->pEnd->Position(false)-pLinePart->pStart->Position(false);
		normalize(line_vec);
		if(pLinePart->connectStart.m_hCoupleRod<=0x20)
		{	//杆件始端无共线关联杆件信息
			BOOL pushed=Ta.Parts.push_stack();
			for(pCoupleRod=Ta.Parts.GetNextLinePart();pCoupleRod.IsHasPtr();pCoupleRod=Ta.Parts.GetNextLinePart())
			{
				if(pCoupleRod->pStart!=pLinePart->pStart&&pCoupleRod->pEnd!=pLinePart->pStart)
					continue;	//非共点杆件
				if(pCoupleRod->pStart==NULL||pCoupleRod->pEnd==NULL)
					continue;	//非完整力学分析杆件
				if(pCoupleRod->size_idClassType!=idClassType&&idClassType!=CLS_LINEPART&&idClassType!=0)
					continue;	//非当前匹配范围杆件
				if(pCoupleRod->connectStart.m_hCoupleRod>0x20&&pCoupleRod->connectEnd.m_hCoupleRod>0x20)
					continue;	//两端均已与其余杆件配对，不可能与当前杆件再配对
				f3dPoint line_vec2=pCoupleRod->pEnd->Position(false)-pCoupleRod->pStart->Position(false);
				normalize(line_vec2);
				if(fabs(line_vec*line_vec2)>=EPS_COS)	//找到共线关联杆件
				{
					pLinePart->connectStart.m_hCoupleRod=pCoupleRod->handle;
					if(pCoupleRod->pStart==pLinePart->pStart)
						pCoupleRod->connectStart.m_hCoupleRod=pLinePart->handle;
					else
						pCoupleRod->connectEnd.m_hCoupleRod=pLinePart->handle;
					pCouple=pListCouple->append();
					pCouple->pNode=pLinePart->pStart;
					strcpy(pCouple->label,"");
					strcpy(pCouple->labelCouple,"");
					if(pLinePart->GetWidth()>pCoupleRod->GetWidth())
					{
						pCouple->pRod=pLinePart;
						pCouple->pLinkRod=pCoupleRod;
					}
					else if(pLinePart->GetWidth()<pCoupleRod->GetWidth())
					{
						pCouple->pRod=pCoupleRod;
						pCouple->pLinkRod=pLinePart;
					}
					else if(pLinePart->GetThick()>=pCoupleRod->GetThick())
					{
						pCouple->pRod=pLinePart;
						pCouple->pLinkRod=pCoupleRod;
					}
					else 
					{
						pCouple->pRod=pCoupleRod;
						pCouple->pLinkRod=pLinePart;
					}
					break;
				}
			}
			Ta.Parts.pop_stack(pushed);
		}
		if (pLinePart->connectEnd.m_hCoupleRod<=0x20)
		{	//杆件终端无共线关联杆件信息
			BOOL pushed=Ta.Parts.push_stack();
			for(pCoupleRod=Ta.Parts.GetNextLinePart();pCoupleRod.IsHasPtr();pCoupleRod=Ta.Parts.GetNextLinePart())
			{
				if(pCoupleRod->pStart!=pLinePart->pEnd&&pCoupleRod->pEnd!=pLinePart->pEnd)
					continue;	//非共点杆件
				if(pCoupleRod->pStart==NULL||pCoupleRod->pEnd==NULL)
					continue;	//非完整力学分析杆件
				if(pCoupleRod->size_idClassType!=idClassType&&idClassType!=CLS_LINEPART&&idClassType!=0)
					continue;	//非当前匹配范围杆件
				if(pCoupleRod->connectStart.m_hCoupleRod>0x20&&pCoupleRod->connectEnd.m_hCoupleRod>0x20)
					continue;	//两端均已与其余杆件配对，不可能与当前杆件再配对
				f3dPoint line_vec2=pCoupleRod->pEnd->Position(false)-pCoupleRod->pStart->Position(false);
				normalize(line_vec2);
				if(fabs(line_vec*line_vec2)>=EPS_COS)	//找到共线关联杆件
				{
					pLinePart->connectStart.m_hCoupleRod=pCoupleRod->handle;
					if(pCoupleRod->pStart==pLinePart->pEnd)
						pCoupleRod->connectStart.m_hCoupleRod=pLinePart->handle;
					else
						pCoupleRod->connectEnd.m_hCoupleRod=pLinePart->handle;
					pCouple=pListCouple->append();
					pCouple->pNode=pLinePart->pEnd;
					strcpy(pCouple->label,"");
					strcpy(pCouple->labelCouple,"");
					if(pLinePart->GetWidth()>pCoupleRod->GetWidth())
					{
						pCouple->pRod=pLinePart;
						pCouple->pLinkRod=pCoupleRod;
					}
					else if(pLinePart->GetWidth()<pCoupleRod->GetWidth())
					{
						pCouple->pRod=pCoupleRod;
						pCouple->pLinkRod=pLinePart;
					}
					else if(pLinePart->GetThick()>=pCoupleRod->GetThick())
					{
						pCouple->pRod=pLinePart;
						pCouple->pLinkRod=pCoupleRod;
					}
					else 
					{
						pCouple->pRod=pCoupleRod;
						pCouple->pLinkRod=pLinePart;
					}
					break;
				}
			}
			Ta.Parts.pop_stack(pushed);
		}
	}
	for(pCouple=pListCouple->GetFirst();pCouple;pCouple=pListCouple->GetNext())
	{
		if(pCouple->pRod->pStart==pCouple->pNode&&pCouple->pRod->connectStart.maxTension>pCouple->N)
			pCouple->N=pCouple->pRod->connectStart.maxTension;
		else if(pCouple->pRod->pEnd==pCouple->pNode&&pCouple->pRod->connectEnd.maxTension>pCouple->N)
			pCouple->N=pCouple->pRod->connectEnd.maxTension;
		else if(pCouple->pLinkRod->pStart==pCouple->pNode&&pCouple->pLinkRod->connectStart.maxTension>pCouple->N)
			pCouple->N=pCouple->pLinkRod->connectStart.maxTension;
		else if(pCouple->pLinkRod->pEnd==pCouple->pNode&&pCouple->pLinkRod->connectEnd.maxTension>pCouple->N)
			pCouple->N=pCouple->pLinkRod->connectEnd.maxTension;
	}
}
//0为螺栓序号基数
double CFLSizeOptimizor::CalYi(int i,int n,double bolt_radius,double axisCoordX)
{
	if(n<4)
		n=4;
	double sector=2*Pi/n;
	double alfa=(i+0.5)*sector;
	double cosa=cos(alfa);
	double coordX=cosa*bolt_radius;
	if(coordX>axisCoordX)	//旋转轴右侧
		return 0;
	else
		return axisCoordX-coordX;
}
double CFLSizeOptimizor::CalY1(int n,double bolt_radius,double axisCoordX)
{
	if(n<4)
		n=4;
	double alfa=Pi/n;
	if(n%2==0)
		return axisCoordX+bolt_radius*cos(alfa);
	else
		return axisCoordX+bolt_radius;
}
double CFLSizeOptimizor::CalNtbmaxCoef(int n,double bolt_radius,double axisCoordX)
{
	double Y1=CalY1(n,bolt_radius,axisCoordX);
	double sum_Yi2=0;
	for(int j=0;j<n;j++)
	{
		double Yi=CalYi(j,n,bolt_radius,axisCoordX);
		sum_Yi2+=Yi*Yi;
	}
	return Y1/sum_Yi2;
}
double CFLSizeOptimizor::QueryBetaCoef(double Ly_Lx)
{
	double LyLxTbl[21]={0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95,1.0,1.1,1.2,1.3,1.4,1.5,1.75,2.0};
	double BetaTbl[22]={
		0.0785,0.0834,0.0874,0.0895,0.0900,0.0901,0.0900,0.0897,0.0892,0.0884,0.0872,
		0.0860,0.0848,0.0843,0.0840,0.0838,0.0836,0.0835,0.0834,0.0833,0.0833,0.0833,};
	if(Ly_Lx<=LyLxTbl[0])
		return BetaTbl[0];
	for(int i=0;i<20;i++)
	{
		if(Ly_Lx>LyLxTbl[i]&&Ly_Lx<=LyLxTbl[i+1])
			return BetaTbl[i]+(Ly_Lx-LyLxTbl[i])*(BetaTbl[i+1]-BetaTbl[i])/(LyLxTbl[i+1]-LyLxTbl[i]);
	}
	//if(Ly_Lx>LyLxTbl[20])
	return BetaTbl[21];
}
double CFLSizeOptimizor::QueryAlfaCoef(double Ly_Lx)
{
	double LyLxTbl[21]={0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95,1.0,1.1,1.2,1.3,1.4,1.5,1.75,2.0};
	double AlfaTbl[22]={
		0.67,0.71,0.73,0.74,0.76,0.79,0.80,0.80,0.81,0.82,0.83,
		0.83,0.84,0.85,0.86,0.87,0.88,0.89,0.90,0.91,0.92,1.0 ,};
	if(Ly_Lx<=LyLxTbl[0])
		return AlfaTbl[0];
	for(int i=0;i<20;i++)
	{
		if(Ly_Lx>LyLxTbl[i]&&Ly_Lx<=LyLxTbl[i+1])
			return AlfaTbl[i]+(Ly_Lx-LyLxTbl[i])*(AlfaTbl[i+1]-AlfaTbl[i])/(LyLxTbl[i+1]-LyLxTbl[i]);
	}
	//if(Ly_Lx>LyLxTbl[20])
	return AlfaTbl[21];
}

//自动根据重量优先是否要加劲法兰
//M:弯矩;N轴力(拉力为正);inner_radius:钢管半径;bolt_radius:螺栓圆半径;cMaterial:钢管材质;
CConnectFL CFLSizeOptimizor::SelFL(double M,double N,double inner_radius_arr[2],char cMaterial)
{
	CConnectFL ribFL;
	ribFL=SelFLHasRib(NULL,M,N,inner_radius_arr,cMaterial);
	if(inner_radius_arr[0]!=inner_radius_arr[1])	//原则上不设计管径不一致的无加劲法兰对
		return ribFL;
	CConnectFL noneRibFL;
	noneRibFL=SelFLNoneRib(NULL,M,N,inner_radius_arr[0],cMaterial);
	if(ribFL.SumWeight()>noneRibFL.SumWeight())
		return noneRibFL;
	else
		return ribFL;
}

//M:弯矩(N*m);N轴力(N,拉力为正);inner_radius:钢管半径;bolt_radius:螺栓圆半径;cMaterial:钢管材质;
CConnectFL CFLSizeOptimizor::SelFLHasRib(FLDESIGN_PROC_INFO* pDesign, double M,double N,double inner_radius_arr[2],char cMaterial)
{
	STEELMAT* pMat=QuerySteelMatByBriefMark(cMaterial);
	CConnectFL fl,flTemp;
	flTemp.m_nRibS=20;
	double min_fl_weight;
	double bolt_radius=0;
	if(pDesign)
		pDesign->retCode=0;	//正常设计
	if(pDesign&&designStyleFlag.IsHasFlag(eSpecBoltDiameter))
		bolt_radius=0.5*pDesign->bolt_dia;
	M=fabs(M);
	double max_radius=max(inner_radius_arr[0],inner_radius_arr[1]);
	double min_radius=min(inner_radius_arr[0],inner_radius_arr[1]);
	flTemp.m_fInnerRadius=inner_radius_arr[0];
	flTemp.m_fCoupleInnerRadius=inner_radius_arr[1];
	for(int i=0;i<BOLT_TYPE_N;i++)
	{
		flTemp.bolt_d=BOLTSIZE_TBL[i].d;
		sprintf(flTemp.bolt_grade,"%.1f",BOLTSIZE_TBL[i].gradeF);
		if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecBoltNum))
		{
			flTemp.bolt_n=pDesign->bolt_n;
			if(N>flTemp.bolt_n*BOLTSIZE_TBL[i].Nt)
				continue;	//当前螺栓强度不足
		}
		else
		{
			flTemp.bolt_n=(int)ceil(N/BOLTSIZE_TBL[i].Nt);
			flTemp.bolt_n+=flTemp.bolt_n%2;	//偶数个螺栓
		}
		//下面螺栓数的规定见:
		//《钢结构单管通信塔技术规程》 CECS 236:2008,P22
		//《架空输电线路钢管塔设计技术规定》 DL/T XXXX-2009 P33
		if(flTemp.m_fInnerRadius<60)			
			flTemp.bolt_n=max(flTemp.bolt_n,4);
		else
			flTemp.bolt_n=max(flTemp.bolt_n,6);
		// 计算螺栓圆半径
		flTemp.m_fBoltRadius=max_radius+BOLTSIZE_TBL[i].inner_space;
		if(bolt_radius>flTemp.m_fBoltRadius)
			flTemp.m_fBoltRadius=bolt_radius;

		double Ntbmax=N/flTemp.bolt_n;
		if(M>0)	//拉(压）弯组合
		{	//验证确认螺栓数
			double axisCoordX=0.8*min_radius;
			if(M/fabs(N)<0.0008*min_radius)	// M/N>=0.8*r2
				axisCoordX=0;
			double coefNtbmax=CalNtbmaxCoef(flTemp.bolt_n,flTemp.m_fBoltRadius,axisCoordX);
			Ntbmax=1000*M*coefNtbmax+N/flTemp.bolt_n;
			if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecBoltNum)&&Ntbmax>BOLTSIZE_TBL[i].Nt)
				continue;	//当前螺栓强度不足
			else
			{
				do
				{
					if(Ntbmax>BOLTSIZE_TBL[i].Nt)
						flTemp.bolt_n++;
					else
						break;
					coefNtbmax=CalNtbmaxCoef(flTemp.bolt_n,flTemp.m_fBoltRadius,axisCoordX);
					Ntbmax=1000*M*coefNtbmax+N/flTemp.bolt_n;
				}while(true);
			}
		}
		if(pDesign)
			pDesign->Ntbmax=Ntbmax;
		// 计算法兰盘环形宽度
		double outer_space;
		if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecFlDiameter))	//指定了法兰宽度
		{
			flTemp.m_nWidthFL=ftoi(pDesign->fl_dia*0.5-flTemp.m_fInnerRadius);
			outer_space=pDesign->fl_dia*0.5-flTemp.m_fBoltRadius;
		}
		else
		{
			outer_space=BOLTSIZE_TBL[i].outer_space;
			flTemp.m_nWidthFL=ftoi(flTemp.m_fBoltRadius-flTemp.m_fInnerRadius+outer_space);
		}

		// 计算法兰盘厚度
		// Ly越大法兰盘厚会越小，但由于一般配对法兰允许法兰盘厚不同固应分别设计配对法兰的盘厚
		double Lx=2*Pi*flTemp.m_fBoltRadius/flTemp.bolt_n;
		if(Lx<BOLTSIZE_TBL[i].inner_space+BOLTSIZE_TBL[i].outer_space)
		{
			if(pDesign&&designStyleFlag.IsHasFlag(eSpecBoltDiameter))
			{
				pDesign->retCode=5;	//指定螺栓圆直径太小
				continue;
			}
			else
			{
				int increment=0;
				do{
					increment+=2;
					flTemp.m_fBoltRadius+=2;	//螺栓圆半径以2mm为增量
					Lx=2*Pi*flTemp.m_fBoltRadius/flTemp.bolt_n;
				}while(Lx<BOLTSIZE_TBL[i].inner_space+BOLTSIZE_TBL[i].outer_space);
				if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecFlDiameter))
				{
					if(flTemp.m_fBoltRadius+BOLTSIZE_TBL[i].outer_space>max_radius+flTemp.m_nWidthFL)
					{
						pDesign->retCode=6;	//指定法兰直径太小
						continue;
					}
				}
				else
					flTemp.m_nWidthFL+=increment;
			}

		}
		if(pDesign)
			pDesign->Lx=Lx;
		//计算当前法兰盘板厚
		double Ly=flTemp.m_nWidthFL;
		if(pDesign)
			pDesign->Ly=Ly;
		double q=Ntbmax/(Lx*Ly);
		if(pDesign)
			pDesign->q=q;
		double beta=QueryBetaCoef(Ly/Lx);
		if(pDesign)
			pDesign->beta=beta;
		double Mmax=beta*q*Lx*Lx;
		if(pDesign)
			pDesign->Mox=Mmax;
		//下面法兰盘厚度的规定见:
		//《架空输电线路钢管塔设计技术规定》 DL/T XXXX-2009 P32
		flTemp.m_nThickFL=16;	//初始值
		while(true)
		{	//不同厚度板强度值不同
			long thick=(int)ceil(sqrt(5*Mmax/pMat->f(flTemp.m_nThickFL)));
			thick+=thick%2;	//对齐成偶数板厚
			if(thick<16)
				thick=16;	//规范规定最小厚度为16mm
			if(flTemp.m_nThickFL==thick)
				break;
			flTemp.m_nThickFL=thick;
		}
		if(pDesign)
			pDesign->f_fl=pMat->f(flTemp.m_nThickFL);
		//flTemp.m_nThickFL=max(flTemp.m_nThickFL,10);
		//计算配对法兰盘板厚
		Ly-=flTemp.m_fCoupleInnerRadius-flTemp.m_fInnerRadius;
		q=Ntbmax/(Lx*Ly);
		beta=QueryBetaCoef(Ly/Lx);
		Mmax=beta*q*Lx*Lx;
		flTemp.m_nCoupleThickFL=16;	//初始值
		while(true)
		{	//不同厚度板强度值不同
			long thick=(int)ceil(sqrt(5*Mmax/pMat->f(flTemp.m_nCoupleThickFL)));
			thick+=thick%2;
			if(thick<16)
				thick=16;	//规范规定最小厚度为16mm
			if(flTemp.m_nCoupleThickFL==thick)
				break;
			flTemp.m_nCoupleThickFL=thick;
		}
		flTemp.m_nCoupleThickFL=max(flTemp.m_nCoupleThickFL,10);
		//下面加劲肋板厚度的规定见:
		//《架空输电线路钢管塔设计技术规定》 DL/T XXXX-2009 P33
		//加劲板的尺寸及焊缝长度应由计算结果确定，但其厚度不应小于板长的1/15，并不小于4.0mm

		// 计算加劲肋板厚度
		// 对于加劲肋板Ly越大则加劲板受力就越大，所以此时取较大的Ly设计肋板
		if(flTemp.m_fCoupleInnerRadius<flTemp.m_fInnerRadius)	//Ly越大法兰盘厚会越小，所以在此取较小的Ly进行设计底板厚
			Ly=flTemp.m_nWidthFL+flTemp.m_fInnerRadius-flTemp.m_fCoupleInnerRadius;
		else
			Ly=flTemp.m_nWidthFL;
		double alfa=QueryAlfaCoef(Ly/Lx);
		if(pDesign)
			pDesign->alfa=alfa;
		double alfaNtbmax=alfa*Ntbmax;
		if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecRibThick))
		{
			flTemp.m_nThickRib=min(pDesign->rib_thick,6);
			double sigema=alfaNtbmax/(flTemp.m_nThickRib*(flTemp.m_nWidthFL-flTemp.m_nRibS-2*flTemp.m_nThickRib));
			if(sigema>pMat->fw1(flTemp.m_nThickRib))
			{
				pDesign->retCode=2;
				continue;	//肋板厚度小，不满足强度要求
			}
		}
		else
		{
			for(flTemp.m_nThickRib=6;flTemp.m_nThickFL<flTemp.m_nWidthFL/2-10;flTemp.m_nThickFL+=2)
			{
				double sigema=alfaNtbmax/(flTemp.m_nThickRib*(flTemp.m_nWidthFL-flTemp.m_nRibS-2*flTemp.m_nThickRib));
				if(sigema<=pMat->fw1(flTemp.m_nThickRib))
					break;
			}
		}
		if(pDesign)
		{
			pDesign->fw_rib=pMat->fw1(flTemp.m_nThickRib);
			pDesign->fvw_rib=pMat->fvw(flTemp.m_nThickRib);
		}
		// 计算加劲肋板高度,由于配对法兰加劲肋板宽度可能不同，所以设计高度也可能不同，配对法兰应分别设计
		double e=flTemp.m_fBoltRadius-flTemp.m_fInnerRadius;
		double tao,sigema,sigema_tao;
		if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecRibHeight))
		{
			flTemp.m_nHeightRib=pDesign->rib_height;
			tao=alfaNtbmax/(flTemp.m_nHeightRib*flTemp.m_nThickRib);		//肋板高度(mm)
			sigema=6*alfaNtbmax*e/(flTemp.m_nThickRib*pow((double)(flTemp.m_nHeightRib-2*flTemp.m_nThickRib),2));
			sigema_tao=sqrt(sigema*sigema+3*tao*tao)/1.1;
			if(tao>pMat->fvw(flTemp.m_nThickRib)||sigema>pMat->fw1(flTemp.m_nThickRib)||sigema_tao>pMat->fw1(flTemp.m_nThickRib))
			{
				pDesign->retCode=3;
				continue;	//肋板高度不满足强度要求
			}
		}
		else
		{
			for(flTemp.m_nHeightRib=100;true;flTemp.m_nHeightRib+=5)
			{
				tao=alfaNtbmax/(flTemp.m_nHeightRib*flTemp.m_nThickRib);		//肋板高度(mm)
				sigema=6*alfaNtbmax*e/(flTemp.m_nThickRib*pow((double)(flTemp.m_nHeightRib-2*flTemp.m_nThickRib),2));
				sigema_tao=sqrt(sigema*sigema+3*tao*tao)/1.1;
				int verify_thick=(int)ceil((flTemp.m_nHeightRib+flTemp.m_nRibS)/15.0);
				if(verify_thick>flTemp.m_nThickRib)
				{
					flTemp.m_nThickRib=verify_thick;
					flTemp.m_nThickRib+=flTemp.m_nThickRib%2;
				}
				if(tao<=pMat->fvw(flTemp.m_nThickRib)&&sigema<=pMat->fw1(flTemp.m_nThickRib)&&sigema_tao<=pMat->fw1(flTemp.m_nThickRib))
					break;
			}
		}
		if(pDesign)
		{
			pDesign->tao_rib=tao;
			pDesign->sigm_rib=sigema;
		}
		//设计配对法兰盘加劲肋板高度
		e=flTemp.m_fBoltRadius-flTemp.m_fCoupleInnerRadius;
		if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecRibHeight))
		{
			flTemp.m_nCoupleHeightRib=pDesign->rib_height_couple;
			tao=alfaNtbmax/(flTemp.m_nCoupleHeightRib*flTemp.m_nThickRib);		//肋板高度(mm)
			sigema=6*alfaNtbmax*e/(flTemp.m_nThickRib*pow((double)(flTemp.m_nCoupleHeightRib-2*flTemp.m_nThickRib),2));
			sigema_tao=sqrt(sigema*sigema+3*tao*tao)/1.1;
			if(tao>pMat->fvw(flTemp.m_nThickRib)||sigema>pMat->fw1(flTemp.m_nThickRib)||sigema_tao>pMat->fw1(flTemp.m_nThickRib))
			{
				pDesign->retCode=4;
				continue;	//配对法兰肋板高度不满足强度要求
			}
		}
		else
		{
			for(flTemp.m_nCoupleHeightRib=100;true;flTemp.m_nCoupleHeightRib+=5)
			{
				double tao=alfaNtbmax/(flTemp.m_nCoupleHeightRib*flTemp.m_nThickRib);		//肋板高度(mm)
				double sigema=6*alfaNtbmax*e/(flTemp.m_nThickRib*pow((double)(flTemp.m_nCoupleHeightRib-2*flTemp.m_nThickRib),2));
				double sigema_tao=sqrt(sigema*sigema+3*tao*tao)/1.1;
				int verify_thick=(int)ceil((flTemp.m_nCoupleHeightRib+flTemp.m_nRibS)/15.0);
				if(verify_thick>flTemp.m_nThickRib)
				{
					flTemp.m_nThickRib=verify_thick;
					flTemp.m_nThickRib+=flTemp.m_nThickRib%2;
				}
				if(tao<=pMat->fvw(flTemp.m_nThickRib)&&sigema<=pMat->fw1(flTemp.m_nThickRib)&&sigema_tao<=pMat->fw1(flTemp.m_nThickRib))
					break;
			}
		}
		flTemp.m_nHeightRib+=flTemp.m_nRibS;	//增加一个20mm倒角高度
		flTemp.m_nCoupleHeightRib+=flTemp.m_nRibS;	//增加一个20mm倒角高度
		if(i==0)
		{
			fl=flTemp;
			min_fl_weight=fl.SumWeight();
		}
		else if(flTemp.SumWeight()<fl.SumWeight())
		{
			min_fl_weight=flTemp.SumWeight();
			fl=flTemp;
		}
		else
			break;
	}
	if(i<BOLT_TYPE_N&&pDesign)
		pDesign->retCode=0;
	return fl;
}
//M:弯矩;N轴力(拉力为正);inner_radius:钢管半径;bolt_radius:螺栓圆半径;cMaterial:钢管材质;
CConnectFL CFLSizeOptimizor::SelFLNoneRib(FLDESIGN_PROC_INFO* pDesign, double M,double N,double inner_radius,char cMaterial)
{
	STEELMAT* pMat=QuerySteelMatByBriefMark(cMaterial);
	CConnectFL fl,flTemp;
	double min_fl_weight;
	double bolt_radius=0;
	if(pDesign)
		pDesign->retCode=0;	//正常设计
	if(pDesign&&designStyleFlag.IsHasFlag(eSpecBoltDiameter))
		bolt_radius=0.5*pDesign->bolt_dia;
	M=fabs(M);
	flTemp.m_fInnerRadius=inner_radius;
	for(int i=0;i<BOLT_TYPE_N;i++)
	{
		flTemp.bolt_d=BOLTSIZE_TBL[i].d;
		sprintf(flTemp.bolt_grade,"%.1f",BOLTSIZE_TBL[i].gradeF);
		// 计算螺栓圆半径
		flTemp.m_fBoltRadius=flTemp.m_fInnerRadius+BOLTSIZE_TBL[i].inner_space;
		if(bolt_radius>flTemp.m_fBoltRadius)
			flTemp.m_fBoltRadius=bolt_radius;
		// 计算法兰盘环形宽度
		double outer_space=BOLTSIZE_TBL[i].outer_space;
		if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecFlDiameter))	//指定了法兰宽度
		{
			flTemp.m_nWidthFL=ftoi(pDesign->fl_dia*0.5-flTemp.m_fInnerRadius);
			outer_space=pDesign->fl_dia*0.5-flTemp.m_fBoltRadius;
		}
		else
			flTemp.m_nWidthFL=ftoi(flTemp.m_fBoltRadius-flTemp.m_fInnerRadius+outer_space);
		if(pDesign)
			pDesign->m=0.65;
		double Ntbmax=0;
		double Nts=0.65*(flTemp.m_nWidthFL/outer_space)*N;
		if(M>0)	//拉(压）弯组合
			Nts+=2000*M/flTemp.m_fInnerRadius;
		if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecBoltNum))
		{
			flTemp.bolt_n=pDesign->bolt_n;
			if(Nts>flTemp.bolt_n*BOLTSIZE_TBL[i].Nt)
				continue;	//当前螺栓强度不足
		}
		else
		{
			flTemp.bolt_n=(int)ceil(Nts/BOLTSIZE_TBL[i].Nt);
			flTemp.bolt_n+=flTemp.bolt_n%2;	//偶数个螺栓
		}
		//下面螺栓数的规定见:
		//《钢结构单管通信塔技术规程》 CECS 236:2008,P22
		//《架空输电线路钢管塔设计技术规定》 DL/T XXXX-2009 P33
		if(flTemp.m_fInnerRadius<60)
			flTemp.bolt_n=max(flTemp.bolt_n,4);
		else
			flTemp.bolt_n=max(flTemp.bolt_n,6);
		Ntbmax=Nts/flTemp.bolt_n;
		// 计算法兰盘厚度
		double Rf=Ntbmax*(flTemp.m_fBoltRadius-flTemp.m_fInnerRadius)/outer_space;
		//下面法兰盘厚度的规定见:
		//《架空输电线路钢管塔设计技术规定》 DL/T XXXX-2009 P33
		long thick=20;
		flTemp.m_nThickFL=20;
		do
		{
			flTemp.m_nThickFL=thick;
			double S=2*Pi*flTemp.m_fBoltRadius/flTemp.bolt_n;
			double e0=0.5*BOLTSIZE_TBL[i].outer_space;
			double t1=1.5*Rf/(S*pMat->fv(thick));
			double t2=sqrt(5*Rf*e0/(S*pMat->f(thick)));
			thick=ftoi(ceil(max(t1,t2)));
			thick+=thick%2;
			if(thick<20)
				thick=20;	//规范规定最小厚度为16mm
		}while(flTemp.m_nThickFL!=thick);
		flTemp.m_nThickFL=thick;
		flTemp.m_nThickRib=0;	//无加劲肋板
		if(i==0)
		{
			fl=flTemp;
			min_fl_weight=fl.SumWeight();
		}
		else if(flTemp.SumWeight()<fl.SumWeight())
		{
			min_fl_weight=flTemp.SumWeight();
			fl=flTemp;
		}
		else
			break;
	}
	return fl;
}

void CFLSizeOptimizor::GenStep1FLSet()
{
	double N=0,M=0;
	for(CJointCouple *pCouple=listCouple.GetFirst();pCouple;pCouple=listCouple.GetNext())
	{
		if(strlen(pCouple->label)>0)
			continue;	//已设计
		N=pCouple->N;
		M=pCouple->M;
		BOOL bPush=listCouple.push_stack();
		CArray<CJointCouple*,CJointCouple*>relaCouple;
		relaCouple.Add(pCouple);
		for(CJointCouple *pMirCouple=listCouple.GetNext();pMirCouple;pMirCouple=listCouple.GetNext())
		{
			if(strlen(pMirCouple->label)>0)
				continue;	//已设计
			if(pCouple->IsRelativeMirWith(pMirCouple))
			{
				if(pCouple->N>N)
					N=pCouple->N;
				if(pCouple->M>M)
					M=pCouple->M;
				relaCouple.Add(pMirCouple);
			}
		}
		if(bPush)
			listCouple.pop_stack();
		double radius_arr[2]={pCouple->pRod->GetWidth()*0.5,pCouple->pLinkRod->GetWidth()*0.5};
		CConnectFL fl=SelFL(M,N,radius_arr,pCouple->pRod->cMaterial);
		fl.cMaterial=pCouple->pRod->cMaterial;
		//法兰中内径较大的一半
		long iNo=0;
		iNo=flNoGroup.EnumIdleNo();
		flNoGroup.SetNoState(iNo);
		hashFLStep.SetValue(iNo,fl);
		_snprintf(pCouple->label,15,"%d",iNo);
		//法兰中内径较小的一半
		CConnectFL flCouple=fl;
		if(pCouple->pLinkRod->GetWidth()!=pCouple->pRod->GetWidth())
		{	//配对法兰管径不一致时
			flCouple.m_fInnerRadius=fl.m_fCoupleInnerRadius;
			flCouple.m_nThickFL=fl.m_nCoupleThickFL;
			flCouple.m_nHeightRib=fl.m_nCoupleHeightRib;
			flCouple.m_nWidthFL=fl.m_nWidthFL+ftoi(fl.m_fInnerRadius-fl.m_fCoupleInnerRadius);
			iNo=flNoGroup.EnumIdleNo();
			flNoGroup.SetNoState(iNo);
			hashFLStep.SetValue(iNo,flCouple);
		}
		_snprintf(pCouple->labelCouple,15,"%d",iNo);
		for(int i=0;i<relaCouple.GetSize();i++)
		{
			if(relaCouple[i]!=pCouple)
			{
				if(relaCouple[i]->pRod->GetWidth()==fl.m_fInnerRadius*2)
				{
					strcpy(relaCouple[i]->label,pCouple->label);
					strcpy(relaCouple[i]->labelCouple,pCouple->labelCouple);
				}
				else
				{
					strcpy(relaCouple[i]->labelCouple,pCouple->label);
					strcpy(relaCouple[i]->label,pCouple->labelCouple);
				}
			}
			if(relaCouple[i]->pRod->pStart==relaCouple[i]->pNode)
			{
				relaCouple[i]->pRod->connectStart.m_iConnectType=0;
				strcpy(relaCouple[i]->pRod->connectStart.flNo,relaCouple[i]->label);
			}
			else
			{
				strcpy(relaCouple[i]->pRod->connectEnd.flNo,relaCouple[i]->label);
				relaCouple[i]->pRod->connectEnd.m_iConnectType=0;
			}
			if(relaCouple[i]->pLinkRod->pStart==relaCouple[i]->pNode)
			{
				relaCouple[i]->pLinkRod->connectStart.m_iConnectType=0;
				strcpy(relaCouple[i]->pLinkRod->connectStart.flNo,relaCouple[i]->labelCouple);
			}
			else
			{
				strcpy(relaCouple[i]->pLinkRod->connectEnd.flNo,relaCouple[i]->labelCouple);
				relaCouple[i]->pLinkRod->connectEnd.m_iConnectType=0;
			}
		}
	}
}
//法兰螺栓圆直径由小到大，管径小的在前，管径大的在后
typedef CConnectFL* CConnectFLPtr;
int compare_fl_func(const CConnectFLPtr& fl1,const CConnectFLPtr& fl2)
{
	if(fl1->m_fBoltRadius>fl2->m_fBoltRadius)
		return 1;
	else if(fl1->m_fBoltRadius<fl2->m_fBoltRadius)
		return -1;
	else if(fl1->m_fInnerRadius>fl2->m_fInnerRadius)
		return 1;
	else if(fl1->m_fInnerRadius<fl2->m_fInnerRadius)
		return -1;
	else
		return 0;
}
void CFLSizeOptimizor::GenFLSet()
{
	InitBoltSizeTbl(NULL);
	designStyleFlag.Clear();	//清空所有指定项设计选项，进行全自由优化设计
	UINT i,serial;
	CConnectFL *pFL;
	CHashList<CXhChar16>hashNoStep1,hashNoStep2;
	//1. 汇总分类配对原始法兰集合为最终法兰
	//分两步进行，每一步骤结束都会生成新的法兰汇总配对分类结果和新法兰序号
	//1.1 按法兰的对称分类
	GenStep1FLSet();
	//1.2 减少法兰分类优化，重点是将内径一致、螺栓圆近似，但法兰类型、法兰底板厚、螺栓直径、螺栓数不一致的情况统一设计明细
	//优化结果是，同一内径及螺栓圆的法兰，设计明细相同
	for(pFL=hashFLStep.GetFirst();pFL;pFL=hashFLStep.GetNext())
	{	//暂未进行优化
		CConnectFL *pFL2=hashConnectFL.SetValue(hashFLStep.GetCursorKey(),*pFL);
		_snprintf(pFL2->m_sPartNo,15,"%d",hashFLStep.GetCursorKey());
	}
	//1.3 按法兰螺栓圆直径分类命名，管径小的在前，管径大的在后
	CConnectFLPtr* fl_arr = new CConnectFLPtr[hashFLStep.GetNodeNum()];
	for(pFL=hashConnectFL.GetFirst(),i=0;pFL;pFL=hashConnectFL.GetNext(),i++)
		fl_arr[i]=pFL;
	CHeapSort<CConnectFLPtr>::HeapSort(fl_arr,hashConnectFL.GetNodeNum(),compare_fl_func);
	serial=1;
	char cRank='A';
	for(i=0;i<hashConnectFL.GetNodeNum();i++)
	{
		if(i>0&&fl_arr[i]->m_fBoltRadius!=fl_arr[i-1]->m_fBoltRadius)
		{	//一个新系列法兰
			serial++;
			cRank='A';
		}
		else if(i>0)
			cRank+=1;
		int iNo=atoi(fl_arr[i]->m_sPartNo);
		_snprintf(fl_arr[i]->m_sPartNo,15,"FL%d%C",serial,cRank);
		hashNoStep2.SetValue(iNo,CXhChar16(fl_arr[i]->m_sPartNo));
	}
	//2. 向塔数据库装载汇总分类配对后的法兰集合
	Ta.hashConnectFl.Empty();
	for(i=0;i<hashConnectFL.GetNodeNum();i++)
		Ta.hashConnectFl.append(*fl_arr[i]);
	delete []fl_arr;
	//3. 更新
	for(CJointCouple *pJoint=listCouple.GetFirst();pJoint;pJoint=listCouple.GetNext())
	{
		long iNo=atoi(pJoint->label);
		pFL=hashConnectFL.GetValue(iNo);
		if(pFL)
			strcpy(pJoint->label,pFL->m_sPartNo);
		iNo=atoi(pJoint->labelCouple);
		pFL=hashConnectFL.GetValue(iNo);
		if(pFL)
			strcpy(pJoint->labelCouple,pFL->m_sPartNo);
	}
	//4. 向装配法兰的钢管杆件赋值法兰最终编号信息
	for(CLDSLinePart* pLinePart = Ta.Parts.GetFirstLinePart();pLinePart;pLinePart = Ta.Parts.GetNextLinePart())
	{
		if(pLinePart->size_idClassType==CLS_LINETUBE)
		{
			if(pLinePart->connectStart.m_iConnectType==0)
			{
				long iNo=atoi(pLinePart->connectStart.flNo);
				//long iNoStep1=hashNoStep1[iNo];
				//long iNoStep2=hashNoStep2[iNoStep1];
				pFL=hashConnectFL.GetValue(iNo);
				if(pFL)
					strncpy(pLinePart->connectStart.flNo,pFL->m_sPartNo,15);
				else
					strcpy(pLinePart->connectStart.flNo,"");
			}
			if(pLinePart->connectEnd.m_iConnectType==0)
			{
				long iNo=atoi(pLinePart->connectEnd.flNo);
				pFL=hashConnectFL.GetValue(iNo);
				if(pFL)
					strncpy(pLinePart->connectEnd.flNo,pFL->m_sPartNo,15);
				else
					strcpy(pLinePart->connectEnd.flNo,"");
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
// class CAngleJointSizeOptimizor Construction/Destruction
//////////////////////////////////////////////////////////////////////
//根据主材规格以及螺栓数自动判断角钢接头类型以及参数,判断规则如下:
//1.接头为单剪连接时,采用外包角钢,外包角钢的宽度应比被连接角钢肢宽大一级
//  (长细比在80以下时,外包角钢肢厚再大一级),被连接角钢规格不同时,应取其小的规格
//2.接头为双剪连接时,采用内包角钢外贴板,内包角钢和外贴板的面积和宜不小于被连接角钢面积的1.3倍
//3.L140以上规格的角钢宜采用双包连接
//cJointType 0.外包铁 1.内外包铁 2.内包铁外包钢板 3.内包钢板外包铁 4.对角组合接头A 
//			 5.对角组合接头B 6.对角组合接头C 7.对角组合接头D 8.十字组合接头A 9.十字组合接头B
double CalAngleBoltHoleCoef(int rows,int bolt_d,JG_PARA_TYPE* pAngleType)
{
	JGZJ jgzj;
	double lsn_coef,holedia=bolt_d+1.5;
	getjgzj(jgzj,pAngleType->wing_wide);
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,bolt_d);
	if(rows==1||jgzj.g2-jgzj.g1==0)
	{	//螺栓单排布置
		double lsn_coef1=1;	//仍要考虑两肢同有螺栓的情形
		double g=jgzj.g*2-pAngleType->wing_thick;	//对应规定:g=2d1-t
		double lsn_coef2=2-pow(LsSpace.SingleRowSpace/2.0,2)/(4*g*holedia);
		lsn_coef=max(lsn_coef1,lsn_coef2);
	}
	else if(rows==2)
	{	//螺栓双排布置时
		double lsn_coef1=2;
		double lsn_coef2=3-pow(LsSpace.doubleRowSpace/2.0,2)/(4*(jgzj.g2-jgzj.g1)*holedia);
		double g=jgzj.g1*2-pAngleType->wing_thick;	//对应规定:g=2d1-t
		double lsn_coef3=4-pow(LsSpace.doubleRowSpace/2.0,2)*
			(2.0/(jgzj.g2-jgzj.g1)+1/g)/(4*holedia);
		lsn_coef=max(lsn_coef1,lsn_coef2);
		lsn_coef=max(lsn_coef,lsn_coef3);
	}
	else if(rows==3)
		return 3.8;	//三排，暂按3.8减孔系数
	else
		return 1.33;//按单排处理，一般不会运行至此
	return lsn_coef;
}
bool CAngleJointDesigner::SelectAngleJoint(ANGLE_JOINT* pAngleJoint,char cJointType)
{
	if(pAngleJoint==NULL)
		return false;
	ANGLE_JOINT& joint=*pAngleJoint;
	if(!pAngleJoint->VerifyLinkAngles())
	{
#ifdef AFX_TARG_ENU_ENGLISH
		logerr.Log("The connect angle's information of curent joint is wrong!");
#else 
		logerr.Log("当前接头的连接角钢信息有误!");
#endif
		return false;
	}
	bool finished=false;	//设计未完成
	JG_JOINT_PARA* pJointPara=FindJgJointPara(pAngleJoint->smallAngle.width,pAngleJoint->smallAngle.thick);
	if(pJointPara!=NULL)
		pAngleJoint->cJointType=0;	//默认为内包铁，外包钢板
	else if(pAngleJoint->smallAngle.width<100)	//默认为斜材外包铁形式
		pAngleJoint->cJointType=1;
	else	//默认为主材内包铁，外包钢板形式
		pAngleJoint->cJointType=0;

	pAngleJoint->bolt.d=SelBoltDByWingWide(pAngleJoint->smallAngle.width);
	JG_PARA_TYPE* pJgType=FindJgType(pAngleJoint->smallAngle.width,pAngleJoint->smallAngle.thick,'L');
	BYTE gradeArr[3]={48,68,88};
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,pAngleJoint->bolt.d);
	JGZJ bigjgzj,smalljgzj;
	getjgzj(bigjgzj,pAngleJoint->bigAngle.width);
	getjgzj(smalljgzj,pAngleJoint->smallAngle.width);
	bool inited=false;
	int grade_index=0;
	pAngleJoint->bolt.grade=gradeArr[grade_index];
	if(pAngleJoint->smallAngle.cMaterial==CSteelMatLibrary::Q420BriefMark()||
		pAngleJoint->smallAngle.cMaterial==CSteelMatLibrary::Q460BriefMark())
		pAngleJoint->bolt.grade=88;
	else if(pAngleJoint->bolt.d==16)
		pAngleJoint->bolt.grade=48;
	else if(pAngleJoint->bolt.d=20)
		pAngleJoint->bolt.grade=68;
	else
		pAngleJoint->bolt.grade=88;
	pAngleJoint->bolt.n=4;
	if(pAngleJoint->smallAngle.width<=110)
		pAngleJoint->bolt.rows=1;
	else //if(pAngleJoint->smallAngle.width<250)
		pAngleJoint->bolt.rows=2;
	do{
		if(!inited && pJointPara!=NULL)
		{
			pAngleJoint->innerAngle.width=pJointPara->fInnerJgWidth;
			pAngleJoint->innerAngle.thick=pJointPara->fInnerJgThick;
			pAngleJoint->innerAngle.cMaterial=pAngleJoint->smallAngle.cMaterial;
			pAngleJoint->innerAngle.cSubClassType='L';
			pAngleJoint->bolt.n=pAngleJoint->TestCalMinBoltN();
			inited=true;
		}
		else if(!inited)
		{
			pAngleJoint->bolt.n=pAngleJoint->TestCalMinBoltN();
			if(pAngleJoint->cJointType==1)	//默认为斜材外包铁形式
			{
				pAngleJoint->outer.angle.cMaterial=pAngleJoint->smallAngle.cMaterial;
				for(int i=0;i<jgguige_N;i++)
				{
					if(jgguige_table[i].cType!='L')
						continue;
					if( jgguige_table[i].wing_wide > pAngleJoint->smallAngle.width&&
						jgguige_table[i].wing_thick>=pAngleJoint->smallAngle.thick)
					{
						pAngleJoint->outer.angle.width=jgguige_table[i].wing_wide;
						pAngleJoint->outer.angle.thick=jgguige_table[i].wing_thick;
						inited=true;
						break;	//找到合适的默认外包铁
					}
				}
			}
			else //if(pAngleJoint->cJointType==0)//默认为主材内包铁，外包钢板形式
			{
				pAngleJoint->outer.plate.cMaterial=pAngleJoint->innerAngle.cMaterial=pAngleJoint->smallAngle.cMaterial;
				for(int i=jgguige_N-1;i>=0;i--)
				{
					if(jgguige_table[i].cType!='L')
						continue;
					if( jgguige_table[i].wing_wide < pAngleJoint->smallAngle.width&&
						jgguige_table[i].wing_thick<=pAngleJoint->smallAngle.thick)
					{
						pAngleJoint->innerAngle.width=jgguige_table[i].wing_wide;
						pAngleJoint->innerAngle.thick=jgguige_table[i].wing_thick;
						pAngleJoint->outer.plate.thick=pAngleJoint->smallAngle.thick;
						if(pAngleJoint->bolt.rows==1)	//单排
							pAngleJoint->outer.plate.width=LsSpace.EndSpace*2+20;
						else if(pAngleJoint->bolt.rows==2)
							pAngleJoint->outer.plate.width=LsSpace.EndSpace*2+20+((bigjgzj.g2-bigjgzj.g1)/5)*5;
						else
							pAngleJoint->outer.plate.width=LsSpace.EndSpace*2+20+((bigjgzj.g3-bigjgzj.g1)/5)*5;
						inited=true;
						break;	//找到合适的默认外包铁
					}
				}
			}
		}
		if(!inited)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Can't find right angle specification in angle's joint library.");
#else 
			logerr.Log("未在角钢规格库中找到合适的接头角钢规格");
#endif
			if(pAngleJoint->cJointType==1)	//默认为斜材外包铁形式
				pAngleJoint->outer.angle=pAngleJoint->smallAngle;
			else
				pAngleJoint->outer.plate=pAngleJoint->innerAngle=pAngleJoint->smallAngle;
			return false;
		}
		inited=true;
		if(pAngleJoint->smallAngle.width<=110)
			pAngleJoint->bolt.rows=1;
		else if(pAngleJoint->smallAngle.width<250)
			pAngleJoint->bolt.rows=2;
		else
			pAngleJoint->bolt.rows=3;
		pAngleJoint->bolt.holeout_coef=CalAngleBoltHoleCoef(pAngleJoint->bolt.rows,pAngleJoint->bolt.d,pJgType);
		if(pAngleJoint->bolt.rows==3&&pAngleJoint->bolt.n<=20)
		{
			pAngleJoint->bolt.rows=2;
			pAngleJoint->bolt.holeout_coef=CalAngleBoltHoleCoef(pAngleJoint->bolt.rows,pAngleJoint->bolt.d,pJgType);
		}
		pAngleJoint->DesignCheckAreaAndBoltForceScale();
		if(pAngleJoint->pTagInfo->scaleAreaJ2A2<1.3)
		{	//截面积裕度小
			bool incremented=false;
			if(pAngleJoint->cJointType==0)	//内包铁外包钢板
			{
				for(int i=0;i<jgguige_N;i++)
				{
					if(jgguige_table[i].cType!='L')
						continue;
					if( jgguige_table[i].wing_wide > pAngleJoint->outer.angle.width&&
						jgguige_table[i].wing_thick>=pAngleJoint->outer.angle.thick)
					{
						pAngleJoint->outer.angle.width=jgguige_table[i].wing_wide;
						pAngleJoint->outer.angle.thick=jgguige_table[i].wing_thick;
						incremented=true;
						break;	//找到合适的默认外包铁
					}
				}
			}
			else if(pAngleJoint->cJointType==1)	//斜材外包铁形式
			{
				for(int i=0;i<jgguige_N;i++)
				{
					if(jgguige_table[i].cType!='L')
						continue;
					if( jgguige_table[i].wing_wide > pAngleJoint->outer.angle.width&&
						jgguige_table[i].wing_thick>=pAngleJoint->outer.angle.thick)
					{
						pAngleJoint->outer.angle.width=jgguige_table[i].wing_wide;
						pAngleJoint->outer.angle.thick=jgguige_table[i].wing_thick;
						incremented=true;
						break;	//找到合适的默认外包铁
					}
				}
			}
			if(!incremented)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Can't find right angle specification in angle's joint library.");
#else 
				logerr.Log("未在角钢规格库中找到合适的接头角钢规格");
#endif
				return false;
			}
		}
		if(pAngleJoint->pTagInfo->scaleForceB2A2<1.1)
		{
			pAngleJoint->bolt.n+=2;
			if(pAngleJoint->bolt.n>10&&pAngleJoint->bolt.rows==1&&smalljgzj.g2>smalljgzj.g1)
				pAngleJoint->bolt.rows=2;	//螺栓排数由单排升为双排
			else if(pAngleJoint->bolt.n>=24&&pAngleJoint->bolt.rows==2&&smalljgzj.g3>smalljgzj.g3)
				pAngleJoint->bolt.rows=3;	//螺栓排数由双排升为三排
		}
		if( pAngleJoint->pTagInfo->scaleAreaJ2A2>=1.3&&pAngleJoint->pTagInfo->scaleForceB2A2>=1.1)
			break;	//接头截面积裕度30%（行业习惯），螺栓承载力裕度10%（信狐自定）
	}while(!finished);
	return true;
}
void CAngleJointDesigner::GenAngleJointSet(ATOM_LIST<CJointCouple> *pListCouple,CHashList<ANGLE_JOINT>* pHashAngleJoints)
{
	pHashAngleJoints->Empty();
	for(CJointCouple* pCouple=pListCouple->GetFirst();pCouple;pCouple=pListCouple->GetNext())
	{
		ANGLE_JOINT* pAngleJoint=pCouple->info.pAngleJoint=pHashAngleJoints->Add(0);
		pAngleJoint->hNode=pCouple->pNode->handle;
		pAngleJoint->hSmallRod=pCouple->pRod->handle;
		pAngleJoint->hBigRod=pCouple->pLinkRod->handle;
		pAngleJoint->LoadTagInfo()->pSmallAngle=(CLDSLineAngle*)pCouple->pRod;
		pAngleJoint->LoadTagInfo()->pBigAngle=(CLDSLineAngle*)pCouple->pLinkRod;
		SelectAngleJoint(pAngleJoint);
		if(pCouple->pRod->pStart==pCouple->pNode)
			pCouple->pRod->connectStart.idAngleJoint=pAngleJoint->Id();
		else if(pCouple->pRod->pEnd==pCouple->pNode)
			pCouple->pRod->connectEnd.idAngleJoint=pAngleJoint->Id();
		if(pCouple->pLinkRod->pStart==pCouple->pNode)
			pCouple->pLinkRod->connectStart.idAngleJoint=pAngleJoint->Id();
		else if(pCouple->pLinkRod->pEnd==pCouple->pNode)
			pCouple->pLinkRod->connectEnd.idAngleJoint=pAngleJoint->Id();
	}
}
#endif


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
void CFLSizeOptimizor::Clear()	//�����ǰ������ѡ��������
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
	//��չ����˼�
	for(pLinePart=Ta.Parts.GetFirstLinePart();pLinePart.IsHasPtr();pLinePart=Ta.Parts.GetNextLinePart())
		pLinePart->connectStart.m_hCoupleRod=pLinePart->connectEnd.m_hCoupleRod=0;
	for(pLinePart=Ta.Parts.GetFirstLinePart();pLinePart.IsHasPtr();pLinePart=Ta.Parts.GetNextLinePart())
	{
		if(pLinePart->pStart==NULL||pLinePart->pEnd==NULL)
			continue;	//��������ѧ�����˼�
		if(pLinePart->size_idClassType!=idClassType&&idClassType!=CLS_LINEPART&&idClassType!=0)
			continue;	//�ǵ�ǰƥ�䷶Χ�˼�

		f3dPoint line_vec=pLinePart->pEnd->Position(false)-pLinePart->pStart->Position(false);
		normalize(line_vec);
		if(pLinePart->connectStart.m_hCoupleRod<=0x20)
		{	//�˼�ʼ���޹��߹����˼���Ϣ
			BOOL pushed=Ta.Parts.push_stack();
			for(pCoupleRod=Ta.Parts.GetNextLinePart();pCoupleRod.IsHasPtr();pCoupleRod=Ta.Parts.GetNextLinePart())
			{
				if(pCoupleRod->pStart!=pLinePart->pStart&&pCoupleRod->pEnd!=pLinePart->pStart)
					continue;	//�ǹ���˼�
				if(pCoupleRod->pStart==NULL||pCoupleRod->pEnd==NULL)
					continue;	//��������ѧ�����˼�
				if(pCoupleRod->size_idClassType!=idClassType&&idClassType!=CLS_LINEPART&&idClassType!=0)
					continue;	//�ǵ�ǰƥ�䷶Χ�˼�
				if(pCoupleRod->connectStart.m_hCoupleRod>0x20&&pCoupleRod->connectEnd.m_hCoupleRod>0x20)
					continue;	//���˾���������˼���ԣ��������뵱ǰ�˼������
				f3dPoint line_vec2=pCoupleRod->pEnd->Position(false)-pCoupleRod->pStart->Position(false);
				normalize(line_vec2);
				if(fabs(line_vec*line_vec2)>=EPS_COS)	//�ҵ����߹����˼�
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
		{	//�˼��ն��޹��߹����˼���Ϣ
			BOOL pushed=Ta.Parts.push_stack();
			for(pCoupleRod=Ta.Parts.GetNextLinePart();pCoupleRod.IsHasPtr();pCoupleRod=Ta.Parts.GetNextLinePart())
			{
				if(pCoupleRod->pStart!=pLinePart->pEnd&&pCoupleRod->pEnd!=pLinePart->pEnd)
					continue;	//�ǹ���˼�
				if(pCoupleRod->pStart==NULL||pCoupleRod->pEnd==NULL)
					continue;	//��������ѧ�����˼�
				if(pCoupleRod->size_idClassType!=idClassType&&idClassType!=CLS_LINEPART&&idClassType!=0)
					continue;	//�ǵ�ǰƥ�䷶Χ�˼�
				if(pCoupleRod->connectStart.m_hCoupleRod>0x20&&pCoupleRod->connectEnd.m_hCoupleRod>0x20)
					continue;	//���˾���������˼���ԣ��������뵱ǰ�˼������
				f3dPoint line_vec2=pCoupleRod->pEnd->Position(false)-pCoupleRod->pStart->Position(false);
				normalize(line_vec2);
				if(fabs(line_vec*line_vec2)>=EPS_COS)	//�ҵ����߹����˼�
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
//0Ϊ��˨��Ż���
double CFLSizeOptimizor::CalYi(int i,int n,double bolt_radius,double axisCoordX)
{
	if(n<4)
		n=4;
	double sector=2*Pi/n;
	double alfa=(i+0.5)*sector;
	double cosa=cos(alfa);
	double coordX=cosa*bolt_radius;
	if(coordX>axisCoordX)	//��ת���Ҳ�
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

//�Զ��������������Ƿ�Ҫ�Ӿ�����
//M:���;N����(����Ϊ��);inner_radius:�ֹܰ뾶;bolt_radius:��˨Բ�뾶;cMaterial:�ֹܲ���;
CConnectFL CFLSizeOptimizor::SelFL(double M,double N,double inner_radius_arr[2],char cMaterial)
{
	CConnectFL ribFL;
	ribFL=SelFLHasRib(NULL,M,N,inner_radius_arr,cMaterial);
	if(inner_radius_arr[0]!=inner_radius_arr[1])	//ԭ���ϲ���ƹܾ���һ�µ��޼Ӿ�������
		return ribFL;
	CConnectFL noneRibFL;
	noneRibFL=SelFLNoneRib(NULL,M,N,inner_radius_arr[0],cMaterial);
	if(ribFL.SumWeight()>noneRibFL.SumWeight())
		return noneRibFL;
	else
		return ribFL;
}

//M:���(N*m);N����(N,����Ϊ��);inner_radius:�ֹܰ뾶;bolt_radius:��˨Բ�뾶;cMaterial:�ֹܲ���;
CConnectFL CFLSizeOptimizor::SelFLHasRib(FLDESIGN_PROC_INFO* pDesign, double M,double N,double inner_radius_arr[2],char cMaterial)
{
	STEELMAT* pMat=QuerySteelMatByBriefMark(cMaterial);
	CConnectFL fl,flTemp;
	flTemp.m_nRibS=20;
	double min_fl_weight;
	double bolt_radius=0;
	if(pDesign)
		pDesign->retCode=0;	//�������
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
				continue;	//��ǰ��˨ǿ�Ȳ���
		}
		else
		{
			flTemp.bolt_n=(int)ceil(N/BOLTSIZE_TBL[i].Nt);
			flTemp.bolt_n+=flTemp.bolt_n%2;	//ż������˨
		}
		//������˨���Ĺ涨��:
		//���ֽṹ����ͨ����������̡� CECS 236:2008,P22
		//���ܿ������·�ֹ�����Ƽ����涨�� DL/T XXXX-2009 P33
		if(flTemp.m_fInnerRadius<60)			
			flTemp.bolt_n=max(flTemp.bolt_n,4);
		else
			flTemp.bolt_n=max(flTemp.bolt_n,6);
		// ������˨Բ�뾶
		flTemp.m_fBoltRadius=max_radius+BOLTSIZE_TBL[i].inner_space;
		if(bolt_radius>flTemp.m_fBoltRadius)
			flTemp.m_fBoltRadius=bolt_radius;

		double Ntbmax=N/flTemp.bolt_n;
		if(M>0)	//��(ѹ�������
		{	//��֤ȷ����˨��
			double axisCoordX=0.8*min_radius;
			if(M/fabs(N)<0.0008*min_radius)	// M/N>=0.8*r2
				axisCoordX=0;
			double coefNtbmax=CalNtbmaxCoef(flTemp.bolt_n,flTemp.m_fBoltRadius,axisCoordX);
			Ntbmax=1000*M*coefNtbmax+N/flTemp.bolt_n;
			if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecBoltNum)&&Ntbmax>BOLTSIZE_TBL[i].Nt)
				continue;	//��ǰ��˨ǿ�Ȳ���
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
		// ���㷨���̻��ο��
		double outer_space;
		if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecFlDiameter))	//ָ���˷������
		{
			flTemp.m_nWidthFL=ftoi(pDesign->fl_dia*0.5-flTemp.m_fInnerRadius);
			outer_space=pDesign->fl_dia*0.5-flTemp.m_fBoltRadius;
		}
		else
		{
			outer_space=BOLTSIZE_TBL[i].outer_space;
			flTemp.m_nWidthFL=ftoi(flTemp.m_fBoltRadius-flTemp.m_fInnerRadius+outer_space);
		}

		// ���㷨���̺��
		// LyԽ�����̺��ԽС��������һ����Է����������̺�ͬ��Ӧ�ֱ������Է������̺�
		double Lx=2*Pi*flTemp.m_fBoltRadius/flTemp.bolt_n;
		if(Lx<BOLTSIZE_TBL[i].inner_space+BOLTSIZE_TBL[i].outer_space)
		{
			if(pDesign&&designStyleFlag.IsHasFlag(eSpecBoltDiameter))
			{
				pDesign->retCode=5;	//ָ����˨Բֱ��̫С
				continue;
			}
			else
			{
				int increment=0;
				do{
					increment+=2;
					flTemp.m_fBoltRadius+=2;	//��˨Բ�뾶��2mmΪ����
					Lx=2*Pi*flTemp.m_fBoltRadius/flTemp.bolt_n;
				}while(Lx<BOLTSIZE_TBL[i].inner_space+BOLTSIZE_TBL[i].outer_space);
				if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecFlDiameter))
				{
					if(flTemp.m_fBoltRadius+BOLTSIZE_TBL[i].outer_space>max_radius+flTemp.m_nWidthFL)
					{
						pDesign->retCode=6;	//ָ������ֱ��̫С
						continue;
					}
				}
				else
					flTemp.m_nWidthFL+=increment;
			}

		}
		if(pDesign)
			pDesign->Lx=Lx;
		//���㵱ǰ�����̰��
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
		//���淨���̺�ȵĹ涨��:
		//���ܿ������·�ֹ�����Ƽ����涨�� DL/T XXXX-2009 P32
		flTemp.m_nThickFL=16;	//��ʼֵ
		while(true)
		{	//��ͬ��Ȱ�ǿ��ֵ��ͬ
			long thick=(int)ceil(sqrt(5*Mmax/pMat->f(flTemp.m_nThickFL)));
			thick+=thick%2;	//�����ż�����
			if(thick<16)
				thick=16;	//�淶�涨��С���Ϊ16mm
			if(flTemp.m_nThickFL==thick)
				break;
			flTemp.m_nThickFL=thick;
		}
		if(pDesign)
			pDesign->f_fl=pMat->f(flTemp.m_nThickFL);
		//flTemp.m_nThickFL=max(flTemp.m_nThickFL,10);
		//������Է����̰��
		Ly-=flTemp.m_fCoupleInnerRadius-flTemp.m_fInnerRadius;
		q=Ntbmax/(Lx*Ly);
		beta=QueryBetaCoef(Ly/Lx);
		Mmax=beta*q*Lx*Lx;
		flTemp.m_nCoupleThickFL=16;	//��ʼֵ
		while(true)
		{	//��ͬ��Ȱ�ǿ��ֵ��ͬ
			long thick=(int)ceil(sqrt(5*Mmax/pMat->f(flTemp.m_nCoupleThickFL)));
			thick+=thick%2;
			if(thick<16)
				thick=16;	//�淶�涨��С���Ϊ16mm
			if(flTemp.m_nCoupleThickFL==thick)
				break;
			flTemp.m_nCoupleThickFL=thick;
		}
		flTemp.m_nCoupleThickFL=max(flTemp.m_nCoupleThickFL,10);
		//����Ӿ��߰��ȵĹ涨��:
		//���ܿ������·�ֹ�����Ƽ����涨�� DL/T XXXX-2009 P33
		//�Ӿ���ĳߴ缰���쳤��Ӧ�ɼ�����ȷ���������Ȳ�ӦС�ڰ峤��1/15������С��4.0mm

		// ����Ӿ��߰���
		// ���ڼӾ��߰�LyԽ����Ӿ���������Խ�����Դ�ʱȡ�ϴ��Ly����߰�
		if(flTemp.m_fCoupleInnerRadius<flTemp.m_fInnerRadius)	//LyԽ�����̺��ԽС�������ڴ�ȡ��С��Ly������Ƶװ��
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
				continue;	//�߰���С��������ǿ��Ҫ��
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
		// ����Ӿ��߰�߶�,������Է����Ӿ��߰��ȿ��ܲ�ͬ��������Ƹ߶�Ҳ���ܲ�ͬ����Է���Ӧ�ֱ����
		double e=flTemp.m_fBoltRadius-flTemp.m_fInnerRadius;
		double tao,sigema,sigema_tao;
		if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecRibHeight))
		{
			flTemp.m_nHeightRib=pDesign->rib_height;
			tao=alfaNtbmax/(flTemp.m_nHeightRib*flTemp.m_nThickRib);		//�߰�߶�(mm)
			sigema=6*alfaNtbmax*e/(flTemp.m_nThickRib*pow((double)(flTemp.m_nHeightRib-2*flTemp.m_nThickRib),2));
			sigema_tao=sqrt(sigema*sigema+3*tao*tao)/1.1;
			if(tao>pMat->fvw(flTemp.m_nThickRib)||sigema>pMat->fw1(flTemp.m_nThickRib)||sigema_tao>pMat->fw1(flTemp.m_nThickRib))
			{
				pDesign->retCode=3;
				continue;	//�߰�߶Ȳ�����ǿ��Ҫ��
			}
		}
		else
		{
			for(flTemp.m_nHeightRib=100;true;flTemp.m_nHeightRib+=5)
			{
				tao=alfaNtbmax/(flTemp.m_nHeightRib*flTemp.m_nThickRib);		//�߰�߶�(mm)
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
		//�����Է����̼Ӿ��߰�߶�
		e=flTemp.m_fBoltRadius-flTemp.m_fCoupleInnerRadius;
		if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecRibHeight))
		{
			flTemp.m_nCoupleHeightRib=pDesign->rib_height_couple;
			tao=alfaNtbmax/(flTemp.m_nCoupleHeightRib*flTemp.m_nThickRib);		//�߰�߶�(mm)
			sigema=6*alfaNtbmax*e/(flTemp.m_nThickRib*pow((double)(flTemp.m_nCoupleHeightRib-2*flTemp.m_nThickRib),2));
			sigema_tao=sqrt(sigema*sigema+3*tao*tao)/1.1;
			if(tao>pMat->fvw(flTemp.m_nThickRib)||sigema>pMat->fw1(flTemp.m_nThickRib)||sigema_tao>pMat->fw1(flTemp.m_nThickRib))
			{
				pDesign->retCode=4;
				continue;	//��Է����߰�߶Ȳ�����ǿ��Ҫ��
			}
		}
		else
		{
			for(flTemp.m_nCoupleHeightRib=100;true;flTemp.m_nCoupleHeightRib+=5)
			{
				double tao=alfaNtbmax/(flTemp.m_nCoupleHeightRib*flTemp.m_nThickRib);		//�߰�߶�(mm)
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
		flTemp.m_nHeightRib+=flTemp.m_nRibS;	//����һ��20mm���Ǹ߶�
		flTemp.m_nCoupleHeightRib+=flTemp.m_nRibS;	//����һ��20mm���Ǹ߶�
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
//M:���;N����(����Ϊ��);inner_radius:�ֹܰ뾶;bolt_radius:��˨Բ�뾶;cMaterial:�ֹܲ���;
CConnectFL CFLSizeOptimizor::SelFLNoneRib(FLDESIGN_PROC_INFO* pDesign, double M,double N,double inner_radius,char cMaterial)
{
	STEELMAT* pMat=QuerySteelMatByBriefMark(cMaterial);
	CConnectFL fl,flTemp;
	double min_fl_weight;
	double bolt_radius=0;
	if(pDesign)
		pDesign->retCode=0;	//�������
	if(pDesign&&designStyleFlag.IsHasFlag(eSpecBoltDiameter))
		bolt_radius=0.5*pDesign->bolt_dia;
	M=fabs(M);
	flTemp.m_fInnerRadius=inner_radius;
	for(int i=0;i<BOLT_TYPE_N;i++)
	{
		flTemp.bolt_d=BOLTSIZE_TBL[i].d;
		sprintf(flTemp.bolt_grade,"%.1f",BOLTSIZE_TBL[i].gradeF);
		// ������˨Բ�뾶
		flTemp.m_fBoltRadius=flTemp.m_fInnerRadius+BOLTSIZE_TBL[i].inner_space;
		if(bolt_radius>flTemp.m_fBoltRadius)
			flTemp.m_fBoltRadius=bolt_radius;
		// ���㷨���̻��ο��
		double outer_space=BOLTSIZE_TBL[i].outer_space;
		if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecFlDiameter))	//ָ���˷������
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
		if(M>0)	//��(ѹ�������
			Nts+=2000*M/flTemp.m_fInnerRadius;
		if(pDesign&&designStyleFlag.IsHasFlag(this->eSpecBoltNum))
		{
			flTemp.bolt_n=pDesign->bolt_n;
			if(Nts>flTemp.bolt_n*BOLTSIZE_TBL[i].Nt)
				continue;	//��ǰ��˨ǿ�Ȳ���
		}
		else
		{
			flTemp.bolt_n=(int)ceil(Nts/BOLTSIZE_TBL[i].Nt);
			flTemp.bolt_n+=flTemp.bolt_n%2;	//ż������˨
		}
		//������˨���Ĺ涨��:
		//���ֽṹ����ͨ����������̡� CECS 236:2008,P22
		//���ܿ������·�ֹ�����Ƽ����涨�� DL/T XXXX-2009 P33
		if(flTemp.m_fInnerRadius<60)
			flTemp.bolt_n=max(flTemp.bolt_n,4);
		else
			flTemp.bolt_n=max(flTemp.bolt_n,6);
		Ntbmax=Nts/flTemp.bolt_n;
		// ���㷨���̺��
		double Rf=Ntbmax*(flTemp.m_fBoltRadius-flTemp.m_fInnerRadius)/outer_space;
		//���淨���̺�ȵĹ涨��:
		//���ܿ������·�ֹ�����Ƽ����涨�� DL/T XXXX-2009 P33
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
				thick=20;	//�淶�涨��С���Ϊ16mm
		}while(flTemp.m_nThickFL!=thick);
		flTemp.m_nThickFL=thick;
		flTemp.m_nThickRib=0;	//�޼Ӿ��߰�
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
			continue;	//�����
		N=pCouple->N;
		M=pCouple->M;
		BOOL bPush=listCouple.push_stack();
		CArray<CJointCouple*,CJointCouple*>relaCouple;
		relaCouple.Add(pCouple);
		for(CJointCouple *pMirCouple=listCouple.GetNext();pMirCouple;pMirCouple=listCouple.GetNext())
		{
			if(strlen(pMirCouple->label)>0)
				continue;	//�����
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
		//�������ھ��ϴ��һ��
		long iNo=0;
		iNo=flNoGroup.EnumIdleNo();
		flNoGroup.SetNoState(iNo);
		hashFLStep.SetValue(iNo,fl);
		_snprintf(pCouple->label,15,"%d",iNo);
		//�������ھ���С��һ��
		CConnectFL flCouple=fl;
		if(pCouple->pLinkRod->GetWidth()!=pCouple->pRod->GetWidth())
		{	//��Է����ܾ���һ��ʱ
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
//������˨Բֱ����С���󣬹ܾ�С����ǰ���ܾ�����ں�
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
	designStyleFlag.Clear();	//�������ָ�������ѡ�����ȫ�����Ż����
	UINT i,serial;
	CConnectFL *pFL;
	CHashList<CXhChar16>hashNoStep1,hashNoStep2;
	//1. ���ܷ������ԭʼ��������Ϊ���շ���
	//���������У�ÿһ����������������µķ���������Է��������·������
	//1.1 �������ĶԳƷ���
	GenStep1FLSet();
	//1.2 ���ٷ��������Ż����ص��ǽ��ھ�һ�¡���˨Բ���ƣ����������͡������װ����˨ֱ������˨����һ�µ����ͳһ�����ϸ
	//�Ż�����ǣ�ͬһ�ھ�����˨Բ�ķ����������ϸ��ͬ
	for(pFL=hashFLStep.GetFirst();pFL;pFL=hashFLStep.GetNext())
	{	//��δ�����Ż�
		CConnectFL *pFL2=hashConnectFL.SetValue(hashFLStep.GetCursorKey(),*pFL);
		_snprintf(pFL2->m_sPartNo,15,"%d",hashFLStep.GetCursorKey());
	}
	//1.3 ��������˨Բֱ�������������ܾ�С����ǰ���ܾ�����ں�
	CConnectFLPtr* fl_arr = new CConnectFLPtr[hashFLStep.GetNodeNum()];
	for(pFL=hashConnectFL.GetFirst(),i=0;pFL;pFL=hashConnectFL.GetNext(),i++)
		fl_arr[i]=pFL;
	CHeapSort<CConnectFLPtr>::HeapSort(fl_arr,hashConnectFL.GetNodeNum(),compare_fl_func);
	serial=1;
	char cRank='A';
	for(i=0;i<hashConnectFL.GetNodeNum();i++)
	{
		if(i>0&&fl_arr[i]->m_fBoltRadius!=fl_arr[i-1]->m_fBoltRadius)
		{	//һ����ϵ�з���
			serial++;
			cRank='A';
		}
		else if(i>0)
			cRank+=1;
		int iNo=atoi(fl_arr[i]->m_sPartNo);
		_snprintf(fl_arr[i]->m_sPartNo,15,"FL%d%C",serial,cRank);
		hashNoStep2.SetValue(iNo,CXhChar16(fl_arr[i]->m_sPartNo));
	}
	//2. �������ݿ�װ�ػ��ܷ�����Ժ�ķ�������
	Ta.hashConnectFl.Empty();
	for(i=0;i<hashConnectFL.GetNodeNum();i++)
		Ta.hashConnectFl.append(*fl_arr[i]);
	delete []fl_arr;
	//3. ����
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
	//4. ��װ�䷨���ĸֹܸ˼���ֵ�������ձ����Ϣ
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
//�������Ĺ���Լ���˨���Զ��жϽǸֽ�ͷ�����Լ�����,�жϹ�������:
//1.��ͷΪ��������ʱ,��������Ǹ�,����ǸֵĿ��Ӧ�ȱ����ӽǸ�֫���һ��
//  (��ϸ����80����ʱ,����Ǹ�֫���ٴ�һ��),�����ӽǸֹ��ͬʱ,Ӧȡ��С�Ĺ��
//2.��ͷΪ˫������ʱ,�����ڰ��Ǹ�������,�ڰ��Ǹֺ��������������˲�С�ڱ����ӽǸ������1.3��
//3.L140���Ϲ��ĽǸ��˲���˫������
//cJointType 0.����� 1.������� 2.�ڰ�������ְ� 3.�ڰ��ְ������ 4.�Խ���Ͻ�ͷA 
//			 5.�Խ���Ͻ�ͷB 6.�Խ���Ͻ�ͷC 7.�Խ���Ͻ�ͷD 8.ʮ����Ͻ�ͷA 9.ʮ����Ͻ�ͷB
double CalAngleBoltHoleCoef(int rows,int bolt_d,JG_PARA_TYPE* pAngleType)
{
	JGZJ jgzj;
	double lsn_coef,holedia=bolt_d+1.5;
	getjgzj(jgzj,pAngleType->wing_wide);
	LSSPACE_STRU LsSpace;
	GetLsSpace(LsSpace,bolt_d);
	if(rows==1||jgzj.g2-jgzj.g1==0)
	{	//��˨���Ų���
		double lsn_coef1=1;	//��Ҫ������֫ͬ����˨������
		double g=jgzj.g*2-pAngleType->wing_thick;	//��Ӧ�涨:g=2d1-t
		double lsn_coef2=2-pow(LsSpace.SingleRowSpace/2.0,2)/(4*g*holedia);
		lsn_coef=max(lsn_coef1,lsn_coef2);
	}
	else if(rows==2)
	{	//��˨˫�Ų���ʱ
		double lsn_coef1=2;
		double lsn_coef2=3-pow(LsSpace.doubleRowSpace/2.0,2)/(4*(jgzj.g2-jgzj.g1)*holedia);
		double g=jgzj.g1*2-pAngleType->wing_thick;	//��Ӧ�涨:g=2d1-t
		double lsn_coef3=4-pow(LsSpace.doubleRowSpace/2.0,2)*
			(2.0/(jgzj.g2-jgzj.g1)+1/g)/(4*holedia);
		lsn_coef=max(lsn_coef1,lsn_coef2);
		lsn_coef=max(lsn_coef,lsn_coef3);
	}
	else if(rows==3)
		return 3.8;	//���ţ��ݰ�3.8����ϵ��
	else
		return 1.33;//�����Ŵ���һ�㲻����������
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
		logerr.Log("��ǰ��ͷ�����ӽǸ���Ϣ����!");
#endif
		return false;
	}
	bool finished=false;	//���δ���
	JG_JOINT_PARA* pJointPara=FindJgJointPara(pAngleJoint->smallAngle.width,pAngleJoint->smallAngle.thick);
	if(pJointPara!=NULL)
		pAngleJoint->cJointType=0;	//Ĭ��Ϊ�ڰ���������ְ�
	else if(pAngleJoint->smallAngle.width<100)	//Ĭ��Ϊб���������ʽ
		pAngleJoint->cJointType=1;
	else	//Ĭ��Ϊ�����ڰ���������ְ���ʽ
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
			if(pAngleJoint->cJointType==1)	//Ĭ��Ϊб���������ʽ
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
						break;	//�ҵ����ʵ�Ĭ�������
					}
				}
			}
			else //if(pAngleJoint->cJointType==0)//Ĭ��Ϊ�����ڰ���������ְ���ʽ
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
						if(pAngleJoint->bolt.rows==1)	//����
							pAngleJoint->outer.plate.width=LsSpace.EndSpace*2+20;
						else if(pAngleJoint->bolt.rows==2)
							pAngleJoint->outer.plate.width=LsSpace.EndSpace*2+20+((bigjgzj.g2-bigjgzj.g1)/5)*5;
						else
							pAngleJoint->outer.plate.width=LsSpace.EndSpace*2+20+((bigjgzj.g3-bigjgzj.g1)/5)*5;
						inited=true;
						break;	//�ҵ����ʵ�Ĭ�������
					}
				}
			}
		}
		if(!inited)
		{
#ifdef AFX_TARG_ENU_ENGLISH
			logerr.Log("Can't find right angle specification in angle's joint library.");
#else 
			logerr.Log("δ�ڽǸֹ������ҵ����ʵĽ�ͷ�Ǹֹ��");
#endif
			if(pAngleJoint->cJointType==1)	//Ĭ��Ϊб���������ʽ
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
		{	//�����ԣ��С
			bool incremented=false;
			if(pAngleJoint->cJointType==0)	//�ڰ�������ְ�
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
						break;	//�ҵ����ʵ�Ĭ�������
					}
				}
			}
			else if(pAngleJoint->cJointType==1)	//б���������ʽ
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
						break;	//�ҵ����ʵ�Ĭ�������
					}
				}
			}
			if(!incremented)
			{
#ifdef AFX_TARG_ENU_ENGLISH
				logerr.Log("Can't find right angle specification in angle's joint library.");
#else 
				logerr.Log("δ�ڽǸֹ������ҵ����ʵĽ�ͷ�Ǹֹ��");
#endif
				return false;
			}
		}
		if(pAngleJoint->pTagInfo->scaleForceB2A2<1.1)
		{
			pAngleJoint->bolt.n+=2;
			if(pAngleJoint->bolt.n>10&&pAngleJoint->bolt.rows==1&&smalljgzj.g2>smalljgzj.g1)
				pAngleJoint->bolt.rows=2;	//��˨�����ɵ�����Ϊ˫��
			else if(pAngleJoint->bolt.n>=24&&pAngleJoint->bolt.rows==2&&smalljgzj.g3>smalljgzj.g3)
				pAngleJoint->bolt.rows=3;	//��˨������˫����Ϊ����
		}
		if( pAngleJoint->pTagInfo->scaleAreaJ2A2>=1.3&&pAngleJoint->pTagInfo->scaleForceB2A2>=1.1)
			break;	//��ͷ�����ԣ��30%����ҵϰ�ߣ�����˨������ԣ��10%���ź��Զ���
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


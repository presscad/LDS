#include "stdafx.h"
#include "LDS.h"
#include "stdio.h"
#include "math.h"
#include "Tower.h"
#include "database.h"
#include "Query.h"
#include "GlobalFunc.h"
#include "env_def.h"
#include "FLSizeOptimizor.h"
#include "PartSizeOptimizor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if defined(__LDS_)||defined(__TSA_)
#include "PreProcessor.h"

static void CalElemHoleOutNum(CElement *pElem)
{	//���㵱ǰ��Ԫ����˨������
	char sError[100]="";
	JGZJ jgzj;
	double lsn_coef;		//��˨������
	int ls_n,ls_d;
	if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE
		||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		//�õ�Ԫ���²��Թ���׵����񵴲�������������ʵ��ͳ�Ĳ������ʴ˴�������ѡ��ǰ�˼����
		double wing_wide=pElem->pOrgPole->GetWidth();	//pElem->GetWidth();
		double wing_thick=pElem->pOrgPole->GetThick();	//pElem->GetThick();
		getjgzj(jgzj,wing_wide);//��ýǸ�׼���
		
		char cType='L';	//��ͨ���Ǹ�
		if(pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
			cType=pElem->pOrgPole->size_cSubClassType;
		JG_PARA_TYPE *pJgType=FindJgType(wing_wide,wing_thick,cType);
		if(pJgType==NULL)
		{
			sprintf(sError,"�������Ҳ���0X%X�ĽǸֹ��%s%.0f*%.0f����˨����������ʧ�ܣ�",pElem->pOrgPole->handle,pElem->pOrgPole->GetBriefPartTypeMark(),wing_wide,wing_thick);
			throw sError;
		}
		//JG_PARA_TYPE *pJgType=(JG_PARA_TYPE*)pElem->pParaType;
		if(pElem->pStart==pElem->pOrgPole->pStart)
		{
			ls_d=pElem->pOrgPole->connectStart.d;
			ls_n=pElem->pOrgPole->connectStart.wnConnBoltN;
		}
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		{
			ls_d=pElem->pOrgPole->connectEnd.d;
			ls_n=pElem->pOrgPole->connectEnd.wnConnBoltN;
		}
		else
		{
			ls_d=pElem->pOrgPole->connectStart.d;
			ls_n=max(pElem->pOrgPole->connectStart.wnConnBoltN,pElem->pOrgPole->connectEnd.wnConnBoltN);
		}

		//���´������ڼ�����˨�����Ľ������ЧӦ
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,ls_d);
		if(!pElem->pOrgPole->m_bAutoCalHoleOutCoef)	//�����û�ָ���˼����������
			lsn_coef=pElem->pOrgPole->hole_out;
		else	//��Ҫ������˨���Զ�����˼�����ļ�����
		{	
			if(pElem->start_force_type==ECCENTRIC_FORCE&&pElem->end_force_type==ECCENTRIC_FORCE)
			{	//�����Ϊ��֫����(��Ҫ������һ֫�����в�����˨������)
				int nMaxLsRows;
				//�Զ��жϸ˼��ϵ���˨����
				if(ls_n<5)
					nMaxLsRows=1;
				else
					nMaxLsRows=2;
				
				if(nMaxLsRows==1||jgzj.g2-jgzj.g1==0)
				{
					if(toupper(pElem->pOrgPole->layer(1))=='H')
					{	//��˨���Ų��ã����ں����Ҫ������һ֫ͬ�е�������˨������
						double lsn_coef1=1;
						double g=jgzj.g*2-wing_thick;	//��Ӧ�涨:g=2d1-t
						double lsn_coef2=2-pow(LsSpace.SingleRowSpace/2.0,2)/(4*g*ls_d);
						lsn_coef=max(lsn_coef1,lsn_coef2);
					}
					else
						lsn_coef=1;
				}
				else// if(nMaxLsRows==2)
				{	//��˨˫�Ų���ʱ
					double lsn_coef1=2;
					double lsn_coef2=3-pow(LsSpace.doubleRowSpace/2.0,2)/(4*(jgzj.g2-jgzj.g1)*ls_d);
					lsn_coef=max(lsn_coef1,lsn_coef2);
				}
			}
			else
			{	//����������һ��Ϊ˫֫����
				int nMaxLsRows;
				//�Զ��жϸ˼��ϵ���˨����
				if(ls_n<5)
					nMaxLsRows=1;
				else
					nMaxLsRows=2;

				if(nMaxLsRows==1||jgzj.g2-jgzj.g1==0)
				{	//��˨���Ų���
					double lsn_coef1=1;	//��Ҫ������֫ͬ����˨������
					double g=jgzj.g*2-wing_thick;	//��Ӧ�涨:g=2d1-t
					double lsn_coef2=2-pow(LsSpace.SingleRowSpace/2.0,2)/(4*g*ls_d);
					lsn_coef=max(lsn_coef1,lsn_coef2);
				}
				else //if(nMaxLsRows==2)
				{	//��˨˫�Ų���ʱ
					double lsn_coef1=2;
					double lsn_coef2=3-pow(LsSpace.doubleRowSpace/2.0,2)/(4*(jgzj.g2-jgzj.g1)*ls_d);
					double g=jgzj.g1*2-wing_thick;	//��Ӧ�涨:g=2d1-t
					double lsn_coef3=4-pow(LsSpace.doubleRowSpace/2.0,2)*
						(2.0/(jgzj.g2-jgzj.g1)+1/g)/(4*ls_d);
					lsn_coef=max(lsn_coef1,lsn_coef2);
					lsn_coef=max(lsn_coef,lsn_coef3);
				}
			}
			if(pElem->pOrgPole->layer(1)=='Z')
				lsn_coef=max(lsn_coef,2);	//��Ϊ��������֫�����п��ܳ��ֶԿ�����
		}
		if(pElem->pOrgPole->m_bAutoCalHoleOutCoef)
			pElem->pOrgPole->hole_out=lsn_coef;
		if(toupper(pJgType->cType)=='T'||toupper(pJgType->cType)=='D')
			lsn_coef*=2;
		else if(toupper(pJgType->cType)=='X')
			lsn_coef*=4;
		pElem->design_info.hole_out=lsn_coef;
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
	{
		double wing_wide=pElem->GetWidth();
		double wing_thick=pElem->GetThick();
		getjgzj(jgzj,wing_wide);
		
		if(pElem->pStart==pElem->pOrgPole->pStart)
		{
			ls_d=pElem->pOrgPole->connectStart.d;
			ls_n=pElem->pOrgPole->connectStart.wnConnBoltN;
		}
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		{
			ls_d=pElem->pOrgPole->connectEnd.d;
			ls_n=pElem->pOrgPole->connectEnd.wnConnBoltN;
		}
		else
		{
			ls_d=pElem->pOrgPole->connectStart.d;
			ls_n=max(pElem->pOrgPole->connectStart.wnConnBoltN,pElem->pOrgPole->connectEnd.wnConnBoltN);
		}

		//���´������ڼ�����˨�����Ľ������ЧӦ
		LSSPACE_STRU LsSpace;
		GetLsSpace(LsSpace,ls_d);
		if(pElem->pOrgPole->hole_out>0)	//�����û�ָ���˼����������
			lsn_coef=pElem->pOrgPole->hole_out;
		else	//��Ҫ������˨���Զ�����˼�����ļ�����
		{	
			//�����Ϊ��֫����(��Ҫ������һ֫�����в�����˨������)
			int nMaxLsRows;
			//�Զ��жϸ˼��ϵ���˨����
			if(ls_n<5)
				nMaxLsRows=1;
			else
				nMaxLsRows=2;
			
			if(nMaxLsRows==1||jgzj.g2-jgzj.g1==0)	//��˨���Ų���
				lsn_coef=1;
			else// if(nMaxLsRows==2)				//��˨˫�Ų���ʱ
				lsn_coef=2-pow(LsSpace.doubleRowSpace/2.0,2)/(4*(jgzj.g2-jgzj.g1)*ls_d);
		}
		pElem->pOrgPole->hole_out=pElem->design_info.hole_out=lsn_coef;
	}
	else
		pElem->design_info.hole_out=0;
}
#include "Codes.h"
int MemberCheck2(CElement *pElem,IMemberDesignCode* pDesignCode,BOOL bCheckAll,bool bEqCheck=false,BOOL bUpdateBoltInfo=false)
{
	double new_wing_wide,new_wing_thick;
	STEELMAT *pMatType=QuerySteelMatByBriefMark(pElem->pOrgPole->cMaterial);
	if(pMatType==NULL)
		return 2;
	char sError[100]="";
	//���¼������Ԫ���������
	if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE||
		pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		JG_PARA_TYPE *pJgParaType=(JG_PARA_TYPE*)pElem->pParaType;
		if(pJgParaType)
		{
			new_wing_wide = pJgParaType->wing_wide;
			new_wing_thick= pJgParaType->wing_thick;
		}
		else
		{
			new_wing_wide = pElem->pOrgPole->GetWidth();
			new_wing_thick= pElem->pOrgPole->GetThick();
			if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE)
				pElem->pParaType=FindJgType(new_wing_wide,new_wing_thick,'L');
			else
				pElem->pParaType=FindJgType(new_wing_wide,new_wing_thick,pElem->pOrgPole->size_cSubClassType);
		}
		if(bUpdateBoltInfo)	//���½Ǹֶ�ͷ������˨��Ϣ
			CalElemRelaBoltInfo(pElem);
		CalElemHoleOutNum(pElem);	//���㵱ǰ������Ԫ�ļ�����
		//���밴��Ԫ��ǰѡ�й�����¼���ÿ����Ԫ��ѹ����Ϣ���Ա�����ʵ�����������ѹ��״̬
		CPreProcessor::CalElemCompStableInfo(pElem,pDesignCode);		//���㵱ǰ������Ԫ��ϸ��
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		TUBE_PARA_TYPE *pTubeParaType=(TUBE_PARA_TYPE*)pElem->pParaType;
		if(pTubeParaType)
		{
			new_wing_wide = pTubeParaType->D;
			new_wing_thick= pTubeParaType->thick;
		}
		else
		{
			new_wing_wide = pElem->pOrgPole->GetWidth();
			new_wing_thick= pElem->pOrgPole->GetThick();
			pElem->pParaType=FindTubeType(new_wing_wide,new_wing_thick);
		}
		CalElemHoleOutNum(pElem);	//���㵱ǰ������Ԫ�ļ�����
		//���밴��Ԫ��ǰѡ�й�����¼���ÿ����Ԫ��ѹ����Ϣ���Ա�����ʵ�����������ѹ��״̬
		CPreProcessor::CalElemCompStableInfo(pElem,pDesignCode);		//���㵱ǰ������Ԫ��ϸ��
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
	{
		BIAN_PARA_TYPE *pFlatParaType=(BIAN_PARA_TYPE*)pElem->pParaType;
		if(pFlatParaType)
		{
			new_wing_wide = pFlatParaType->wide;
			new_wing_thick= pFlatParaType->thick;
		}
		else
		{
			new_wing_wide = pElem->pOrgPole->GetWidth();
			new_wing_thick= pElem->pOrgPole->GetThick();
			pElem->pParaType=FindBianType(new_wing_wide,new_wing_thick);
		}
		CalElemHoleOutNum(pElem);	//���㵱ǰ������Ԫ�ļ�����
		//CPreProcessor::CalElemCompStableInfo(pElem,pDesignCode);		//���㵱ǰ������Ԫ��ϸ��
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_CABLE)
	{
		pElem->design_info.fCoefTensStrengthDecrease=1.0;
		pElem->design_info.iDangerousRule= 2;
		if(pElem->MaxTension.N>pElem->pOrgPole->GetWireForcePermit())	//ǿ�ȿ���
			pElem->m_iCtrlRule= 2;
		else
			pElem->m_iCtrlRule=0;
		pElem->design_info.fCoefOfBearLoad=pElem->design_info.fCoefEffic=pElem->MaxTension.N/pElem->pOrgPole->GetWireForcePermit();
		return pElem->m_iCtrlRule;
	}
	//����ȷ���˼�����ͺ�
	double L=pElem->Length();
	pElem->m_iCtrlRule=0;
	pElem->design_info.iDangerousRule = pElem->design_info.iDangerousLoadCase = 0;
	pElem->design_info.fCoefEffic=0;
	pElem->design_info.fCoefOfBearLoad = 0;
	double fy=pMatType->fy(new_wing_thick);
	double fv=pMatType->fv(new_wing_thick);
	double ft=pMatType->f(new_wing_thick);	//���ǿ��
	ft=pDesignCode->GetSteelDesignStrenth(fy,ft);
		
	double coefEqGamaRE=1.0;
	if(bEqCheck)
		coefEqGamaRE=Ta.EarthQuake.gamaRE;	//���ǿ�������ʱ�ṹ������������ϵ��Ӱ��
	if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE
		||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		double An;	//�������
		double wing_wide=new_wing_wide;
		double wing_thick=new_wing_thick;
		int ls_n,ls_d;
		
		char cType='L';
		if(pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
			cType=pElem->pOrgPole->size_cSubClassType;
		JG_PARA_TYPE *pJgType=(JG_PARA_TYPE*)pElem->pParaType;
		if(pJgType==NULL)
		{
			sprintf(sError,"�������Ҳ���0X%X�ĽǸֹ��%s%.0f*%.0f���������ʧ�ܣ�",pElem->pOrgPole->handle,pElem->pOrgPole->GetBriefPartTypeMark(),wing_wide,wing_thick);
			throw sError;
		}
		double maxValidTensionN=pElem->MaxTension.N;	//����˨ǿ�����Ƶ������Ч��������ֹ����˨�����´��󱨸������޴��� wjh-2017.7.7
		//��ʹ����ѡ�ĵĸ˼�Ҳ�����¼���һ����˨
		if(pElem->pStart==pElem->pOrgPole->pStart)
		{
			ls_d=pElem->pOrgPole->connectStart.d;	//�м�������
			ls_n=pElem->pOrgPole->connectStart.wnConnBoltN;	//��ѹ������
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectStart.grade);
			if(pLsType==NULL)
			{
				sprintf(sError,"��˨����δ�ҵ���ǰ����{%s}����˨����",pElem->pOrgPole->connectStart.grade);
				throw sError;
			}
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			pElem->pOrgPole->connectStart.bearloadcoef = maxN/(ls_n*min(Nbv,Nbc));
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				maxValidTensionN=ls_n*min(Nbv,Nbc);
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//��˨ǿ�Ȳ�����
			}
		}
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		{
			ls_d=pElem->pOrgPole->connectEnd.d;
			ls_n=pElem->pOrgPole->connectEnd.wnConnBoltN;
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectEnd.grade);
			if(pLsType==NULL)
			{
				sprintf(sError,"��˨����δ�ҵ���ǰ����{%s}����˨����",pElem->pOrgPole->connectStart.grade);
				throw sError;
			}
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			pElem->pOrgPole->connectEnd.bearloadcoef = maxN/(ls_n*min(Nbv,Nbc));
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				maxValidTensionN=ls_n*min(Nbv,Nbc);
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//��˨ǿ�Ȳ�����
			}
		}
		else
			ls_d=SelBoltDByWingWide(wing_wide,NULL);

		An=pJgType->area*100-pElem->design_info.hole_out*(ls_d+1.5)*wing_thick;	//��1.5�����ڿ�����˨�׾�Ҫ����˨ֱ����1.5mm
		pElem->design_info.hole_d=ls_d+1.5;
		//������������������ѹʱǿ���ۼ�ϵ��С,�ʻ���Ҫ����ͬ��������ͬ�ȴ�С���������� wjh-2011.9.22
		if(pElem->pOrgPole->IsAuxPole())
		{
			pElem->MaxTension=pElem->MaxCompression;
			pElem->MaxTension.N=fabs(pElem->MaxTension.N);	//ͬ����������ͬ�ȴ�С�������
		}
		if(pElem->MaxTension.N>0||pElem->MaxCompression.N>CElement::COMPRESSION_ZERO_FORCE)	//����������ѹ����
		{	//�����ˣ�Ԥ���������˿ɲ��ܳ�ϸ�ȵ����ƣ�
			double m;	//����ǿ���ۼ�ϵ��
			if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
				m=1.0;
			else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
			{
				if(pDesignCode->CodeSerial()==CODES::ASCE10_97)
				{
					if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE)
						m=0.9;
					else
						m=1;
				}
				else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
					m=0.85;	//�ֹ��๹��
				else// if(pElem->pOrgPole->guige.pole_type==TRUSS_POLE_JG)
				{	//�Ǹ��๹��
					if(wing_wide>40)
						m=0.70;
					else
						m=0.55;
				}
			}
			//ѡ������2��������������ǿ��
			pElem->design_info.fCoefTensStrengthDecrease=m;
			double bearcoef = pElem->MaxTension.N*coefEqGamaRE/(An*m*ft);
			pElem->design_info.bBlockShearActivated=false;
			if(pDesignCode->IsSupportBlockShearCheck()&&bearcoef<Ta.m_fLimitBearCoef)
			{	//���㳣��˼�ǿ������ʱ��������п������
				CConnectInfo connect;
				connect.wnConnBoltN=0;
				if(pElem->pStart==pElem->pOrgPole->pStart)
					connect=pElem->pOrgPole->connectStart;
				else if(pElem->pEnd==pElem->pOrgPole->pEnd)
					connect=pElem->pOrgPole->connectEnd;
				if(connect.wnConnBoltN>0&&connect.rows==1)
				{
					JGZJ jgzj;
					getjgzj(jgzj,wing_wide);//��ýǸ�׼���
					double c=wing_wide-jgzj.g-(connect.d*0.5+1);	//connect.d*0.5+1=�װ뾶+�׾���϶
					LSSPACE_STRU LsSpace;
					GetLsSpace(LsSpace,connect.d);
					double midSpace=LsSpace.SingleRowSpace*(connect.wnConnBoltN-1);
					//����˨���ǿ������,�������ʱ��Ҫ���뿼����˨��������������Ч���� wjh-2017.7.7
					double coef=pDesignCode->BlockShearCheck(maxValidTensionN,LsSpace.EndSpace,midSpace,c,wing_thick,ft,fv);
					if(coef>Ta.m_fLimitBearCoef&&coef>bearcoef)
					{
						pElem->design_info.bBlockShearActivated=true;
						bearcoef=coef;
					}
				}
			}
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 2;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 2;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//������˼�ǿ������
			{
				pElem->m_iCtrlRule=2;
				if(!bCheckAll)
					return 2;	//����ǿ�ȿ���
			}
		}
		if(pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//��ѹ��
		{
			double m;	//����ǿ���ۼ�ϵ��
			if(pDesignCode->CodeSerial()==CODES::ASCE10_97)
				m=1.0;
			else if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
				m=1.0;
			else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
				m=0.85;
			//ѡ������2��������������ǿ��
			pElem->design_info.fCoefCompStrengthDecrease=m;
			double bearcoef = fabs(pElem->MaxCompression.N)*coefEqGamaRE/(An*m*ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 3;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 3;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//������˼�ǿ������
			{
				pElem->m_iCtrlRule=3;
				if(!bCheckAll)
					return 3;	//��ѹǿ�ȿ���
			}
			
			double t=wing_thick;
			double b=pJgType->wing_wide-pJgType->r-t;
			//ѹ���ȶ�ǿ���ۼ�ϵ��
			double mN = pDesignCode->CompReduceCoef(b,t,ft,fy,pElem->CompStableLamda(),false,false);
			double fai=pDesignCode->CompStableCoef('B',pElem->CompStableLamda(),pMatType,new_wing_thick);
			//ѡ������3��������ѹ�ȶ�����
			bearcoef = fabs(pElem->MaxCompStable.N)*coefEqGamaRE/(fai*pJgType->area*100*mN*ft);
			if(bearcoef<pElem->design_info.fCoefOfBearLoad)
			{	//��ѹ�ȿ���ʱӦ���ݵ�ǰ��Σ�յĳ���ָ�����ʱ��ͬʱ��ѹ״̬����mN��ѹ��ϵ��fai wjh-2017.2.4
				bool bothComp=(pElem->design_info.iDangerousLoadCase==3)?pElem->MaxCompression.bothComp:false;
				if(bothComp!=pElem->MaxCompStable.bothComp)	//��ǰ��ѹ����
					fai=pDesignCode->CompStableCoef('B',pElem->CompStableLamda(&bothComp),pMatType,new_wing_thick);
			}
			pElem->design_info.mN =mN;
			pElem->design_info.w_t=b/t;
			pElem->design_info.fCoefStablity=fai;
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 4;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 4;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)
			{
				pElem->m_iCtrlRule=4;
				if(!bCheckAll)
					return 4;	//��ѹ�ȶ�����
			}
		}
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		double D=new_wing_wide;
		double thick=new_wing_thick;
		TUBE_PARA_TYPE *pTubeType=(TUBE_PARA_TYPE*)pElem->pParaType;
		if(pTubeType==NULL)
		{
			sprintf(sError,"�������Ҳ���0X%X�ĸֹܹ�񣬹������ʧ�ܣ�",pElem->pOrgPole->handle);
			throw sError;
		}

		if(pElem->MaxTension.N>0)
		{	//�����ˣ�Ԥ���������˿ɲ��ܳ�ϸ�ȵ����ƣ�
			double m;	//����ǿ���ۼ�ϵ��
			if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
				m=1.0;
			else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
				m=0.85;	//�ֹ��๹��
			//ѡ������2��������������ǿ��
			pElem->design_info.fCoefTensStrengthDecrease=m;
			double bearcoef = pElem->MaxTension.N*coefEqGamaRE/((pTubeType->area*100)*m*ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 2;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 2;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//������˼�ǿ������
			{
				pElem->m_iCtrlRule=2;
				if(!bCheckAll)
					return 2;	//����ǿ�ȿ���
			}
		}
		if(pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//��ѹ��
		{
			double m;	//����ǿ���ۼ�ϵ��
			if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
				m=1.0;
			else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
				m=0.85;
			//ѡ������2��������������ǿ��
			pElem->design_info.fCoefCompStrengthDecrease=m;
			double bearcoef = fabs(pElem->MaxCompression.N)*coefEqGamaRE/((pTubeType->area*100)*m*ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 3;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 3;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//������˼�ǿ������
			{
				pElem->m_iCtrlRule=3;
				if(!bCheckAll)
					return 3;	//��ѹǿ�ȿ���
			}
			
			//ѡ������3��������ѹ�ȶ�����
			double fai,D0,t;	//�⾶��ں�
			D0=pElem->GetWidth();
			t =pElem->GetThick();
			double mN = pDesignCode->CompReduceCoef(D0,t,ft,fy,pElem->CompStableLamda(),true,false);//ѹ���ȶ�ǿ���ۼ�ϵ��
			if(pTubeType->D<g_sysPara.fTubeWeldRoadMinD)	//�޺������޷�ֹ�
				fai=pDesignCode->CompStableCoef('A',pElem->CompStableLamda(),pMatType,new_wing_thick);
			else
				fai=pDesignCode->CompStableCoef('B',pElem->CompStableLamda(),pMatType,new_wing_thick);
			bearcoef = fabs(pElem->MaxCompStable.N)*coefEqGamaRE/(fai*pTubeType->area*100*mN*ft);	//
			if(bearcoef<pElem->design_info.fCoefOfBearLoad)
			{	//��ѹ�ȿ���ʱӦ���ݵ�ǰ��Σ�յĳ���ָ�����ʱ��ͬʱ��ѹ״̬����mN��ѹ��ϵ��fai wjh-2017.2.4
				bool bothComp=(pElem->design_info.iDangerousLoadCase==3)?pElem->MaxCompression.bothComp:false;
				if(bothComp!=pElem->MaxCompStable.bothComp)	//��ǰ��ѹ����
				{
					if(pTubeType->D<g_sysPara.fTubeWeldRoadMinD)	//�޺������޷�ֹ�
						fai=pDesignCode->CompStableCoef('A',pElem->CompStableLamda(&bothComp),pMatType,new_wing_thick);
					else
						fai=pDesignCode->CompStableCoef('B',pElem->CompStableLamda(&bothComp),pMatType,new_wing_thick);
				}
			}
			pElem->design_info.mN =mN;
			pElem->design_info.w_t=D0/t;
			pElem->design_info.fCoefStablity=fai;
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 4;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 4;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)
			{
				pElem->m_iCtrlRule=4;
				if(!bCheckAll)
					return 4;	//��ѹ�ȶ�����
			}
		}
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
	{
		double An;	//�������
		double wing_wide=new_wing_wide;
		double wing_thick=new_wing_thick;
		int ls_n,ls_d;
		
		BIAN_PARA_TYPE *pBianType=(BIAN_PARA_TYPE*)pElem->pParaType;
		if(pBianType==NULL)
		{
			sprintf(sError,"�������Ҳ���0X%X�ı�����񣬹������ʧ�ܣ�",pElem->pOrgPole->handle);
			throw sError;
		}
		if(pElem->pStart==pElem->pOrgPole->pStart)
		{
			ls_d=pElem->pOrgPole->connectStart.d;
			ls_n=pElem->pOrgPole->connectStart.wnConnBoltN;
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectStart.grade);
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//��˨ǿ�Ȳ�����
			}
		}
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		{
			ls_d=pElem->pOrgPole->connectEnd.d;
			ls_n=pElem->pOrgPole->connectEnd.wnConnBoltN;
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectEnd.grade);
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//��˨ǿ�Ȳ�����
			}
		}

		An=pBianType->wide*pBianType->thick-pElem->design_info.hole_out*(ls_d+1.5)*wing_thick;	//��1.5�����ڿ�����˨�׾�Ҫ����˨ֱ����1.5mm
		
		if(pElem->MaxTension.N>0)
		{	//�����ˣ�Ԥ���������˿ɲ��ܳ�ϸ�ȵ����ƣ�
			double m=1.0;	//����ǿ���ۼ�ϵ��
			//ѡ������2��������������ǿ��
			pElem->design_info.fCoefTensStrengthDecrease=m;
			double bearcoef=pElem->MaxTension.N*coefEqGamaRE/(An*m*ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 2;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 2;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//������˼�ǿ������
			{
				pElem->m_iCtrlRule=2;
				if(!bCheckAll)
					return 2;	//����ǿ�ȿ���
			}
		}
	}
	else// if(pElem->pOrgPole->guige.pole_type==TRUSS_POLE_USERDEF)
	{
		double area = pElem->pOrgPole->userdef.area;
		//ѡ������1���˼���ϸ��
		if(pElem->MaxTension.N>0)
		{	//�����ˣ�Ԥ���������˿ɲ��ܳ�ϸ�ȵ����ƣ�
			double m=1.0;	//����ǿ���ۼ�ϵ��
			//ѡ������2��������������ǿ��
			pElem->design_info.fCoefTensStrengthDecrease=m;
			double bearcoef=pElem->MaxTension.N*coefEqGamaRE/(area*m*pElem->pOrgPole->userdef.ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 2;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic= bearcoef;
				pElem->design_info.iDangerousRule = 2;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//������˼�ǿ������
			{
				pElem->m_iCtrlRule=2;
				if(!bCheckAll)
					return 2;	//����ǿ�ȿ���
			}
		}
		if(pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//��ѹ��
		{
			double m=1.0;	//����ǿ���ۼ�ϵ��
			//ѡ������2��������������ǿ��
			pElem->design_info.fCoefCompStrengthDecrease=m;
			double bearcoef=fabs(pElem->MaxCompression.N)*coefEqGamaRE/(area*m*pElem->pOrgPole->userdef.fc);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 3;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic= bearcoef;
				pElem->design_info.iDangerousRule = 3;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//������˼�ǿ������
			{
				pElem->m_iCtrlRule=3;
				if(!bCheckAll)
					return 3;	//��ѹǿ�ȿ���
			}
		}
	}
	//���µ�Ԫ��ϸ�ȼ�����̼���������Ϣ
	pElem->UpdateLamdaInfo();
	//�˶Գ�ϸ���Ƿ񳬳��������ϸ������
	double bearcoef =pElem->design_info.Lo_r/pElem->design_info.UltimateLo_r;
	if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
	{
		pElem->design_info.fCoefEffic = bearcoef;
		pElem->design_info.iDangerousRule = 1;
	}
	if(bearcoef>1.0)
	{
		pElem->m_iCtrlRule=1;
		if(!bCheckAll)
			return 1;	//��ϸ�ȿ���
	}

	return pElem->m_iCtrlRule;
}
//from database.cpp
//�й��淶
double CalStableCoef1(double lamda,char sectionType,char cMaterial);
//��������
double CalStableCoef2(double lamda,double E,double fy);
//��Ԫ��顣iDesignCode=0:���ڵ����淶 =1:���ڸ��ʹ淶 =2:����ASCE�淶
int MemberCheck(CElement *pElem,BOOL bCheckAll,int iDesignCode,bool bEqCheck=false,bool bUpdateBoltInfo=false)
{
	return MemberCheck2(pElem,CPreProcessor::ActiveDesignCode,bCheckAll,bEqCheck,bUpdateBoltInfo);
}
/*{
	double new_wing_wide,new_wing_thick;
	STEELMAT *pMatType=QuerySteelMatByBriefMark(pElem->pOrgPole->cMaterial);
	if(pMatType==NULL)
		return 2;
	char sError[100]="";
	//���¼������Ԫ���������
	if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE||
		pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		JG_PARA_TYPE *pJgParaType=(JG_PARA_TYPE*)pElem->pParaType;
		if(pJgParaType)
		{
			new_wing_wide = pJgParaType->wing_wide;
			new_wing_thick= pJgParaType->wing_thick;
		}
		else
		{
			new_wing_wide = pElem->pOrgPole->GetWidth();
			new_wing_thick= pElem->pOrgPole->GetThick();
			if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE)
				pElem->pParaType=FindJgType(new_wing_wide,new_wing_thick,'L');
			else
				pElem->pParaType=FindJgType(new_wing_wide,new_wing_thick,pElem->pOrgPole->size_cSubClassType);
		}
		CalElemHoleOutNum(pElem);	//���㵱ǰ������Ԫ�ļ�����
		//���밴��Ԫ��ǰѡ�й�����¼���ÿ����Ԫ��ѹ����Ϣ���Ա�����ʵ�����������ѹ��״̬
		CPreProcessor::CalElemCompStableInfo(pElem,iDesignCode);		//���㵱ǰ������Ԫ��ϸ��
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		TUBE_PARA_TYPE *pTubeParaType=(TUBE_PARA_TYPE*)pElem->pParaType;
		if(pTubeParaType)
		{
			new_wing_wide = pTubeParaType->D;
			new_wing_thick= pTubeParaType->thick;
		}
		else
		{
			new_wing_wide = pElem->pOrgPole->GetWidth();
			new_wing_thick= pElem->pOrgPole->GetThick();
			pElem->pParaType=FindTubeType(new_wing_wide,new_wing_thick);
		}
		CalElemHoleOutNum(pElem);	//���㵱ǰ������Ԫ�ļ�����
		//���밴��Ԫ��ǰѡ�й�����¼���ÿ����Ԫ��ѹ����Ϣ���Ա�����ʵ�����������ѹ��״̬
		CPreProcessor::CalElemCompStableInfo(pElem,iDesignCode);		//���㵱ǰ������Ԫ��ϸ��
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
	{
		BIAN_PARA_TYPE *pFlatParaType=(BIAN_PARA_TYPE*)pElem->pParaType;
		if(pFlatParaType)
		{
			new_wing_wide = pFlatParaType->wide;
			new_wing_thick= pFlatParaType->thick;
		}
		else
		{
			new_wing_wide = pElem->pOrgPole->GetWidth();
			new_wing_thick= pElem->pOrgPole->GetThick();
			pElem->pParaType=FindBianType(new_wing_wide,new_wing_thick);
		}
		CalElemHoleOutNum(pElem);	//���㵱ǰ������Ԫ�ļ�����
		//CPreProcessor::CalElemCompStableInfo(pElem,iDesignCode);		//���㵱ǰ������Ԫ��ϸ��
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_CABLE)
	{
		pElem->design_info.fCoefTensStrengthDecrease=1.0;
		pElem->design_info.iDangerousRule= 2;
		if(pElem->MaxTension.N>pElem->pOrgPole->GetWireForcePermit())	//ǿ�ȿ���
			pElem->m_iCtrlRule= 2;
		else
			pElem->m_iCtrlRule=0;
		pElem->design_info.fCoefOfBearLoad=pElem->design_info.fCoefEffic=pElem->MaxTension.N/pElem->pOrgPole->GetWireForcePermit();
		return pElem->m_iCtrlRule;
	}
	//����ȷ���˼�����ͺ�
	double L=pElem->Length();
	pElem->m_iCtrlRule=0;
	pElem->design_info.iDangerousRule = pElem->design_info.iDangerousLoadCase = 0;
	pElem->design_info.fCoefEffic=0;
	pElem->design_info.fCoefOfBearLoad = 0;
	double ft=pMatType->f(new_wing_thick);
	if(iDesignCode==2)	//ASCE �淶
		ft=0.9*pMatType->fy(new_wing_thick);

	double lamdaMax=pElem->LamdaMax();
	double UltimateLo_r;
	int tensLamdaLim,mainLamdaLim,xieLamdaLim,auxLamdaLim;
	if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		tensLamdaLim=Ta.SelMatPara.m_nTensiveTubeSlendernessLimit;
		mainLamdaLim=Ta.SelMatPara.m_nMainTubeSlendernessLimit;
		xieLamdaLim =Ta.SelMatPara.m_nXieTubeSlendernessLimit;
		auxLamdaLim =Ta.SelMatPara.m_nAuxTubeSlendernessLimit;
	}
	else
	{
		tensLamdaLim=Ta.SelMatPara.m_nTensivePoleSlendernessLimit;
		mainLamdaLim=Ta.SelMatPara.m_nMainPoleSlendernessLimit;
		xieLamdaLim =Ta.SelMatPara.m_nXiePoleSlendernessLimit;
		auxLamdaLim =Ta.SelMatPara.m_nAuxPoleSlendernessLimit;
	}
	double coefEqGamaRE=1.0;
	if(bEqCheck)
		coefEqGamaRE=Ta.EarthQuake.gamaRE;	//���ǿ�������ʱ�ṹ������������ϵ��Ӱ��
	if(pElem->pOrgPole->size_idClassType==CLS_LINEANGLE
		||pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
	{
		double An;	//�������
		double wing_wide=new_wing_wide;
		double wing_thick=new_wing_thick;
		int ls_n,ls_d;
		
		char cType='L';
		if(pElem->pOrgPole->size_idClassType==CLS_GROUPLINEANGLE)
			cType=pElem->pOrgPole->size_cSubClassType;
		JG_PARA_TYPE *pJgType=(JG_PARA_TYPE*)pElem->pParaType;
		if(pJgType==NULL)
		{
			sprintf(sError,"�������Ҳ���0X%X�ĽǸֹ�񣬹������ʧ�ܣ�",pElem->pOrgPole->handle);
			throw sError;
		}
		//��ʹ����ѡ�ĵĸ˼�Ҳ�����¼���һ����˨
		if(pElem->pStart==pElem->pOrgPole->pStart)
		{
			ls_d=pElem->pOrgPole->connectStart.d;	//�м�������
			ls_n=pElem->pOrgPole->connectStart.N;	//��ѹ������
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectStart.grade);
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			pElem->pOrgPole->connectStart.bearloadcoef = maxN/(ls_n*min(Nbv,Nbc));
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//��˨ǿ�Ȳ�����
			}
		}
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		{
			ls_d=pElem->pOrgPole->connectEnd.d;
			ls_n=pElem->pOrgPole->connectEnd.N;
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectEnd.grade);
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			pElem->pOrgPole->connectEnd.bearloadcoef = maxN/(ls_n*min(Nbv,Nbc));
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//��˨ǿ�Ȳ�����
			}
		}
		else
			ls_d=SelBoltDByWingWide(wing_wide,NULL);

		An=pJgType->area*100-pElem->design_info.hole_out*(ls_d+1.5)*wing_thick;	//��1.5�����ڿ�����˨�׾�Ҫ����˨ֱ����1.5mm
		pElem->design_info.hole_d=ls_d+1.5;
		//������������������ѹʱǿ���ۼ�ϵ��С,�ʻ���Ҫ����ͬ��������ͬ�ȴ�С���������� wjh-2011.9.22
		if(pElem->pOrgPole->IsAuxPole())
		{
			pElem->MaxTension=pElem->MaxCompression;
			pElem->MaxTension.N=fabs(pElem->MaxTension.N);	//ͬ����������ͬ�ȴ�С�������
		}
		if(pElem->MaxTension.N>0||pElem->MaxCompression.N>CElement::COMPRESSION_ZERO_FORCE)	//����������ѹ����
		{	//�����ˣ�Ԥ���������˿ɲ��ܳ�ϸ�ȵ����ƣ�
			UltimateLo_r=tensLamdaLim;
			double bearcoef=lamdaMax/UltimateLo_r;
			pElem->design_info.fCoefEffic = bearcoef;
			pElem->design_info.iDangerousRule = 1;
			if(bearcoef>1.0)	//��ϸ�Ȳ�����ѡ������
			{
				pElem->m_iCtrlRule=1;
				if(!bCheckAll)
					return 1;	//��ϸ�ȿ���
			}
			else 
			{
				double m;	//����ǿ���ۼ�ϵ��
				if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
					m=1.0;
				else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
				{
					if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
						m=0.85;	//�ֹ��๹��
					else// if(pElem->pOrgPole->guige.pole_type==TRUSS_POLE_JG)
					{	//�Ǹ��๹��
						if(wing_wide>40)
							m=0.70;
						else
							m=0.55;
					}
				}
				//ѡ������2��������������ǿ��
				pElem->design_info.fCoefTensStrengthDecrease=m;
				double bearcoef = pElem->MaxTension.N*coefEqGamaRE/(An*m*ft);
				if(bearcoef>pElem->design_info.fCoefOfBearLoad)
				{
					pElem->design_info.fCoefOfBearLoad = bearcoef;
					pElem->design_info.iDangerousLoadCase = 2;
				}
				if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
				{
					pElem->design_info.fCoefEffic = bearcoef;
					pElem->design_info.iDangerousRule = 2;
				}
				if(bearcoef>Ta.m_fLimitBearCoef)	//������˼�ǿ������
				{
					pElem->m_iCtrlRule=2;
					if(!bCheckAll)
						return 2;	//����ǿ�ȿ���
				}
			}
		}
		if(pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//��ѹ��
		{
			double m;	//����ǿ���ۼ�ϵ��
			if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
				m=1.0;
			else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
				m=0.85;
			//ѡ������2��������������ǿ��
			pElem->design_info.fCoefCompStrengthDecrease=m;
			double bearcoef = fabs(pElem->MaxCompression.N)*coefEqGamaRE/(An*m*ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 3;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 3;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//������˼�ǿ������
			{
				pElem->m_iCtrlRule=3;
				if(!bCheckAll)
					return 3;	//��ѹǿ�ȿ���
			}
			
			//ѡ������1��������ϸ��
			if(pElem->pOrgPole->CalLenCoef.hRefPole>0x20&&
				(pElem->pOrgPole->layer(1)=='X'||pElem->pOrgPole->layer(1)=='H'))
			{
				double bearCoef1=pElem->lamdaRc.Lo_r/auxLamdaLim;
				double bearCoef2=pElem->lamdaRyo.Lo_r/xieLamdaLim;
				double bearCoef3=pElem->lamdaRx.Lo_r/xieLamdaLim;
				if(!pElem->smcinfo.hasBothComp)
					bearCoef1=0;	//��ͬʱ��ѹ�������
				if(bearCoef1>bearCoef2&&bearCoef1>bearCoef3)
				{
					bearcoef=bearCoef1;
					UltimateLo_r=auxLamdaLim;	//ͬʱ��ѹʱ��ϸ�ȸ�Σ��
				}
				else if(bearCoef2>=bearCoef3)
				{
					bearcoef=bearCoef2;
					UltimateLo_r=xieLamdaLim;	//ͬʱ��ѹʱ��ϸ�ȸ�Σ��
				}
				else
				{
					bearcoef=bearCoef3;
					UltimateLo_r=xieLamdaLim;	//ͬʱ��ѹʱ��ϸ�ȸ�Σ��
				}
			}
			else
			{
				if(pElem->pOrgPole->layer(1)=='Z')
					UltimateLo_r=mainLamdaLim;	//����lamda��150
				else if(pElem->pOrgPole->layer(1)=='X'||pElem->pOrgPole->layer(1)=='H')
					UltimateLo_r=xieLamdaLim;	//б�Ļ��ģ����������ģ�lamda��220
				else
					UltimateLo_r=auxLamdaLim;
				bearcoef = lamdaMax/UltimateLo_r;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 1;
			}
			if(bearcoef>1.0)// 150)
			{
				pElem->m_iCtrlRule=1;
				if(!bCheckAll)
					return 1;	//��ϸ�ȿ���
			}
			double mN;	//ѹ���ȶ�ǿ���ۼ�ϵ��
			if(iDesignCode<2)
			{	//���ڹ淶
				double fai=CalStableCoef1(pElem->CompStableLamda(),'B',pElem->pOrgPole->cMaterial);
				pElem->design_info.fCoefStablity=fai;
				//�Ǹ���
				double t=wing_thick;
				double b=pJgType->wing_wide-pJgType->r-t;
				double bt_lim=202/sqrt(ft);
				if(b/t<=bt_lim)
					mN=1.0;
				else
					mN=1.677-0.677*(b/t)/bt_lim;
				pElem->design_info.mN =mN;
				pElem->design_info.w_t=b/t;
				
				//ѡ������3��������ѹ�ȶ�����
				bearcoef = fabs(pElem->MaxCompStable.N)*coefEqGamaRE/(fai*pJgType->area*100*mN*ft);
				if(bearcoef>pElem->design_info.fCoefOfBearLoad)
				{
					pElem->design_info.fCoefOfBearLoad = bearcoef;
					pElem->design_info.iDangerousLoadCase = 4;
				}
				if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
				{
					pElem->design_info.fCoefEffic = bearcoef;
					pElem->design_info.iDangerousRule = 4;
				}
				if(bearcoef>Ta.m_fLimitBearCoef)
				{
					pElem->m_iCtrlRule=4;
					if(!bCheckAll)
						return 4;	//��ѹ�ȶ�����
				}
			}
			else
			{	//ASCE�淶�Ǹ���
				double t=wing_thick;
				double b=pJgType->wing_wide-pJgType->r-t;
				double bt_lim=209.6/sqrt(pMatType->fy(t));
				if(b/t<=bt_lim)
					mN=1.0;
				else
					mN=1.677-0.677*(b/t)/bt_lim;
				pElem->design_info.mN =mN;
				pElem->design_info.w_t=b/t;
				
				//ѡ������3��������ѹ�ȶ�����
				double fy=pMatType->fy(t);
				double fai=CalStableCoef2(pElem->CompStableLamda(),pMatType->E,fy);
				pElem->design_info.fCoefStablity=fai;//fa/pMatType->fy(t);
				bearcoef = fabs(pElem->MaxCompStable.N)*coefEqGamaRE/(fai*pJgType->area*100*mN*fy);
				if(bearcoef>pElem->design_info.fCoefOfBearLoad)
				{
					pElem->design_info.fCoefOfBearLoad = bearcoef;
					pElem->design_info.iDangerousLoadCase = 4;
				}
				if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
				{
					pElem->design_info.fCoefEffic = bearcoef;
					pElem->design_info.iDangerousRule = 4;
				}
				if(bearcoef>Ta.m_fLimitBearCoef)
				{
					pElem->m_iCtrlRule=4;
					if(!bCheckAll)
						return 4;	//��ѹ�ȶ�����
				}
			}
		}
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINETUBE)
	{
		double D=new_wing_wide;
		double thick=new_wing_thick;
		TUBE_PARA_TYPE *pTubeType=(TUBE_PARA_TYPE*)pElem->pParaType;
		if(pTubeType==NULL)
		{
			sprintf(sError,"�������Ҳ���0X%X�ĸֹܹ�񣬹������ʧ�ܣ�",pElem->pOrgPole->handle);
			throw sError;
		}

		if(pElem->MaxTension.N>0)
		{	//�����ˣ�Ԥ���������˿ɲ��ܳ�ϸ�ȵ����ƣ�
			UltimateLo_r=tensLamdaLim;
			double bearcoef = lamdaMax/UltimateLo_r;
			pElem->design_info.fCoefEffic = bearcoef;
			pElem->design_info.iDangerousRule = 1;

			if(bearcoef>1.0)// 400)	//��ϸ�Ȳ�����ѡ������
			{
				pElem->m_iCtrlRule=1;
				if(!bCheckAll)
					return 1;	//��ϸ�ȿ���
			}
			else 
			{
				double m;	//����ǿ���ۼ�ϵ��
				if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
					m=1.0;
				else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
					m=0.85;	//�ֹ��๹��
				//ѡ������2��������������ǿ��
				pElem->design_info.fCoefTensStrengthDecrease=m;
				double bearcoef = pElem->MaxTension.N*coefEqGamaRE/((pTubeType->area*100)*m*ft);
				if(bearcoef>pElem->design_info.fCoefOfBearLoad)
				{
					pElem->design_info.fCoefOfBearLoad = bearcoef;
					pElem->design_info.iDangerousLoadCase = 2;
				}
				if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
				{
					pElem->design_info.fCoefEffic = bearcoef;
					pElem->design_info.iDangerousRule = 2;
				}
				if(bearcoef>Ta.m_fLimitBearCoef)	//������˼�ǿ������
				{
					pElem->m_iCtrlRule=2;
					if(!bCheckAll)
						return 2;	//����ǿ�ȿ���
				}
			}
		}
		if(pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//��ѹ��
		{
			double m;	//����ǿ���ۼ�ϵ��
			if(pElem->start_force_type==CENTRIC_FORCE&&pElem->end_force_type==CENTRIC_FORCE)
				m=1.0;
			else if(pElem->start_force_type==ECCENTRIC_FORCE||pElem->end_force_type==ECCENTRIC_FORCE)
				m=0.85;
			//ѡ������2��������������ǿ��
			pElem->design_info.fCoefCompStrengthDecrease=m;
			double bearcoef = fabs(pElem->MaxCompression.N)*coefEqGamaRE/((pTubeType->area*100)*m*ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 3;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 3;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//������˼�ǿ������
			{
				pElem->m_iCtrlRule=3;
				if(!bCheckAll)
					return 3;	//��ѹǿ�ȿ���
			}
			
			//ѡ������1��������ϸ��
			if(pElem->pOrgPole->CalLenCoef.hRefPole>0x20&&
				(pElem->pOrgPole->layer(1)=='X'||pElem->pOrgPole->layer(1)=='H'))
			{
				double bearCoef1=pElem->lamdaRc.Lo_r/auxLamdaLim;
				double bearCoef2=pElem->lamdaRyo.Lo_r/xieLamdaLim;
				double bearCoef3=pElem->lamdaRx.Lo_r/xieLamdaLim;
				if(!pElem->smcinfo.hasBothComp)
					bearCoef1=0;	//��ͬʱ��ѹ�������
				if(bearCoef1>bearCoef2&&bearCoef1>bearCoef3)
				{
					bearcoef=bearCoef1;
					UltimateLo_r=auxLamdaLim;	//ͬʱ��ѹʱ��ϸ�ȸ�Σ��
				}
				else if(bearCoef2>=bearCoef3)
				{
					bearcoef=bearCoef2;
					UltimateLo_r=xieLamdaLim;	//ͬʱ��ѹʱ��ϸ�ȸ�Σ��
				}
				else
				{
					bearcoef=bearCoef3;
					UltimateLo_r=xieLamdaLim;	//ͬʱ��ѹʱ��ϸ�ȸ�Σ��
				}
			}
			else
			{
				if(pElem->pOrgPole->layer(1)=='Z')
					UltimateLo_r=mainLamdaLim;	//����lamda��150
				else if(pElem->pOrgPole->layer(1)=='X'||pElem->pOrgPole->layer(1)=='H')
					UltimateLo_r=xieLamdaLim;	//б�Ļ��ģ����������ģ�lamda��220
				else
					UltimateLo_r=auxLamdaLim;
				bearcoef = lamdaMax/UltimateLo_r;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 1;
			}
			if(bearcoef>1.0)// 150)
			{
				pElem->m_iCtrlRule=1;
				if(!bCheckAll)
					return 1;	//��ϸ�ȿ���
			}
			double mN;	//ѹ���ȶ�ǿ���ۼ�ϵ��
			if(iDesignCode<2)
			{	//���ڹ淶
				double fai;
				if(pTubeType->D<g_sysPara.fTubeWeldRoadMinD)	//�޺������޷�ֹ�
					fai=CalStableCoef1(pElem->CompStableLamda(),'A',pElem->pOrgPole->cMaterial);
				else
					fai=CalStableCoef1(pElem->CompStableLamda(),'B',pElem->pOrgPole->cMaterial);
				pElem->design_info.fCoefStablity=fai;
				
				//�ֹ���
				double D0,t;	//�⾶��ں�
				D0=pElem->GetWidth();
				t =pElem->GetThick();
				if(D0/t<=24100/pMatType->f(t))
					mN=1.0;
				else
					mN=0.75+6025/(D0*pMatType->f(t)/t);
				pElem->design_info.mN =mN;
				pElem->design_info.w_t=D0/t;
				
				//ѡ������3��������ѹ�ȶ�����
				bearcoef = fabs(pElem->MaxCompStable.N)*coefEqGamaRE/(fai*pTubeType->area*100*mN*ft);	//
				if(bearcoef>pElem->design_info.fCoefOfBearLoad)
				{
					pElem->design_info.fCoefOfBearLoad = bearcoef;
					pElem->design_info.iDangerousLoadCase = 3;
				}
				if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
				{
					pElem->design_info.fCoefEffic = bearcoef;
					pElem->design_info.iDangerousRule = 4;
				}
				if(bearcoef>Ta.m_fLimitBearCoef)
				{
					pElem->m_iCtrlRule=4;
					if(!bCheckAll)
						return 4;	//��ѹ�ȶ�����
				}
			}
			else
			{	//ASCE�淶 �ֹ�
				double D0,t;	//�ֹ����⾶��ں�
				D0=pElem->GetWidth();
				t =pElem->GetThick();
				double fy=pMatType->fy(t);
				double fai=CalStableCoef2(pElem->CompStableLamda(),pMatType->E,fy);
				pElem->design_info.fCoefStablity=fai;//fa/pMatType->fy(t);
				
				if(D0/t<=24100/pMatType->f(t))
					mN=1.0;
				else
					mN=0.75+6025/(D0*pMatType->f(t)/t);
				pElem->design_info.mN =mN;
				pElem->design_info.w_t=D0/t;
				
				//ѡ������3��������ѹ�ȶ�����
				bearcoef = fabs(pElem->MaxCompStable.N)*coefEqGamaRE/(fai*pTubeType->area*100*mN*fy);
				if(bearcoef>pElem->design_info.fCoefOfBearLoad)
				{
					pElem->design_info.fCoefOfBearLoad = bearcoef;
					pElem->design_info.iDangerousLoadCase = 3;
				}
				if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
				{
					pElem->design_info.fCoefEffic = bearcoef;
					pElem->design_info.iDangerousRule = 4;
				}
				if(bearcoef>Ta.m_fLimitBearCoef)
				{
					pElem->m_iCtrlRule=4;
					if(!bCheckAll)
						return 4;	//��ѹ�ȶ�����
				}
			}
		}
	}
	else if(pElem->pOrgPole->size_idClassType==CLS_LINEFLAT)
	{
		double An;	//�������
		double wing_wide=new_wing_wide;
		double wing_thick=new_wing_thick;
		int ls_n,ls_d;
		
		BIAN_PARA_TYPE *pBianType=(BIAN_PARA_TYPE*)pElem->pParaType;
		if(pBianType==NULL)
		{
			sprintf(sError,"�������Ҳ���0X%X�ı�����񣬹������ʧ�ܣ�",pElem->pOrgPole->handle);
			throw sError;
		}
		if(pElem->pStart==pElem->pOrgPole->pStart)
		{
			ls_d=pElem->pOrgPole->connectStart.d;
			ls_n=pElem->pOrgPole->connectStart.N;
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectStart.grade);
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//��˨ǿ�Ȳ�����
			}
		}
		else if(pElem->pEnd==pElem->pOrgPole->pEnd)
		{
			ls_d=pElem->pOrgPole->connectEnd.d;
			ls_n=pElem->pOrgPole->connectEnd.N;
			LS_GRADE *pLsType=FindLsMatType(pElem->pOrgPole->connectEnd.grade);
			double Nbv=Ta.m_fLimitBearCoef*Pi*ls_d*ls_d*pLsType->fv/4;
			double Nbc=Ta.m_fLimitBearCoef*ls_d*wing_thick*pMatType->fc(wing_thick);
			double maxN = __max(-pElem->MaxCompression.N,pElem->MaxTension.N);
			if(ls_n<maxN/min(Nbv,Nbc))
			{
				pElem->m_iCtrlRule=6;
				if(!bCheckAll)
					return 6;	//��˨ǿ�Ȳ�����
			}
		}

		An=pBianType->wide*pBianType->thick-pElem->design_info.hole_out*(ls_d+1.5)*wing_thick;	//��1.5�����ڿ�����˨�׾�Ҫ����˨ֱ����1.5mm
		
		if(pElem->MaxTension.N>0)
		{	//�����ˣ�Ԥ���������˿ɲ��ܳ�ϸ�ȵ����ƣ�
			double m=1.0;	//����ǿ���ۼ�ϵ��
			//ѡ������2��������������ǿ��
			pElem->design_info.fCoefTensStrengthDecrease=m;
			double bearcoef=pElem->MaxTension.N*coefEqGamaRE/(An*m*ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 2;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic = bearcoef;
				pElem->design_info.iDangerousRule = 2;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//������˼�ǿ������
			{
				pElem->m_iCtrlRule=2;
				if(!bCheckAll)
					return 2;	//����ǿ�ȿ���
			}
		}
	}
	else// if(pElem->pOrgPole->guige.pole_type==TRUSS_POLE_USERDEF)
	{
		double area = pElem->pOrgPole->userdef.area;
		//ѡ������1���˼���ϸ��
		if(pElem->MaxTension.N>0)
		{	//�����ˣ�Ԥ���������˿ɲ��ܳ�ϸ�ȵ����ƣ�
			double m=1.0;	//����ǿ���ۼ�ϵ��
			//ѡ������2��������������ǿ��
			pElem->design_info.fCoefTensStrengthDecrease=m;
			double bearcoef=pElem->MaxTension.N*coefEqGamaRE/(area*m*pElem->pOrgPole->userdef.ft);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 2;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic= bearcoef;
				pElem->design_info.iDangerousRule = 2;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//������˼�ǿ������
			{
				pElem->m_iCtrlRule=2;
				if(!bCheckAll)
					return 2;	//����ǿ�ȿ���
			}
		}
		if(pElem->MaxCompression.N<=CElement::COMPRESSION_ZERO_FORCE)//��ѹ��
		{
			double m=1.0;	//����ǿ���ۼ�ϵ��
			//ѡ������2��������������ǿ��
			pElem->design_info.fCoefCompStrengthDecrease=m;
			double bearcoef=fabs(pElem->MaxCompression.N)*coefEqGamaRE/(area*m*pElem->pOrgPole->userdef.fc);
			if(bearcoef>pElem->design_info.fCoefOfBearLoad)
			{
				pElem->design_info.fCoefOfBearLoad = bearcoef;
				pElem->design_info.iDangerousLoadCase = 3;
			}
			if(bearcoef>pElem->design_info.fCoefEffic)	//��һ����������������ǿ�ȿ���
			{
				pElem->design_info.fCoefEffic= bearcoef;
				pElem->design_info.iDangerousRule = 3;
			}
			if(bearcoef>Ta.m_fLimitBearCoef)	//������˼�ǿ������
			{
				pElem->m_iCtrlRule=3;
				if(!bCheckAll)
					return 3;	//��ѹǿ�ȿ���
			}
		}
	}
	//���µ�Ԫ��ϸ�ȼ�����̼���������Ϣ
	pElem->UpdateLamdaInfo(iDesignCode!=1);
	return pElem->m_iCtrlRule;
}*/
#endif
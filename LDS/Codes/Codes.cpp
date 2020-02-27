// Codes.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "Codes.h"
#include "PartLib.h"
#include "Hashtable.h"
#include "XhCharString.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#define      Pi               3.14159265358979323846
////////////////////////////////////////////////////////////////////////////////////////////////////

const DWORD HASHTABLESIZE = 500;
double IMemberDesignCode::modulusE=206000;
static CStringKeyHashTable<long> propHashtable;
static CHashList<double> globalItemValues[4];
static CHashList<double>* GetHashPropItems(int iDesignCode)
{
	return &globalItemValues[iDesignCode];
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//这四个规范定义必须置于"CHashList<double> globalItemValues[4]"之后，否则会导致globalItemValues未初始化时被交叉调用
DL_T_2002 dlt_2002;
GB50135_2006 gb50135;
DL_T_2012 dlt_2012;
ASCE10_97 asce10_97;

ILog2File* IMemberDesignCode::ERR_LOGGER=NULL;
ILog2File* IMemberDesignCode::SetHostErrorLogger(ILog2File* pErrLogger)
{
	return IMemberDesignCode::ERR_LOGGER=pErrLogger;
}
IMemberDesignCode* IMemberDesignCode::CODE_DLT_2002()	{return &dlt_2002;}
IMemberDesignCode* IMemberDesignCode::GB50135_2006()	{return &gb50135;}
IMemberDesignCode* IMemberDesignCode::CODE_DLT_2012()	{return &dlt_2012;}
IMemberDesignCode* IMemberDesignCode::CODE_ASCE10_97()	{return &asce10_97;}
IMemberDesignCode* IMemberDesignCode::FromCodeSerial(int codeSerial)
{
	CODES code=codeSerial;
	if(code.serial==CODES::GB50135_2006)
		return GB50135_2006();
	else if(code.serial==CODES::ASCE10_97)
		return CODE_ASCE10_97();
	else if(code.serial==CODES::DLT_2002)
		return CODE_DLT_2002();
	else
		return CODE_DLT_2012();
}
bool IMemberDesignCode::GetSupportCodesStr(char* codesStr,int maxBufLength)
{
	char codes_str[500];
	strcpy(codes_str,CODE_DLT_2012()->CodeName());
	strcat(codes_str,"|");
	strcat(codes_str,GB50135_2006()->CodeName());
	strcat(codes_str,"|");
	strcat(codes_str,CODE_ASCE10_97()->CodeName());
	strcat(codes_str,"|");
	strcat(codes_str,CODE_DLT_2002()->CodeName());
	if((int)strlen(codes_str)>=maxBufLength)
		return false;
	else
		strcpy(codesStr,codes_str);
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
char* DL_T_2002::CodeName()
{
	return "杆塔规定 DL/T 5154-2002(已作废)";
}
char* DL_T_2012::CodeName()
{
	return "杆塔规定 DL/T 5154-2012";
}
char* GB50135_2006::CodeName()
{
	return "高耸规范 GB50135-2006";
}
char* ASCE10_97::CodeName()
{
	return "美国 ASCE 10-97";
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//拉压弯组合梁元强度验算
double CheckBeamBy_GB50017_2003(BEAM_SECTION* pSect,BYTE* cCtrlCond)
{	//钢结构规范　2003版
	return 0;
}
double CheckBeamBy_DLT_2002(BEAM_SECTION* pSect,BYTE* cCtrlCond)
{	//国内电力杆塔结构设计技术规定 2002版
	return 0;
}
double CheckBeamBy_DLT5254_2010(BEAM_SECTION* pSect,BYTE* cCtrlCond)
{	//国内钢管塔设计技术规定 2010版
	return 0;
}
double CheckBeamBy_DLT5130_2001(BEAM_SECTION* pSect,BYTE* cCtrlCond)
{	//钢管杆设计技术规定2001版
	//double fsb;		//受弯局部稳定控制强度设计值MPa,
	//double fsc;		//受压局部稳定控制强度设计值MPa,
	if(pSect->cSectionType==BEAM_SECTION::SECT_TUBE)	//钢管
	{
		double D0_t=pSect->para.tube.D/pSect->para.tube.thick;
		if(D0_t*pSect->f<=38060)
			pSect->fsb=pSect->f;
		else //if(D0_t*pSect->f<=(76130)
			pSect->fsb=0.7*pSect->f+11410/D0_t;
		if(D0_t*pSect->f<=24100)
			pSect->fsc=pSect->f;
		else //if(D0_t*pSect->f<=(76130)
			pSect->fsc=0.75*pSect->f+6025/D0_t;
	}
	else if(pSect->cSectionType==BEAM_SECTION::SECT_POLY)	//多边形钢管
	{
		double W,BR=pSect->para.poly.BR;
		if(BR==0||BR>pSect->para.poly.thick*4)
			BR=pSect->para.poly.thick*4;
		if(pSect->para.poly.slices<=6)
			W=0.577*(pSect->para.poly.D0-2*pSect->para.poly.thick-2*BR);
		else if(pSect->para.poly.slices<=8)
			W=0.414*(pSect->para.poly.D0-2*pSect->para.poly.thick-2*BR);
		else if(pSect->para.poly.slices<=12)
			W=0.268*(pSect->para.poly.D0-2*pSect->para.poly.thick-2*BR);
		else //if(pSect->para.poly.slices<=16)
			W=0.199*(pSect->para.poly.D0-2*pSect->para.poly.thick-2*BR);
		double W_t=W/pSect->para.poly.thick;
		double sqrt_f=sqrt(pSect->f);
		if( (pSect->para.poly.slices<= 8&&W_t*sqrt_f<=660)||
			(pSect->para.poly.slices<=12&&W_t*sqrt_f<=610)||
			(pSect->para.poly.slices<=16&&W_t*sqrt_f<=545))
			pSect->fsb=pSect->fsc=pSect->f;
		else if(pSect->para.poly.slices<=8)//&&W_t*sqrt_f<=925
			pSect->fsb=pSect->fsc=1.42*pSect->f*(1.0-0.000448*sqrt_f*W_t);
		else if(pSect->para.poly.slices<=12)//&&W_t*sqrt_f<=925
			pSect->fsb=pSect->fsc=1.45*pSect->f*(1.0-0.000507*sqrt_f*W_t);
		else //if(pSect->para.poly.slices<=16)//&&W_t*sqrt_f<=925
			pSect->fsb=pSect->fsc=1.42*pSect->f*(1.0-0.000539*sqrt_f*W_t);
	}
	else
		return CheckBeamBy_GB50017_2003(pSect,cCtrlCond);
	pSect->fv=0.58*pSect->f;	//受剪强度允许值MPa,
	if(pSect->An<=0)
		pSect->An=pSect->Ag;
	if(pSect->P[2]>0)
	{
		pSect->sigemaNt = pSect->P[2]/pSect->An;	//轴向正应力MPa(即N/mm2)
		pSect->sigemaNc = 0;
	}
	else
	{
		pSect->sigemaNt = 0;
		pSect->sigemaNc =-pSect->P[2]/pSect->Ag;	//轴向正应力MPa(即N/mm2)
	}
	double Mxy=sqrt(pSect->M[0]*pSect->M[0]+pSect->M[1]*pSect->M[1]);
	double Pxy=sqrt(pSect->P[0]*pSect->P[0]+pSect->P[1]*pSect->P[1]);
	pSect->sigemaBend=Mxy/pSect->W;	//弯曲强度MPa
	pSect->taoShear=Pxy*pSect->Q_It+fabs(pSect->M[2])*pSect->C_J;	//剪切应力M=集中力剪应力+扭转剪应力，MPa
	//复合应力
	pSect->sigemaComplex =(pSect->sigemaNc+pSect->sigemaBend)*(pSect->sigemaNc+pSect->sigemaBend);
	pSect->sigemaComplex+=3*pSect->taoShear*pSect->taoShear;
	pSect->sigemaComplex =sqrt(pSect->sigemaComplex);
	char cDangerCond=BEAM_SECTION::COND_TENSION;
	double effic=pSect->sigemaNt/pSect->f;
	double ef=pSect->sigemaBend/pSect->fsb+pSect->sigemaNc/pSect->fsc;
	if(ef>effic)
	{
		effic=ef;
		cDangerCond=BEAM_SECTION::COND_COMPBEND;
		if(pSect->sigemaNc<EPS)
			cDangerCond=BEAM_SECTION::COND_BENDING;
	}
	ef=pSect->taoShear/pSect->fv;
	if(ef>effic)
	{
		effic=ef;
		cDangerCond=BEAM_SECTION::COND_SHEAR;
	}
	ef=pSect->sigemaComplex/pSect->fsb;
	if(ef>effic)
	{
		effic=ef;
		cDangerCond=BEAM_SECTION::COND_COMPLEX;
	}
	if(cCtrlCond!=NULL)
		*cCtrlCond=cDangerCond;
	return effic;
}
double CheckBeamBy_CECS236_2008(BEAM_SECTION* pSect,BYTE* cCtrlCond)
{	//钢结构单管通信塔技术规程　2008版
	//double fsb;		//受弯局部稳定控制强度设计值MPa,
	//double fsc;		//受压局部稳定控制强度设计值MPa,
	if(pSect->cSectionType==BEAM_SECTION::SECT_TUBE)	//钢管
	{
		double D0_t=pSect->para.tube.D/pSect->para.tube.thick;
		double lim=140,lim2=300;
		if(pSect->fy==235)	//Q235材质
		{
			if(D0_t<lim)
				pSect->fsb=pSect->fsc=pSect->f;
			else //if(D0_t<=300)
				pSect->fsb=pSect->fsc=(0.586+73.85/D0_t-1832.5/(D0_t*D0_t))*pSect->f;
		}
		else if(pSect->fy==345)	//Q235材质
		{
			if(D0_t<114.6)
				pSect->fsb=pSect->fsc=pSect->f;
			else //if(D0_t<=245)
				pSect->fsb=pSect->fsc=(0.565+66.62/D0_t-1926.5/(D0_t*D0_t))*pSect->f;
		}
		else
		{	//按电力钢杆技术规定
			if(D0_t*pSect->f<=38060)
				pSect->fsb=pSect->f;
			else //if(D0_t*pSect->f<=(76130)
				pSect->fsb=0.7*pSect->f+11410/D0_t;
			if(D0_t*pSect->f<=24100)
				pSect->fsc=pSect->f;
			else //if(D0_t*pSect->f<=(76130)
				pSect->fsc=0.75*pSect->f+6025/D0_t;
		}
	}
	else if(pSect->cSectionType==BEAM_SECTION::SECT_POLY)	//多边形钢管
	{
		double W,BR=pSect->para.poly.BR;
		if(BR==0||BR>pSect->para.poly.thick*4)
			BR=pSect->para.poly.thick*4;
		if(pSect->para.poly.slices<=6)
			W=0.577*(pSect->para.poly.D0-2*pSect->para.poly.thick-2*BR);
		else if(pSect->para.poly.slices<=8)
			W=0.414*(pSect->para.poly.D0-2*pSect->para.poly.thick-2*BR);
		else if(pSect->para.poly.slices<=12)
			W=0.268*(pSect->para.poly.D0-2*pSect->para.poly.thick-2*BR);
		else //if(pSect->para.poly.slices<=16)
			W=0.199*(pSect->para.poly.D0-2*pSect->para.poly.thick-2*BR);
		double W_t=W/pSect->para.poly.thick;
		double sqrt_fy=sqrt(pSect->fy);
		if( (pSect->para.poly.slices<= 8&&W_t*sqrt_fy<=683)||
			(pSect->para.poly.slices<=12&&W_t*sqrt_fy<=630)||
			(pSect->para.poly.slices<=16&&W_t*sqrt_fy<=565))
			pSect->fsb=pSect->fsc=pSect->f;
		else if(pSect->para.poly.slices<=8)//&&W_t*sqrt_f<=925
			pSect->fsb=pSect->fsc=1.42*pSect->f*(1.0-0.000434*sqrt_fy*W_t);
		else if(pSect->para.poly.slices<=12)//&&W_t*sqrt_f<=925
			pSect->fsb=pSect->fsc=1.45*pSect->f*(1.0-0.000491*sqrt_fy*W_t);
		else //if(pSect->para.poly.slices<=16)//&&W_t*sqrt_f<=925
			pSect->fsb=pSect->fsc=1.42*pSect->f*(1.0-0.000522*sqrt_fy*W_t);
	}
	else
		return CheckBeamBy_GB50017_2003(pSect,cCtrlCond);
	pSect->fv=0.58*pSect->f;	//受剪强度允许值MPa,
	if(pSect->An<=0)
		pSect->An=pSect->Ag;
	if(pSect->P[2]>0)
	{
		pSect->sigemaNt = pSect->P[2]/pSect->An;	//轴向正应力MPa(即N/mm2)
		pSect->sigemaNc = 0;
	}
	else
	{
		pSect->sigemaNt = 0;
		pSect->sigemaNc =-pSect->P[2]/pSect->Ag;	//轴向正应力MPa(即N/mm2)
	}
	double Mxy=sqrt(pSect->M[0]*pSect->M[0]+pSect->M[1]*pSect->M[1]);
	double Pxy=sqrt(pSect->P[0]*pSect->P[0]+pSect->P[1]*pSect->P[1]);
	pSect->sigemaBend=Mxy/pSect->W;	//弯曲强度MPa
	pSect->taoShear=Pxy*pSect->Q_It+fabs(pSect->M[2])*pSect->C_J;	//剪切应力M=集中力剪应力+扭转剪应力，MPa
	//复合应力
	pSect->sigemaComplex =(pSect->sigemaNc+pSect->sigemaBend)*(pSect->sigemaNc+pSect->sigemaBend);
	pSect->sigemaComplex+=3*pSect->taoShear*pSect->taoShear;
	pSect->sigemaComplex =sqrt(pSect->sigemaComplex);
	char cDangerCond=BEAM_SECTION::COND_TENSION;
	double effic=pSect->sigemaNt/pSect->f;
	double ef=pSect->sigemaBend/pSect->fsb+pSect->sigemaNc/pSect->fsc;
	if(ef>effic)
	{
		effic=ef;
		cDangerCond=BEAM_SECTION::COND_COMPBEND;
		if(pSect->sigemaNc<EPS)
			cDangerCond=BEAM_SECTION::COND_BENDING;
	}
	/*ef=pSect->taoShear/pSect->fv;
	if(ef>effic)
	{
		effic=ef;
		cDangerCond=BEAM_SECTION::COND_SHEAR;
	}
	ef=pSect->sigemaComplex/pSect->fsb;
	if(ef>effic)
	{
		effic=ef;
		cDangerCond=BEAM_SECTION::COND_COMPLEX;
	}*/
	if(cCtrlCond!=NULL)
		*cCtrlCond=cDangerCond;
	return effic;
}
double CheckBeamBy_ASCE_10_97(BEAM_SECTION* pSect,BYTE* cCtrlCond)
{	//ASCE　2008版
	return CheckBeamBy_DLT5130_2001(pSect,cCtrlCond);
}
BEAM_SECTION::BEAM_SECTION(){memset(this,0,sizeof(BEAM_SECTION));}
double BEAM_SECTION::Check(BYTE* cCtrlCond,int exec_code)	//返回控制条件下的功效系数,cCtrlCond返回控制条件或最危险条件
{
	switch(exec_code)
	{
	case CODES::DLT_2012:
	case CODES::DLT5254_2010:
		return CheckBeamBy_DLT5254_2010(this,cCtrlCond);
	case CODES::DLT_2002:
		return CheckBeamBy_DLT_2002(this,cCtrlCond);
	case CODES::DLT5130_2001:
		return CheckBeamBy_DLT5130_2001(this,cCtrlCond);
	case CODES::CECS236_2008:
		return CheckBeamBy_CECS236_2008(this,cCtrlCond);
	case CODES::ASCE10_97:
		return CheckBeamBy_ASCE_10_97(this,cCtrlCond);
	//case CODES::GB50017_2003:
	default:	//钢结构规范
		return CheckBeamBy_GB50017_2003(this,cCtrlCond);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static CLogFile __default_logger;
ILog2File* IMemberDesignCode::Log2File()
{
	if(IMemberDesignCode::ERR_LOGGER)
		return IMemberDesignCode::ERR_LOGGER;
	else
		return &__default_logger;
}
int IMemberDesignCode::InitPropHashtable(int iDesignCode)
{
	int id = 1;
	CHashList<double>* pHashPropItems=GetHashPropItems(iDesignCode);//globalItemValues[iDesignCode];
	propHashtable.CreateHashTable(HASHTABLESIZE);
	pHashPropItems->SetHashTableGrowSize(64,false);
	//
	pHashPropItems->SetValue(id,1.2);	//杆塔设计时重力荷载分项系数
	propHashtable.SetValueAt("m_fGamaGForDesign",id++);
	pHashPropItems->SetValue(id,1.0);	//基础设计时重力荷载分项系数
	propHashtable.SetValueAt("m_fGamaGForDesign",id++);
	pHashPropItems->SetValue(id,1.4);	//杆塔本身及附加风荷载分项系数
	propHashtable.SetValueAt("m_fGamaQForTowerWindLoad",id++);
	//满应力优化先材参数
	m_idPostRodKlamdaLimit=id;
	pHashPropItems->SetValue(id,150);	//主材长细比最大限值
	propHashtable.SetValueAt("m_fPostMemberLamdaLimit",id++);
	if(iDesignCode==0)		//DL/T 5154-2002
		pHashPropItems->SetValue(id,220);	//受力材长细比最大限值
	else if(iDesignCode==2)	//GB50135_2006	
		pHashPropItems->SetValue(id,180);	//受力材长细比最大限值
	else //if(iDesignCode==1,3)	//DL/T 5154-2012 &ASCE 10-97
		pHashPropItems->SetValue(id,200);	//受力材长细比最大限值
	m_idPrimaryRodKlamdaLimit=id;
	propHashtable.SetValueAt("m_fPrimaryMemberLamdaLimit",id++);
	if(iDesignCode==2)	//GB50135_2006	
		pHashPropItems->SetValue(id,200);	//受力材长细比最大限值
	else	//
		pHashPropItems->SetValue(id,250);	//辅助材长细比最大限值
	m_idBraceRodKlamdaLimit=id;
	propHashtable.SetValueAt("m_fRedundantMemberLamdaLimit",id++);
	if(iDesignCode==2)		//GB50135_2006	
		pHashPropItems->SetValue(id,350);	//受力材长细比最大限值
	else if(iDesignCode==3)	//ASCE 10-97
		pHashPropItems->SetValue(id,500);	//受拉材长细比最大限值
	else	//DL/T 5154
		pHashPropItems->SetValue(id,400);	//受拉材长细比最大限值
	m_idTensionElemLamdaLimit=id;
	propHashtable.SetValueAt("m_fTensiveMemberLamdaLimit",id++);
	/* 一般情况钢管长细比都满足，实际意义不大
	pHashPropItems->SetValue(id,130);	//钢管主材长细比最大限值
	propHashtable.SetValueAt("m_fPostTubeMemberLamdaLimit",id++);
	pHashPropItems->SetValue(id,200);	//钢管受力材长细比最大限值
	propHashtable.SetValueAt("m_fPrimaryTubeMemberLamdaLimit",id++);
	pHashPropItems->SetValue(id,250);	//钢管辅助材长细比最大限值
	propHashtable.SetValueAt("m_fRedundantTubeMemberLamdaLimit",id++);
	pHashPropItems->SetValue(id,400);	//钢管受拉材长细比最大限值
	propHashtable.SetValueAt("m_fTensiveTubeMemberLamdaLimit",id++);
	*/
	//double m_fLimitBearCoef;		//承载系数最大设计值
	pHashPropItems->SetValue(id,4);	//主材最小肢厚
	propHashtable.SetValueAt("m_fPostAngleMinThick",id++);
	pHashPropItems->SetValue(id,3);	//受力材最小肢厚
	propHashtable.SetValueAt("m_fPrimaryAngleMinThick",id++);
	pHashPropItems->SetValue(id,3);	//辅材最小肢厚
	propHashtable.SetValueAt("m_fRedundantAngleMinThick",id++);
	//pHashPropItems->SetValue(id,4);	//钢板最小厚
	//propHashtable.SetValueAt("m_fPlateMinThick",id++);
	propHashtable.SetValueAt("m_fSafetyCoef",id++);
	pHashPropItems->SetValue(id,1.1);	//特别重要的杆塔
	propHashtable.SetValueAt("m_fSafetyCoef_Grade1",id++);
	pHashPropItems->SetValue(id,1.0);	//一般的各类杆塔
	propHashtable.SetValueAt("m_fSafetyCoef_Grade2",id++);
	pHashPropItems->SetValue(id,0.9);	//临时使用的杆塔
	propHashtable.SetValueAt("m_fSafetyCoef_Grade3",id++);
	pHashPropItems->SetValue(id,900);	//覆冰密度，kg/m3
	propHashtable.SetValueAt("ice_density",id++);
	m_idBraceCoefOfRedundantToPost=id;		//计算辅材对主材的支撑力占比系数
	pHashPropItems->SetValue(id,0.02);
	propHashtable.SetValueAt("m_fBraceCoefOfRedundantToPost",id++);
	m_idBraceCoefOfRedundantToPrimary=id;	//计算辅材对斜材的支撑力占比系数
	pHashPropItems->SetValue(id,0.05);
	propHashtable.SetValueAt("m_fBraceCoefOfRedundantToPrimary",id++);
	m_idBraceCoefOfPrimaryToPost=id;		//计算斜材对主材的支撑力占比系数
	pHashPropItems->SetValue(id,0.03);
	propHashtable.SetValueAt("m_fBraceCoefOfPrimaryToPost",id++);

	return id;
}
int DL_T_2002::InitPropHashtable()
{
	int maxid=IMemberDesignCode::InitPropHashtable(0);
	//2002版技术规定未规定斜材对主材的支撑力占比系数,故设为0值
	GetHashPropItems(0)->SetValue(m_idBraceCoefOfPrimaryToPost,0.0);
	return maxid;
}
int GB50135_2006::InitPropHashtable()
{
	return IMemberDesignCode::InitPropHashtable(2);
}
int DL_T_2012::InitPropHashtable()
{
	return IMemberDesignCode::InitPropHashtable(1);
}
int ASCE10_97::InitPropHashtable()
{
	return IMemberDesignCode::InitPropHashtable(3);
}
//规范中指定项的参数值，如单肢连接受拉构件强度折减系数
static double GetItemValueInteral(long idItemProp,int iDesignCode)
{
	if(iDesignCode<0||iDesignCode>=4)
		return 0;
	CHashList<double>* pHashPropItems=GetHashPropItems(iDesignCode);
	double *pVal=pHashPropItems->GetValue(idItemProp);
	if(pVal)
		return *pVal;
	else
		return 0;
}
double DL_T_2002::GetItemValue(long idItemProp){return GetItemValueInteral(idItemProp,0);}
double DL_T_2012::GetItemValue(long idItemProp){return GetItemValueInteral(idItemProp,1);}
double GB50135_2006::GetItemValue(long idItemProp){return GetItemValueInteral(idItemProp,2);}
double ASCE10_97::GetItemValue(long idItemProp){return GetItemValueInteral(idItemProp,3);}
long IMemberDesignCode::GetPropId(char* itemKeyStr)
{
	long prop_id=0;
	if(propHashtable.GetValueAt(itemKeyStr,prop_id))
		return prop_id;
	else
		return 0;
}
int IMemberDesignCode::GetItemValueStr(long idItemProp,char* valueStr)
{
	double fVal=GetItemValue(idItemProp);
	if(fVal<0)
		fVal=GetItemValue(idItemProp);
	CXhChar16 value_str("%f",fVal);
	if(valueStr)
		strcpy(valueStr,value_str);
	return value_str.Length();
}
//计算角钢单元的螺栓减孔数
double IMemberDesignCode::CalElemHoleOutNum(int style,double t,double d0,double d1,double gx/*=0*/,double gy/*=0*/,double S/*=0*/)
{
	double coef=2;
	if(style==1)
		coef=2;
	else if(style==3)
	{
		double g=2*d1-t;
		double gmin=min(gx,gy);
		double gmax=max(gx,gy);
		double coef1=2;
		double coef2=3-S*S/(4*gmax*d0);
		double coef3=4-S*S*(1/g+1/gx+1/gy)/(4*d0);
		coef=max(coef1,coef2);
		coef=max(coef3,coef);
	}
	else //if(style==2)
	{
		double g=2*d1-t;
		double coef=2-S*S/(4*g*d0);
		coef=max(1.0,coef);
	}
	return coef;
}
//根据交叉斜材内力及布材型式，返回斜材长细比计算参数,N0为绝对值较小内力的代数值
IMemberDesignCode::BevelRodLamdaPara IMemberDesignCode::IntersBevelRodLamdaCal(double N,double N0,double ratioL2toL3/*=0*/)
{
	double Na=N;	//主压杆压力（代数值较小，且为负值）
	double N0a=N0;	//辅杆可能为压杆也可能为小拉力拉杆
	if(N0<N && N<0)
	{
		Na=N0;
		N0a=N;
	}
	IMemberDesignCode::BevelRodLamdaPara rslt(0);
	if(Na<0)
	{
		if(N0<0)
		{	//两杆同时受压
			rslt.K=sqrt(0.5*(1+fabs(N0a/Na)));
			rslt.cType='C';
		}
		else if(N0<0.2*fabs(Na))
		{	//两杆一拉一压，但拉杆拉力小于压杆压力绝对值20%
			rslt.K=sqrt(ratioL2toL3*(1-0.75*fabs(N0a/Na)));
			if(rslt.K<0.5)
				rslt.K=0.5;
			rslt.cType='c';
		}
	}
	return rslt;
}
//块剪验算，endSpace:螺栓端距;midSpace:螺栓间距总和;c螺栓心线到肢翼边距离,f:设计强度;fv:抗剪强度
double IMemberDesignCode::BlockShearCheck(double N,double endSpace,double midSpace,double c,double t,double f,double fv)
{
	double Av=t*(endSpace+midSpace);
	double At=t*c;
	double maxN=Av*fv+At*f;
	return N/maxN;
}
long IMemberDesignCode::GetPostRodKlamdaLimit(bool bTube/*=false*/)
{
	return (long)(GetItemValue(m_idPostRodKlamdaLimit));
}
long IMemberDesignCode::GetPrimaryRodKlamdaLimit(bool bTube/*=false*/)
{
	return (long)(GetItemValue(m_idPrimaryRodKlamdaLimit));
}
long IMemberDesignCode::GetBraceRodKlamdaLimit(bool bTube/*=false*/)
{
	return (long)(GetItemValue(m_idBraceRodKlamdaLimit));
}
long IMemberDesignCode::GetTensionRodKlamdaLimit(bool bTube/*=false*/)
{
	return (long)(GetItemValue(m_idTensionElemLamdaLimit));
}
double IMemberDesignCode::GetBraceCoefOfRedundantToPost()	//计算辅材对主材的支撑力占比系数
{
	return GetItemValue(m_idBraceCoefOfRedundantToPost);
}
double IMemberDesignCode::GetBraceCoefOfRedundantToPrimary()//计算辅材对斜材的支撑力占比系数
{
	return GetItemValue(m_idBraceCoefOfRedundantToPrimary);
}
double IMemberDesignCode::GetBraceCoefOfPrimaryToPost()		//计算斜材对主材的支撑力占比系数
{	//见DL/T5154-2012  P79 A.0.4条，计算埃菲尔效应时斜材设计内力不应小于主材内力的3% wjh-2017.3.20
	return GetItemValue(m_idBraceCoefOfPrimaryToPost);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 DL_T-2002.h
DL_T_2002::DL_T_2002()
{
	InitPropHashtable();
}
GB50135_2006::GB50135_2006()
{
	InitPropHashtable();
}
DL_T_2012::DL_T_2012()
{ 
	InitPropHashtable();
}
ASCE10_97::ASCE10_97()
{ 
	InitPropHashtable();
}
const int JOINT_HINGE=0;
const int JOINT_RIGID=1;		//连接形式
const int CENTRIC_FORCE=0;
const int ECCENTRIC_FORCE=1;	//端部受力连接的两种情况：中心受力；偏心受力
//普通受力材及辅材杆件修正生长细比,cRodType,'P'：受力材(Primary)；'B':辅助材：辅助材(Brace);endType：杆端状况，具体见DL/T 5154
double DL_T_2002::CalRodKLamda(double lamda,char cRodType,
							   int start_joint_type,int end_joint_type,int start_force_type,int end_force_type)
{
	if(!IsIncLamdaK())
		return lamda;
	int endType=1;
	if(cRodType=='B')	//辅助材（不受力材）
	{
		if(start_joint_type==JOINT_HINGE&&end_joint_type==JOINT_HINGE)
			endType=2;	//两端无约束
		else if(start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
			endType=3;	//两端有约束
		else if(start_joint_type==JOINT_RIGID||end_joint_type==JOINT_RIGID)
			endType=4;	//一端有约束
		else
			endType=5;
		if(lamda<120)
			return lamda;				//两端偏心受压
		else if(lamda<=250&&endType==2)
			return lamda;				//两端无约束
		else if(lamda<=330&&endType==3)
			return 0.615*lamda+46.2;	//两端有约束
		else if(lamda<=290)	//endType==4
			return 0.762*lamda+28.6;	//一端有约束
		else
			return lamda;
	}
	else
	{
		if(lamda<120)
		{
			if(start_force_type==CENTRIC_FORCE&&end_force_type==CENTRIC_FORCE)
				endType=1;	//两端中心受压
			else if(start_force_type==CENTRIC_FORCE||end_force_type==CENTRIC_FORCE)
				endType=2;	//一端中心一端偏心受压
			else //if(start_force_type==ECCENTRIC_FORCE||end_force_type==ECCENTRIC_FORCE)
				endType=3;	//两端偏心受压
			if(endType==1)
				return lamda;			//两端中心受力
			//根据设计技术规定 DL/T 5154-2002,	交叉斜材一拉一压时，中间部分认为是中心受力
			else if(endType==2)
				return 0.75*lamda+30;	//一端中心一端偏心受压
			else// if(endType==3)	//pElem->start_force_type==ECCENTRIC_FORCE&&pElem->end_force_type==ECCENTRIC_FORCE)
				return 0.5*lamda+60;	//两端偏心受压
		}
		else //if(lamda>=120)
		{
			if(start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
				endType=6;	//两端有约束
			else if(start_joint_type==JOINT_RIGID||end_joint_type==JOINT_RIGID)
				endType=5;	//一端有约束
			else
				endType=4;	//两端无约束
			if(endType==5)//&&lamda<=231		//start_joint_type!=end_joint_type)	//
				return 0.90*lamda+11.89;//一端有约束
			else if(endType==6)//&&lamda<242	//start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
				return 0.82*lamda+21.64;//两端有约束
			else// if(endType==4)
				return lamda;			//两端无约束
		}
	}
}
//普通受力材及辅材杆件修正生长细比,cRodType,'P'：受力材(Primary)；'B':辅助材：辅助材(Brace);endType：杆端状况，具体见DL/T 5154
double DL_T_2012::CalRodKLamda(double lamda,char cRodType,
							   int start_joint_type,int end_joint_type,int start_force_type,int end_force_type)
{
	int endType=1;
	if(cRodType=='B')	//辅助材（不受力材）
	{
		if(start_joint_type==JOINT_HINGE&&end_joint_type==JOINT_HINGE)
			endType=2;	//两端无约束
		else if(start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
			endType=3;	//两端有约束
		else if(start_joint_type==JOINT_RIGID||end_joint_type==JOINT_RIGID)
			endType=4;	//一端有约束
		else
			endType=5;
		if(lamda<120)
			return lamda;				//两端偏心受压
		else if(lamda<=250&&endType==2)
			return lamda;				//两端无约束
		else if(lamda<=330&&endType==3)
			return 0.615*lamda+46.2;	//两端有约束
		else if(lamda<=290)	//endType==4
			return 0.762*lamda+28.6;	//一端有约束
		else
			return lamda;
	}
	else
	{
		if(lamda<120)
		{
			if(start_force_type==CENTRIC_FORCE&&end_force_type==CENTRIC_FORCE)
				endType=1;	//两端中心受压
			else if(start_force_type==CENTRIC_FORCE||end_force_type==CENTRIC_FORCE)
				endType=2;	//一端中心一端偏心受压
			else //if(start_force_type==ECCENTRIC_FORCE||end_force_type==ECCENTRIC_FORCE)
				endType=3;	//两端偏心受压
			if(endType==1)
				return lamda;			//两端中心受力
			//根据设计技术规定 DL/T 5154-2002,	交叉斜材一拉一压时，中间部分认为是中心受力
			else if(endType==2)
				return 0.75*lamda+30;	//一端中心一端偏心受压
			else// if(endType==3)	//pElem->start_force_type==ECCENTRIC_FORCE&&pElem->end_force_type==ECCENTRIC_FORCE)
				return 0.5*lamda+60;	//两端偏心受压
		}
		else //if(lamda>=120)
		{
			if(start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
				endType=6;	//两端有约束
			else if(start_joint_type==JOINT_RIGID||end_joint_type==JOINT_RIGID)
				endType=5;	//一端有约束
			else
				endType=4;	//两端无约束
			//225和250，仅是为了凑够200有允许长细比，大于此值折算结果超限，所以这些值没有实际物理意义，故可不考虑
			if(endType==5)//&&lamda<=225)// ;start_joint_type!=end_joint_type)	//一端有约束
				return 0.762*lamda+28.6;//一端有约束
			else if(endType==6)//&&lamda<250)	// ;start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
				return 0.615*lamda+46.2;//两端有约束
			else// if(lamda<=200&&endType==4)
				return lamda;			//两端无约束
		}
	}
}
//普通受力材及辅材杆件修正生长细比,cRodType,'P'：受力材(Primary)；'B':辅助材：辅助材(Brace);endType：杆端状况，具体见DL/T 5154
double ASCE10_97::CalRodKLamda(double lamda,char cRodType,
							   int start_joint_type,int end_joint_type,int start_force_type,int end_force_type)
{
	int endType=1;
	if(cRodType=='B')	//辅助材（不受力材）
	{
		if(start_joint_type==JOINT_HINGE&&end_joint_type==JOINT_HINGE)
			endType=2;	//两端无约束
		else if(start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
			endType=3;	//两端有约束
		else if(start_joint_type==JOINT_RIGID||end_joint_type==JOINT_RIGID)
			endType=4;	//一端有约束
		else
			endType=5;
		if(lamda<120)	//endType=1;
			return lamda;				//两端偏心受压
		else if(lamda<=250&&endType==2)
			return lamda;				//两端无约束
		else if(lamda<=330&&endType==3)
			return 0.615*lamda+46.2;	//两端有约束
		else if(lamda<=290)	//endType==4
			return 0.762*lamda+28.6;	//一端有约束
		else
			return lamda;
	}
	else
	{
		if(lamda<120)
		{
			if(start_force_type==CENTRIC_FORCE&&end_force_type==CENTRIC_FORCE)
				endType=1;	//两端中心受压
			else if(start_force_type==CENTRIC_FORCE||end_force_type==CENTRIC_FORCE)
				endType=2;	//一端中心一端偏心受压
			else //if(start_force_type==ECCENTRIC_FORCE||end_force_type==ECCENTRIC_FORCE)
				endType=3;	//两端偏心受压
			if(endType==1)
				return lamda;			//两端中心受力
			//根据设计技术规定 DL/T 5154-2002,	交叉斜材一拉一压时，中间部分认为是中心受力
			else if(endType==2)
				return 0.75*lamda+30;	//一端中心一端偏心受压
			else// if(endType==3)	//pElem->start_force_type==ECCENTRIC_FORCE&&pElem->end_force_type==ECCENTRIC_FORCE)
				return 0.5*lamda+60;	//两端偏心受压
		}
		else //if(lamda>=120)
		{
			if(start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
				endType=6;	//两端有约束
			else if(start_joint_type==JOINT_RIGID||end_joint_type==JOINT_RIGID)
				endType=5;	//一端有约束
			else
				endType=4;	//两端无约束
			if(lamda<=225&&endType==5)		//start_joint_type!=end_joint_type)	//一端有约束
				return 0.762*lamda+28.6;//一端有约束
			else if(lamda<250&&endType==6)	//start_joint_type==JOINT_RIGID&&end_joint_type==JOINT_RIGID)
				return 0.615*lamda+46.2;//两端有约束
			else// if(lamda<=200&&endType==4)
				return lamda;			//两端无约束
		}
	}
}
///////////////////////////////////////////////////////////////////////
//压杆稳定设计强度需输入信息：b:肢翼自由外伸宽度,t：肢厚;f:强度设计或屈服强度,bending:是否有弯矩
double DL_T_2002::CompReduceCoef(double b,double t,double f,double fy/*=0*/,double Klamda/*=0*/,bool isTube/*=false*/,bool bending/*=false*/)
{
	double mN;
	double b2t=b/t;
	if(!isTube)
	{
		//double b=pJgType->wing_wide-pJgType->r-t;
		double bt_lim=202/sqrt(f);
		if(b2t<=bt_lim)
			mN=1.0;
		else //if(b/t<=363/sqrt(f)
			mN=1.677-0.677*b2t/bt_lim;
	}
	else
	{	//此时b相当于外径D0
		if(b2t<=24100/f)
			mN=1.0;
		else
			mN=0.75+6025/(b*f/t);
	}
	return mN;
}
double DL_T_2012::CompReduceCoef(double b,double t,double f,double fy/*=0*/,double Klamda/*=0*/,bool isTube/*=false*/,bool bending/*=false*/)
{
	double mN;
	double b2t=b/t;
	if(!isTube)
	{
		double bt_lim;
		//double b=pJgType->wing_wide-pJgType->r-t;
		if(bending)
			bt_lim=15*sqrt(235/fy);
		else
		{
			if(Klamda<30)
				Klamda=30;
			else if(Klamda>100)
				Klamda=100;
			bt_lim=(10+0.1*Klamda)*sqrt(235/fy);
		}
		if(b2t<=bt_lim)
			mN=1.0;
		else //if(b/t<=380/sqrt(fy)
			mN=1.677-0.677*b2t/bt_lim;
	}
	else
	{	//此时b相当于外径D0
		if(b2t<=24100/f)
			mN=1.0;
		else
			mN=0.75+6025/(b*f/t);
	}
	return mN;
}
//double DL_T_2012
double ASCE10_97::CompReduceCoef(double b,double t,double f,double fy/*=0*/,double Klamda/*=0*/,bool isTube/*=false*/,bool bending/*=false*/)
{
	double mN;
	double b2t=b/t;
	if(!isTube)
	{
		double bt_lim=209.6/sqrt(fy);
		if(b2t<=bt_lim)
			mN=1.0;
		else if(b2t<=377.3/sqrt(f))
			mN=1.677-0.677*b2t/bt_lim;
		else
			mN=0.32767*IMemberDesignCode::modulusE/(b2t*b2t*f);//0.32767=0.0332*Pi*Pi	//1 ksi=6.89476 MPa
	}
	else
	{	//此时b相当于外径D0
		if(b2t<=24100/f)
			mN=1.0;
		else
			mN=0.75+6025/(b*f/t);
	}
	return mN;
}
///////////////////////////////////////////////////////////////////////
//压稳系数

double Q235A_STABLE_COEF[251]=
{
	1.000, 	1.000, 	1.000, 	1.000, 	0.999, 	0.999, 	0.998, 	0.998, 	0.997, 	0.996, 
	0.995, 	0.994, 	0.993, 	0.992, 	0.991, 	0.989, 	0.988, 	0.986, 	0.985, 	0.983, 
	0.981, 	0.979, 	0.977, 	0.976, 	0.974, 	0.972, 	0.970, 	0.968, 	0.966, 	0.964, 
	0.963, 	0.961, 	0.959, 	0.957, 	0.955, 	0.952, 	0.950, 	0.948, 	0.946, 	0.944, 
	0.941, 	0.939, 	0.937, 	0.934, 	0.932, 	0.929, 	0.927, 	0.924, 	0.921, 	0.919, 
	0.916, 	0.913, 	0.910, 	0.907, 	0.904, 	0.900, 	0.897, 	0.894, 	0.890, 	0.886, 
	0.883, 	0.879, 	0.875, 	0.871, 	0.867, 	0.863, 	0.858, 	0.854, 	0.849, 	0.844, 
	0.839, 	0.834, 	0.829, 	0.824, 	0.818, 	0.813, 	0.807, 	0.801, 	0.795, 	0.789, 
	0.783, 	0.776, 	0.770, 	0.763, 	0.757, 	0.750, 	0.743, 	0.736, 	0.728, 	0.721, 
	0.714, 	0.706, 	0.699, 	0.691, 	0.684, 	0.676, 	0.668, 	0.661, 	0.653, 	0.645, 
	0.638, 	0.630, 	0.622, 	0.615, 	0.607, 	0.600, 	0.592, 	0.585, 	0.577, 	0.570, 
	0.563, 	0.555, 	0.548, 	0.541, 	0.534, 	0.527, 	0.520, 	0.514, 	0.507, 	0.500, 
	0.494, 	0.488, 	0.481, 	0.475, 	0.469, 	0.463, 	0.457, 	0.451, 	0.445, 	0.440, 
	0.434, 	0.429, 	0.423, 	0.418, 	0.412, 	0.407, 	0.402, 	0.397, 	0.392, 	0.387, 
	0.383, 	0.378, 	0.373, 	0.369, 	0.364, 	0.360, 	0.356, 	0.351, 	0.347, 	0.343, 
	0.339, 	0.335, 	0.331, 	0.327, 	0.323, 	0.320, 	0.316, 	0.312, 	0.309, 	0.305, 
	0.302, 	0.298, 	0.295, 	0.292, 	0.289, 	0.285, 	0.282, 	0.279, 	0.276, 	0.273, 
	0.270, 	0.267, 	0.264, 	0.262, 	0.259, 	0.256, 	0.253, 	0.251, 	0.248, 	0.246, 
	0.243, 	0.241, 	0.238, 	0.236, 	0.233, 	0.231, 	0.229, 	0.226, 	0.224, 	0.222, 
	0.220, 	0.218, 	0.215, 	0.213, 	0.211, 	0.209, 	0.207, 	0.205, 	0.203, 	0.201, 
	0.199, 	0.198, 	0.196, 	0.194, 	0.192, 	0.190, 	0.189, 	0.187, 	0.185, 	0.183, 
	0.182, 	0.180, 	0.179, 	0.177, 	0.175, 	0.174, 	0.172, 	0.171, 	0.169, 	0.168, 
	0.166, 	0.165, 	0.164, 	0.162, 	0.161, 	0.159, 	0.158, 	0.157, 	0.155, 	0.154, 
	0.153, 	0.152, 	0.150, 	0.149, 	0.148, 	0.147, 	0.146, 	0.144, 	0.143, 	0.142, 
	0.141, 	0.140, 	0.139, 	0.138, 	0.136, 	0.135, 	0.134, 	0.133, 	0.132, 	0.131, 
	0.130,
};
double Q235B_STABLE_COEF[251]=
{
	1.000, 	1.000, 	1.000, 	0.999, 	0.999, 	0.998, 	0.997, 	0.996, 	0.995, 	0.994, 
	0.992, 	0.991, 	0.989, 	0.987, 	0.985, 	0.983, 	0.981, 	0.978, 	0.976, 	0.973, 
	0.970, 	0.967, 	0.963, 	0.960, 	0.957, 	0.953, 	0.950, 	0.946, 	0.943, 	0.939, 
	0.936, 	0.932, 	0.929, 	0.925, 	0.922, 	0.918, 	0.914, 	0.910, 	0.906, 	0.903, 
	0.899, 	0.895, 	0.891, 	0.887, 	0.882, 	0.878, 	0.874, 	0.870, 	0.865, 	0.861, 
	0.856, 	0.852, 	0.847, 	0.842, 	0.838, 	0.833, 	0.828, 	0.823, 	0.818, 	0.813, 
	0.807,	0.802, 	0.797, 	0.791, 	0.786, 	0.780, 	0.774, 	0.769, 	0.763, 	0.757, 
	0.751,	0.745, 	0.739, 	0.732, 	0.726, 	0.720, 	0.714, 	0.707, 	0.701, 	0.694, 
	0.688,	0.681, 	0.675, 	0.668, 	0.661, 	0.655, 	0.648, 	0.641, 	0.635, 	0.628, 
	0.621,	0.614, 	0.608, 	0.601, 	0.594, 	0.588, 	0.581, 	0.575, 	0.568, 	0.561, 
	0.555,	0.549, 	0.542, 	0.536, 	0.529, 	0.523, 	0.517, 	0.511, 	0.505, 	0.499, 
	0.493,	0.487, 	0.481, 	0.475, 	0.470, 	0.464, 	0.458, 	0.453, 	0.447, 	0.442, 
	0.437,	0.432, 	0.426, 	0.421, 	0.416, 	0.411, 	0.406, 	0.402, 	0.397, 	0.392, 
	0.387,	0.383, 	0.378, 	0.374, 	0.370, 	0.365, 	0.361, 	0.357, 	0.353, 	0.349, 
	0.345,	0.341, 	0.337, 	0.333, 	0.329, 	0.326, 	0.322, 	0.318, 	0.315, 	0.311, 
	0.308,	0.304, 	0.301, 	0.298, 	0.295, 	0.291, 	0.288, 	0.285, 	0.282, 	0.279, 
	0.276,	0.273, 	0.270, 	0.267, 	0.265, 	0.262, 	0.259, 	0.256, 	0.254, 	0.251, 
	0.249,	0.246, 	0.244, 	0.241, 	0.239, 	0.236, 	0.234, 	0.232, 	0.229, 	0.227, 
	0.225,	0.223, 	0.220, 	0.218, 	0.216, 	0.214, 	0.212, 	0.210, 	0.208, 	0.206, 
	0.204,	0.202, 	0.200, 	0.198, 	0.197, 	0.195, 	0.193, 	0.191, 	0.190, 	0.188, 
	0.186,	0.184, 	0.183, 	0.181, 	0.180, 	0.178, 	0.176, 	0.175, 	0.173, 	0.172, 
	0.170,	0.169, 	0.167, 	0.166, 	0.165, 	0.163, 	0.162, 	0.160, 	0.159, 	0.158, 
	0.156,	0.155, 	0.154, 	0.153, 	0.151, 	0.150, 	0.149, 	0.148, 	0.146, 	0.145, 
	0.144,	0.143, 	0.142, 	0.141, 	0.140, 	0.138, 	0.137, 	0.136, 	0.135, 	0.134, 
	0.133,	0.132, 	0.131, 	0.130, 	0.129, 	0.128, 	0.127, 	0.126, 	0.125, 	0.124, 
	0.123,
};
double Q345A_STABLE_COEF[251]=
{
	1.0000,  1.0000,  1.0000,  0.9990,  0.9990,  0.9980,  0.9970,  0.9970,  0.9960,  0.9940,  
	0.9930,  0.9920,  0.9900,  0.9880,  0.9860,  0.9840,  0.9820,  0.9800,  0.9780,  0.9750,  
	0.9730,  0.9710,  0.9690,  0.9670,  0.9640,  0.9620,  0.9600,  0.9570,  0.9550,  0.9520,  
	0.9500,  0.9470,  0.9440,  0.9410,  0.9390,  0.9360,  0.9330,  0.9300,  0.9270,  0.9230,  
	0.9200,  0.9170,  0.9130,  0.9090,  0.9060,  0.9020,  0.8980,  0.8940,  0.8890,  0.8850,  
	0.8810,  0.8760,  0.8710,  0.8660,  0.8610,  0.8550,  0.8500,  0.8440,  0.8380,  0.8320,  
	0.8250,  0.8190,  0.8120,  0.8050,  0.7980,  0.7910,  0.7830,  0.7750,  0.7670,  0.7590,  
	0.7510,  0.7420,  0.7340,  0.7250,  0.7160,  0.7070,  0.6980,  0.6890,  0.6800,  0.6710,  
	0.6610,  0.6520,  0.6430,  0.6330,  0.6240,  0.6150,  0.6060,  0.5960,  0.5870,  0.5780,  
	0.5700,  0.5610,  0.5520,  0.5430,  0.5350,  0.5270,  0.5180,  0.5100,  0.5020,  0.4940,  
	0.4870,  0.4790,  0.4710,  0.4640,  0.4570,  0.4500,  0.4430,  0.4360,  0.4290,  0.4230,  
	0.4160,  0.4100,  0.4040,  0.3980,  0.3920,  0.3860,  0.3800,  0.3740,  0.3690,  0.3630,  
	0.3580,  0.3530,  0.3480,  0.3430,  0.3380,  0.3330,  0.3280,  0.3240,  0.3190,  0.3150,  
	0.3100,  0.3060,  0.3020,  0.2980,  0.2940,  0.2900,  0.2860,  0.2820,  0.2780,  0.2750,  
	0.2710,  0.2680,  0.2640,  0.2610,  0.2570,  0.2540,  0.2510,  0.2480,  0.2450,  0.2420,  
	0.2390,  0.2360,  0.2330,  0.2300,  0.2270,  0.2240,  0.2220,  0.2190,  0.2170,  0.2140,  
	0.2120,  0.2090,  0.2070,  0.2040,  0.2020,  0.2000,  0.1970,  0.1950,  0.1930,  0.1910,  
	0.1890,  0.1870,  0.1840,  0.1820,  0.1800,  0.1790,  0.1770,  0.1750,  0.1730,  0.1710,  
	0.1690,  0.1670,  0.1660,  0.1640,  0.1620,  0.1610,  0.1590,  0.1570,  0.1560,  0.1540,  
	0.1530,  0.1510,  0.1500,  0.1480,  0.1470,  0.1450,  0.1440,  0.1420,  0.1410,  0.1400,  
	0.1380,  0.1370,  0.1360,  0.1340,  0.1330,  0.1320,  0.1310,  0.1290,  0.1280,  0.1270,  
	0.1260,  0.1250,  0.1240,  0.1230,  0.1210,  0.1200,  0.1190,  0.1180,  0.1170,  0.1160,  
	0.1150,  0.1140,  0.1130,  0.1120,  0.1110,  0.1100,  0.1090,  0.1080,  0.1070,  0.1060,  
	0.1060,  0.1050,  0.1040,  0.1030,  0.1020,  0.1010,  0.1000,  0.0996,  0.0988,  0.0980,  
	0.0972,  0.0964,  0.0957,  0.0949,  0.0942,  0.0934,  0.0970,  0.0919,  0.0912,  0.0905,  
	0.0898,
};
double Q345B_STABLE_COEF[251]=
{
	1.0000, 1.0000, 1.0000, 0.9990, 0.9980, 0.9970, 0.9960, 0.9950, 0.9930, 0.9910,  
	0.9890, 0.9870, 0.9840, 0.9810, 0.9780, 0.9750, 0.9720, 0.9680, 0.9640, 0.9600,  
	0.9560, 0.9520, 0.9480, 0.9430, 0.9390, 0.9350, 1.0000, 0.9260, 0.9220, 0.9170,  
	0.9130, 0.9080, 0.9030, 0.8990, 0.8940, 0.8890, 0.8840, 0.8790, 0.8740, 0.8690,  
	0.8630, 0.8580, 0.8520, 0.8470, 0.8410, 0.8350, 0.8290, 0.8230, 0.8170, 0.8110,  
	0.8040, 0.7980, 0.7910, 0.7840, 0.7780, 0.7710, 0.7640, 0.7560, 0.7490, 0.7420,  
	0.7340, 0.7270, 0.7190, 0.7110, 0.7040, 0.6960, 0.6880, 0.6800, 0.6720, 0.6640,  
	0.6560, 0.6480, 0.6400, 0.6320, 0.6230, 0.6150, 0.6070, 0.5990, 0.5910, 0.5830,  
	0.5750, 0.5670, 0.5590, 0.5510, 0.5440, 0.5360, 0.5280, 0.5210, 0.5130, 0.5060,  
	0.4990, 0.4910, 0.4840, 0.4770, 0.4700, 0.4630, 0.4570, 0.4500, 0.4430, 0.4370,  
	0.4310, 0.4240, 0.4180, 0.4120, 0.4060, 0.4000, 0.3950, 0.3890, 0.3840, 0.3780,  
	0.3730, 0.3670, 0.3620, 0.3570, 0.3520, 0.3470, 0.3430, 0.3380, 0.3330, 0.3290,  
	0.3240, 0.3200, 0.3150, 0.3110, 0.3070, 0.3030, 0.2990, 0.2950, 0.2910, 0.2870,  
	0.2830, 0.2800, 0.2760, 0.2730, 0.2690, 0.2660, 0.2620, 0.2590, 0.2560, 0.2530,  
	0.2490, 0.2460, 0.2430, 0.2400, 0.2370, 0.2350, 0.2320, 0.2290, 0.2260, 0.2240,  
	0.2210, 0.2180, 0.2160, 0.2130, 0.2110, 0.2080, 0.2060, 0.2040, 0.2010, 0.1990,  
	0.1970, 0.1950, 0.1930, 0.1900, 0.1880, 0.1860, 0.1840, 0.1820, 0.1800, 0.1780,  
	0.1760, 0.1750, 0.1730, 0.1710, 0.1690, 0.1670, 0.1660, 0.1640, 0.1620, 0.1610,  
	0.1590, 0.1570, 0.1560, 0.1540, 0.1530, 0.1510, 0.1500, 0.1480, 0.1470, 0.1450,  
	0.1440, 0.1420, 0.1410, 0.1400, 0.1380, 0.1370, 0.1360, 0.1350, 0.1330, 0.1320,  
	0.1310, 0.1300, 0.1280, 0.1270, 0.1260, 0.1250, 0.1240, 0.1230, 0.1220, 0.1200,  
	0.1190, 0.1180, 0.1170, 0.1160, 0.1150, 0.1140, 0.1130, 0.1120, 0.1110, 0.1100,  
	0.1090, 0.1080, 0.1080, 0.1070, 0.1060, 0.1050, 0.1040, 0.1030, 0.1020, 0.1010,  
	0.1010, 0.0998, 0.0990, 0.0982, 0.0974, 0.0966, 0.0959, 0.0951, 0.0943, 0.0936,  
	0.0929, 0.0921, 0.0914, 0.0907, 0.0900, 0.0893, 0.0886, 0.0879, 0.0873, 0.0866,  
	0.0859,
};
double Q390A_STABLE_COEF[251]=
{
	1.0000,  1.0000,  1.0000,  0.9990,  0.9990,  0.9980,  0.9970,  0.9960,  0.9950,  0.9940,  
	0.9920,  0.9900,  9.9890,  0.9870,  0.9850,  0.9820,  0.9800,  0.9780,  0.9750,  0.9730,  
	0.9710,  0.9680,  0.9660,  0.9630,  0.9610,  0.9580,  0.9560,  0.9530,  0.9500,  0.9470,  
	0.9450,  0.9420,  0.9390,  0.9360,  0.9320,  0.9290,  0.9260,  0.9220,  0.9190,  0.9150,  
	0.9110,  0.9070,  0.9030,  0.8990,  0.8950,  0.8900,  0.8850,  0.8810,  0.8760,  0.8700,  
	0.8650,  0.8600,  0.8540,  0.8480,  0.8420,  0.8350,  0.8280,  0.8220,  0.8140,  0.8070,  
	0.8000,  0.7920,  0.7840,  0.7750,  0.7670,  0.7580,  0.7490,  0.7400,  0.7310,  0.7220,  
	0.7120,  0.7030,  0.6930,  0.6830,  0.6740,  0.6640,  0.6540,  0.6440,  0.6340,  0.6240,  
	0.6140,  0.6040,  0.5950,  0.5850,  0.5760,  0.5660,  0.5570,  0.5480,  0.5390,  0.5300,  
	0.5210,  0.5120,  0.5040,  0.4950,  0.4870,  0.4790,  0.4710,  0.4630,  0.4550,  0.4480,  
	0.4410,  0.4330,  0.4260,  0.4190,  0.4130,  0.4060,  0.3990,  0.3930,  0.3870,  0.3810,  
	0.3750,  0.3690,  0.3630,  0.3570,  0.3520,  0.3470,  0.3410,  0.3360,  0.3310,  0.3260,  
	0.3210,  0.3160,  0.3120,  0.3070,  0.3030,  0.2980,  0.2940,  0.2900,  0.2860,  0.2820,  
	0.2780,  0.2740,  0.2700,  0.2660,  0.2630,  0.2590,  0.2560,  0.2520,  0.2490,  0.2450,  
	0.2420,  0.2390,  0.2360,  0.2330,  0.2300,  0.2270,  0.2240,  0.2210,  0.2180,  0.2150,  
	0.2130,  0.2100,  0.2080,  0.2050,  0.2030,  0.2000,  0.1980,  0.1950,  0.1930,  0.1910,  
	0.1880,  0.1860,  0.1840,  0.1820,  0.1800,  0.1780,  0.1760,  0.1740,  0.1720,  0.1700,  
	0.1680,  0.1660,  0.1640,  0.1620,  0.1610,  0.1590,  0.1570,  0.1550,  0.1540,  0.1520,  
	0.1500,  0.1490,  0.1470,  0.1460,  0.1440,  0.1430,  0.1410,  0.1400,  0.1380,  0.1370,  
	0.1360,  0.1340,  0.1330,  0.1320,  0.1300,  0.1290,  0.1280,  0.1270,  0.1260,  0.1240,  
	0.1230,  0.1220,  0.1210,  0.1190,  0.1180,  0.1170,  0.1160,  0.1150,  0.1140,  0.1130,  
	0.1120,  0.1110,  0.1100,  0.1090,  0.1080,  0.1070,  0.1060,  0.1050,  0.1040,  0.1030,  
	0.1020,  0.1010,  0.1000,  0.0996,  0.0987,  0.0970,  0.0962,  0.0954,  0.0954,  0.0946,  
	0.0938,  0.0930,  0.0922,  0.0914,  0.0907,  0.0899,  0.0892,  0.0884,  0.0877,  0.0870,  
	0.0863,  0.0856,  0.0849,  0.0842,  0.0836,  0.0829,  0.0823,  0.0816,  0.0810,  0.0803,  
	0.0797,
};
double Q390B_STABLE_COEF[251]=
{
	1.0000, 1.0000, 1.0000, 0.9990, 0.9980, 0.9970, 0.9960, 0.9940, 0.9920, 0.9900, 
	0.9880, 0.9850, 0.9820, 0.9790, 0.9760, 0.9720, 0.9680, 0.9640, 0.9590, 0.9550, 
	0.9510, 0.9460, 0.9420, 0.9370, 0.9330, 0.9280, 0.9230, 0.9190, 0.9140, 0.9090, 
	0.9040, 0.8990, 0.8940, 0.8890, 0.8830, 0.8780, 0.8720, 0.8670, 0.8610, 0.8550, 
	0.8490, 0.8430, 0.8370, 0.8310, 0.8240, 0.8180, 0.8110, 0.8040, 0.7970, 0.7900, 
	0.7830, 0.7760, 0.7690, 0.7610, 0.7530, 0.7460, 0.7380, 0.7300, 0.7220, 0.7140, 
	0.7050, 0.6970, 0.6890, 0.6800, 0.6720, 0.6630, 0.6540, 0.6460, 0.6370, 0.6290, 
	0.6200, 0.6110, 0.6030, 0.5940, 0.5860, 0.5770, 0.5690, 0.5600, 0.5520, 0.5440, 
	0.5350, 0.5270, 0.5190, 0.5110, 0.5040, 0.4960, 0.4880, 0.4810, 0.4730, 0.4660, 
	0.4590, 0.4520, 0.4450, 0.4380, 0.4310, 0.4240, 0.4180, 0.4110, 0.4050, 0.3990, 
	0.3930, 0.3870, 0.3810, 0.3750, 0.3700, 0.3640, 0.3590, 0.3530, 0.3480, 0.3430, 
	0.3380, 0.3330, 0.3280, 0.3230, 0.3190, 0.3140, 0.3100, 0.3050, 0.3010, 0.2970, 
	0.2930, 0.2890, 0.2850, 0.2810, 0.2770, 0.2730, 0.2690, 0.2660, 0.2620, 0.2590, 
	0.2550, 0.2520, 0.2480, 0.2450, 0.2420, 0.2390, 0.2360, 0.2330, 0.2300, 0.2270, 
	0.2240, 0.2210, 0.2180, 0.2160, 0.2130, 0.2100, 0.2080, 0.2050, 0.2030, 0.2000, 
	0.1980, 0.1960, 0.1930, 0.1910, 0.1890, 0.1870, 0.1840, 0.1820, 0.1800, 0.1780, 
	0.1760, 0.1740, 0.1720, 0.1700, 0.1680, 0.1670, 0.1650, 0.1630, 0.1610, 0.1590, 
	0.1580, 0.1560, 0.1540, 0.1530, 0.1510, 0.1500, 0.1480, 0.1460, 0.1450, 0.1430, 
	0.1420, 0.1400, 0.1390, 0.1380, 0.1360, 0.1350, 0.1340, 0.1320, 0.1310, 0.1300, 
	0.1280, 0.1270, 0.1260, 0.1250, 0.1230, 0.1220, 0.1210, 0.1200, 0.1190, 0.1180, 
	0.1170, 0.1160, 0.1140, 0.1130, 0.1120, 0.1110, 0.1100, 0.1090, 0.1080, 0.1070, 
	0.1060, 0.1050, 0.1050, 0.1040, 0.1030, 0.1020, 0.1010, 0.1000, 0.0992, 0.0983, 
	0.0975, 0.0967, 0.0958, 0.0950, 0.0942, 0.0934, 0.0926, 0.0919, 0.0911, 0.0904, 
	0.0896, 0.0889, 0.0882, 0.0874, 0.0867, 0.0860, 0.0853, 0.0847, 0.0840, 0.0833, 
	0.0827, 0.0820, 0.0814, 0.0807, 0.0801, 0.0795, 0.0789, 0.0783, 0.0777, 0.0771, 
	0.0765,									
};
double Q420A_STABLE_COEF[251]=
{
	1.000,1.000,1.000,0.999,0.999,0.998,0.997,0.996,0.995,0.993,
	0.992,0.990,0.988,0.986,0.983,0.981,0.978,0.976,0.974,0.971,
	0.969,0.966,0.964,0.961,0.958,0.956,0.953,0.950,0.947,0.944,
	0.941,0.938,0.935,0.932,0.928,0.925,0.921,0.917,0.913,0.909,
	0.905,0.901,0.896,0.892,0.887,0.882,0.877,0.872,0.866,0.860,
	0.854,0.848,0.842,0.835,0.828,0.821,0.814,0.806,0.798,0.790,
	0.782,0.773,0.764,0.755,0.746,0.736,0.727,0.717,0.707,0.697,
	0.687,0.677,0.666,0.656,0.646,0.636,0.625,0.615,0.605,0.595,
	0.585,0.575,0.565,0.556,0.546,0.537,0.527,0.518,0.509,0.501,
	0.492,0.483,0.475,0.467,0.459,0.451,0.443,0.436,0.428,0.421,
	0.414,0.407,0.400,0.394,0.387,0.381,0.375,0.369,0.363,0.357,
	0.351,0.346,0.340,0.335,0.329,0.324,0.319,0.314,0.310,0.305,
	0.300,0.296,0.291,0.287,0.283,0.279,0.275,0.271,0.267,0.263,
	0.259,0.256,0.252,0.249,0.245,0.242,0.239,0.235,0.232,0.229,
	0.226,0.223,0.220,0.217,0.214,0.212,0.209,0.206,0.204,0.201,
	0.198,0.196,0.194,0.191,0.189,0.186,0.184,0.182,0.180,0.178,
	0.176,0.174,0.171,0.169,0.168,0.166,0.164,0.162,0.160,0.158,
	0.156,0.155,0.153,0.151,0.150,0.148,0.146,0.145,0.143,0.142,
	0.140,0.139,0.137,0.136,0.134,0.133,0.132,0.130,0.129,0.128,
	0.126,0.125,0.124,0.123,0.121,0.120,0.119,0.118,0.117,0.116,
	0.114,0.113,0.112,0.111,0.110,0.109,0.108,0.107,0.106,0.105,
	0.104,0.103,0.102,0.101,0.100,0.099,0.099,0.098,0.097,0.096,
	0.095,0.094,0.093,0.093,0.092,0.091,0.090,0.089,0.089,0.088,
	0.087,0.087,0.086,0.085,0.084,0.084,0.083,0.082,0.082,0.081,
	0.080,0.080,0.079,0.078,0.078,0.077,0.077,0.076,0.075,0.075,
	0.074,
};
double Q420B_STABLE_COEF[251]=
{
	1.000,1.000,0.999,0.999,0.998,0.997,0.995,0.993,0.991,0.989,
	0.987,0.984,0.981,0.977,0.974,0.970,0.965,0.961,0.956,0.952,
	0.947,0.943,0.938,0.933,0.929,0.924,0.919,0.914,0.909,0.903,
	0.898,0.893,0.887,0.882,0.876,0.871,0.865,0.859,0.853,0.846,
	0.840,0.834,0.827,0.820,0.813,0.806,0.799,0.792,0.785,0.777,
	0.769,0.762,0.754,0.746,0.737,0.729,0.721,0.712,0.704,0.695,
	0.686,0.678,0.669,0.660,0.651,0.642,0.633,0.624,0.615,0.606,
	0.597,0.588,0.579,0.571,0.562,0.553,0.545,0.536,0.528,0.519,
	0.511,0.503,0.495,0.487,0.479,0.472,0.464,0.457,0.449,0.442,
	0.435,0.428,0.421,0.415,0.408,0.402,0.395,0.389,0.383,0.377,
	0.371,0.365,0.360,0.354,0.349,0.343,0.338,0.333,0.328,0.323,
	0.318,0.313,0.309,0.304,0.300,0.295,0.291,0.287,0.283,0.279,
	0.275,0.271,0.267,0.263,0.260,0.256,0.253,0.249,0.246,0.242,
	0.239,0.236,0.233,0.230,0.227,0.224,0.221,0.218,0.215,0.212,
	0.210,0.207,0.204,0.202,0.199,0.197,0.194,0.192,0.190,0.187,
	0.185,0.183,0.181,0.179,0.177,0.175,0.172,0.170,0.169,0.167,
	0.165,0.163,0.161,0.159,0.157,0.156,0.154,0.152,0.151,0.149,
	0.147,0.146,0.144,0.143,0.141,0.140,0.138,0.137,0.135,0.134,
	0.132,0.131,0.130,0.128,0.127,0.126,0.125,0.123,0.122,0.121,
	0.120,0.119,0.117,0.116,0.115,0.114,0.113,0.112,0.111,0.110,
	0.109,0.108,0.107,0.106,0.105,0.104,0.103,0.102,0.101,0.100,
	0.099,0.098,0.097,0.097,0.096,0.095,0.094,0.093,0.092,0.092,
	0.091,0.090,0.089,0.089,0.088,0.087,0.086,0.086,0.085,0.084,
	0.084,0.083,0.082,0.081,0.081,0.080,0.080,0.079,0.078,0.078,
	0.077,0.076,0.076,0.075,0.075,0.074,0.073,0.073,0.072,0.072,
	0.071,
};
double Q460A_STABLE_COEF[251]=
{
	1.000,1.000,1.000,0.999,0.999,0.998,0.997,0.995,0.994,0.992,
	0.991,0.989,0.987,0.984,0.982,0.979,0.977,0.974,0.972,0.969,
	0.966,0.964,0.961,0.958,0.955,0.952,0.950,0.946,0.943,0.940,
	0.937,0.933,0.930,0.926,0.923,0.919,0.915,0.910,0.906,0.902,
	0.897,0.892,0.887,0.882,0.877,0.871,0.865,0.859,0.853,0.846,
	0.840,0.833,0.825,0.818,0.810,0.802,0.793,0.784,0.776,0.766,
	0.757,0.747,0.737,0.727,0.717,0.707,0.696,0.686,0.675,0.664,
	0.654,0.643,0.632,0.621,0.611,0.600,0.590,0.579,0.569,0.559,
	0.549,0.539,0.529,0.520,0.510,0.501,0.492,0.483,0.474,0.466,
	0.457,0.449,0.441,0.433,0.426,0.418,0.411,0.404,0.397,0.390,
	0.383,0.377,0.370,0.364,0.358,0.352,0.346,0.340,0.335,0.329,
	0.324,0.319,0.313,0.308,0.304,0.299,0.294,0.290,0.285,0.281,
	0.276,0.272,0.268,0.264,0.260,0.256,0.253,0.249,0.245,0.242,
	0.238,0.235,0.232,0.228,0.225,0.222,0.219,0.216,0.213,0.210,
	0.207,0.205,0.202,0.199,0.197,0.194,0.192,0.189,0.187,0.184,
	0.182,0.180,0.178,0.175,0.173,0.171,0.169,0.167,0.165,0.163,
	0.161,0.159,0.157,0.155,0.154,0.152,0.150,0.148,0.147,0.145,
	0.143,0.142,0.140,0.139,0.137,0.136,0.134,0.133,0.131,0.130,
	0.128,0.127,0.126,0.124,0.123,0.122,0.121,0.119,0.118,0.117,
	0.116,0.115,0.113,0.112,0.111,0.110,0.109,0.108,0.107,0.106,
	0.105,0.104,0.103,0.102,0.101,0.100,0.099,0.098,0.097,0.096,
	0.095,0.094,0.094,0.093,0.092,0.091,0.090,0.089,0.089,0.088,
	0.087,0.086,0.086,0.085,0.084,0.083,0.083,0.082,0.081,0.081,
	0.080,0.079,0.079,0.078,0.077,0.077,0.076,0.075,0.075,0.074,
	0.073,0.073,0.072,0.072,0.071,0.071,0.070,0.069,0.069,0.068,
	0.068,
};
double Q460B_STABLE_COEF[251]=
{
	1.000,1.000,0.999,0.999,0.998,0.996,0.995,0.993,0.991,0.988,
	0.985,0.982,0.979,0.975,0.971,0.967,0.962,0.957,0.953,0.948,
	0.943,0.938,0.933,0.928,0.923,0.918,0.913,0.907,0.902,0.896,
	0.891,0.885,0.879,0.873,0.867,0.861,0.855,0.848,0.842,0.835,
	0.828,0.821,0.814,0.806,0.799,0.791,0.784,0.776,0.768,0.759,
	0.751,0.743,0.734,0.725,0.716,0.708,0.699,0.689,0.680,0.671,
	0.662,0.652,0.643,0.634,0.624,0.615,0.605,0.596,0.587,0.578,
	0.568,0.559,0.550,0.541,0.532,0.524,0.515,0.506,0.498,0.490,
	0.482,0.473,0.466,0.458,0.450,0.442,0.435,0.428,0.421,0.414,
	0.407,0.400,0.393,0.387,0.381,0.374,0.368,0.362,0.356,0.351,
	0.345,0.340,0.334,0.329,0.324,0.319,0.314,0.309,0.304,0.299,
	0.295,0.290,0.286,0.282,0.277,0.273,0.269,0.265,0.262,0.258,
	0.254,0.250,0.247,0.243,0.240,0.237,0.233,0.230,0.227,0.224,
	0.221,0.218,0.215,0.212,0.209,0.206,0.204,0.201,0.198,0.196,
	0.193,0.191,0.188,0.186,0.184,0.181,0.179,0.177,0.175,0.173,
	0.171,0.168,0.166,0.164,0.163,0.161,0.159,0.157,0.155,0.153,
	0.151,0.150,0.148,0.146,0.145,0.143,0.142,0.140,0.138,0.137,
	0.135,0.134,0.132,0.131,0.130,0.128,0.127,0.126,0.124,0.123,
	0.122,0.120,0.119,0.118,0.117,0.116,0.114,0.113,0.112,0.111,
	0.110,0.109,0.108,0.107,0.106,0.105,0.104,0.103,0.102,0.101,
	0.100,0.099,0.098,0.097,0.096,0.095,0.094,0.094,0.093,0.092,
	0.091,0.090,0.089,0.089,0.088,0.087,0.086,0.086,0.085,0.084,
	0.083,0.083,0.082,0.081,0.081,0.080,0.079,0.079,0.078,0.077,
	0.077,0.076,0.075,0.075,0.074,0.074,0.073,0.072,0.072,0.071,
	0.071,0.070,0.070,0.069,0.068,0.068,0.067,0.067,0.066,0.066,
	0.065,
};
//中国规范压稳系数函数的原始计算公式，现暂时不用 wjh-2013.5.30
static double DL_T_RawCalStableCoef(double fy,double lamda,char sectionType)
{
	double lamda_=lamda/Pi*sqrt(fy/206000);
	double lamda_2=lamda_*lamda_;
	double alfa1=0.41;
	double alfa2=0.986;
	double alfa3=0.152;
	if(sectionType!='A')
	{
		alfa1=0.65;
		alfa2=0.965;
		alfa3=0.3;
	}
	double fai;
	if(lamda_<=0.215)
		fai=1-alfa1*lamda_2;
	else
	{
		double expr=alfa2+alfa3*lamda_+lamda_2;
		fai=0.5*(expr-sqrt(expr*expr-4*lamda_2))/lamda_2;
	}
	return fai;
}
//中国规范
//K为轴心受压杆件长细比修正系数，lamda为长细比，fy材料屈服强度(MPa)
double DL_T_2002::CompStableCoef(char cSectionType,double Klamda,STEELMAT *pMatType,double t/*=10*/)
{
	double fai=0;
	int index=min((int)Klamda,249);
	double coef=Klamda-index;
	if(Klamda<EPS)
		pLogErr->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"有单元的长细比为0，数据异常");
	if(cSectionType=='A')
	{
		if(pMatType->cBriefMark==CSteelMatLibrary::Q235BriefMark())
			fai=Q235A_STABLE_COEF[index]+coef*(Q235A_STABLE_COEF[index+1]-Q235A_STABLE_COEF[index]);
		else if(pMatType->cBriefMark==CSteelMatLibrary::Q345BriefMark())//'H')	//Q345	
			fai=Q345A_STABLE_COEF[index]+coef*(Q345A_STABLE_COEF[index+1]-Q345A_STABLE_COEF[index]);
		else if(pMatType->cBriefMark==CSteelMatLibrary::Q390BriefMark())//'G')	//Q390	
			fai=Q390A_STABLE_COEF[index]+coef*(Q390A_STABLE_COEF[index+1]-Q390A_STABLE_COEF[index]);
		else if(pMatType->cBriefMark==CSteelMatLibrary::Q420BriefMark())//'P')	//Q420	
			fai=Q420A_STABLE_COEF[index]+coef*(Q420A_STABLE_COEF[index+1]-Q420A_STABLE_COEF[index]);
		else if(pMatType->cBriefMark==CSteelMatLibrary::Q460BriefMark())//'T')	//Q460	
			fai=Q460A_STABLE_COEF[index]+coef*(Q460A_STABLE_COEF[index+1]-Q460A_STABLE_COEF[index]);
	}
	else if(cSectionType=='B')
	{
		if(coef>1)
			coef=1;
		if(pMatType->cBriefMark==CSteelMatLibrary::Q235BriefMark())//'S')	//Q235
			fai=Q235B_STABLE_COEF[index]+coef*(Q235B_STABLE_COEF[index+1]-Q235B_STABLE_COEF[index]);
		else if(pMatType->cBriefMark==CSteelMatLibrary::Q345BriefMark())//'H')	//Q345	
			fai=Q345B_STABLE_COEF[index]+coef*(Q345B_STABLE_COEF[index+1]-Q345B_STABLE_COEF[index]);
		else if(pMatType->cBriefMark==CSteelMatLibrary::Q390BriefMark())//'G')	//Q390	
			fai=Q390B_STABLE_COEF[index]+coef*(Q390B_STABLE_COEF[index+1]-Q390B_STABLE_COEF[index]);
		else if(pMatType->cBriefMark==CSteelMatLibrary::Q420BriefMark())//'P')	//Q420	
			fai=Q420B_STABLE_COEF[index]+coef*(Q420B_STABLE_COEF[index+1]-Q420B_STABLE_COEF[index]);
		else if(pMatType->cBriefMark==CSteelMatLibrary::Q460BriefMark())//'T')	//Q460	
			fai=Q460B_STABLE_COEF[index]+coef*(Q460B_STABLE_COEF[index+1]-Q460B_STABLE_COEF[index]);
	}
	return fai;
}
//DL_T_2002::CompStableCoef()函数的原始计算公式
double RawCalStableCoef(double fy,double lamda,char sectionType)
{
	double lamda_=lamda/Pi*sqrt(fy/206000);
	double lamda_2=lamda_*lamda_;
	double alfa1=0.41;
	double alfa2=0.986;
	double alfa3=0.152;
	if(sectionType!='A')
	{
		alfa1=0.65;
		alfa2=0.965;
		alfa3=0.3;
	}
	double fai;
	if(lamda_<=0.215)
		fai=1-alfa1*lamda_2;
	else
	{
		double expr=alfa2+alfa3*lamda_+lamda_2;
		fai=0.5*(expr-sqrt(expr*expr-4*lamda_2))/lamda_2;
	}
	return fai;
}
double DL_T_2012::CompStableCoef(char cSectionType,double Klamda,STEELMAT *pMatType,double t/*=10*/)
{
	double fy=pMatType->fy(t);
	double fai=0;
	Klamda*=sqrt(fy/235);
	if(Klamda>250)
		return RawCalStableCoef(235,Klamda,cSectionType);
	if(Klamda<EPS)
		pLogErr->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"有单元的长细比为0，数据异常");
	int index=min((int)Klamda,249);
	double coef=min(1.0,Klamda-index);
	if(cSectionType=='A')
		fai=Q235A_STABLE_COEF[index]+coef*(Q235A_STABLE_COEF[index+1]-Q235A_STABLE_COEF[index]);
	else if(cSectionType=='B')
		fai=Q235B_STABLE_COEF[index]+coef*(Q235B_STABLE_COEF[index+1]-Q235B_STABLE_COEF[index]);
	return fai;
}
double ASCE10_97::CompStableCoef(char cSectionType,double Klamda,STEELMAT *pMatType,double t/*=10*/)
{
	double fy=pMatType->fy(t);
	double fa,Cc;	//受压稳定强度，受压稳定长细比临界值
	Cc=Pi*sqrt(2*IMemberDesignCode::modulusE/fy);
	if(Klamda<EPS)
		pLogErr->LevelLog(CLogFile::WARNING_LEVEL1_IMPORTANT,"有单元的长细比为0，数据异常");
	if(Klamda<=Cc)
		fa=(1-0.5*pow(Klamda/Cc,2))*fy;
	else//输入异常（Klamda=0)时避免除零压稳系数取1.0 wjh-2017.2.22
		fa=Klamda<EPS ? fy : Pi*Pi*IMemberDesignCode::modulusE/(Klamda*Klamda);
	return fa/fy;
}

static double COMPRESSION_ZERO_FORCE=-0.1;	//N
//根据交叉斜材内力及布材型式，返回斜材长细比计算参数,N0为绝对值较小内力的代数值
IMemberDesignCode::BevelRodLamdaPara DL_T_2002::IntersBevelRodLamdaCal(double N,double N0,double ratioL2toL3/*=0*/)
{
	double Na=fabs(N),N0a=fabs(N0);
	if(Na<N0a)
	{
		Na=fabs(N0);
		N0a=fabs(N);
	}
	IMemberDesignCode::BevelRodLamdaPara rslt;
	double threshold=0.2*Na;
	if(N0<COMPRESSION_ZERO_FORCE)
	{
		rslt.K=0.8;
		rslt.cType='C';
	}
	else
	{
		rslt.K=1.1;
		rslt.cType=0;
	}
	return rslt;
}
///////////////////////////////////////////////////////////////////////
//拉弯构件计算，返回值为功效系数，参数单位：N(N),A(mm^2),M(N*m),W(cm^3),f(MPa)；
double IMemberDesignCode::BendingTensionRodCheck(double m,double N,double An,double M,double W,double f)
{
	double effic=N/(m*An)+M/W;
	return effic/f;
}
//受弯构件计算，返回值为功效系数；f为杆件材料的强度设计值,Do>0&&t>0时表示为钢管构件的外径和壁厚
double IMemberDesignCode::BendingRodCheck(double Mx,double Wx,double My,double Wy,double f,double Do/*=0*/,double t/*=0*/,bool isTube/*=false*/)
{
	double mM=1.0;
	if(isTube)
	{
		double Do2t=Do/t;
		if(Do2t>38060/f)//&&Do2t<=76130/f
			mM = 0.7+11410/(Do2t*f);
	}
	return (Mx/Wx+My/Wy)/(mM*f);
}
//压弯构件计算，返回值为功效系数，参数单位：N(N),A(mm^2),M(N*m),W(cm^3),f(MPa)；
double DL_T_2002::BendingCompRodCheck(double N,double A,double mN,double fai,double M,double W,double lamda,double f)
{
	double Nex=Pi*Pi*IMemberDesignCode::modulusE*A/(1.1*lamda*lamda);
	double effic=N/(fai*mN*A)+M/W;
	return effic/f;
}
//压弯构件计算，返回值为功效系数；
double DL_T_2012::BendingCompRodCheck(double N,double A,double mN,double fai,double M,double W,double lamda,double f)
{
	double Nex=Pi*Pi*IMemberDesignCode::modulusE*A/(1.1*lamda*lamda);
	double effic=N/(fai*mN*A)+M/(W*(1-0.8*N/Nex));
	return effic/f;
}
//压弯构件计算，返回值为功效系数；
double ASCE10_97::BendingCompRodCheck(double N,double A,double mN,double fai,double M,double W,double lamda,double f)
{
	double Nex=Pi*Pi*IMemberDesignCode::modulusE*A/(1.1*lamda*lamda);
	double effic=N/(fai*mN*A)+M/(W*(1-0.8*N/Nex));
	return effic/f;
}

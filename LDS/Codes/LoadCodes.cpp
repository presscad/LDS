// Codes.cpp : ���� DLL Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Codes.h"
#include "PartLib.h"
#include "XhCharString.h"
#include "Hashtable.h"
#include "Interpolator.h"

////////////////////////////////////////////////////////////////////////////////////////////////
static const DWORD LOADCODEHASHTABLESIZE = 100;
static CStringKeyHashTable<long> propLoadCodeHashtable;
static CHashList<double> globalLoadCodeItemValues[4];
static CHashList<double>* GetLoadCodeHashPropItems(int iDesignCode)
{
	return &globalLoadCodeItemValues[iDesignCode];
}
//�Զ������Դ洢��
CHashStrList<CODE_PROPITEM> hashCodeProp_GB50135_2006;
CHashStrList<CODE_PROPITEM> hashCodeProp_ASCE10_97;
CHashStrList<CODE_PROPITEM> hashCodeProp_DLT_2002;
CHashStrList<CODE_PROPITEM> hashCodeProp_DLT_2012;
CHashStrList<CODE_PROPITEM> hashCodeProp_DLT_2018;
static CHashStrList<CODE_PROPITEM>* CodePropHashSet(int iLoadCode)
{
	if(iLoadCode==LOAD_CODES::GB50135_2006)
		return &hashCodeProp_GB50135_2006;	//���ʽṹ��ƹ淶2006��
	else if(iLoadCode==LOAD_CODES::ASCE10_97)
		return &hashCodeProp_ASCE10_97;	//����ASCE10-97
	else if(iLoadCode==LOAD_CODES::DLT_2002)
		return &hashCodeProp_DLT_2002;	//���ڵ��������ṹ��Ƽ����涨 2002��
	else if(iLoadCode==LOAD_CODES::DLT_2012)
		return &hashCodeProp_DLT_2012;	//���ڵ��������ṹ��Ƽ����涨 2012��
	else //if(iLoadCode==LOAD_CODES::DLT_2018)
		return &hashCodeProp_DLT_2018;	//���ڼܿ������·���ع淶DLT5551-2018. 2018��
}
CODE_PROPITEM* ILoadCalCode::AddLocaleItemLong(const char* keyidstr,long liPropVal,const char* name/*=NULL*/)
{
	if(keyidstr==NULL||keyidstr[0]=='0')
		return NULL;
	CODE_PROPITEM* pItem=CodePropHashSet(liCodeSerial)->Add(keyidstr);
	StrCopy(pItem->idKeyStr,keyidstr,32);
	StrCopy(pItem->name,name,96);
	pItem->ciValType=CODE_PROPITEM::DTVALTYPE_LONG;
	pItem->liVal=liPropVal;
	return pItem;
}
CODE_PROPITEM* ILoadCalCode::AddLocaleItemDouble(const char* keyidstr,double dfPropVal,const char* name/*=NULL*/)
{
	if(keyidstr==NULL||keyidstr[0]=='0')
		return NULL;
	CODE_PROPITEM* pItem=CodePropHashSet(liCodeSerial)->Add(keyidstr);
	StrCopy(pItem->idKeyStr,keyidstr,32);
	StrCopy(pItem->name,name,96);
	pItem->ciValType=CODE_PROPITEM::DTVALTYPE_LONG;
	pItem->dfVal=dfPropVal;
	return pItem;
}

CODE_PROPITEM* ILoadCalCode::AddLocaleItemBool(const char* keyidstr,bool blPropVal,const char* name/*=NULL*/)
{
	if(keyidstr==NULL||keyidstr[0]=='0')
		return NULL;
	CODE_PROPITEM* pItem=CodePropHashSet(liCodeSerial)->Add(keyidstr);
	StrCopy(pItem->idKeyStr,keyidstr,32);
	StrCopy(pItem->name,name,96);
	pItem->ciValType=CODE_PROPITEM::DTVALTYPE_BOOL;
	pItem->bVal=blPropVal;
	return pItem;
}
CODE_PROPITEM* ILoadCalCode::AddLocaleItemStr100(const char* keyidstr,const char* pszPropVal,const char* name/*=NULL*/)
{
	if(keyidstr==NULL||keyidstr[0]=='0')
		return NULL;
	CODE_PROPITEM* pItem=CodePropHashSet(liCodeSerial)->Add(keyidstr);
	StrCopy(pItem->idKeyStr,keyidstr,32);
	StrCopy(pItem->name,name,96);
	pItem->ciValType=CODE_PROPITEM::DTVALTYPE_STR100;
	CXhString szValStr100(pItem->szStrBuff,101);
	szValStr100.Copy(pszPropVal);
	return pItem;
}
int ILoadCalCode::InitPropHashtable(int iLoadCalCode)
{
	int id = 1;
	CHashList<double>* pHashPropItems=GetLoadCodeHashPropItems(iLoadCalCode);//globalLoadCodeItemValues[iLoadCalCode];
	propLoadCodeHashtable.CreateHashTable(LOADCODEHASHTABLESIZE);
	pHashPropItems->SetHashTableGrowSize(64,false);
	//
	pHashPropItems->SetValue(id,1.2);	//�������ʱ�������ط���ϵ��
	propLoadCodeHashtable.SetValueAt("m_fGamaGForDesign",id++);
	pHashPropItems->SetValue(id,1.0);	//�������ʱ�������ط���ϵ��
	propLoadCodeHashtable.SetValueAt("m_fGamaGForDesign",id++);
	pHashPropItems->SetValue(id,1.4);	//�����������ӷ���ط���ϵ��
	propLoadCodeHashtable.SetValueAt("m_fGamaQForTowerWindLoad",id++);
	//pHashPropItems->SetValue(id,1.0);	//��ϽǸ�������ض�����״ϵ���ļ��뷽ʽ��true��ɢ���룬false�������
	//propLoadCodeHashtable.SetValueAt("DistributeK1",id++);
	//if(iLoadCalCode==1)
	//	;���ݲ�ͬ�淶�ֱ�ֵ
	//else if(iLoadCalCode==2)
	//	;���ݲ�ͬ�淶�ֱ�ֵ

	return id;
}
LOADCOAD_DL_T_2012 loadcode_dlt_2012;
LOADCOAD_DL_T_2018 loadcode_dlt_2018;
LOADCOAD_ASCE10_97 loadcode_asce10_97;
//static ILoadCalCode* ILoadCalCode::CODE_DLT_2002();
ILoadCalCode* ILoadCalCode::CODE_DLT_2012() { return &loadcode_dlt_2012; }
ILoadCalCode* ILoadCalCode::CODE_DLT_2018() { return &loadcode_dlt_2018; }
ILoadCalCode* ILoadCalCode::CODE_ASCE10_97() { return &loadcode_asce10_97; }
ILoadCalCode* ILoadCalCode::FromCodeSerial(int codeSerial)
{
	CODES code=codeSerial;
	if (code.serial == LOAD_CODES::ASCE10_97)
		return CODE_ASCE10_97();
	else if (code.serial == LOAD_CODES::DLT_2012)
		return CODE_DLT_2012();
	else //if(code.serial==LOAD_CODES::DLT_2018)
		return CODE_DLT_2018();
}
bool ILoadCalCode::GetSupportCodesStr(char* codesStr,int maxStrBufLength)
{
	CXhChar500 codes_str;
	codes_str.Copy(CODE_DLT_2012()->CodeName());
	codes_str.Append(CODE_ASCE10_97()->CodeName(),'|');
	StrCopy(codesStr,codes_str,maxStrBufLength);
	return strlen(codesStr)>0;
}
//�淶��ָ����Ĳ���ֵ���絥֫������������ǿ���ۼ�ϵ��
static double GetItemValueInteral(long idItemProp,int iLoadCodeCode)
{
	if(iLoadCodeCode<0||iLoadCodeCode>=4)
		return 0;
	CHashList<double>* pHashPropItems=GetLoadCodeHashPropItems(iLoadCodeCode);
	double *pVal=pHashPropItems->GetValue(idItemProp);
	if(pVal)
		return *pVal;
	else
		return 0;
}
long ILoadCalCode::GetPropId(const char* itemKeyStr)
{
	long prop_id=0;
	if(propLoadCodeHashtable.GetValueAt(itemKeyStr,prop_id))
		return prop_id;
	else
		return 0;
}
double ILoadCalCode::GetItemValue(const char* szItemKeyStr)
{
	long prop_id=0;
	if (!propLoadCodeHashtable.GetValueAt(szItemKeyStr,prop_id))
		return 0;
	return GetItemValue(prop_id);
}
CODE_PROPITEM* ILoadCalCode::GetCodeItem(const char* keyidstr)
{
	return CodePropHashSet(liCodeSerial)->GetValue(keyidstr);
}
	//��������ID�õ�����ֵ�ַ���
int ILoadCalCode::GetItemValueStr(long idItemProp,char* valueStr)
{
	double fVal=GetItemValue(idItemProp);
	if(fVal<0)
		fVal=GetItemValue(idItemProp);
	CXhChar16 value_str("%f",fVal);
	if(valueStr)
		strcpy(valueStr,value_str);
	return value_str.Length();
}
//-----------------------VVV--��׼��ѹ�߶ȱ仯ϵ�����ѯ--VVV-------------------------
//Ӧ��Ϊ�����涨�еķ�ѹ�߶ȱ仯ϵ����
//��׼��ѹ�߶ȱ仯ϵ��mu_z��ժ�ԡ��͵���·�����涨��DL/T 5154-2002��
//[...][0]:�߶ȣ�[...][i](i>0):��Ӧ����ֲڶ����ķ�ѹ�߶ȱ仯ϵ��
//'A'��ָ��������ͺ���\����\������ɳĮ����;
//'B'��ָ��Ұ\���\����\�����Լ����ݱȽ�ϡ�������ͳ��н���;
//'C'��ָ���ܼ�����Ⱥ�ĳ�������
double mu_z_0KV_arr[19][5]=	//���ԡ������ṹ���ع淶��GB50009-2001
{
	  5,1.17,1.00,0.74,0.62,
	 10,1.38,1.00,0.74,0.62,
	 15,1.52,1.14,0.74,0.62,
	 20,1.63,1.25,0.84,0.62,
	 30,1.80,1.42,1.00,0.62,
	 40,1.92,1.56,1.13,0.73,
	 50,2.03,1.67,1.25,0.84,
	 60,2.12,1.77,1.35,0.93,
	 70,2.20,1.86,1.45,1.02,
	 80,2.27,1.95,1.54,1.11,
	 90,2.34,2.02,1.62,1.19,
	100,2.40,2.09,1.70,1.27,
	150,2.64,2.38,2.03,1.61,
	200,2.83,2.61,2.30,1.92,
	250,2.99,2.80,2.54,2.19,
	300,3.12,2.97,2.75,2.45,
	350,3.12,3.12,2.94,2.68,
	400,3.12,3.12,3.12,2.91,
	450,3.12,3.12,3.12,3.12,
};
double mu_z_330KV_arr[17][4]=
{
	 10,1.21,0.88,0.63,
	 15,1.34,1.00,0.74,
	 20,1.43,1.10,0.83,
	 30,1.58,1.25,0.97,
	 40,1.69,1.37,1.09,
	 50,1.78,1.47,1.19,
	 60,1.86,1.56,1.28,
	 70,1.93,1.64,1.36,
	 80,2.00,1.71,1.44,
	 90,2.05,1.77,1.51,
	100,2.10,1.84,1.57,
	150,2.32,2.09,1.85,
	200,2.49,2.29,2.08,
	250,2.62,2.46,2.27,
	300,2.74,2.61,2.44,
	350,2.74,2.74,2.60,
	400,2.74,2.74,2.74,
};
double mu_z_500KV_arr[17][4]=
{
	 10,1.11,0.80,0.57,
	 15,1.22,0.91,0.67,
	 20,1.31,1.00,0.75,
	 30,1.44,1.14,0.89,
	 40,1.54,1.25,0.99,
	 50,1.63,1.34,1.09,
	 60,1.70,1.42,1.17,
	 70,1.76,1.49,1.24,
	 80,1.82,1.56,1.31,
	 90,1.87,1.62,1.38,
	100,1.92,1.67,1.44,
	150,2.12,1.91,1.69,
	200,2.27,2.09,1.89,
	250,2.39,2.24,2.07,
	300,2.50,2.38,2.23,
	350,2.50,2.50,2.37,
	400,2.50,2.50,2.50,
};
//���ݸ߶Ⱥ͵���ֲڶ�(�ؿ�)���ұ�׼��ѹ�߶ȱ仯ϵ��mu_z
//high����������ƽ��߶�(m), cLandEnv�������ֲڶ����
double ILoadCalCode::Find_MuZ(double high,char cLandEnv)
{
	cLandEnv=toupper(cLandEnv);
	if(cLandEnv<'A'||cLandEnv>'D')
	{
		logerr.Log("��ǰ����ķ���ص���ֲڶ��������,ϵͳĬ��Ϊ'B'��,��ѹ�߶ȱ仯ϵ����ȡ��������");
		return 0;					//�������·��
	}

	if (liCodeSerial==LOAD_CODES::DLT_2018)	//���ս����ṹ���ع淶��
	{	//�������ṹ���ع淶��GB50009-2012
		if (cLandEnv=='A')
			return min(2.91,1.284*pow(high*0.1,0.24));
		else if (cLandEnv=='B')
			return min(2.91,1.000*pow(high*0.1,0.30));
		else if (cLandEnv=='C')
			return min(2.91,0.544*pow(high*0.1,0.44));
		else if (cLandEnv=='D')
			return min(2.91,0.262*pow(high*0.1,0.60));
		else
			return 0;
	}
	else //if(liCodeSerial==LOAD_CODES::DLT_2012)
	{	//�������ṹ���ع淶��GB50009-2001
		if (cLandEnv=='A')
			return min(2.91,1.379*pow(high*0.1,0.24));
		else if (cLandEnv=='B')
			return min(2.91,1.000*pow(high*0.1,0.32));
		else if (cLandEnv=='C')
			return min(2.91,0.616*pow(high*0.1,0.44));
		else if (cLandEnv=='D')
			return min(2.91,0.318*pow(high*0.1,0.60));
		else
			return 0;
	}
	/*if(voltage_grade==0)
	{	if(high<=mu_z_0KV_arr[0][0])
			return mu_z_0KV_arr[0][cLandEnv-'A'+1];
		else
		{
			for(int i=0;i<19;i++)
			{
				if(high==mu_z_0KV_arr[i+1][0])		//ǡ�ò鵽��������Ӧ���ߵ�mu_zֵ
					return mu_z_0KV_arr[i+1][cLandEnv-'A'+1];
				else if(high<mu_z_0KV_arr[i+1][0])	//��ֵ����mu_z
				{
					return mu_z_0KV_arr[i][cLandEnv-'A'+1]+(mu_z_0KV_arr[i+1][cLandEnv-'A'+1]-mu_z_0KV_arr[i][cLandEnv-'A'+1])*
						(high-mu_z_0KV_arr[i][0])/(mu_z_0KV_arr[i+1][0]-mu_z_0KV_arr[i][0]);
				}
			}
			return mu_z_0KV_arr[16][cLandEnv-'A'+1];	//�߶ȳ�����mu_z_arr[18][0] (400) ��
		}
	}
	else if(voltage_grade==330)
	{
		if(high<=mu_z_330KV_arr[0][0])
			return mu_z_330KV_arr[0][cLandEnv-'A'+1];
		else
		{
			for(int i=0;i<17;i++)
			{
				if(high==mu_z_330KV_arr[i+1][0])		//ǡ�ò鵽��������Ӧ���ߵ�mu_zֵ
					return mu_z_330KV_arr[i+1][cLandEnv-'A'+1];
				else if(high<mu_z_330KV_arr[i+1][0])	//��ֵ����mu_z
				{
					return mu_z_330KV_arr[i][cLandEnv-'A'+1]+(mu_z_330KV_arr[i+1][cLandEnv-'A'+1]-mu_z_330KV_arr[i][cLandEnv-'A'+1])*
						(high-mu_z_330KV_arr[i][0])/(mu_z_330KV_arr[i+1][0]-mu_z_330KV_arr[i][0]);
				}
			}
			return mu_z_330KV_arr[16][cLandEnv-'A'+1];	//�߶ȳ�����mu_z_arr[18][0] (400) ��
		}
	}
	else //if(voltage_grade==500)
	{
		if(high<=mu_z_500KV_arr[0][0])
			return mu_z_500KV_arr[0][cLandEnv-'A'+1];
		else
		{
			for(int i=0;i<17;i++)
			{
				if(high==mu_z_500KV_arr[i+1][0])		//ǡ�ò鵽��������Ӧ���ߵ�mu_zֵ
					return mu_z_500KV_arr[i+1][cLandEnv-'A'+1];
				else if(high<mu_z_500KV_arr[i+1][0])	//��ֵ����mu_z
				{
					return mu_z_500KV_arr[i][cLandEnv-'A'+1]+(mu_z_500KV_arr[i+1][cLandEnv-'A'+1]-mu_z_500KV_arr[i][cLandEnv-'A'+1])*
						(high-mu_z_500KV_arr[i][0])/(mu_z_500KV_arr[i+1][0]-mu_z_500KV_arr[i][0]);
				}
			}
			return mu_z_500KV_arr[16][cLandEnv-'A'+1];	//�߶ȳ�����mu_z_arr[18][0] (450) ��
		}
	}*/
}
double LOADCOAD_DL_T_2018::Find_MuZ(double high,char cLandEnv)
{
	//���ա������ṹ���ع淶��GB50009-2012�����ܿ������·���ع淶��DL/T5551-2018
	if(cLandEnv=='A')
		return min(2.91, 1.284*pow(high*0.1,0.24));
	else if(cLandEnv=='B')
		return min(2.91, 1.000*pow(high*0.1,0.30));
	else if(cLandEnv=='C')
		return min(2.91, 0.544*pow(high*0.1,0.44));
	else if(cLandEnv=='D')
		return min(2.91, 0.262*pow(high*0.1,0.60));
	else
		return 0;
}
double ILoadCalCode::CalMusOfTube(double dfMuzW0D_2)
{	//dfMuzW0D_2=��z*Wo*d^2
	if (liCodeSerial==LOAD_CODES::DLT_2002)
	{
		if (dfMuzW0D_2<=0.002)
			return 1.2;
		else if (dfMuzW0D_2>=0.015)
			return 0.7;
		else
			return (0.70 + (1.2 - 0.70)*(0.015 - dfMuzW0D_2)/ 0.013);
	}
	else if (liCodeSerial==LOAD_CODES::DLT_2012||liCodeSerial==LOAD_CODES::GB50135_2006)
	{	//��ͬ�������ṹ���ع淶��GB50009-2001
		if (dfMuzW0D_2<=0.002)
			return 1.04;
		else if (dfMuzW0D_2>=0.015)
			return 0.78;
		else
			return (0.78 + (1.04 - 0.78)*(0.015 - dfMuzW0D_2)/ 0.013);
	}
	else //if (liCodeSerial==LOAD_CODES::DLT_2018)
	{	//���淶��P77ҳ����˵��:һ������¦�z*Wo*d^2>0.021,��s=0.78�����ǵ�����һЩ��������һ��ȡ0.85
		//if (dfMuzW0D_2<=0.003)
		//	return 1.04;
		//else if (dfMuzW0D_2>=0.021)
		//	return 0.78;
		//else
		//	return (0.78 + (1.04 - 0.78)*(0.021 - dfMuzW0D_2)/ 0.018);
		return 0.85;
	}
}
//�Ƕȷ�����ʱ����ط����
static double GetSumAngleWindLoadCoef(long azimuth)
{
	//���Ƕ�ת����0~90��Χ
	if(azimuth>=90)
		azimuth%=90;
	else if(azimuth<0)
	{
		azimuth%=90;
		azimuth+=90;
	}
	//��һ�����Ƕ�ת��Ϊ0~45�ȷ�Χ
	double coef=1.1+0.1*sin((azimuth*4-90)*RADTODEG_COEF);
	return coef;
}
bool ILoadCalCode::GetWindLoadAllocCoef(double *Xcoef_inWx,double *Xcoef_inWy,double *Ycoef_inWx,double *Ycoef_inWy,long azimuth,long iStyle)
{
	if(Xcoef_inWx)
		*Xcoef_inWx=0.0;
	if(Xcoef_inWy)
		*Xcoef_inWy=0.0;
	if(Ycoef_inWx)
		*Ycoef_inWx=0.0;
	if (Ycoef_inWy)
		*Ycoef_inWy=0.0;
	if(iStyle==1)	//X��ᵣ
		return GetWindLoadArmAllocCoef(Xcoef_inWy,Ycoef_inWy,azimuth,1);
	else if(iStyle==2)	//Y��ᵣ
		return GetWindLoadArmAllocCoef(Xcoef_inWx,Ycoef_inWx,azimuth,2);
	else //if(iStyle==0)
		return GetWindLoadBodyAllocCoef(Xcoef_inWx,Xcoef_inWy,Ycoef_inWx,Ycoef_inWy,azimuth);
}
bool ILoadCalCode::GetWindLoadBodyAllocCoef(
	double *Xcoef_inWx,double *Xcoef_inWy,double *Ycoef_inWx,double *Ycoef_inWy,long azimuth)
{
	azimuth=azimuth%360;
	if(azimuth>360)
		azimuth-=360;
	else if(azimuth<0)
		azimuth+=360;
	int sign_x=1,sign_y=1;
	if (azimuth>90&&azimuth<=180)
	{
		azimuth=180-azimuth;
		sign_y=-1;
	}
	else if (azimuth>180&&azimuth<=270)
	{
		azimuth-=180;
		sign_x=sign_y=-1;
	}
	else if (azimuth>270)
	{
		azimuth=360-azimuth;
		sign_x=-1;
	}
	//����ʽ
	double alfa_Wy=azimuth*RADTODEG_COEF;
	//double alfa_Wx=(aximuth-90)*RADTODEG_COEF;
	double sina_Wy=sin(alfa_Wy);
	double cosa_Wy=cos(alfa_Wy);
	//double sina_Wx=-cosa_Wy;
	//double cosa_Wx= sina_Wy;
	double coefWindLoad=GetSumAngleWindLoadCoef(azimuth);
	double coefWindLoadWy=coefWindLoad*cosa_Wy*cosa_Wy;
	double coefWindLoadWx=coefWindLoad*sina_Wy*sina_Wy;//cosa_Wx*cosa_Wx;
	*Xcoef_inWx=sign_x*coefWindLoadWx*sina_Wy;
	*Ycoef_inWx=sign_y*coefWindLoadWx*cosa_Wy;
	*Xcoef_inWy=sign_x*coefWindLoadWy*sina_Wy;
	*Ycoef_inWy=sign_y*coefWindLoadWy*cosa_Wy;
	return true;
}
bool ILoadCalCode::GetWindLoadArmAllocCoef(
	double *Xcoef_inWsc,double *Ycoef_inWsc,long azimuth,long iStyle)
{
	azimuth=azimuth%360;
	if(azimuth>360)
		azimuth-=360;
	else if(azimuth<0)
		azimuth+=360;
	int sign_x=1,sign_y=1;
	if (azimuth>90&&azimuth<=180)
	{
		azimuth=180-azimuth;
		sign_y=-1;
	}
	else if (azimuth>180&&azimuth<=270)
	{
		azimuth-=180;
		sign_x=sign_y=-1;
	}
	else if (azimuth>270)
	{
		azimuth=360-azimuth;
		sign_x=-1;
	}
	*Xcoef_inWsc=*Ycoef_inWsc=0;
	if(iStyle==1)	//X��ˮƽ�ᵣ
	{
		if (liCodeSerial!=LOAD_CODES::DLT_2018)
		{
			if (azimuth<45)
			{
				*Xcoef_inWsc=0;
				*Ycoef_inWsc=1;
			}
			else if (azimuth<=75)
			{
				*Ycoef_inWsc=0.7;
				*Xcoef_inWsc=0.4;
			}
			else //if (azimuth<=90)
			{
				*Xcoef_inWsc=0.4;
				*Ycoef_inWsc=0;
			}

		}
		else //if (CPreProcessor::ActiveLoadCode->liCodeSerial==LOAD_CODES::DLT_2018)
		{
			if (azimuth<45)
			{
				*Xcoef_inWsc=0;
				*Ycoef_inWsc=1;
			}
			else if (azimuth<=50)	//��Ӧ45�ȷ紵
			{
				*Xcoef_inWsc=0.35;
				*Ycoef_inWsc=0.7;
			}
			else if (azimuth<=75)	//��Ӧ60�ȷ紵
			{
				*Xcoef_inWsc=0.4;
				*Ycoef_inWsc=0.55;
			}
			else if (azimuth<=90)	//��Ӧ90�ȷ紵
			{
				*Xcoef_inWsc=0.45;
				*Ycoef_inWsc=0;
			}
		}
		*Xcoef_inWsc*=sign_x;
		*Ycoef_inWsc*=sign_y;
	}
	else if (iStyle==2)	//Y��ˮƽ�ᵣ
	{
		if (liCodeSerial!=LOAD_CODES::DLT_2018)
		{
			if (azimuth<15)
			{
				*Xcoef_inWsc= 0;
				*Ycoef_inWsc= 0.4;
			}
			else if (azimuth<=45)
			{
				*Xcoef_inWsc= 0.7;
				*Ycoef_inWsc= 0.4;
			}
			else //if(azimuth<=90)
			{
				*Xcoef_inWsc=1;
				*Ycoef_inWsc=0;
			}
		}
		else //if (CPreProcessor::ActiveLoadCode->liCodeSerial==LOAD_CODES::DLT_2018)
		{
			if (azimuth<15)
			{
				*Xcoef_inWsc= 0;
				*Ycoef_inWsc= 0.45;
			}
			else if (azimuth<=40)
			{
				*Xcoef_inWsc= 0.55;
				*Ycoef_inWsc= 0.40;
			}
			else if (azimuth<=45)
			{
				*Xcoef_inWsc= 0.7;
				*Ycoef_inWsc= 0.35;
			}
			else //if(azimuth<=90)
			{
				*Xcoef_inWsc=1;
				*Ycoef_inWsc=0;
			}
		}
		*Xcoef_inWsc*=sign_x;
		*Ycoef_inWsc*=sign_y;
	}
	return true;
}
#ifdef _DEBUG
static void TestWindLoadAllocCoef()
{
	ILoadCalCode* pLoadCode2012=ILoadCalCode::CODE_DLT_2012();
	ILoadCalCode* pLoadCode2018=ILoadCalCode::CODE_DLT_2018();
	logerr.Log("angle        X.WsxCoef X.WsyCoef Y.WsxCoef Y.WsyCoef X.WscCoef Y.WscCoef");
	for (int i=0;i<360;i+=5)
	{
		double X_WsxCoef,X_WsyCoef,Y_WsxCoef,Y_WsyCoef,X_WscCoef,Y_WscCoef;
		pLoadCode2012->GetWindLoadBodyAllocCoef(&X_WsxCoef,&X_WsyCoef,&Y_WsxCoef,&Y_WsyCoef,i);	//����ᵣ
		pLoadCode2012->GetWindLoadArmAllocCoef(&X_WscCoef,&Y_WscCoef,i,1);	//X��ᵣ
		logerr.Log("%3d �� 2012 %5.3f %9.3f %9.3f %9.3f %9.3f %9.3f",i,X_WsxCoef,X_WsyCoef,Y_WsxCoef,Y_WsyCoef,X_WscCoef,Y_WscCoef);
		pLoadCode2018->GetWindLoadBodyAllocCoef(&X_WsxCoef,&X_WsyCoef,&Y_WsxCoef,&Y_WsyCoef,i);	//����ᵣ
		pLoadCode2018->GetWindLoadArmAllocCoef(&X_WscCoef,&Y_WscCoef,i,1);	//X��ᵣ
		logerr.Log("       2018 %5.3f %9.3f %9.3f %9.3f %9.3f %9.3f",X_WsxCoef,X_WsyCoef,Y_WsxCoef,Y_WsyCoef,X_WscCoef,Y_WscCoef);
	}
	logerr.ShowToScreen();
	return;
}
#endif
//bool LOADCOAD_DL_T_2018::GetWindloadAllocCoef(double *Xcoef_inWx,double *Xcoef_inWy,double *Ycoef_inWx,double *Ycoef_inWy,
//	long azimuth,long iStyle)
//{
//	return false;
//}

double ILoadCalCode::GetSetupAttachWeightLoad(long volt_grade/*=500*/,char ciWireType/*='E'*/,bool bNaiZhang/*=true*/)
{	//��2012�桶�����涨��P16, wjh-2014.5.15
	double m_fSetupGa=0;
	if(ciWireType=='E')
	{	//����
		if(volt_grade<=110)
		{
			if(bNaiZhang)
				m_fSetupGa=1500;
			else
				m_fSetupGa=1000;
		}
		else //if(pRawWire->wireInfo.m_nVoltGrade<=750)
			m_fSetupGa=2000;
	}
	else if(ciWireType=='C')
	{	//����
		if(volt_grade<=110)
		{
			if(bNaiZhang)
				m_fSetupGa=2000;
			else
				m_fSetupGa=1500;
		}
		else if(volt_grade<=330)
		{
			if(bNaiZhang)
				m_fSetupGa=4500;
			else
				m_fSetupGa=3500;
		}
		else
		{
			if(bNaiZhang)
				m_fSetupGa=6000;
			else
				m_fSetupGa=4000;
		}
	}
	else if(ciWireType=='J')
	{	//����
		if(volt_grade<=110)
			m_fSetupGa=1000;
		else if(volt_grade<=330)
			m_fSetupGa=2000;
		else
			m_fSetupGa=3000;
	}
	return m_fSetupGa;
}
double ILoadCalCode::FindPercentInHeavyIceArea(long volt_grade/*=500*/,bool bNaiZhang/*=true*/)
{
	if(volt_grade>=500&&bNaiZhang)
		return 0;	//�������ٱ���
	else if(volt_grade>=500)
		return 0.20;	//�������ٱ���
	else if(volt_grade>=330&&bNaiZhang)
		return 0.15;
	else if(volt_grade>=330)
		return 0.30;
	else if(bNaiZhang)
		return 0.30;
	else
		return 0.40;
}
double ILoadCalCode::FindUnbalanceIceCoef(double ice_thick,char cWireType/*='E'*/,long volt_grade/*=500*/,bool bNaiZhang/*=true*/)
{
	if(cWireType!='E'&&cWireType!='C')
		return 0;
	if(ice_thick<=10)	//10mm����
	{	//��2012�桶�����涨��P15
		if(bNaiZhang)
			return (cWireType=='E') ? 0.4 : 0.3;
		else
			return (cWireType=='E') ? 0.2 : 0.1;
	}
	else if(ice_thick<20)	//<20mm�б���
	{
		if(bNaiZhang)
			return (cWireType=='E') ? 0.4+(ice_thick-10)*0.01 : 0.3+(ice_thick-10)*0.01;
		else
			return (cWireType=='E') ? 0.2+(ice_thick-10)*0.01 : 0.1+(ice_thick-10)*0.01;
	}
	else //if(ice_thick>=20)	//>20mm�ر���
	{
		double icediff_coef=0;
		CInterpolator<double> interp;
		double ice_thick_arr[4]={20,30,40,50};
		double diff_coef_arr[4]={0};
		if(bNaiZhang&&cWireType=='E')
		{
			double diff_coef_arr[4]={54,58,63,67};
			interp.Init(4,ice_thick_arr,diff_coef_arr);
		}
		else if(bNaiZhang&&cWireType=='C')
		{
			double diff_coef_arr[4]={42,46,50,54};
			interp.Init(4,ice_thick_arr,diff_coef_arr);
		}
		else if(cWireType=='E')
		{
			double diff_coef_arr[4]={46,50,54,58};
			interp.Init(4,ice_thick_arr,diff_coef_arr);
		}
		else if(cWireType=='C')
		{
			double diff_coef_arr[4]={25,29,33,38};
			interp.Init(4,ice_thick_arr,diff_coef_arr);
		}
		else
			return 0;
		return interp.Interpolate(ice_thick);
	}
}
double ILoadCalCode::FindBreakWireCoefLoadY(
	long wVoltGrade/*=500*/,long wFenLieNum/*=1*/,		//��ѹ�ȼ�,������
	char ciWireFuncType/*='C'*/,	//'E':����;'C':����;'O':����
	double ice_thick/*=10*/,	//�������,mm
	char ciHangWireStyle/*=1*/,	//���߷�ʽXUAN_CHUI = 0;//��������;NAI_ZHANG = 1;//���Ź���
	char ciLineRoadType/*=0*/)	//��·����,ƽ����·��������·
{
	if(ciWireFuncType!='E'&&ciWireFuncType!='C')
		return 0;
	//��2012�桶�����涨��P13
	if(ice_thick<=10)	//10mm����
	{
		if(ciWireFuncType=='E')
			return 1.0;	//����
		else if(ciWireFuncType!='C')
			return 0;	//����
		//����
		if(ciLineRoadType<=1)	//ƽ����·��������·
		{
			if(ciHangWireStyle==ILoadCalCode::NAI_ZHANG)
			{	//����
				if(wFenLieNum==1)
					return 1.0;	//������
				else //˫���ѻ����ѵ���
					return 0.7;
			}
			else
			{	//����
				if(wFenLieNum==1)
					return 0.5;	//������
				else if(wFenLieNum==2)
					return 0.25;//˫����
				else
					return 0.2;	//����ѵ���
			}
		}
		else	//ɽ����·
		{
			if(ciHangWireStyle==ILoadCalCode::NAI_ZHANG)
			{	//����
				if(wFenLieNum==1)
					return 1.0;	//������
				else //˫���ѻ����ѵ���
					return 0.7;
			}
			else
			{	//����
				if(wFenLieNum==1)
					return 0.5;	//������
				else if(wFenLieNum==2)
					return 0.3;	//˫����
				else
					return 0.25;//����ѵ���
			}
		}
	}
	else if(ice_thick<20)	//<20mm�б���
	{
		if(ciWireFuncType=='E')
			return 1.0;	//����
		else if(ciWireFuncType!='C')
			return 0;	//����
		//����
		if(ice_thick<=15)
		{	//15mm��
			if(ciHangWireStyle==ILoadCalCode::NAI_ZHANG)
			{	//����
				if(wFenLieNum==1)
					return 1.0;	//������
				else //˫���ѻ����ѵ���
					return 0.7;
			}
			else
			{	//����
				if(wFenLieNum==1)
					return 0.5;	//������
				else if(wFenLieNum==2)
					return 0.4;	//˫����
				else
					return 0.35;//����ѵ���
			}
		}
		else
		{	//20mm��
			if(ciHangWireStyle==ILoadCalCode::NAI_ZHANG)
			{	//����
				if(wFenLieNum==1)
					return 1.0;	//������
				else //˫���ѻ����ѵ���
					return 0.7;
			}
			else
			{	//����
				if(wFenLieNum==1)
					return 0.5;	//������
				else if(wFenLieNum==2)
					return 0.5;	//˫����
				else
					return 0.45;//����ѵ���
			}
		}
	}
	else //if(ice_thick>=20)	//>20mm�ر���
	{
		double icediff_coef=0;
		CInterpolator<double> interp;
		double ice_thick_arr[4]={20,30,40,50};
		if(ciHangWireStyle!=ILoadCalCode::NAI_ZHANG&&wVoltGrade>=500)
		{
			double diff_coef_arr[4]={0.70,0.80,0.90,0.100};
			interp.Init(4,ice_thick_arr,diff_coef_arr);
		}
		else if(ciHangWireStyle==ILoadCalCode::NAI_ZHANG&&wVoltGrade>=500)
		{
			double diff_coef_arr[4]={0.100,0.100,0.100,0.100};
			interp.Init(4,ice_thick_arr,diff_coef_arr);
		}
		else if(ciHangWireStyle!=ILoadCalCode::NAI_ZHANG&&wVoltGrade>=330)
		{
			double diff_coef_arr[4]={0.60,0.70,0.80,0.90};
			interp.Init(4,ice_thick_arr,diff_coef_arr);
		}
		else if(ciHangWireStyle==ILoadCalCode::NAI_ZHANG&&wVoltGrade>=330)
		{
			double diff_coef_arr[4]={0.70,0.80,0.90,0.100};
			interp.Init(4,ice_thick_arr,diff_coef_arr);
		}
		else if(ciHangWireStyle!=ILoadCalCode::NAI_ZHANG)
		{
			double diff_coef_arr[4]={0.50,0.60,0.70,0.80};
			interp.Init(4,ice_thick_arr,diff_coef_arr);
		}
		else if(ciHangWireStyle==ILoadCalCode::NAI_ZHANG)
		{
			double diff_coef_arr[4]={0.60,0.70,0.80,0.90};
			interp.Init(4,ice_thick_arr,diff_coef_arr);
		}
		else
			return 0;
		return interp.Interpolate(ice_thick);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////
//�����涨
LOADCOAD_DL_T_2012::LOADCOAD_DL_T_2012()
{
	InitPropHashtable();
}
char* LOADCOAD_DL_T_2012::CodeName()
{
	return "�����涨 DL/T 5154-2012";
}
int LOADCOAD_DL_T_2012::InitPropHashtable()
{
	return ILoadCalCode::InitPropHashtable(1);
}
double LOADCOAD_DL_T_2012::GetItemValue(long idItemProp)
{
	return GetItemValueInteral(idItemProp,1);
}
////////////////////////////////////////////////////////////////////////////////////////////////
//�����涨
LOADCOAD_DL_T_2018::LOADCOAD_DL_T_2018()
{
	InitPropHashtable();
}
char* LOADCOAD_DL_T_2018::CodeName()
{
	return "�ܿ������·���ع淶DL/T 5551-2018";
}
int LOADCOAD_DL_T_2018::InitPropHashtable()
{
	AddLocaleItemBool("DistributeK1",true);
	return ILoadCalCode::InitPropHashtable(1);
}
//double DL_T_2002::GetItemValue(long idItemProp){return GetItemValueInteral(idItemProp,0);}
double LOADCOAD_DL_T_2018::GetItemValue(long idItemProp)
{
	return GetItemValueInteral(idItemProp,1);
}
//���� ASCE 10-97
LOADCOAD_ASCE10_97::LOADCOAD_ASCE10_97()
{ 
	InitPropHashtable();
}
char* LOADCOAD_ASCE10_97::CodeName()
{
	return "���� ASCE 10-97";
}
int LOADCOAD_ASCE10_97::InitPropHashtable()
{
	return ILoadCalCode::InitPropHashtable(2);
}
double LOADCOAD_ASCE10_97::GetItemValue(long idItemProp)
{
	return GetItemValueInteral(idItemProp,2);
}

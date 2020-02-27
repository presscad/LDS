// Codes.cpp : 定义 DLL 应用程序的入口点。
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
//自定义属性存储区
CHashStrList<CODE_PROPITEM> hashCodeProp_GB50135_2006;
CHashStrList<CODE_PROPITEM> hashCodeProp_ASCE10_97;
CHashStrList<CODE_PROPITEM> hashCodeProp_DLT_2002;
CHashStrList<CODE_PROPITEM> hashCodeProp_DLT_2012;
CHashStrList<CODE_PROPITEM> hashCodeProp_DLT_2018;
static CHashStrList<CODE_PROPITEM>* CodePropHashSet(int iLoadCode)
{
	if(iLoadCode==LOAD_CODES::GB50135_2006)
		return &hashCodeProp_GB50135_2006;	//高耸结构设计规范2006版
	else if(iLoadCode==LOAD_CODES::ASCE10_97)
		return &hashCodeProp_ASCE10_97;	//美国ASCE10-97
	else if(iLoadCode==LOAD_CODES::DLT_2002)
		return &hashCodeProp_DLT_2002;	//国内电力杆塔结构设计技术规定 2002版
	else if(iLoadCode==LOAD_CODES::DLT_2012)
		return &hashCodeProp_DLT_2012;	//国内电力杆塔结构设计技术规定 2012版
	else //if(iLoadCode==LOAD_CODES::DLT_2018)
		return &hashCodeProp_DLT_2018;	//国内架空输电线路荷载规范DLT5551-2018. 2018版
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
	pHashPropItems->SetValue(id,1.2);	//杆塔设计时重力荷载分项系数
	propLoadCodeHashtable.SetValueAt("m_fGamaGForDesign",id++);
	pHashPropItems->SetValue(id,1.0);	//基础设计时重力荷载分项系数
	propLoadCodeHashtable.SetValueAt("m_fGamaGForDesign",id++);
	pHashPropItems->SetValue(id,1.4);	//杆塔本身及附加风荷载分项系数
	propLoadCodeHashtable.SetValueAt("m_fGamaQForTowerWindLoad",id++);
	//pHashPropItems->SetValue(id,1.0);	//组合角钢塔身风载断面形状系数的计入方式，true分散计入，false整体计入
	//propLoadCodeHashtable.SetValueAt("DistributeK1",id++);
	//if(iLoadCalCode==1)
	//	;根据不同规范分别赋值
	//else if(iLoadCalCode==2)
	//	;根据不同规范分别赋值

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
//规范中指定项的参数值，如单肢连接受拉构件强度折减系数
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
	//根据属性ID得到属性值字符串
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
//-----------------------VVV--标准风压高度变化系数表查询--VVV-------------------------
//应改为技术规定中的风压高度变化系数表
//标准风压高度变化系数mu_z（摘自《送电线路技术规定》DL/T 5154-2002）
//[...][0]:高度，[...][i](i>0):相应地面粗糙度类别的风压高度变化系数
//'A'类指近海海面和海岛\海岸\湖岸及沙漠地区;
//'B'类指因野\乡村\从林\丘陵以及房屋比较稀疏的乡镇和城市郊区;
//'C'类指有密集建筑群的城市市区
double mu_z_0KV_arr[19][5]=	//来自《建筑结构荷载规范》GB50009-2001
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
//根据高度和地面粗糙度(地况)查找标准风压高度变化系数mu_z
//high代表离地面或海平面高度(m), cLandEnv代表地面粗糙度类别
double ILoadCalCode::Find_MuZ(double high,char cLandEnv)
{
	cLandEnv=toupper(cLandEnv);
	if(cLandEnv<'A'||cLandEnv>'D')
	{
		logerr.Log("当前输入的风荷载地面粗糙度类别有误,系统默认为'B'型,风压高度变化系数提取可能有误");
		return 0;					//不合理的路况
	}

	if (liCodeSerial==LOAD_CODES::DLT_2018)	//按照建筑结构荷载规范算
	{	//《建筑结构荷载规范》GB50009-2012
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
	{	//《建筑结构荷载规范》GB50009-2001
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
				if(high==mu_z_0KV_arr[i+1][0])		//恰好查到表中有相应塔高的mu_z值
					return mu_z_0KV_arr[i+1][cLandEnv-'A'+1];
				else if(high<mu_z_0KV_arr[i+1][0])	//插值计算mu_z
				{
					return mu_z_0KV_arr[i][cLandEnv-'A'+1]+(mu_z_0KV_arr[i+1][cLandEnv-'A'+1]-mu_z_0KV_arr[i][cLandEnv-'A'+1])*
						(high-mu_z_0KV_arr[i][0])/(mu_z_0KV_arr[i+1][0]-mu_z_0KV_arr[i][0]);
				}
			}
			return mu_z_0KV_arr[16][cLandEnv-'A'+1];	//高度超过了mu_z_arr[18][0] (400) 米
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
				if(high==mu_z_330KV_arr[i+1][0])		//恰好查到表中有相应塔高的mu_z值
					return mu_z_330KV_arr[i+1][cLandEnv-'A'+1];
				else if(high<mu_z_330KV_arr[i+1][0])	//插值计算mu_z
				{
					return mu_z_330KV_arr[i][cLandEnv-'A'+1]+(mu_z_330KV_arr[i+1][cLandEnv-'A'+1]-mu_z_330KV_arr[i][cLandEnv-'A'+1])*
						(high-mu_z_330KV_arr[i][0])/(mu_z_330KV_arr[i+1][0]-mu_z_330KV_arr[i][0]);
				}
			}
			return mu_z_330KV_arr[16][cLandEnv-'A'+1];	//高度超过了mu_z_arr[18][0] (400) 米
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
				if(high==mu_z_500KV_arr[i+1][0])		//恰好查到表中有相应塔高的mu_z值
					return mu_z_500KV_arr[i+1][cLandEnv-'A'+1];
				else if(high<mu_z_500KV_arr[i+1][0])	//插值计算mu_z
				{
					return mu_z_500KV_arr[i][cLandEnv-'A'+1]+(mu_z_500KV_arr[i+1][cLandEnv-'A'+1]-mu_z_500KV_arr[i][cLandEnv-'A'+1])*
						(high-mu_z_500KV_arr[i][0])/(mu_z_500KV_arr[i+1][0]-mu_z_500KV_arr[i][0]);
				}
			}
			return mu_z_500KV_arr[16][cLandEnv-'A'+1];	//高度超过了mu_z_arr[18][0] (450) 米
		}
	}*/
}
double LOADCOAD_DL_T_2018::Find_MuZ(double high,char cLandEnv)
{
	//参照《建筑结构荷载规范》GB50009-2012及《架空输电线路荷载规范》DL/T5551-2018
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
{	//dfMuzW0D_2=μz*Wo*d^2
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
	{	//等同《建筑结构荷载规范》GB50009-2001
		if (dfMuzW0D_2<=0.002)
			return 1.04;
		else if (dfMuzW0D_2>=0.015)
			return 0.78;
		else
			return (0.78 + (1.04 - 0.78)*(0.015 - dfMuzW0D_2)/ 0.013);
	}
	else //if (liCodeSerial==LOAD_CODES::DLT_2018)
	{	//《规范》P77页条文说明:一般情况下μz*Wo*d^2>0.021,μs=0.78，考虑到管塔一些附属件，一般取0.85
		//if (dfMuzW0D_2<=0.003)
		//	return 1.04;
		//else if (dfMuzW0D_2>=0.021)
		//	return 0.78;
		//else
		//	return (0.78 + (1.04 - 0.78)*(0.021 - dfMuzW0D_2)/ 0.018);
		return 0.85;
	}
}
//角度风作用时风荷载分配表
static double GetSumAngleWindLoadCoef(long azimuth)
{
	//将角度转换到0~90范围
	if(azimuth>=90)
		azimuth%=90;
	else if(azimuth<0)
	{
		azimuth%=90;
		azimuth+=90;
	}
	//进一步将角度转换为0~45度范围
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
	if(iStyle==1)	//X向横担
		return GetWindLoadArmAllocCoef(Xcoef_inWy,Ycoef_inWy,azimuth,1);
	else if(iStyle==2)	//Y向横担
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
	//塔身方式
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
	if(iStyle==1)	//X向水平横担
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
			else if (azimuth<=50)	//对应45度风吹
			{
				*Xcoef_inWsc=0.35;
				*Ycoef_inWsc=0.7;
			}
			else if (azimuth<=75)	//对应60度风吹
			{
				*Xcoef_inWsc=0.4;
				*Ycoef_inWsc=0.55;
			}
			else if (azimuth<=90)	//对应90度风吹
			{
				*Xcoef_inWsc=0.45;
				*Ycoef_inWsc=0;
			}
		}
		*Xcoef_inWsc*=sign_x;
		*Ycoef_inWsc*=sign_y;
	}
	else if (iStyle==2)	//Y向水平横担
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
		pLoadCode2012->GetWindLoadBodyAllocCoef(&X_WsxCoef,&X_WsyCoef,&Y_WsxCoef,&Y_WsyCoef,i);	//塔身横担
		pLoadCode2012->GetWindLoadArmAllocCoef(&X_WscCoef,&Y_WscCoef,i,1);	//X向横担
		logerr.Log("%3d ° 2012 %5.3f %9.3f %9.3f %9.3f %9.3f %9.3f",i,X_WsxCoef,X_WsyCoef,Y_WsxCoef,Y_WsyCoef,X_WscCoef,Y_WscCoef);
		pLoadCode2018->GetWindLoadBodyAllocCoef(&X_WsxCoef,&X_WsyCoef,&Y_WsxCoef,&Y_WsyCoef,i);	//塔身横担
		pLoadCode2018->GetWindLoadArmAllocCoef(&X_WscCoef,&Y_WscCoef,i,1);	//X向横担
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
{	//见2012版《技术规定》P16, wjh-2014.5.15
	double m_fSetupGa=0;
	if(ciWireType=='E')
	{	//地线
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
	{	//导线
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
	{	//跳线
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
		return 0;	//耐张塔少冰测
	else if(volt_grade>=500)
		return 0.20;	//悬垂塔少冰侧
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
	if(ice_thick<=10)	//10mm冰区
	{	//见2012版《技术规定》P15
		if(bNaiZhang)
			return (cWireType=='E') ? 0.4 : 0.3;
		else
			return (cWireType=='E') ? 0.2 : 0.1;
	}
	else if(ice_thick<20)	//<20mm中冰区
	{
		if(bNaiZhang)
			return (cWireType=='E') ? 0.4+(ice_thick-10)*0.01 : 0.3+(ice_thick-10)*0.01;
		else
			return (cWireType=='E') ? 0.2+(ice_thick-10)*0.01 : 0.1+(ice_thick-10)*0.01;
	}
	else //if(ice_thick>=20)	//>20mm重冰区
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
	long wVoltGrade/*=500*/,long wFenLieNum/*=1*/,		//电压等级,分裂数
	char ciWireFuncType/*='C'*/,	//'E':地线;'C':导线;'O':跳线
	double ice_thick/*=10*/,	//覆冰厚度,mm
	char ciHangWireStyle/*=1*/,	//挂线方式XUAN_CHUI = 0;//悬垂挂线;NAI_ZHANG = 1;//耐张挂线
	char ciLineRoadType/*=0*/)	//线路类型,平地线路、丘陵线路
{
	if(ciWireFuncType!='E'&&ciWireFuncType!='C')
		return 0;
	//见2012版《技术规定》P13
	if(ice_thick<=10)	//10mm冰区
	{
		if(ciWireFuncType=='E')
			return 1.0;	//地线
		else if(ciWireFuncType!='C')
			return 0;	//跳线
		//导线
		if(ciLineRoadType<=1)	//平地线路、丘陵线路
		{
			if(ciHangWireStyle==ILoadCalCode::NAI_ZHANG)
			{	//耐张
				if(wFenLieNum==1)
					return 1.0;	//单导线
				else //双分裂或多分裂导线
					return 0.7;
			}
			else
			{	//悬垂
				if(wFenLieNum==1)
					return 0.5;	//单导线
				else if(wFenLieNum==2)
					return 0.25;//双分裂
				else
					return 0.2;	//多分裂导线
			}
		}
		else	//山地线路
		{
			if(ciHangWireStyle==ILoadCalCode::NAI_ZHANG)
			{	//耐张
				if(wFenLieNum==1)
					return 1.0;	//单导线
				else //双分裂或多分裂导线
					return 0.7;
			}
			else
			{	//悬垂
				if(wFenLieNum==1)
					return 0.5;	//单导线
				else if(wFenLieNum==2)
					return 0.3;	//双分裂
				else
					return 0.25;//多分裂导线
			}
		}
	}
	else if(ice_thick<20)	//<20mm中冰区
	{
		if(ciWireFuncType=='E')
			return 1.0;	//地线
		else if(ciWireFuncType!='C')
			return 0;	//跳线
		//导线
		if(ice_thick<=15)
		{	//15mm冰
			if(ciHangWireStyle==ILoadCalCode::NAI_ZHANG)
			{	//耐张
				if(wFenLieNum==1)
					return 1.0;	//单导线
				else //双分裂或多分裂导线
					return 0.7;
			}
			else
			{	//悬垂
				if(wFenLieNum==1)
					return 0.5;	//单导线
				else if(wFenLieNum==2)
					return 0.4;	//双分裂
				else
					return 0.35;//多分裂导线
			}
		}
		else
		{	//20mm冰
			if(ciHangWireStyle==ILoadCalCode::NAI_ZHANG)
			{	//耐张
				if(wFenLieNum==1)
					return 1.0;	//单导线
				else //双分裂或多分裂导线
					return 0.7;
			}
			else
			{	//悬垂
				if(wFenLieNum==1)
					return 0.5;	//单导线
				else if(wFenLieNum==2)
					return 0.5;	//双分裂
				else
					return 0.45;//多分裂导线
			}
		}
	}
	else //if(ice_thick>=20)	//>20mm重冰区
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
//杆塔规定
LOADCOAD_DL_T_2012::LOADCOAD_DL_T_2012()
{
	InitPropHashtable();
}
char* LOADCOAD_DL_T_2012::CodeName()
{
	return "杆塔规定 DL/T 5154-2012";
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
//杆塔规定
LOADCOAD_DL_T_2018::LOADCOAD_DL_T_2018()
{
	InitPropHashtable();
}
char* LOADCOAD_DL_T_2018::CodeName()
{
	return "架空输电线路荷载规范DL/T 5551-2018";
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
//美国 ASCE 10-97
LOADCOAD_ASCE10_97::LOADCOAD_ASCE10_97()
{ 
	InitPropHashtable();
}
char* LOADCOAD_ASCE10_97::CodeName()
{
	return "美国 ASCE 10-97";
}
int LOADCOAD_ASCE10_97::InitPropHashtable()
{
	return ILoadCalCode::InitPropHashtable(2);
}
double LOADCOAD_ASCE10_97::GetItemValue(long idItemProp)
{
	return GetItemValueInteral(idItemProp,2);
}

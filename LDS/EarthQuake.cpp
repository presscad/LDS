// AttachWindLoad.cpp: implementation of the CAttachWindLoad class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "math.h"
#include "Tower.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
/*
	高压输电铁塔抗震分析及验算需要用户输入以下信息
	1.铁塔前m阶自振周期及振型数据
	2.周边环境的特征周期Tg，可根据场地类型及设计地震分组从现行《高耸结构设计规范》中查表获取；
	3.水平地震影响系数最大值αmax，可根据验算地震烈度从现行《高耸结构设计规范》或其余抗震设计规范中获取；
	4.阻尼比ζ，对于钢结构一般取0.02;
	具体计算时可根据铁塔的前7阶实际振型，选择性进行：
		X主轴方向水平地震验算
		Y主轴方向水平地震验算
		竖向地震验算
		复合型地震验算（水平与竖向同时考虑）
	5.钢结构构件（不含螺栓及焊缝）承载力抗震调整系数，一般跨越塔取0.85，其余塔取0.8
	6.水平及竖向地震作用分项系数
	7.导地线张力可变荷载的分项综合系数，一般取0.5
*/

/*
	功能：计算特定振型(指定自振周期)的水平地震影响系数
	T    －结构自振周期(s);Tg－周边环境特征周期(s);
	αmax－水平地震影响系数最大值，与地震烈度相关;
	ζ   －阻尼比，对于钢结构一般取值为0.02（见《高耸结构设计规范》）;
	iCode－0:表示遵循《高耸结构设计规范》GB50135-2006及《建筑抗震设计规范》GB50011-2001;
	       1:表示遵循《构筑物抗震设计规范》GB50191-2006;
*/
double CalAlfaOfEQ(double T,double Tg,double maxAlfa,double zeta,int iCode=0)
{
	double yita2=1+(0.05-zeta)/(0.06+1.7*zeta);	//阻尼调整系数
	if(yita2<0.55) yita2=0.55;
	if(T<=0)
		return 0.45*maxAlfa;
	else if(T<=0.1)
		return (0.45+(yita2-0.45)*10*T)*maxAlfa;
	else if(T<=Tg)
		return yita2*maxAlfa;
	else
	{
		if(iCode==1)
		{	//《高耸结构设计规范》
			double yita1=0.02+(0.05-zeta)/8;			//直线下降阶段的下降斜率调整系数
			if(T>6) T=6;
			double gama=0.9+(0.05-zeta)/(0.5+5*zeta);	//衰减指数
			if(T<=5*Tg)
				return pow(Tg/T,gama)*yita2*maxAlfa;
			else //if(T<=6)
				return (yita2*pow(0.2,gama)-yita1*(T-5*Tg))*maxAlfa;
		}
		else
		{	//《电力线路设计规范》(等同于《构筑物抗震设计规范》GB50191-2006)
			if(T>7) T=7;
			if(T<=8*Tg)
				return pow(Tg/T,0.9)*yita2*maxAlfa;
			else //if(T<=7)
				return (0.154-0.01*(T-8*Tg))*yita2*maxAlfa;
		}
	}
}
/*
	功能：计算特定振型(指定自振周期)的振型参预系数γ
	m_arr[]－质点质量数组，即节点等效质量数组
	x_arr[]－质点振型位移数组，即节点在特定振型下的位移值
	n　　　－质点(节点)数量
*/
double CalVibrationModeInvolvedCoef(double m_arr[],double x_arr[],double n)
{
	double num_up=0,num_down=0;
	for(int i=0;i<n;i++)
	{
		double temp=m_arr[i]*x_arr[i];
		num_up+=temp;
		num_down+=temp*x_arr[i];
	}
	return num_up/num_down;
}
/*
	功能：计算指定地震烈度下特定振型中各质点的水平地震作用力
	γ     －振型参预系数
	α　　 －指定地震烈度特定振型下的水平地震影响系数
	m_arr[]－质点质量数组，即节点等效质量数组
	x_arr[]－质点振型位移数组，即节点在特定振型下的位移值
	f_arr[]－指定情况下各质点所受的水平地震作用力,N（返回值）
	n　　　－质点(节点)数量
*/
void CalHoriEarthQuakeEffect(double gama,double alfa,double m_arr[],double x_arr[],double f_arr[],double n)
{
	for(int i=0;i<n;i++)
		f_arr[i]=gama*x_arr[i]*m_arr[i]*9.8*alfa;	//9.8为重力加速度
}
/*
	功能：计算指定地震烈度下各质点的竖向地震作用力
	αmax　－指定地震烈度所有振型中水平地震影响系数最大值（未考虑阻尼比影响）
	m_arr[]－质点质量数组，即节点等效质量数组
	h_arr[]－质点高度数组，即节点在特定振型下的位移值
	f_arr[]－指定情况下各质点所受的竖向地震作用力,N（返回值）
	n　　　－质点(节点)数量
*/
void CalVertEarthQuakeEffect(double alfaMax,double m_arr[],double h_arr[],double f_arr[],double n)
{
	int i;
	double Ge=0;	//地震作用时铁塔总质量(重力荷载代表值/g)
	for(i=0;i<n;i++)
		Ge+=m_arr[i];
	double Fev=4.7775*Ge*alfaMax;	//4.7775=0.75*0.65*9.8，结构底部总竖向地震作用标准值
	double num_sum=0;
	for(i=0;i<n;i++)
		num_sum+=m_arr[i]*h_arr[i];
	for(i=0;i<n;i++)
		f_arr[i]=Fev*(m_arr[i]*h_arr[i])/num_sum;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

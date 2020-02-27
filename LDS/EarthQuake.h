// EarthQuake.h: interface for the CTower class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EARTHQUAKE_H__06FC49B6_50F1_4315_A1B2_BAFF7A74AA17__INCLUDED_)
#define AFX_EARTHQUAKE_H__06FC49B6_50F1_4315_A1B2_BAFF7A74AA17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
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
double CalAlfaOfEQ(double T,double Tg,double maxAlfa,double zeta,int iCode=0);
/*
	功能：计算特定振型(指定自振周期)的振型参预系数γ
	m_arr[]－质点质量数组，即节点等效质量数组
	x_arr[]－质点振型位移数组，即节点在特定振型下的位移值
	n　　　－质点(节点)数量
*/
double CalVibrationModeInvolvedCoef(double m_arr[],double x_arr[],double n);
/*
	功能：计算指定地震烈度下特定振型中各质点的水平地震作用力
	γ     －振型参预系数
	α　　 －指定地震烈度特定振型下的水平地震影响系数
	m_arr[]－质点质量数组，即节点等效质量数组
	x_arr[]－质点振型位移数组，即节点在特定振型下的位移值
	f_arr[]－指定情况下各质点所受的水平地震作用力,N（返回值）
	n　　　－质点(节点)数量
*/
void CalHoriEarthQuakeEffect(double gama,double alfa,double m_arr[],double x_arr[],double f_arr[],double n);
/*
	功能：计算指定地震烈度下各质点的竖向地震作用力
	αmax　－指定地震烈度所有振型中水平地震影响系数最大值（未考虑阻尼比影响）
	m_arr[]－质点质量数组，即节点等效质量数组
	h_arr[]－质点高度数组，即节点在特定振型下的位移值
	f_arr[]－指定情况下各质点所受的竖向地震作用力,N（返回值）
	n　　　－质点(节点)数量
*/
void CalVertEarthQuakeEffect(double alfaMax,double m_arr[],double h_arr[],double f_arr[],double n);

#endif
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
	��ѹ����������������������Ҫ�û�����������Ϣ
	1.����ǰm���������ڼ���������
	2.�ܱ߻�������������Tg���ɸ��ݳ������ͼ���Ƶ����������С����ʽṹ��ƹ淶���в���ȡ��
	3.ˮƽ����Ӱ��ϵ�����ֵ��max���ɸ�����������Ҷȴ����С����ʽṹ��ƹ淶�������࿹����ƹ淶�л�ȡ��
	4.����Ȧƣ����ڸֽṹһ��ȡ0.02;
	�������ʱ�ɸ���������ǰ7��ʵ�����ͣ�ѡ���Խ��У�
		X���᷽��ˮƽ��������
		Y���᷽��ˮƽ��������
		�����������
		�����͵������㣨ˮƽ������ͬʱ���ǣ�
	5.�ֽṹ������������˨�����죩�������������ϵ����һ���Խ��ȡ0.85��������ȡ0.8
	6.ˮƽ������������÷���ϵ��
	7.�����������ɱ���صķ����ۺ�ϵ����һ��ȡ0.5
*/

/*
	���ܣ������ض�����(ָ����������)��ˮƽ����Ӱ��ϵ��
	T    ���ṹ��������(s);Tg���ܱ߻�����������(s);
	��max��ˮƽ����Ӱ��ϵ�����ֵ��������Ҷ����;
	��   ������ȣ����ڸֽṹһ��ȡֵΪ0.02���������ʽṹ��ƹ淶����;
	iCode��0:��ʾ��ѭ�����ʽṹ��ƹ淶��GB50135-2006��������������ƹ淶��GB50011-2001;
	       1:��ʾ��ѭ�������￹����ƹ淶��GB50191-2006;
*/
double CalAlfaOfEQ(double T,double Tg,double maxAlfa,double zeta,int iCode=0)
{
	double yita2=1+(0.05-zeta)/(0.06+1.7*zeta);	//�������ϵ��
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
		{	//�����ʽṹ��ƹ淶��
			double yita1=0.02+(0.05-zeta)/8;			//ֱ���½��׶ε��½�б�ʵ���ϵ��
			if(T>6) T=6;
			double gama=0.9+(0.05-zeta)/(0.5+5*zeta);	//˥��ָ��
			if(T<=5*Tg)
				return pow(Tg/T,gama)*yita2*maxAlfa;
			else //if(T<=6)
				return (yita2*pow(0.2,gama)-yita1*(T-5*Tg))*maxAlfa;
		}
		else
		{	//��������·��ƹ淶��(��ͬ�ڡ������￹����ƹ淶��GB50191-2006)
			if(T>7) T=7;
			if(T<=8*Tg)
				return pow(Tg/T,0.9)*yita2*maxAlfa;
			else //if(T<=7)
				return (0.154-0.01*(T-8*Tg))*yita2*maxAlfa;
		}
	}
}
/*
	���ܣ������ض�����(ָ����������)�����Ͳ�Ԥϵ����
	m_arr[]���ʵ��������飬���ڵ��Ч��������
	x_arr[]���ʵ�����λ�����飬���ڵ����ض������µ�λ��ֵ
	n���������ʵ�(�ڵ�)����
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
	���ܣ�����ָ�������Ҷ����ض������и��ʵ��ˮƽ����������
	��     �����Ͳ�Ԥϵ��
	������ ��ָ�������Ҷ��ض������µ�ˮƽ����Ӱ��ϵ��
	m_arr[]���ʵ��������飬���ڵ��Ч��������
	x_arr[]���ʵ�����λ�����飬���ڵ����ض������µ�λ��ֵ
	f_arr[]��ָ������¸��ʵ����ܵ�ˮƽ����������,N������ֵ��
	n���������ʵ�(�ڵ�)����
*/
void CalHoriEarthQuakeEffect(double gama,double alfa,double m_arr[],double x_arr[],double f_arr[],double n)
{
	for(int i=0;i<n;i++)
		f_arr[i]=gama*x_arr[i]*m_arr[i]*9.8*alfa;	//9.8Ϊ�������ٶ�
}
/*
	���ܣ�����ָ�������Ҷ��¸��ʵ���������������
	��max����ָ�������Ҷ�����������ˮƽ����Ӱ��ϵ�����ֵ��δ���������Ӱ�죩
	m_arr[]���ʵ��������飬���ڵ��Ч��������
	h_arr[]���ʵ�߶����飬���ڵ����ض������µ�λ��ֵ
	f_arr[]��ָ������¸��ʵ����ܵ��������������,N������ֵ��
	n���������ʵ�(�ڵ�)����
*/
void CalVertEarthQuakeEffect(double alfaMax,double m_arr[],double h_arr[],double f_arr[],double n)
{
	int i;
	double Ge=0;	//��������ʱ����������(�������ش���ֵ/g)
	for(i=0;i<n;i++)
		Ge+=m_arr[i];
	double Fev=4.7775*Ge*alfaMax;	//4.7775=0.75*0.65*9.8���ṹ�ײ�������������ñ�׼ֵ
	double num_sum=0;
	for(i=0;i<n;i++)
		num_sum+=m_arr[i]*h_arr[i];
	for(i=0;i<n;i++)
		f_arr[i]=Fev*(m_arr[i]*h_arr[i])/num_sum;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
